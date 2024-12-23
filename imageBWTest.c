// imageBWTest - A program that performs some image processing.
//
// This program is an example use of the imageBW module,
// a programming project for the course AED, DETI / UA.PT
//
// You may freely use and modify this code, NO WARRANTY, blah blah,
// as long as you give proper credit to the original and subsequent authors.
//
// The AED Team <jmadeira@ua.pt, jmr@ua.pt, ...>
// 2024

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "imageBW.h"
#include "instrumentation.h"

// Declaração explícita dos contadores
extern unsigned long InstrCount[];  // Declaração para acessar os contadores
#define PIXMEM InstrCount[0]
#define BOOL_OP InstrCount[1]
#define RLEMEM InstrCount[2]

typedef struct image* Image; 

void TestImage(const Image img) {
    printf("RAW representation:\n");
    ImageRAWPrint(img);
    printf("RLE representation:\n");
    ImageRLEPrint(img);
}

void PerformanceTest() {

    Image teste1 = ImageCreateChessboard(24, 12, 6, WHITE);
    Image teste2 = ImageCreateChessboard(24, 12, 4, BLACK);
    Image teste4=ImageAND2(teste1, teste2);
    Image teste3=ImageAND(teste1, teste2);
    TestImage(teste1);
    TestImage(teste2);
    TestImage(teste4);
    TestImage(teste3);

    printf("Performance Test:\n");
    printf("+---------------+---------------------------+---------------------------+\n");
    printf("| Image Size    | Pixel Accesses (PIXMEM)  | Boolean Ops (BOOL_OP)     |\n");
    printf("+---------------+---------------------------+---------------------------+\n");

    // Test for multiple sizes
    uint32 sizes[] = {8, 16, 16, 32, 32, 64, 64, 128, 128, 256,256,512,512,1024,1024 };
    uint32 sizes2[] = {8, 8, 16, 16, 32, 32, 64, 64, 128, 128,256,256,512,512,1024};
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        uint32 size = sizes[i];
        uint32 size2=sizes2[i];
        Image img1 = ImageCreateChessboard(size, size2, 2, BLACK);
        Image img2 = ImageCreateChessboard(size, size2, 2, WHITE);

        PIXMEM = 0; // Reset pixel memory accesses counter
        BOOL_OP = 0; // Reset boolean operations counter
        
        Image and_result = ImageAND(img1, img2);

        printf("| %4dx%-6d   | %25lu | %25lu |\n", size, size2, PIXMEM, BOOL_OP);

        ImageDestroy(&img1);
        ImageDestroy(&img2);
        ImageDestroy(&and_result);
    }
    printf("+---------------+---------------------------+---------------------------+\n");
    
    ImageDestroy(&teste1);
    ImageDestroy(&teste2);
    ImageDestroy(&teste3);
    ImageDestroy(&teste4);

}
void WorstCaseAnd() {
    printf("Performance Test:WORST CASE AND\n");
    printf("+---------------+---------------------------+---------------------------+\n");
    printf("| Image Size    | Pixel Accesses (PIXMEM)  | Boolean Ops (BOOL_OP)     |\n");
    printf("+---------------+---------------------------+---------------------------+\n");

    // Test for multiple sizes
    uint32 sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024};
    uint32 sizes2[] = {8, 16, 32, 64, 128, 256, 512, 1024};
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        uint32 size = sizes[i];
        uint32 size2=sizes2[i];
        Image img1 = ImageCreateChessboard(size, size2, 1, BLACK);
        Image img2 = ImageCreateChessboard(size, size2, 1, WHITE);

        PIXMEM = 0; // Reset pixel memory accesses counter
        BOOL_OP = 0; // Reset boolean operations counter
        
        Image and_result = ImageAND(img1, img2);

        printf("| %4dx%-6d   | %25lu | %25lu |\n", size, size2, PIXMEM, BOOL_OP);

        ImageDestroy(&img1);
        ImageDestroy(&img2);
        ImageDestroy(&and_result);
    }
    printf("+---------------+---------------------------+---------------------------+\n");
}
void BestCaseAnd() {
    printf("Performance Test:BEST CASE AND\n");
    printf("+---------------+---------------------------+---------------------------+\n");
    printf("| Image Size    | Pixel Accesses (PIXMEM)  | Boolean Ops (BOOL_OP)     |\n");
    printf("+---------------+---------------------------+---------------------------+\n");

    // Test for multiple sizes
    uint32 sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024};
    uint32 sizes2[] = {8, 16, 32, 64, 128, 256, 512, 1024};
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        uint32 size = sizes[i];
        uint32 size2=sizes2[i];
        Image img1 = ImageCreateChessboard(size, size2, size, BLACK);
        Image img2 = ImageCreateChessboard(size, size2, size, WHITE);

        PIXMEM = 0; // Reset pixel memory accesses counter
        BOOL_OP = 0; // Reset boolean operations counter
        
        Image and_result = ImageAND(img1, img2);

        printf("| %4dx%-6d   | %25lu | %25lu |\n", size, size2, PIXMEM, BOOL_OP);

        ImageDestroy(&img1);
        ImageDestroy(&img2);
        ImageDestroy(&and_result);
    }
    printf("+---------------+---------------------------+---------------------------+\n");
}

void AverageCaseAnd() {
    printf("Performance Test:Average CASE AND\n");
    printf("+---------------+---------------------------+---------------------------+\n");
    printf("| Image Size    | Pixel Accesses (PIXMEM)  | Boolean Ops (BOOL_OP)     |\n");
    printf("+---------------+---------------------------+---------------------------+\n");

    // Test for multiple sizes
    uint32 sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024};
    uint32 sizes2[] = {8, 16, 32, 64, 128, 256, 512, 1024};
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        uint32 size = sizes[i];
        uint32 size2=sizes2[i];
        Image img1 = ImageCreateChessboard(size, size2, 2, BLACK);
        Image img2 = ImageCreateChessboard(size, size2, 2, WHITE);

        PIXMEM = 0; // Reset pixel memory accesses counter
        BOOL_OP = 0; // Reset boolean operations counter
        
        Image and_result = ImageAND(img1, img2);

        printf("| %4dx%-6d   | %25lu | %25lu |\n", size, size2, PIXMEM, BOOL_OP);

        ImageDestroy(&img1);
        ImageDestroy(&img2);
        ImageDestroy(&and_result);
    }
    printf("+---------------+---------------------------+---------------------------+\n");
}

void WORSTCASE2() {
    printf("Performance Test: WORST CASE AND2\n");
    printf("+---------------+---------------------------+---------------------------+\n");
    printf("| Image Size    |         RLE ACCESS        |                            |\n");
    printf("+---------------+---------------------------+---------------------------+\n");

    // Test for multiple sizes
    uint32 sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024};
    uint32 sizes2[] = {8, 16, 32, 64, 128, 256, 512, 1024};
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        uint32 size = sizes[i];
        uint32 size2=sizes2[i];
        Image img1 = ImageCreateChessboard(size, size2, 1, BLACK);
        Image img2 = ImageCreateChessboard(size, size2, 1, WHITE);

        PIXMEM = 0; // Reset pixel memory accesses counter
        BOOL_OP = 0; // Reset boolean operations counter
        RLEMEM =0;

        Image and_result = ImageAND2(img1, img2);

        printf("| %4dx%-6d   | %25lu | %25lu \n", size, size2,RLEMEM, BOOL_OP);

        ImageDestroy(&img1);
        ImageDestroy(&img2);
        ImageDestroy(&and_result);
    }
    printf("+---------------+---------------------------+---------------------------+\n");
}
void BestCASE2() {
    printf("Performance Test: Best CASE AND2\n");
    printf("+---------------+---------------------------+---------------------------+\n");
    printf("| Image Size    |         RLE ACCESS        |                            |\n");
    printf("+---------------+---------------------------+---------------------------+\n");

    // Test for multiple sizes
    uint32 sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024};
    uint32 sizes2[] = {8, 16, 32, 64, 128, 256, 512, 1024};
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        uint32 size = sizes[i];
        uint32 size2=sizes2[i];
        Image img1 = ImageCreateChessboard(size, size2, size, BLACK);
        Image img2 = ImageCreateChessboard(size, size2, size, WHITE);

        PIXMEM = 0; // Reset pixel memory accesses counter
        BOOL_OP = 0; // Reset boolean operations counter
        RLEMEM =0;

        Image and_result = ImageAND2(img1, img2);

        printf("| %4dx%-6d   | %25lu | %25lu \n", size, size2,RLEMEM, BOOL_OP);

        ImageDestroy(&img1);
        ImageDestroy(&img2);
        ImageDestroy(&and_result);
    }
    printf("+---------------+---------------------------+---------------------------+\n");
}
void AverageCASE2() {
    printf("Performance Test: Average CASE AND2\n");
    printf("+---------------+---------------------------+---------------------------+\n");
    printf("| Image Size    |         RLE ACCESS        |                            |\n");
    printf("+---------------+---------------------------+---------------------------+\n");

    // Test for multiple sizes
    uint32 sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024};
    uint32 sizes2[] = {8, 16, 32, 64, 128, 256, 512, 1024};
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        uint32 size = sizes[i];
        uint32 size2=sizes2[i];
        Image img1 = ImageCreateChessboard(size, size2, 2, BLACK);
        Image img2 = ImageCreateChessboard(size, size2, 2, WHITE);

        PIXMEM = 0; // Reset pixel memory accesses counter
        BOOL_OP = 0; // Reset boolean operations counter
        RLEMEM =0;

        Image and_result = ImageAND2(img1, img2);

        printf("| %4dx%-6d   | %25lu | %25lu \n", size, size2,RLEMEM, BOOL_OP);

        ImageDestroy(&img1);
        ImageDestroy(&img2);
        ImageDestroy(&and_result);
    }
    printf("+---------------+---------------------------+---------------------------+\n");
}

int main(int argc, char* argv[]) {
    if (argc != 1) {
        fprintf(stderr, "Usage: %s  # no arguments required (for now)\n", argv[0]);
        exit(1);
    }

    // Initialize operation counters
    ImageInit();

    // Performance Test
    PerformanceTest();
    BestCaseAnd();
    BestCASE2();
    AverageCaseAnd();
    AverageCASE2();
    WorstCaseAnd();
    WORSTCASE2();

    int w = 1000;
    int h = 1000;
    Image img1 = ImageCreate(w, h, BLACK);
    Image img2 = ImageCreateChessboard(w, h, 2, BLACK);
    Image img3 = ImageReplicateAtRight(img2, img1);
    Image img4 = ImageReplicateAtBottom(img3, img3);
    Image img5 = ImageHorizontalMirror(img1);
    Image img6 = ImageVerticalMirror(img1);
    Image img7 = ImageNEG(img2);
    Image img8 = ImageAND(img1, img2);
    Image img9 = ImageAND2(img1, img2);
    Image img10 = ImageOR(img1, img2);
    Image img11 = ImageXOR(img1, img2);

    ImageDestroy(&img1);
    ImageDestroy(&img2);
    ImageDestroy(&img3);
    ImageDestroy(&img4);
    ImageDestroy(&img5);
    ImageDestroy(&img6);
    ImageDestroy(&img7);
    ImageDestroy(&img8);
    ImageDestroy(&img9);
    ImageDestroy(&img10);
    ImageDestroy(&img11);

    return 0;
}
