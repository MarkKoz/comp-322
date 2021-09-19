#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// region structs
/**
 * @brief Process Control Block
 */
typedef struct pcb
{
    size_t parent;
    size_t first_child;
    size_t older_sibling;
    size_t younger_sibling;
} __attribute__((aligned(32))) pcb;

/**
 * @brief A PCB array with a field for the array's size.
 */
typedef struct pcb_array
{
    pcb* data;
    size_t size;
} __attribute__((aligned(16))) pcb_array;
// endregion

// region function prototypes
/**
 * @brief Initialise an array of PCBs to the inputted size.
 *
 * Prompt for a maximum process count and allocate memory accordingly into `array->data` as by
 * `realloc`. Effectively overwrite any extant PCBs, unless a fatal error is encountered while
 * prompting for a maximum process count.
 *
 * Use each PCB's index in the array as the initial value for all fields of that PCB. Thus, a field
 * can be considered unset if its value equals the index. The index can be used because it doesn't
 * make sense for a process to be its own parent, child, or sibling. If a PCB's parent is equal to
 * its own index, then that PCB is said to be "inactive". The exception is PCB 0, which is always
 * active and is its own parent.
 *
 * @param array array to initialise with PCBs
 *
 * @return 0 if successful; -1 otherwise
 */
int initialise(pcb_array* array);

/**
 * @brief Create a new child process.
 *
 * @param array array containing the PCBs
 *
 * @return -1 upon a fatal error; 0 otherwise (could be success or non-fatal error)
 */
int create(pcb_array* array);

/**
 * @brief Destroy the child processes of an inputted parent.
 *
 * @param array array containing the PCBs
 *
 * @return -1 upon a fatal error; 0 otherwise (could be success or non-fatal error)
 */
int destroy(pcb_array* array);

/**
 * @brief Free memory and display an exit message.
 *
 * @param array array containing the PCBs
 */
void quit(pcb_array* array);

/**
 * @brief Destroy the PCB at `proc_index` and all its younger siblings.
 *
 * @param array array containing the PCBs
 * @param proc_index the index of the PCB in `array`
 */
void destroy_recursive(pcb_array* array, size_t proc_index);

/**
 * @brief Display a table of all PCBs in the given array.
 *
 * Don't display inactive PCBs. Display nothing for unset fields (i.e. when value == process index).
 *
 * @param array array containing the PCBs to display
 */
void show_table(pcb_array* array);

/**
 * @brief Prompt for an index of an active PCB until a valid value is given.
 *
 * @param array array containing the PCBs
 * @param index pointer to the `size_t` where the input will be stored
 *
 * @return 0 if successful; -1 otherwise
 */
int get_active_process(pcb_array* array, size_t* index);

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
        array->data[i].parent = i;
        array->data[i].first_child = i;
        array->data[i].older_sibling = i;
        array->data[i].younger_sibling = i;
    }

    printf("You entered %zu\n", max);
    return 0;
}

int create(pcb_array* const array)
{
    if (array->size == 0) {
        fputs("ERROR: The PCB array must first be initialised (menu option 1).", stderr);
        return 0;
    }

    size_t parent_index = 0;
    size_t child_index = 1; // Process 0 is always active.

    fputs("Enter the parent process index: ", stdout);
    if (get_active_process(array, &parent_index)) {
        return -1;
    }

    // Search for an inactive process.
    while (child_index < array->size && array->data[child_index].parent != child_index) {
        ++child_index;
    }

    if (child_index == array->size) {
        fputs("ERROR: There is no space for a new process.\n", stderr);
        return 0;
    }

    // Add the child.
    array->data[child_index].parent = parent_index;

    if (array->data[parent_index].first_child == parent_index) {
        array->data[parent_index].first_child = child_index;
    } else {
        // Search for the youngest sibling starting at the first child of the parent.
        size_t youngest_sibling = array->data[parent_index].first_child;
        while (array->data[youngest_sibling].younger_sibling != youngest_sibling) {
            youngest_sibling = array->data[youngest_sibling].younger_sibling;
        }

        array->data[youngest_sibling].younger_sibling = child_index;
        array->data[child_index].older_sibling = youngest_sibling;
    }

    printf("Created process %zu as a child of process %zu.\n", child_index, parent_index);
    show_table(array);

    return 0;
}

int destroy(pcb_array* const array)
{
    if (array->size == 0) {
        fputs("ERROR: The PCB array must first be initialised (menu option 1).", stderr);
        return 0;
    }

    size_t proc_index = 0;

    fputs("Enter the process whose descendants are to be destroyed: ", stdout);
    if (get_active_process(array, &proc_index)) {
        return -1;
    }

    // Destroy all children.
    destroy_recursive(array, array->data[proc_index].first_child);

    // Reset the fields of the process.
    array->data[proc_index].first_child = proc_index;

    printf("Deleted all descendants of process %zu.\n", proc_index);
    show_table(array);

    return 0;
}

void quit(pcb_array* const array)
{
    free(array->data);
    puts("Quitting program...");
}

void destroy_recursive(pcb_array* const array, size_t proc_index)
{
    size_t next = array->data[proc_index].younger_sibling;

    // Reset the PCB.
    array->data[proc_index].parent = proc_index;
    array->data[proc_index].first_child = proc_index;
    array->data[proc_index].older_sibling = proc_index;
    array->data[proc_index].younger_sibling = proc_index;

    // Destroy the younger sibling if the current process has one.
    if (next != proc_index) {
        destroy_recursive(array, next);
    }
}

void show_table(pcb_array* const array)
{
    puts("i       Parent  First   Older   Younger\n--------------------------------------");

    size_t i = 0;
    for (; i < array->size; ++i) {
        // Skip inactive processes. Process 0 is always active.
        if (i != 0 && array->data[i].parent == i) {
            continue;
        }

        printf("%zu\t%zu\t", i, array->data[i].parent);

        // TODO: reduce code redundancy
        // TODO: support alignment of arbitrary column widths
        if (array->data[i].first_child != i) {
            printf("%zu\t", array->data[i].first_child);
        } else {
            fputs("\t", stdout);
        }

        if (array->data[i].older_sibling != i) {
            printf("%zu\t", array->data[i].older_sibling);
        } else {
            fputs("\t", stdout);
        }

        if (array->data[i].younger_sibling != i) {
            printf("%zu\n", array->data[i].younger_sibling);
        } else {
            fputs("\n", stdout);
        }
    }
}

int get_active_process(pcb_array* const array, size_t* const index)
{
    while (1) {
        if (get_size_t(index, 10, 0, array->size - 1)) {
            return -1;
        }

        if (*index != 0 && array->data[*index].parent == *index) {
            fputs("ERROR: The selected process is not active, try again: ", stderr);
        } else {
            break;
        }
    }

    return 0;
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
