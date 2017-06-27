/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Copies a BMP and resizes it by a factor given by the user.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize resizefactor infile outfile\n");
        return 1; //input error
    }

    // remember filenames
    int n = atoi(argv[1]);
    char* infile = argv[2];
    char* outfile = argv[3];
    
    if(n < 1 || n > 100){
        printf("Please enter a resize factor of 1 - 100");
        return 1; //input error
    }
    

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    //store values of Width and Height of original and calculate of new image
    int Orig_biWidth = bi.biWidth;
    int Orig_biHeight = bi.biHeight;
    bi.biWidth *= n;
    bi.biHeight *= n;

    // Determine padding of original file and new file for scanlines
    int Orig_padding =  (4 - (Orig_biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    int Orig_scanline_size = (Orig_biWidth * sizeof(RGBTRIPLE)) + Orig_padding;
    //int scanline_size = bi.biWidth + padding;

    // make required changes for new values in HEADERS
    bi.biSizeImage = ((bi.biWidth * sizeof(RGBTRIPLE)) + padding) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + 54;

    //write outfile's BITMAPFILEHEADER & BITMAPINFOHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(Orig_biHeight); i < biHeight; i++)
    {
        //iterate over outfile's scanlines 
        for (int z = 0; z < n; z++)
        {
            // reposition to beginning of scanline until last iteration
            if(z != 0)
            {
                fseek(inptr, -1*(Orig_scanline_size), SEEK_CUR);
            }
            
            // iterate over pixels in scanline
            for (int j = 0; j < Orig_biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;
        
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                for (int q = 0; q < n; q++){
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
                
            }
        
            // skip over padding, if any
            fseek(inptr, Orig_padding, SEEK_CUR);
        
            // then add it back (to demonstrate how)
            for (int k = 0; k < padding; k++)
            {
                fputc(0x00, outptr);
            }

            
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
