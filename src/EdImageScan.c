#include <stdio.h>
#include <stdlib.h>
#include "../libs/EdImageStruct.h"
// #include "../../libs/ImageStruct.h"

void decodageMCU(FILE *file,EdImage* image)
{
    if (!file)
    {
        fprintf(stderr, "Introuvable\n");
        exit(EXIT_FAILURE);
    }
    unsigned char bit, marker;
    while (fread(&bit,1,1,file))
    {
        if(bit == 0xFF){
            if(!fread(&marker,1,1,file)){
                printf("lecture fini");
                exit(1);
            }
            if(marker == 0xDB){
                unsigned short lenght = LENGTH(file) - 2 ;
                printf("lenght de findquantifier : %u\n",lenght);
            }
        }
    }
    
}

EdImage *findResolution(FILE *file)
{
    if (!file)
    {
        fprintf(stderr, "Introuvable\n");
        exit(EXIT_FAILURE);
    }

    EdImage *image = malloc(sizeof(EdImage));
    if (!image)
    {
        fprintf(stderr, "Erreur allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    unsigned char bit, marker;

    while (fread(&bit, 1, 1, file) == 1)
    {
        if (bit == 0xFF)
        {
            if (fread(&marker, 1, 1, file) != 1)
                break;

            printf("Marker : %02X\n", marker);

            if (marker == 0xC0 || marker == 0xC2)
            {
                int b1, b2;

                int length = LENGTH(file);
                (void)length; // si pas utilisé

                int precision = fgetc(file);
                (void)precision;

                b1 = fgetc(file);
                b2 = fgetc(file);
                if (b1 == EOF || b2 == EOF)
                    break;
                image->height = (b1 << 8) | b2;

                b1 = fgetc(file);
                b2 = fgetc(file);
                if (b1 == EOF || b2 == EOF)
                    break;
                image->width = (b1 << 8) | b2;

                image->channel = fgetc(file);

                break;
            }
        }
    }
    // free(image);
    return image;
}