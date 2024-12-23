#include "imageBW.h"
#include "instrumentation.h"
#include "stdlib.h"
#include "stdio.h"

int power(int b, int e) {
    if (e <= 0) return 1;

    return b * power(b, e - 1);
}

int main(int argc, char* argv[])
{
    
    if (argc != 2) {
        printf("INVALID SINTAX");
        return EXIT_FAILURE;
    }

    int edge = 0;
    int BEST = 0;
    switch (atoi(argv[1])) {
        case 1: { //WORST CASE
            edge = 1;
            break;
        }
        case 2: { //AVERAGE CASE
            edge = 2; 
            break;
        }
        case 3: { //BEST CASE
            BEST = 1;
            break;
        }
        default: {
            printf("INVALID MODE\n");
            return EXIT_FAILURE;
        }
    }
    
    ImageInit();
    
    Image img1;
    int height = 0;
    int width = 0;
    for (int i = 1; i < 1000 ; i++) {
        
        width = 2 * i; 
        height = 2 * i;

        if (BEST == 1) {
            img1 = ImageCreate(width, height, BLACK);
        } else {
            img1 = ImageCreateChessboard(width, height, edge, BLACK);
        }

        InstrReset();
        Image img2 = ImageAND(img1, img1);
        InstrPrintTest(width, 1);
        InstrReset();
        Image img3 = ImageAND2(img1, img1);
        InstrPrintTest(width, 2);

        ImageDestroy(&img1);
        ImageDestroy(&img2);
        ImageDestroy(&img3);
    }

    return EXIT_SUCCESS;
}
