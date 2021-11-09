#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum algorithm
{
    alg_first = 0, // First fit
    alg_best = 1 // Best fit
};

// region structs
typedef struct block
{
    size_t start;
    size_t size;
} __attribute__((aligned(16))) block;

typedef struct memory
{
    block* blocks;
    size_t physical_size;
    size_t free_index;
} __attribute__((aligned(32))) memory;
// endregion

// region function prototypes
int initialise(memory* mem, enum algorithm* alg);

int allocate(memory* mem, enum algorithm alg);

int deallocate(memory* mem);

void defragment(memory* mem);

void release(memory* mem);

void print_blocks(const memory* mem);

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
        "Memory Allocation\n"
        "-----------------\n"
        "1) Enter parameters\n"
        "2) Allocate memory for block\n"
        "3) Deallocate memory for block\n"
        "4) Defragment memory\n"
        "5) Quit program and free memory\n\n"
        "Enter selection: ";

    memory mem = {.blocks = NULL, .physical_size = 0, .free_index = 0};
    enum algorithm alg = alg_best;

    int is_failure = 0;
    while (!is_failure) {
        fputs(menu_text, stdout);

        size_t choice = 5;
        if (get_size_t(&choice, 10, 1, 5)) {
            break;
        }
        puts(""); // Add a newline after the prompt.

        switch (choice) {
            case 1:
                is_failure = initialise(&mem, &alg);
                break;
            case 2:
                is_failure = allocate(&mem, alg);
                break;
            case 3:
                is_failure = deallocate(&mem);
                break;
            case 4:
                defragment(&mem);
                break;
            default:
                puts("Goodbye.");
                release(&mem);
                return EXIT_SUCCESS;
        }

        puts(""); // Add some space before the menu is shown again.
    }

    release(&mem);
    return EXIT_FAILURE;
}

int initialise(memory* const mem, enum algorithm* const alg)
{
    fputs("Enter size of physical memory: ", stdout);
    size_t size = 0;
    if (get_size_t(&size, 10, 1, SIZE_MAX)) {
        return -1;
    }

    fputs("Enter hole-fitting algorithm (0=first fit, 1=best fit): ", stdout);
    size_t alg_input = 0;
    if (get_size_t(&alg_input, 10, 0, 1)) {
        return -1;
    }

    // Replace the previous array if a new maximum is set.
    void* new_data = realloc(mem->blocks, size * sizeof(block));
    if (new_data == NULL) {
        fputs("FATAL: Failed to allocate memory for process array.\n", stderr);
        return -1;
    }

    mem->blocks = new_data;
    mem->physical_size = size;
    mem->free_index = 0;
    *alg = (unsigned) alg_input;

    return 0;
}

int allocate(memory* const mem, const enum algorithm alg)
{
    if (mem->physical_size == 0) {
        fputs("ERROR: Memory must first be initialised (menu option 1)\n", stderr);
        return 0;
    }

    if (mem->free_index == mem->physical_size) {
        fputs("ERROR: Memory is full. Deallocate first.\n", stderr);
        return 0;
    }

    fputs("Enter block size: ", stdout);
    size_t size = 0;
    if (get_size_t(&size, 10, 1, SIZE_MAX)) {
        return -1;
    }

    size_t i = 0;
    size_t start = 0;
    size_t open_start = 0;
    size_t open_size = 0;

    // selection is unsigned, so use a boolean instead of -1.
    bool selection_found = false;
    size_t selection = 0;

    // This initial value is guaranteed to always be larger than the largest possible open size.
    size_t smallest_size = mem->physical_size + 1;

    // This is a hacky way to get the loop below to check openings between allocated blocks as well
    // as the opening between the last allocated block's address and the physical memory size.
    // Since the block at the free index is not allocated, the fields can be set to any value
    // without any concern (the fields for deallocated blocks are normally never read/used).
    mem->blocks[mem->free_index].start = mem->physical_size;

    for (; i <= mem->free_index; ++i) {
        open_size = mem->blocks[i].start - open_start;
        if (size <= open_size && (alg == alg_first || smallest_size > open_size)) {
            selection = i;
            selection_found = true;
            start = open_start;
            smallest_size = open_size;

            if (alg == alg_first) {
                // No need to keep checking once the first open location is found.
                break;
            }
        }

        // 1 past the end address of the current block.
        open_start = mem->blocks[i].start + mem->blocks[i].size;
    }

    if (!selection_found) {
        fputs("ERROR: Unable to insert a new block.\n", stderr);
        return 0;
    }

    // To create space for the selection, starting at the selection, move all blocks forward by one.
    for (i = mem->free_index; i > selection; --i) {
        mem->blocks[i].start = mem->blocks[i - 1].start;
        mem->blocks[i].size = mem->blocks[i - 1].size;
    }

    // Now that there is space, set the fields for the new block.
    mem->blocks[selection].start = start;
    mem->blocks[selection].size = size;
    ++mem->free_index;

    size_t end = mem->blocks[selection].start + mem->blocks[selection].size;
    printf(
        "New block inserted, starting at %zu and ending before %zu\n",
        mem->blocks[selection].start,
        end);

    print_blocks(mem);
    return 0;
}

int deallocate(memory* const mem)
{
    if (mem->physical_size == 0) {
        fputs("ERROR: Memory must first be initialised (menu option 1)\n", stderr);
        return 0;
    }

    if (mem->free_index == 0) {
        fputs("ERROR: Memory is empty. Allocate first.\n", stderr);
        return 0;
    }

    fputs("Enter block index: ", stdout);
    size_t i = 0;
    if (get_size_t(&i, 10, 0, mem->free_index - 1)) {
        return -1;
    }

    --mem->free_index;

    // Starting at the block after the one being deallocated, move all allocated blocks back by 1.
    for (; i < mem->free_index; ++i) {
        mem->blocks[i].start = mem->blocks[i + 1].start;
        mem->blocks[i].size = mem->blocks[i + 1].size;
    }

    puts("Block successfully deallocated.");
    print_blocks(mem);

    return 0;
}

void defragment(memory* const mem)
{
    if (mem->physical_size == 0) {
        fputs("ERROR: Memory must first be initialised (menu option 1)\n", stderr);
        return;
    }

    if (mem->free_index == 0) {
        fputs("ERROR: Memory is empty. Allocate first.\n", stderr);
        return;
    }

    // The first block is a special case since it has no previous block.
    mem->blocks[0].start = 0;

    // Change every block's starting address into the previous block's ending address.
    size_t i = 1;
    for (; i < mem->free_index; ++i) {
        mem->blocks[i].start = mem->blocks[i - 1].start + mem->blocks[i - 1].size;
    }

    puts("Memory successfully defragmented.");
    print_blocks(mem);
}

void release(memory* const mem)
{
    if (mem->physical_size > 0) {
        free(mem->blocks);
        mem->physical_size = 0;
        mem->free_index = 0;
        mem->blocks = NULL;
    }
}

void print_blocks(const memory* const mem)
{
    puts("\nIndex\tStart\tOpening\n-----------------------");

    size_t i = 0;
    for (; i < mem->free_index; ++i) {
        size_t end = mem->blocks[i].start + mem->blocks[i].size;
        printf("%zu\t%zu\t%zu\n", i, mem->blocks[i].start, end);
    }

    printf("Size: %zu\n", mem->physical_size);
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
