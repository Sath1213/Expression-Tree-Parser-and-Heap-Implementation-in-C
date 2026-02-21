/*
Name: Sathursan Rasatheepan
Date September 26, 2025
*/

#include <stdio.h>
#include <stdlib.h>

#define ROWS 20
#define COLS 10
#define TOTAL (ROWS * COLS)

int read_file(int arr[ROWS][COLS], const char *filename);
void write_file(int arr[ROWS][COLS], const char *filename);
int row_key(int arr[COLS]);
void swap_rows(int arr[ROWS][COLS], int i, int j);
void downheap(int arr[ROWS][COLS], int n, int i);
void build_maxheap(int arr[ROWS][COLS], int n);

int main(void) {
    int arr[ROWS][COLS];

    // Read input file
    int count = read_file(arr, "f.dat");
    if (count < 0) {
        fprintf(stderr, "Error: file f.dat does not exist.\n");
        return 1;
    }
    if (count != TOTAL) {
        fprintf(stderr, "Error: input does not contain 200 2-digit integers.\n");
        return 1;
    }

    // Build max heap
    build_maxheap(arr, ROWS);

    // Write output to file and also display
    write_file(arr, "f.dat");

    return 0;
}

// Reads integers from f.dat into a 20×10 array
int read_file(int arr[ROWS][COLS], const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;

    int value, count = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            // Stop early if fewer than 200 numbers are present
            if (fscanf(fp, "%d", &value) != 1) {
                fclose(fp);
                return count;
            }
            arr[i][j] = value;
            count++;
        }
    }

    int extra;
    if (fscanf(fp, "%d", &extra) == 1) {  // too many numbers
        fclose(fp);
        return count + 1;
    }

    fclose(fp);
    return count;
}

// Writes the heap array to f.dat and prints it
void write_file(int arr[ROWS][COLS], const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: cannot write to file.\n");
        return;
    }

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%02d ", arr[i][j]);
            fprintf(fp, "%02d ", arr[i][j]);
        }
        printf("\n");
        fprintf(fp, "\n");
    }

    fclose(fp);
}

// Returns the sum of the first three numbers in a row
int row_key(int arr[COLS]) {
    return arr[0] + arr[1] + arr[2];
}

// Swaps two rows element-by-element
void swap_rows(int arr[ROWS][COLS], int i, int j) {
    for (int k = 0; k < COLS; k++) {
        int tmp = arr[i][k];
        arr[i][k] = arr[j][k];
        arr[j][k] = tmp;
    }
}

// Moves a row down the heap until the max-heap order is correct
void downheap(int arr[ROWS][COLS], int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    int key_i = row_key(arr[i]);

    // Left child
    if (left < n && row_key(arr[left]) > key_i)
        largest = left;
    else
        largest = i;

    // Right child
    if (right < n && row_key(arr[right]) > row_key(arr[largest]))
        largest = right;

    // If root is not largest, swap and continue
    if (largest != i) {
        swap_rows(arr, i, largest);
        downheap(arr, n, largest);
    }
}

// Builds a max heap by applying downheap bottom-up
void build_maxheap(int arr[ROWS][COLS], int n) {
    for (int i = (n / 2) - 1; i >= 0; i--) {
        downheap(arr, n, i);
    }
}
