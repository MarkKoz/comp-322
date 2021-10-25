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

// region function prototypes
int prompt_reallocate(void** array, size_t element_size, size_t* element_count);

int try_realloc(void** array, size_t new_size);

int initialise(operating_system* os);

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

int prompt_reallocate(void** const array, const size_t element_size, size_t* const element_count)
{
    size_t max = 0;
    if (get_size_t(&max, 10, 1, SIZE_MAX)) {
        return -1;
    }

    // Replace the previous array if a new maximum is set.
    if (try_realloc(array, max * element_size)) {
        return -1;
    }

    *element_count = max;
    return 0;
}

int try_realloc(void** const array, const size_t new_size)
{
    void* new_array = realloc(*array, new_size);
    if (new_array == NULL) {
        fputs("FATAL: Failed to allocate memory for array.\n", stderr);
        return -1;
    }

    *array = new_array;
    return 0;
}

int initialise(operating_system* const os)
{
    fputs("Enter total number of processes: ", stdout);
    if (prompt_reallocate((void**) &os->processes, sizeof(process), &os->process_count)) {
        return -1;
    }

    fputs("Enter total number of resources: ", stdout);
    if (prompt_reallocate((void**) &os->resources, sizeof(resource), &os->resource_count)) {
        return -1;
    }

    size_t i = 0;
    for (; i <= os->process_count; ++i) {
        size_t size = sizeof(size_t) * os->resource_count;

        if (try_realloc((void**) &os->processes[i].max_requestable, size)) {
            return -1;
        }
        if (try_realloc((void**) &os->processes[i].allocated, size)) {
            return -1;
        }
        if (try_realloc((void**) &os->processes[i].needed, size)) {
            return -1;
        }
    }

    return 0;
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
