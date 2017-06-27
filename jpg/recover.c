/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t  BYTE;

int main(void)
{
    // open file to recover
    FILE* pftr = fopen("card.raw", "r"); 
    if(pftr == NULL){
        printf("Could not open 'card.raw'.\n");
        return 2;
    }
   
    // output filenames initialization
    int filename_counter = -1;
    char filename[8]; 
    
    // read in blocks of 512 bytes and write to .jpg-files
    do{
        BYTE block_buffer[512];
        fread(&block_buffer, (512 * sizeof(BYTE)), 1, pftr);
        
        //check for end of file
        if(feof(pftr)){
            //printf("The complete file has been checked.\n");
            fclose(pftr);
            break;
        }
        
        if(block_buffer[0] == 0xff && block_buffer[1] == 0xd8 && block_buffer[2] == 0xff && block_buffer[3] >= 0xe0 && block_buffer[3] <= 0xef){
            // create new filename
            filename_counter++;
            if(filename_counter > 999){
                printf("The maximum number (999) of files is reached");
                break;
            }    
            snprintf(filename, 8, "%03d.jpg", filename_counter);       
        }    
        
        if(filename_counter >= 0){
            FILE* outptr = fopen(filename, "a");
            fwrite(&block_buffer, (512 * sizeof(BYTE)), 1, outptr);
            fclose(outptr);
        }    
    }while(1);

return 0;    
}
