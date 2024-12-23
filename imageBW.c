/// imageBW - A simple image processing module for BW images
///           represented using run-length encoding (RLE)
///
/// This module is part of a programming project
/// for the course AED, DETI / UA.PT
///
/// You may freely use and modify this code, at your own risk,
/// as long as you give proper credit to the original and subsequent authors.
///
/// The AED Team <jmadeira@ua.pt, jmr@ua.pt, ...>
/// 2024

// Student authors (fill in below):
// NMec: 118831
// Name: Igor Baltarejo
// NMec: 120054
// Name: Joao Barreira
//
// Date: 02/11/24
//

#include "imageBW.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instrumentation.h"

// The data structure
//
// A BW image is stored in a structure containing 3 fields:
// Two integers store the image width and height.
// The other field is a pointer to an array that stores the pointers
// to the RLE compressed image rows.
//
// Clients should use images only through variables of type Image,
// which are pointers to the image structure, and should not access the
// structure fields directly.

// Constant value --- Use them throughout your code
// const uint8 BLACK = 1;  // Black pixel value, defined on .h
// const uint8 WHITE = 0;  // White pixel value, defined on .h
const int EOR = -1;  // Stored as the last element of a RLE row

// Internal structure for storing RLE BW images
struct image {
    uint32 width;
    uint32 height;
    int** row;  // pointer to an array of pointers referencing the compressed rows
};

// This module follows "design-by-contract" principles.
// Read `Design-by-Contract.md` for more details.

/// Error handling functions

// In this module, only functions dealing with memory allocation or
// file (I/O) operations use defensive techniques.
// When one of these functions fails,
// it immediately prints an error and exits the program.
// This fail-fast approach to error handling is simpler for the programmer.

// Use the following function to check a condition
// and exit if it fails.

// Check a condition and if false, print failmsg and exit.
static void check(int condition, const char* failmsg) {
    if (!condition) {
        perror(failmsg);
        exit(errno || 255);
    }
}

/// Init Image library.  (Call once!)
/// Currently, simply calibrate instrumentation and set names of counters.
void ImageInit(void) {  ///
    InstrCalibrate();
    InstrName[0] = "pixmem";  // InstrCount[0] will count pixel array acesses
    InstrName[1] = "bool_op"; // InstrCount[1] counts boolean operations
    // Name other counters here...
}

// Macros to simplify accessing instrumentation counters:
#define PIXMEM InstrCount[0]
#define BOOL_OP InstrCount[1] // Tracks boolean operations (AND)

// TIP: Search for PIXMEM or InstrCount to see where it is incremented!

/// Auxiliary (static) functions

/// Reverse array recursivly
static void ReverseArray(int* arr, size_t size) {
    if (size <= 1) return; //Base case for recursion 
    
    //Switch first with last element
    int aux = arr[0];
    arr[0] = arr[size - 1];
    arr[size - 1] = aux;
    
    ReverseArray(arr + 1, size - 2);
}

/// Copy src row into dst row until it finds EOR
///
/// Its the users job to garantee there is enough space
/// in dst for all of src's content. src isn't modified
static void CopyRLERow(int* dst, const int* src) {
    size_t i = 0;
    while(src[i] != -1) {
        dst[i] = src[i]; i++;
    }
    dst[i] = -1;
}

/// Create the header of an image data structure
/// And allocate the array of pointers to RLE rows
static Image AllocateImageHeader(uint32 width, uint32 height) {
    assert(width > 0 && height > 0);
    Image newHeader = malloc(sizeof(struct image));
    check(newHeader != NULL, "malloc");

    newHeader->width = width;
    newHeader->height = height;

    // Allocating the array of pointers to RLE rows
    newHeader->row = malloc(height * sizeof(int*));
    check(newHeader->row != NULL, "malloc");

    return newHeader;
}

/// Allocate an array to store a RLE row with n elements
static int* AllocateRLERowArray(uint32 n) {
    assert(n > 2);
    int* newArray = malloc(n * sizeof(int));
    check(newArray != NULL, "malloc");

    return newArray;
}

/// Compute the number of runs of a non-compressed (RAW) image row
static uint32 GetNumRunsInRAWRow(uint32 image_width, const uint8* RAW_row) {
    assert(image_width > 0);
    assert(RAW_row != NULL);

    // How many runs?
    uint32 num_runs = 1;
    for (uint32 i = 1; i < image_width; i++) {
        if (RAW_row[i] != RAW_row[i - 1]) {
            num_runs++;
        }
    }

    return num_runs;
}

/// Get the number of runs of a compressed RLE image row
static uint32 GetNumRunsInRLERow(const int* RLE_row) {
    assert(RLE_row != NULL);

    // go through the rle_row until eor is found
    // discard rle_row[0], since it is a pixel color

    uint32 num_runs = 0;
    uint32 i = 1;
    while (RLE_row[i] != EOR) {
        num_runs++;
        i++;
    }

    return num_runs;
}

/// Get the number of elements of an array storing a compressed RLE image row
static uint32 GetSizeRLERowArray(const int* RLE_row) {
    assert(RLE_row != NULL);

    // Go through the array until EOR is found
    uint32 i = 0;
    while (RLE_row[i] != EOR) {
        i++;
    }

    return (i + 1);
}

/// Compress into RLE format a RAW image row
/// Allocates and returns the array storing the image row in RLE format
static int* CompressRow(uint32 image_width, const uint8* RAW_row) {
    assert(image_width > 0);
    assert(RAW_row != NULL);

    // How many runs?
    uint32 num_runs = GetNumRunsInRAWRow(image_width, RAW_row);

    // Allocate the RLE row array
    int* RLE_row = malloc((num_runs + 2) * sizeof(int));
    check(RLE_row != NULL, "malloc");
    
    PIXMEM++;
    // Go through the RAW_row
    RLE_row[0] = (int)RAW_row[0];  // Initial pixel value
    uint32 index = 1;
    int num_pixels = 1;
    for (uint32 i = 1; i < image_width; i++) {
        if (RAW_row[i] != RAW_row[i - 1]) {
            PIXMEM++;
            RLE_row[index++] = num_pixels;
            num_pixels = 0;
        }
        PIXMEM+=2;
        num_pixels++;
        
    }
    RLE_row[index++] = num_pixels;
    RLE_row[index] = EOR;  // Reached the end of the row
    PIXMEM+=2;
    return RLE_row;
}

static uint8* UncompressRow(uint32 image_width, const int* RLE_row) {
    assert(image_width > 0);
    assert(RLE_row != NULL);

    // The uncompressed row
    uint8* row = (uint8*)malloc(image_width * sizeof(uint8));
    check(row != NULL, "malloc");

    // Go through the RLE_row until EOR is found
    PIXMEM++;
    int pixel_value = RLE_row[0];
    uint32 i = 1;
    uint32 dest_i = 0;
    while (RLE_row[i] != EOR) {
        PIXMEM++;
        // For each run
        for (int aux = 0; aux < RLE_row[i]; aux++) {
            row[dest_i++] = (uint8)pixel_value;
            PIXMEM +=2;
        }
        // Next run
        i++;
        pixel_value ^= 1;
    }
    PIXMEM++;

    return row;
}

// Add your auxiliary functions here...

/// Figures out what should be the last pixel of an uncompressed row
/// if it is in its RLE compressed state. row isn't modified.
///
/// Implementation note: when the number of runs is odd the last pixel has the
/// same color as the first one.
int LastPixelRLE(const int* row, const int runs) {
    assert(row != NULL); assert(runs > 0);

    int lpixel = row[0];
    if (runs % 2 == 0) {
        return ! lpixel;
    }
    return lpixel;
}

/// Returns image (chessboard only) size in bytes. img isn't modified
///
/// Implementation note: In a chessboard rows always have the same num of runs
/// per row.
/// DEPRECATED.
int ImageSizeChessBoard(const Image img) {
    assert(img != NULL);
    return ((int) img->height) * (GetSizeRLERowArray(img->row[0])) * (sizeof(int)); 
}

/// Image management functions

/// Create a new BW image, either BLACK or WHITE.
///   width, height : the dimensions of the new image.
///   val: the pixel color (BLACK or WHITE).
/// Requires: width and height must be non-negative, val is either BLACK or
/// WHITE.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageCreate(uint32 width, uint32 height, uint8 val) {
    assert(width > 0 && height > 0);
    assert(val == WHITE || val == BLACK);

    Image newImage = AllocateImageHeader(width, height);

    // All image pixels have the same value
    int pixel_value = (int)val;

    // Creating the image rows, each row has just 1 run of pixels
    // Each row is represented by an array of 3 elements [value,length,EOR]
    for (uint32 i = 0; i < height; i++) {
        newImage->row[i] = AllocateRLERowArray(3);
        newImage->row[i][0] = pixel_value;
        newImage->row[i][1] = (int)width;
        newImage->row[i][2] = EOR;
    }

    return newImage;
}

/// Create a new BW image, with a perfect CHESSBOARD pattern.
///   width, height : the dimensions of the new image.
///   square_edge : the lenght of the edges of the sqares making up the
///   chessboard pattern.
///   first_value: the pixel color (BLACK or WHITE) of the
///   first image pixel.
/// Requires: width and height must be non-negative, val is either BLACK or
/// WHITE.
/// Requires: for the squares, width and height must be multiples of the
/// edge lenght of the squares
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageCreateChessboard(uint32 width, uint32 height, uint32 square_edge,
                            uint8 first_value) {

    // Verificação de argumentos
    assert(width > 0 && height > 0);
    assert(height % square_edge == 0 && width % square_edge == 0);
    assert(first_value == WHITE || first_value == BLACK);

    Image chessImage = AllocateImageHeader(width, height);

    // Number of runs
    uint32 n_runs = width / square_edge;

    // Each line
    for (uint32 i = 0; i < height; i++) {
        uint32 rle_row_size = n_runs + 2;
        chessImage->row[i] = AllocateRLERowArray(rle_row_size);

        // First Value
        if ((i / square_edge) % 2 == 0) {
            chessImage->row[i][0] = first_value;  
        } else {
            
            if (first_value == WHITE) {
                chessImage->row[i][0] = BLACK;
            } else {
                chessImage->row[i][0] = WHITE;
            }
        }

        // Fill the lengths of the squares
        for (uint32 j = 0; j < n_runs; j++) {
            chessImage->row[i][j + 1] = square_edge;
        }

        // The end of the line
        chessImage->row[i][n_runs + 1] = EOR;
    }

    return chessImage;
}

/// Destroy the image pointed to by (*imgp).
///   imgp : address of an Image variable.
/// If (*imgp)==NULL, no operation is performed.
/// Ensures: (*imgp)==NULL.
/// Should never fail.
void ImageDestroy(Image* imgp) {
    assert(imgp != NULL);

    Image img = *imgp;

    for (uint32 i = 0; i < img->height; i++) {
        free(img->row[i]);
    }
    free(img->row);
    free(img);

    *imgp = NULL;
}

/// Printing on the console

/// Output the raw BW image
void ImageRAWPrint(const Image img) {
    assert(img != NULL);

    printf("width = %d height = %d\n", img->width, img->height);
    printf("RAW image:\n");

    // Print the pixels of each image row
    for (uint32 i = 0; i < img->height; i++) {
        // The value of the first pixel in the current row
        int pixel_value = img->row[i][0];
        for (uint32 j = 1; img->row[i][j] != EOR; j++) {
            // Print the current run of pixels
            for (int k = 0; k < img->row[i][j]; k++) {
                printf("%d", pixel_value);
            }
            // Switch (XOR) to the pixel value for the next run, if any
            pixel_value ^= 1;
        }
        // At current row end
        printf("\n");
    }
    printf("\n");
}

/// Output the compressed RLE image
void ImageRLEPrint(const Image img) {
    assert(img != NULL);

    printf("width = %d height = %d\n", img->width, img->height);
    printf("RLE encoding:\n");

    // Print the compressed rows information
    for (uint32 i = 0; i < img->height; i++) {
        uint32 j;
        for (j = 0; img->row[i][j] != EOR; j++) {
            printf("%d ", img->row[i][j]);
        }
        printf("%d\n", img->row[i][j]);
    }
    printf("\n");
}

/// PBM BW file operations

// See PBM format specification: http://netpbm.sourceforge.net/doc/pbm.html

// Auxiliary function
static void unpackBits(int nbytes, const uint8 bytes[], uint8 raw_row[]) {
    // bitmask starts at top bit
    int offset = 0;
    uint8 mask = 1 << (7 - offset);
    while (offset < 8) {  // or (mask > 0)
        for (int b = 0; b < nbytes; b++) {
            raw_row[8 * b + offset] = (bytes[b] & mask) != 0;
        }
        mask >>= 1;
        offset++;
    }
}

// Auxiliary function
static void packBits(int nbytes, uint8 bytes[], const uint8 raw_row[]) {
    // bitmask starts at top bit
    int offset = 0;
    uint8 mask = 1 << (7 - offset);
    while (offset < 8) {  // or (mask > 0)
        for (int b = 0; b < nbytes; b++) {
            if (offset == 0) bytes[b] = 0;
            bytes[b] |= raw_row[8 * b + offset] ? mask : 0;
        }
        mask >>= 1;
        offset++;
    }
}

// Match and skip 0 or more comment lines in file f.
// Comments start with a # and continue until the end-of-line, inclusive.
// Returns the number of comments skipped.
static int skipComments(FILE* f) {
    char c;
    int i = 0;
    while (fscanf(f, "#%*[^\n]%c", &c) == 1 && c == '\n') {
        i++;
    }
    return i;
}

/// Load a raw PBM file.
/// Only binary PBM files are accepted.
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageLoad(const char* filename) {  ///
    int w, h;
    char c;
    FILE* f = NULL;
    Image img = NULL;

    check((f = fopen(filename, "rb")) != NULL, "Open failed");
    // Parse PBM header
    check(fscanf(f, "P%c ", &c) == 1 && c == '4', "Invalid file format");
    skipComments(f);
    check(fscanf(f, "%d ", &w) == 1 && w >= 0, "Invalid width");
    skipComments(f);
    check(fscanf(f, "%d", &h) == 1 && h >= 0, "Invalid height");
    check(fscanf(f, "%c", &c) == 1 && isspace(c), "Whitespace expected");

    // Allocate image
    img = AllocateImageHeader(w, h);

    // Read pixels
    int nbytes = (w + 8 - 1) / 8;  // number of bytes for each row
    // using VLAs...
    uint8 bytes[nbytes];
    uint8 raw_row[nbytes * 8];
    for (uint32 i = 0; i < img->height; i++) {
        check(fread(bytes, sizeof(uint8), nbytes, f) == (size_t)nbytes,
              "Reading pixels");
        unpackBits(nbytes, bytes, raw_row);
        img->row[i] = CompressRow(w, raw_row);
    }

    fclose(f);
    return img;
}

/// Save image to PBM file.
/// On success, returns unspecified integer. (No need to check!)
/// On failure, does not return, EXITS program!
int ImageSave(const Image img, const char* filename) {  ///
    assert(img != NULL);
    int w = img->width;
    int h = img->height;
    FILE* f = NULL;

    check((f = fopen(filename, "wb")) != NULL, "Open failed");
    check(fprintf(f, "P4\n%d %d\n", w, h) > 0, "Writing header failed");

  // Write pixels
  int nbytes = (w + 8 - 1) / 8;  // number of bytes for each row
  // using VLAs...
  uint8 bytes[nbytes];
  // unit8 raw_row[nbytes*8];
  for (uint32 i = 0; i < img->height; i++) {
    // UncompressRow...
    uint8* raw_row = UncompressRow(nbytes * 8, img->row[i]);
    // Fill padding pixels with WHITE
    memset(raw_row + w, WHITE, nbytes * 8 - w);
    packBits(nbytes, bytes, raw_row);
    size_t written = fwrite(bytes, sizeof(uint8), nbytes, f);
    check(written == (size_t)nbytes, "Writing pixels failed");
    free(raw_row);
  }

    // Cleanup
    fclose(f);
    return 0;
}

/// Information queries

/// Get image width
int ImageWidth(const Image img) {
    assert(img != NULL);
    return img->width;
}

/// Get image height
int ImageHeight(const Image img) {
    assert(img != NULL);
    return img->height;
}

/// Get size in bytes occupied by img
int ImageSize(const Image img) {
    int size = 0;
    
    for (uint32 i = 0; i < img->height; i++) {
        size += (GetSizeRLERowArray(img->row[i]) + 2) * sizeof(int);
    }

    return size; 
};

/// Image comparison

int ImageIsEqual(const Image img1, const Image img2) {
    assert(img1 != NULL && img2 != NULL);

    // Check if both images are valid
    assert(img1 != NULL && img2 != NULL);

    // Check if dimensions are equal
    if (img1->width != img2->width || img1->height != img2->height) {
        return 0;  // Images with different dimensions are not equal
    }

    // Check the content row by row
    for (uint32 i = 0; i < img1->height; i++) {
        const int* row1 = img1->row[i];
        const int* row2 = img2->row[i];

        // Check if the RLE arrays are identical
        uint32 j = 0;
        while (row1[j] != EOR && row2[j] != EOR) {
            if (row1[j] != row2[j]) {
                return 0;  // Found a difference
            }
            j++;
        }

        // Check if both reached EOR at the same time
        if (row1[j] != EOR || row2[j] != EOR) {
            return 0;  // One ended before the other
        }
    }

    // If all checks passed, the images are equal
    return 1;
}

int ImageIsDifferent(const Image img1, const Image img2) {
    assert(img1 != NULL && img2 != NULL);
    return !ImageIsEqual(img1, img2);
}

/// Boolean Operations on image pixels

/// These functions apply boolean operations to images,
/// returning a new image as a result.
///
/// Operand images are left untouched and must be of the same size.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)

Image ImageNEG(const Image img) {
    assert(img != NULL);

    uint32 width = img->width;
    uint32 height = img->height;

    Image newImage = AllocateImageHeader(width, height);

    // Directly copying the rows, one by one
    // And changing the value of row[i][0]

    for (uint32 i = 0; i < height; i++) {
        uint32 num_elems = GetSizeRLERowArray(img->row[i]);
        newImage->row[i] = AllocateRLERowArray(num_elems);
        memcpy(newImage->row[i], img->row[i], num_elems * sizeof(int));
        newImage->row[i][0] ^= 1;  // Just negate the value of the first pixel run
    }

    return newImage;
}

Image ImageAND(const Image img1, const Image img2) {
    assert(img1 != NULL && img2 != NULL);

    // Check if the dimensions of the images are equal
    assert(img1->width == img2->width && img1->height == img2->height);

    // Allocate a new image to store the result
    Image result = AllocateImageHeader(img1->width, img1->height);

    // Iterate through each row of the images
    for (uint32 i = 0; i < img1->height; i++) {
        // Uncompress the rows of the images
        uint8* raw_row1 = UncompressRow(img1->width, img1->row[i]);
        uint8* raw_row2 = UncompressRow(img2->width, img2->row[i]);

        // Allocate a RAW row for the result
        uint8* raw_result_row = malloc(img1->width * sizeof(uint8));
        assert(raw_result_row != NULL);

        // Perform the AND operation pixel by pixel
        for (uint32 j = 0; j < img1->width; j++) {
            raw_result_row[j] = raw_row1[j] & raw_row2[j];
            PIXMEM+=3;
            BOOL_OP++;  // Increment boolean operation counter for each AND
        }

        // Compress the resulting row to RLE format
        result->row[i] = CompressRow(img1->width, raw_result_row);
        

        // Free the temporary RAW rows
        free(raw_row1);
        free(raw_row2);
        free(raw_result_row);
    }

    return result;
}


Image ImageAND2(const Image img1, const Image img2) {
    assert(img1 != NULL && img2 != NULL);
    assert(img1->width == img2->width && img1->height == img2->height);

    Image result = AllocateImageHeader(img1->width, img1->height);

    for (uint32 i = 0; i < img1->height; i++) {
        const int* row1 = img1->row[i];
        const int* row2 = img2->row[i];

        // Allocate space for the result row
        int max_size = GetNumRunsInRLERow(row1) + GetNumRunsInRLERow(row2)+2;
        int* result_row = AllocateRLERowArray(max_size);
        
        PIXMEM+=4;
        int res_index = 0;
        int color1 = row1[0], color2 = row2[0];
        int run1 = row1[1], run2 = row2[1];
        int idx1 = 2, idx2 = 2;

        PIXMEM++;
        BOOL_OP++;
        // Determine the first color of the result row
        int result_color = color1 & color2;
        result_row[res_index++] = result_color;

        while (run1 > 0 || run2 > 0) {
            BOOL_OP++;
            int min_run;
            if (run1 < run2) {
                min_run = run1;
            } else {
                min_run = run2;
            }
            
            BOOL_OP++;
            // Perform AND operation between the current colors
            int current_color = color1 && color2;

            BOOL_OP++;
            // Append the run length to the result
            if (result_color !=current_color  || res_index == 1){

              result_row[res_index++] = min_run;

              result_color = current_color;

            }else{

              result_row[res_index-1] += min_run;

            }

            // Decrease runs by the minimum run length
            run1 -= min_run;
            run2 -= min_run;

            BOOL_OP++;
            // Move to the next run in row1, if necessary
            if (run1 == 0 && row1[idx1] != EOR) {
                color1 ^= 1; // Alternate pixel color
                run1 = row1[idx1++];
                PIXMEM+=2;
            }
            
            BOOL_OP++;
            // Move to the next run in row2, if necessary
            if (run2 == 0 && row2[idx2] != EOR) {
                color2 ^= 1; // Alternate pixel color 
                run2 = row2[idx2++];
                PIXMEM+=2;
            }
        }

        PIXMEM++;
        // Mark the end of the result row
        result_row[res_index++] = EOR;
        result->row[i] = result_row;
    }

    return result;
}


Image ImageOR(const Image img1, const Image img2) {
    assert(img1 != NULL && img2 != NULL);

    // Check if the dimensions of the images are equal
    assert(img1->width == img2->width && img1->height == img2->height);

    // Allocate a new image to store the result
    Image result = AllocateImageHeader(img1->width, img1->height);

    // Iterate through each row of the images
    for (uint32 i = 0; i < img1->height; i++) {
        // Uncompress the rows of the images
        uint8* raw_row1 = UncompressRow(img1->width, img1->row[i]);
        uint8* raw_row2 = UncompressRow(img2->width, img2->row[i]);

        // Allocate a RAW row for the result
        uint8* raw_result_row = malloc(img1->width * sizeof(uint8));
        assert(raw_result_row != NULL);

        // Perform the OR operation pixel by pixel
        for (uint32 j = 0; j < img1->width; j++) {
            raw_result_row[j] = raw_row1[j] | raw_row2[j];
        }

        // Compress the resulting row to RLE format
        result->row[i] = CompressRow(img1->width, raw_result_row);

        // Free the temporary RAW rows
        free(raw_row1);
        free(raw_row2);
        free(raw_result_row);
    }

    return result;
}


Image ImageXOR(const Image img1, const Image img2) {
    assert(img1 != NULL && img2 != NULL);

    // Check if the dimensions of the images are equal
    assert(img1->width == img2->width && img1->height == img2->height);

    // Allocate a new image to store the result
    Image result = AllocateImageHeader(img1->width, img1->height);

    // Iterate through each row of the images
    for (uint32 i = 0; i < img1->height; i++) {
        // Uncompress the rows of the images
        uint8* raw_row1 = UncompressRow(img1->width, img1->row[i]);
        uint8* raw_row2 = UncompressRow(img2->width, img2->row[i]);

        // Allocate a RAW row for the result
        uint8* raw_result_row = malloc(img1->width * sizeof(uint8));
        assert(raw_result_row != NULL);

        // Perform the XOR operation pixel by pixel
        for (uint32 j = 0; j < img1->width; j++) {
            raw_result_row[j] = raw_row1[j] ^ raw_row2[j];
        }

        // Compress the resulting row to RLE format
        result->row[i] = CompressRow(img1->width, raw_result_row);

        // Free the temporary RAW rows
        free(raw_row1);
        free(raw_row2);
        free(raw_result_row);
    }

    return result;
}


/// Geometric transformations

/// These functions apply geometric transformations to an image,
/// returning a new image as a result.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)

/// Mirror an image = flip top-bottom.
/// Returns a mirrored version of the image.
/// Ensures: The original img is not modified.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageHorizontalMirror(const Image img) {
    assert(img != NULL);

    uint32 width = img->width;
    uint32 height = img->height;

    Image newImage = AllocateImageHeader(width, height);
    
    int* newRow;
    uint32 size;
    for (uint32 i = 0; i < height; i++) {
        //Get row size
        size = GetSizeRLERowArray(img->row[i]); 
        
        //Allocate row
        newRow = AllocateRLERowArray(size);
        
        //Copy row
        CopyRLERow(newRow, img->row[i]);
        
        //Update newImage row pointer
        newImage->row[height - (i + 1)] = newRow;
    }

    return newImage;
}

/// Mirror an image = flip left-right.
/// Returns a mirrored version of the image.
/// Ensures: The original img is not modified.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageVerticalMirror(const Image img) {
    assert(img != NULL);

    uint32 width = img->width;
    uint32 height = img->height;

    Image newImage = AllocateImageHeader(width, height);

    for (uint32 i = 0; i < height; i++) {
        int* row = img->row[i];
        uint32 rowSize = GetSizeRLERowArray(row);
        uint32 runs = rowSize - 2;
        
        //Allocate row
        int* newRow = AllocateRLERowArray(rowSize);
        
        //Copy row
        CopyRLERow(newRow, img->row[i]);

        //Reverse runs 
        ReverseArray(&newRow[1], (size_t) runs);
        
        //Flip first pixel if necessary
        if (LastPixelRLE(row, runs) != row[0]) {
            newRow[0] = ! row[0]; // ! 0 = 1
        }

        //Update newImage row pointer
        newImage->row[i] = newRow;
    }

    return newImage;
}

/// Replicate img2 at the bottom of imag1, creating a larger image
/// Requires: the width of the two images must be the same.
/// Returns the new larger image.
/// Ensures: The original images are not modified.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageReplicateAtBottom(const Image img1, const Image img2) {
    assert(img1 != NULL && img2 != NULL);
    assert(img1->width == img2->width);

    uint32 new_width = img1->width;
    uint32 new_height = img1->height + img2->height;

    Image newImage = AllocateImageHeader(new_width, new_height);
    
    uint32 i;
    uint32 rowSize;
    int* newRow;
    int* row;
    for (i = 0; i < img1->height ; i++) {
        row = img1->row[i];
        rowSize = GetSizeRLERowArray(row);
        
        //Allocate row
        newRow = AllocateRLERowArray(rowSize);

        //Copy row
        CopyRLERow(newRow, img1->row[i]);
        
        newImage->row[i] = newRow;
    }
    
    for (i = 0; i < img2->height; i++) {
        row = img2->row[i];
        rowSize = GetSizeRLERowArray(row);
        
        //Allocate row
        newRow = AllocateRLERowArray(rowSize);

        //Copy row
        CopyRLERow(newRow, img2->row[i]);
        
        newImage->row[i + img1->height] = newRow;
    }

    return newImage;
}

/// Replicate img2 to the right of imag1, creating a larger image
/// Requires: the height of the two images must be the same.
/// Returns the new larger image.
/// Ensures: The original images are not modified.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageReplicateAtRight(const Image img1, const Image img2) {
    assert(img1 != NULL && img2 != NULL);
    assert(img1->height == img2->height);

    uint32 new_width = img1->width + img2->width;
    uint32 new_height = img1->height;

    Image newImage = AllocateImageHeader(new_width, new_height);
    
    for (uint32 i = 0; i < new_height; i++) {
        
        uint32 numRuns1 = GetNumRunsInRLERow(img1->row[i]);
        uint32 numRuns2 = GetNumRunsInRLERow(img2->row[i]);
        uint32 numRunsNew = numRuns1 + numRuns2;

        int* row1 = img1->row[i];
        int* row2 = img2->row[i];
        
        int joinRuns = LastPixelRLE(row1, numRuns1) == row2[0]; //Bool
        if (joinRuns) numRunsNew--;

        // Allocate row
        int* newRow = AllocateRLERowArray(numRunsNew + 2);
        
        CopyRLERow(newRow, row1);
        if (joinRuns) {
            // Sum last run of 1st row with 1st run of last row
            newRow[numRuns1] += row2[1]; 
            CopyRLERow(&newRow[numRuns1 + 1], &row2[2]);
        } else {
            CopyRLERow(&newRow[numRuns1 + 1], &row2[1]);
        }
        
        newImage->row[i] = newRow;
    }

    return newImage;
}
