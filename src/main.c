#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// region structs
typedef struct process
{
    size_t* max_requestable;
    size_t* allocated;
    size_t* needed;
} __attribute__((aligned(32))) process;

typedef struct resource
{
    size_t total_units;
    size_t available_units;
} __attribute__((aligned(16))) resource;

typedef struct operating_system
{
    process* processes;
    resource* resources;
    size_t process_count;
    size_t resource_count;
} __attribute__((aligned(32))) operating_system;
// endregion

// region enums
typedef enum process_field
{
    proc_max_requestable,
    proc_allocated,
    proc_needed
} process_field;

typedef enum resource_field
{
    res_total_units,
    res_available_units
} resource_field;
// endregion

// region function prototypes
int prompt_malloc(void** array, size_t element_size, size_t* element_count);

int try_malloc(void** array, size_t new_size);

int initialise(operating_system* os);

void print_graphs(operating_system* os);

void print_array(operating_system* os, resource_field field);

void print_matrix(operating_system* os, process_field field);

void release(operating_system* os);

/**
 * @brief Read a string from the input stream `stream` until a newline is encountered.
 *
 * Store the the characters in the buffer of size `*length` pointed to be `*str`, including the
 * newline and null terminator. Automatically increase the buffer's size to fit the entire input.
 * If `*str` is null, then ignore `*length` and allocate a new buffer.
 *
 * `*str` must be freed by the caller; `get_line` never frees it, even upon failure.
 *
 * @param str pointer to a pointer to the initial buffer or to a null pointer
 * @param length pointer to the size of the initial buffer
 * @param stream valid, open input stream
 *
 * @return 0 if successful, with `*length` equal to the length of the read string; otherwise -1,
 *         with feof or ferror potentially set on the stream
 */
int get_line(char** str, size_t* length, FILE* stream);

/**
 * @brief Prompt for an integer input in the given range until a valid value is given.
 *
 * Read a string with `get_line` and convert it to a `size_t`.
 *
 * @param out pointer to the `size_t` where the converted input will be stored
 * @param base base of the interpreted integer value as understood by `strtoumax`
 * @param min minimum allowed value (inclusive)
 * @param max maximum allowed value (inclusive)
 *
 * @return 0 if successful; -1 otherwise
 */
int get_size_t(size_t* out, int base, size_t min, size_t max);
// endregion

int main(void)
{
    const char* const menu_text =
        "Banker's Algorithm\n"
        "------------------\n"
        "1) Enter parameters\n"
        "2) Print resource and process graphs\n"
        "3) Determine a safe sequence\n"
        "4) Quit program and free memory\n\n"
        "Enter selection: ";

    operating_system os = {
        .processes = NULL, .resources = NULL, .process_count = 0, .resource_count = 0};

    int is_failure = 0;
    while (!is_failure) {
        fputs(menu_text, stdout);

        size_t choice = 4;
        if (get_size_t(&choice, 10, 1, 4)) {
            break;
        }
        puts(""); // Add a newline after the prompt.

        switch (choice) {
            case 1:
                is_failure = initialise(&os);
                break;
            case 2:
                print_graphs(&os);
                break;
            case 3:
                break;
            default:
                release(&os);
                puts("Goodbye.");
                return EXIT_SUCCESS;
        }

        puts("\n"); // Add some space before the menu is shown again.
    }

    release(&os);
    return EXIT_FAILURE;
}

int prompt_malloc(void** const array, const size_t element_size, size_t* const element_count)
{
    size_t max = 0;
    if (get_size_t(&max, 10, 1, SIZE_MAX)) {
        return -1;
    }

    if (try_malloc(array, max * element_size)) {
        return -1;
    }

    *element_count = max;
    return 0;
}

int try_malloc(void** const array, const size_t new_size)
{
    void* new_array = malloc(new_size);
    if (new_array == NULL) {
        fputs("FATAL: Failed to allocate memory for array.\n", stderr);
        return -1;
    }

    *array = new_array;
    return 0;
}

int initialise(operating_system* const os)
{
    release(os);

    fputs("Enter total number of processes: ", stdout);
    if (prompt_malloc((void**) &os->processes, sizeof(process), &os->process_count)) {
        return -1;
    }

    fputs("Enter total number of resources: ", stdout);
    if (prompt_malloc((void**) &os->resources, sizeof(resource), &os->resource_count)) {
        return -1;
    }

    size_t i = 0;
    for (; i < os->process_count; ++i) {
        size_t size = sizeof(size_t) * os->resource_count;

        if (try_malloc((void**) &os->processes[i].max_requestable, size)) {
            return -1;
        }
        if (try_malloc((void**) &os->processes[i].allocated, size)) {
            return -1;
        }
        if (try_malloc((void**) &os->processes[i].needed, size)) {
            return -1;
        }
    }

    puts("***");

    for (i = 0; i < os->resource_count; ++i) {
        printf("Enter number of total units for resource %zu: ", i);

        size_t total_units = 0;
        if (get_size_t(&total_units, 10, 0, SIZE_MAX)) {
            return -1;
        }

        os->resources[i].total_units = total_units;
        os->resources[i].available_units = total_units;
    }

    puts("***");

    size_t j = 0;
    for (i = 0; i < os->process_count; ++i) {
        for (j = 0; j < os->resource_count; ++j) {
            printf(
                "Enter max number of units that process %zu can request from resource %zu: ", i, j);

            size_t max_requestable = 0;
            if (get_size_t(&max_requestable, 10, 0, os->resources[j].total_units)) {
                return -1;
            }

            os->processes[i].max_requestable[j] = max_requestable;
            os->processes[i].needed[j] = max_requestable;
        }
    }

    puts("***");

    for (i = 0; i < os->process_count; ++i) {
        for (j = 0; j < os->resource_count; ++j) {
            printf("Enter number of units that process %zu is allocated from resource %zu: ", i, j);

            size_t allocated = 0;
            if (get_size_t(&allocated, 10, 0, os->processes[i].max_requestable[j])) {
                return -1;
            }

            os->processes[i].allocated[j] = allocated;
            os->processes[i].needed[j] -= allocated;
            os->resources[j].available_units -= allocated;
        }
    }

    return 0;
}

void print_graphs(operating_system* const os)
{
    if (os->process_count == 0 || os->resource_count == 0) {
        fputs(
            "ERROR: The processes and resources must first be initialised (menu option 1).",
            stderr);
        return;
    }

    puts("Total units:");
    print_array(os, res_total_units);

    puts("\nAvailable units:");
    print_array(os, res_available_units);

    puts("\nMax requestable units:");
    print_matrix(os, proc_max_requestable);

    puts("\nAllocated units:");
    print_matrix(os, proc_allocated);

    puts("\nNeeded units:");
    print_matrix(os, proc_needed);
}

void print_array(operating_system* const os, const resource_field field)
{
    size_t i = 0;
    for (; i < os->resource_count; ++i) {
        printf("\tr%zu", i);
    }

    puts("");

    for (i = 0; i < os->resource_count; ++i) {
        size_t value = 0;
        switch (field) {
            case res_total_units:
                value = os->resources[i].total_units;
                break;
            case res_available_units:
                value = os->resources[i].available_units;
                break;
            default:
                fputs("\t", stdout); // Unknown field; ignore it.
                break;
        }

        printf("\t%zu", value);
    }

    puts("");
}

void print_matrix(operating_system* const os, const process_field field)
{
    size_t i = 0;
    size_t j = 0;

    for (; i < os->process_count; ++i) {
        if (i == 0) {
            for (j = 0; j < os->resource_count; ++j) {
                printf("\tr%zu", j);
            }
            puts("");
        }

        printf("p%zu", i);

        for (j = 0; j < os->resource_count; ++j) {
            size_t value = 0;
            switch (field) {
                case proc_max_requestable:
                    value = os->processes[i].max_requestable[j];
                    break;
                case proc_allocated:
                    value = os->processes[i].allocated[j];
                    break;
                case proc_needed:
                    value = os->processes[i].needed[j];
                    break;
                default:
                    fputs("\t", stdout); // Unknown field; ignore it.
                    break;
            }

            printf("\t%zu", value);
        }

        puts("");
    }
}

void release(operating_system* const os)
{
    size_t i = 0;
    for (; i < os->process_count; ++i) {
        free(os->processes[i].max_requestable);
        free(os->processes[i].allocated);
        free(os->processes[i].needed);
    }

    free(os->processes);
    free(os->resources);

    os->process_count = 0;
    os->resource_count = 0;
}

// region utilities
int get_line(char** str, size_t* length, FILE* stream)
{
    size_t preallocated_size = 0;
    if (*str != NULL) {
        assert(*length > 0);
        preallocated_size = *length;
    }

    *length = 0;
    const int chunk_size = 256;
    char buffer[chunk_size];
    size_t buffer_length = 0;

    do {
        // Read the input into the buffer.
        fgets(buffer, chunk_size, stream);
        if (errno) {
            return -1;
        }

        // Allocate more memory if needed.
        buffer_length = strlen(buffer);
        const size_t new_length = *length + buffer_length + 1;

        // str may have been preallocated, so reallocation may not be necessary.
        if (new_length > preallocated_size) {
            // Acts as malloc(new_length) if *str == NULL
            void* new_str = realloc(*str, new_length);
            if (new_str == NULL) {
                return -1;
            }
            *str = new_str;
        }

        // Append the buffer to the end of the string.
        strcpy(*str + *length, buffer);
        *length += buffer_length;
    } while (
        // If the buffer isn't full, then fgets stopped on a newline.
        buffer_length == (unsigned) (chunk_size - 1)
        // If it's full, then continue reading until a newline is reached.
        && buffer[chunk_size - 2] != '\n');

    return 0;
}

int get_size_t(size_t* out, int base, size_t min, size_t max)
{
    assert(min <= SIZE_MAX && max <= SIZE_MAX);
    assert(min <= max);

    while (1) {
        char* input = NULL;
        size_t length = 0;

        if (get_line(&input, &length, stdin)) {
            free(input);
            fputs("\nFATAL: Error encountered while reading input.\n", stderr);
            return -1;
        }

        char* end = NULL;
        const uintmax_t converted = strtoumax(input, &end, base);

        if (errno == ERANGE || converted > max || converted < min) {
            fprintf(stderr, "ERROR: Integer must be in range [%zu,%zu], try again: ", min, max);
        } else if (end == input || *end != '\n') {
            fputs("ERROR: Invalid integer, try again: ", stderr);
        } else if (strchr(input, '-') != NULL) {
            // Check after conversion to prioritise the invalid integer error over this one.
            fputs("ERROR: Integer must be positive, try again: ", stderr);
        } else {
            free(input);
            *out = (size_t) converted;
            return 0;
        }
    }
}
// endregion
