#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* const MENU_TEXT =
    "Process creation and destruction\n"
    "--------------------------------\n"
    "1) Enter parameters\n"
    "2) Create a new child process\n"
    "3) Destroy all descendants of a process\n"
    "4) Quit program and free memory\n\n\n"
    "Enter selection: ";

void enter_parameters(void);
void create(void);
void destroy(void);
void quit(void);

/**
 * @brief Read a string from the input stream `stream` until a newline is encountered.
 *
 * Store the the characters in the buffer of size `*length` pointed to be `*str`, including the
 * newline and null terminator. Automatically increase the buffer's size to fit the entire input.
 * If `*str` is null, then ignore `*length` and allocate a new buffer.
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
 * Read a string with `getline` and convert it to an `size_t`. Terminate the program with code 1
 * if there is an error reading stdin.
 *
 * @param base base of the interpreted integer value as understood by `strtoumax`
 * @param min minimum allowed value (inclusive)
 * @param max maximum allowed value (inclusive)
 *
 * @return the inputted integer
 */
size_t get_size_t(int base, size_t min, size_t max);

int main(void)
{
    if (atexit(&quit)) {
        fprintf(stderr, "FATAL: Failed to register atexit handler");
        return EXIT_FAILURE;
    }

    puts(MENU_TEXT);

    while (1) {
        const size_t choice = get_size_t(10, 1, 4);

        switch (choice) {
            case 1:
                enter_parameters();
                break;
            case 2:
                create();
                break;
            case 3:
                destroy();
                break;
            default:
                return EXIT_SUCCESS;
        }

        printf("\n\n%s", MENU_TEXT);
    }
}

void enter_parameters(void)
{
    puts("Enter maximum number of processes: ");
}

void create(void)
{
    puts("Enter the parent process index: ");
}

void destroy(void)
{
    puts("Enter the process whose descendants are to be destroyed: ");
}

void quit(void)
{
    puts("Quitting program...\n");
}

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
            *str = realloc(*str, new_length); // Acts as malloc(new_length) if *str == NULL
            if (*str == NULL) {
                return -1;
            }
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

size_t get_size_t(int base, size_t min, size_t max)
{
    assert(min <= SIZE_MAX && max <= SIZE_MAX);
    assert(min <= max);

    char* input = NULL;
    size_t length = 0;

    if (getline(&input, &length, stdin)) {
        if (input != NULL) {
            free(input);
        }

        fputs("FATAL: Error encountered while reading input.\n", stderr);
        exit(EXIT_FAILURE); // NOLINT(concurrency-mt-unsafe)
    }

    while (1) {
        char* end = NULL;
        const uintmax_t out = strtoumax(input, &end, base);

        if (errno == ERANGE || out > max) {
            fprintf(stderr, "ERROR: Integer must be in range [%zu,%zu], try again: ", min, max);
        } else if (end == input || *end != '\n') {
            fputs("ERROR: Invalid integer, try again: ", stderr);
        } else if (strchr(input, '-') != NULL) {
            // Check after conversion to prioritise the invalid integer error over this one.
            fputs("ERROR: Integer must be positive, try again: ", stderr);
        } else {
            free(input);
            return (size_t) out;
        }
    }
}
