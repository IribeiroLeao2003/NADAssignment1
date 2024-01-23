/*
* FILE :            main.c
* PROJECT :         A-02 : WRITING A LINUX UTILITY
* PROGRAMMER :      Isaac Ribeiro Leao and Vivian Morton
* FIRST VERSION :   2024 - 01 - 23
* DESCRIPTION :     TBA
*/


#include <stdio.h>
#include <string.h>
#include "encodeInput.h"

int main(int argc, char **argv){ 
    char* input; 
    while(1){ 
        if(scanf("%s", input) =! 1){ 
            printf("Please Input something"); 
            continue;

        } else { 
            if(strcmp(argv[1], "encodeInput") != 0 ){  
                printf("Input must include encodeInput in it"); 
                continue;

            } 
            else{ 
                if (strcmp(argv[1], "-h") == 0 ){ 
                    ProgramUsage();  
                    continue; 
                } 
                // parse for -i 
                //parse for -o 
                // parse for -srec
                
                
            }
        }



    }
    
        // -iInputFileName 
        // -oOutputFIleName  
            // If srec is present then the file will end in .srec 
            // if not the file will end in .asm 
        // -srec to output in the S-Record format
        // -h for a help screen 
} 

void ProgramUsage(){ 
    printf("Encode Input Correct Usage \n Type -h for help \n\t Type -iInputFileName to set input file name \n\t Type -src beside your file name to encode the file into a .srec \n\t Dont type -srec to turn input file into a assembly file\n\t"); 
   
}