#include <assert.h>
#include <errno.h>
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

int main(void)
{
    printf(MENU_TEXT);

    while (1) {
        char choice = '\0';
        scanf(" %c", &choice);

        switch (choice) {
            case '1':
                enter_parameters();
                break;
            case '2':
                create();
                break;
            case '3':
                destroy();
                break;
            case '4':
                quit();
                return 0;
            default:
                printf("Invalid selection, try again: ");
                continue; // Avoid printing the menu again.
        }

        printf("\n\n%s", MENU_TEXT);
    }
}

void enter_parameters(void) { }

void create(void) { }

void destroy(void) { }

void quit(void) { }

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
