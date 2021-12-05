#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// region structs
typedef struct disk_request
{
    size_t track_count;
    size_t sequence_length;
    size_t* track_sequence;
} __attribute__((aligned(32))) disk_request;
// endregion

// region function prototypes
int initialise(disk_request* request);

void schedule_fifo(const disk_request* request);

int schedule_sstf(const disk_request* request);

void release(disk_request* request);

bool is_duplicate(const size_t* array, size_t size, size_t value);

void print_traversal(const disk_request* request, const size_t* sequence);

int compare_tracks(const void* a, const void* b);

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
        "Disk Scheduling\n"
        "---------------\n"
        "1) Enter parameters\n"
        "2) Schedule disk tracks with FIFO\n"
        "3) Schedule disk tracks with SSTF\n"
        "4) Quit program and free memory\n\n"
        "Enter selection: ";

    disk_request request = {0, 0, NULL};

    int is_failure = 0;
    while (!is_failure) {
        fputs(menu_text, stdout);

        size_t choice = 5;
        if (get_size_t(&choice, 10, 1, 4)) {
            break;
        }
        puts(""); // Add a newline after the prompt.

        switch (choice) {
            case 1:
                is_failure = initialise(&request);
                break;
            case 2:
                schedule_fifo(&request);
                break;
            case 3:
                is_failure = schedule_sstf(&request);
                break;
            default:
                puts("Goodbye.");
                release(&request);
                return EXIT_SUCCESS;
        }

        puts(""); // Add some space before the menu is shown again.
    }

    release(&request);
    return EXIT_FAILURE;
}

int initialise(disk_request* const request)
{
    fputs("Enter number of concentric tracks (2 or more): ", stdout);
    size_t track_count = 0;
    if (get_size_t(&track_count, 10, 2, SIZE_MAX)) {
        return -1;
    }

    printf("Enter size of sequence (1-%zu): ", track_count - 1);
    size_t sequence_length = 0;
    if (get_size_t(&sequence_length, 10, 1, track_count - 1)) {
        return -1;
    }

    // Replace the previous array if a new size is set.
    size_t* new_sequence = realloc(request->track_sequence, sequence_length * sizeof(size_t));
    if (new_sequence == NULL) {
        fputs("FATAL: Failed to allocate memory for track sequence array.\n", stderr);
        return -1;
    }

    size_t i = 0;
    for (; i < sequence_length; ++i) {
        printf("Enter track index (1-%zu) for sequence index %zu: ", track_count - 1, i);
        size_t track_index = 0;
        if (get_size_t(&track_index, 10, 1, track_count - 1)) {
            return -1;
        }

        while (is_duplicate(new_sequence, i, track_index)) {
            fputs("ERROR: Invalid track - duplicate, try again: ", stderr);
            if (get_size_t(&track_index, 10, 1, track_count - 1)) {
                return -1;
            }
        }

        new_sequence[i] = track_index;
    }

    request->track_count = track_count;
    request->sequence_length = sequence_length;
    request->track_sequence = new_sequence;

    return 0;
}

void schedule_fifo(const disk_request* const request)
{
    if (request->sequence_length == 0) {
        fputs("ERROR: A track sequence must first be entered (menu option 1)\n", stderr);
        return;
    }

    print_traversal(request, request->track_sequence);
}

int schedule_sstf(const disk_request* const request)
{
    if (request->sequence_length == 0) {
        fputs("ERROR: A track sequence must first be entered (menu option 1)\n", stderr);
        return 0;
    }

    // Allocate memory for temporary arrays.
    size_t* ordered_sequence = malloc(request->sequence_length * sizeof(size_t));
    if (ordered_sequence == NULL) {
        fputs("FATAL: Failed to allocate memory for SSTF ordered_sequence array.\n", stderr);
        return -1;
    }

    memcpy(ordered_sequence, request->track_sequence, request->sequence_length * sizeof(size_t));

    size_t* ordered_sequence_delay = calloc(request->sequence_length, sizeof(size_t));
    if (ordered_sequence_delay == NULL) {
        fputs("FATAL: Failed to allocate memory for SSTF ordered_sequence_delay array.\n", stderr);
        free(ordered_sequence);
        return -1;
    }

    // Sort the ordered sequence array in ascending order.
    qsort(ordered_sequence, request->sequence_length, sizeof(size_t), compare_tracks);

    // Compute delays for the ordered sequence array.
    size_t i = 1;
    size_t j = 0;
    for (; i < request->sequence_length; ++i) {
        for (j = 0; j < i; ++j) {
            if (ordered_sequence[i] == request->track_sequence[j]) {
                ordered_sequence_delay[i] = i - j;
                break;
            }
        }
    }

    // Compute delays.
    size_t average_delay_total = 0;
    size_t delayed_tracks = 0;
    size_t max_delay_i = 0;

    for (i = 0; i < request->sequence_length; ++i) {
        if (ordered_sequence_delay[i] > 0) {
            average_delay_total += ordered_sequence_delay[i];
            ++delayed_tracks;

            if (ordered_sequence_delay[i] > ordered_sequence_delay[max_delay_i]) {
                max_delay_i = i;
            }
        }
    }

    // Print all results.
    print_traversal(request, ordered_sequence);

    if (delayed_tracks > 0) {
        printf(
            "The average delay of all tracks processed later is: %.2f\n\n"
            "The longest delay experienced by a track is: %zu by track %zu\n",
            (double) average_delay_total / (double) delayed_tracks,
            ordered_sequence_delay[max_delay_i],
            ordered_sequence[max_delay_i]);
    }

    free(ordered_sequence);
    free(ordered_sequence_delay);

    return 0;
}

void release(disk_request* const request)
{
    if (request->sequence_length > 0) {
        free(request->track_sequence);
        request->track_count = 0;
        request->sequence_length = 0;
        request->track_sequence = NULL;
    }
}

bool is_duplicate(const size_t* const array, const size_t end, const size_t value)
{
    size_t i = 0;
    for (; i < end; ++i) {
        if (array[i] == value) {
            return true;
        }
    }

    return false;
}

void print_traversal(const disk_request* const request, const size_t* const sequence)
{
    fputs("Sequence of tracks to seek:", stdout);

    size_t i = 0;
    for (; i < request->sequence_length; ++i) {
        printf(" %zu", request->track_sequence[i]);
    }

    fputs("\nTraversed sequence:", stdout);

    size_t current_track = 0;
    size_t traversed = 0;

    for (i = 0; i < request->sequence_length; ++i) {
        printf(" %zu", sequence[i]);
        if (current_track > sequence[i]) {
            traversed += current_track - sequence[i];
        } else {
            traversed += sequence[i] - current_track;
        }
        current_track = sequence[i];
    }

    printf("\nThe number of tracks traversed is: %zu\n", traversed);
}

int compare_tracks(const void* const a, const void* const b)
{
    const size_t track_a = *(const size_t*) a;
    const size_t track_b = *(const size_t*) b;

    if (track_a < track_b) {
        return -1;
    }

    if (track_a > track_b) {
        return 1;
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
