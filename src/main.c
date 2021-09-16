#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// region structs
typedef struct pcb
{
    size_t parent;
    size_t first_child;
    size_t older_sibling;
    size_t younger_sibling;
} __attribute__((aligned(32))) pcb;

typedef struct pcb_array
{
    pcb** data;
    size_t size;
} __attribute__((aligned(16))) pcb_array;
// endregion

// region function prototypes
int initialise(pcb_array* array);
int create(pcb_array* array);
int destroy(pcb_array* array);
void quit(pcb_array* array);

/**
 * @brief Read a string from the input stream `stream` until a newline is encountered.
 *
 * Store the the characters in the buffer of size `*length` pointed to be `*str`, including the
 * newline and null terminator. Automatically increase the buffer's size to fit the entire input.
 * If `*str` is null, then ignore `*length` and allocate a new buffer.
 *
 * `*str` must be freed by the caller; `getline` never frees it, even upon failure.
 *
 * @param str pointer to a pointer to the initial buffer or to a null pointer
 * @param length pointer to the size of the initial buffer
 * @param stream valid, open input stream
 *
 * @return 0 if successful, with `*length` equal to the length of the read string; otherwise -1,
 *         with feof or ferror potentially set on the stream
 */
int getline(char** str, size_t* length, FILE* stream);

/**
 * @brief Prompt for an integer input in the given range until a valid value is given.
 *
 * Read a string with `getline` and convert it to a `size_t`.
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
        "Process creation and destruction\n"
        "--------------------------------\n"
        "1) Enter parameters\n"
        "2) Create a new child process\n"
        "3) Destroy all descendants of a process\n"
        "4) Quit program and free memory\n\n\n"
        "Enter selection: ";

    pcb_array array = {
        .data = NULL,
        .size = 0,
    };

    int is_failure = 0;
    while (!is_failure) {
        fputs(menu_text, stdout);

        size_t choice = 4;
        if (get_size_t(&choice, 10, 1, 4)) {
            break;
        }

        switch (choice) {
            case 1:
                is_failure = initialise(&array);
                break;
            case 2:
                is_failure = create(&array);
                break;
            case 3:
                is_failure = destroy(&array);
                break;
            default:
                quit(&array);
                return EXIT_SUCCESS;
        }

        puts("\n"); // Add some space before the menu is shown again.
    }

    quit(&array);
    return EXIT_FAILURE;
}

int initialise(pcb_array* const array)
{
    fputs("Enter maximum number of processes: ", stdout);

    size_t max = 0;
    if (get_size_t(&max, 10, 1, SIZE_MAX)) {
        return -1;
    }

    // Replace the previous array if a new maximum is set.
    void* new_data = realloc(array->data, max * sizeof(pcb));
    if (new_data == NULL) {
        fputs("FATAL: Failed to allocate memory for PCB array.\n", stderr);
        return -1;
    }

    array->data = new_data;
    array->size = max;

    size_t i = 0;
    for (; i < max; ++i) {
        // A process cannot be its own parent, child, or sibling. Therefore, the current index can
        // be used as the initial value for the fields. No need for a signed value (e.g. -1)!
        // The exception to this is process 0, which should be assumed to always exist.
        // It is its own parent.
        array->data[i]->parent = i;
        array->data[i]->first_child = i;
        array->data[i]->older_sibling = i;
        array->data[i]->younger_sibling = i;
    }

    printf("You entered %zu\n", max);
    return 0;
}

int create(pcb_array* const array)
{
    assert(array->size > 0); // TODO: this needs to be a normal check.
    fputs("Enter the parent process index: ", stdout);

    size_t proc_index = 0;
    if (get_size_t(&proc_index, 10, 0, array->size - 1)) {
        return -1;
    }

    printf("You selected %zu\n", proc_index);
    return 0;
}

int destroy(pcb_array* const array)
{
    assert(array->size > 0); // TODO: this needs to be a normal check.
    fputs("Enter the process whose descendants are to be destroyed: ", stdout);

    size_t proc_index = 0;
    if (get_size_t(&proc_index, 10, 0, array->size - 1)) {
        return -1;
    }

    printf("You selected %zu\n", proc_index);
    return 0;
}

void quit(pcb_array* const array)
{
    free(array->data);
    puts("Quitting program...");
}

// region utilities
int getline(char** str, size_t* length, FILE* stream)
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

        if (getline(&input, &length, stdin)) {
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
