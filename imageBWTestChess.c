#include "imageBW.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/// The following code was taken from imageBW.c

typedef struct image* Image;

struct image {
    uint32 width;
    uint32 height;
    int** row; 
};

static uint32 getNumRuns(const int* RLE_row) {
    assert(RLE_row != NULL);
    
    uint32 num_runs = 0;
    uint32 i = 1;
    while (RLE_row[i] != -1) {
        num_runs++;
        i++;
    }

    return num_runs;
}

/// Original code for testing space used by chessboard pattern images, 
/// and their number of runs
int main(void)
{
    Image img;
    int size = 16;
    int edge = 8;
    printf("EXPERIMENTALLY:\n");
    printf("+-- Size(B) --+--- Runs ---+-- Side --+-- Edge --+\n");
    while (size <= 16384) {
        img = ImageCreateChessboard(size, size, edge, BLACK);

        printf("|%13d|%12d|%10d|%10d|\n",
        ImageSize(img),
        getNumRuns(img->row[0]) * img->height,
        img->height,
        edge
        );

        ImageDestroy(&img);

        size *= 2;
    };
   
    edge = 2;
    size = 4096;
    printf("+-------------+------------+----------+----------+\n");
    while (edge <= 2048) {
        img = ImageCreateChessboard(size, size, edge, BLACK);

        printf("|%13d|%12d|%10d|%10d|\n",
        ImageSize(img),
        getNumRuns(img->row[0]) * img->height,
        img->height,
        edge
        );

        ImageDestroy(&img);

        edge *= 2;
    };
    printf("+-------------+------------+----------+----------+\n");

    printf("\n\nRuns = height * (width/edge)\n");
    printf("Where: height - height of image\n");
    printf("       width - width of image\n");
    printf("       edge - edge of the squares in chess patern\n");
    printf("\nNote: In this case we have width == height == 'Side'\n\n");
    
    printf("Size = (Runs + 2 * height) * sizeof(int) (B)");
    printf("Where: height - height of image\n");
    printf("       Runs - num of runs per RLE row\n");
    printf("       sizeof(int) - constant of value 4B\n");
    printf("\nNote: Adding 2 * height to the num of runs acounts for space\n");
    printf("used by first value and EOR\n\n");

    printf("Theoretically the max number of runs in a chessboard pattern would be\n");
    printf("when the edge of each square is 1, in that case the number of runs\n");
    printf("(using the formula already derived previously): Runs= width * height.\n");
    printf("In the other hand, having a square edge of half of the chessboard's length\n");
    printf("would minimize the number of runs\n");
    printf("and then the expression would be: Runs = height * 2\n");

    return EXIT_SUCCESS;
}
