/**
 * @author
 * @date 13 Mai 2026
 * @brief
 */

#ifndef EDIMAGE_H
#define EDIMAGE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define LENGTH(XX) fgetc(XX)<<8 |fgetc(XX) 


typedef struct EdImage
{
    int height,  // Champs hauteur de l'image
        width,   //  Champs largeur de l'image
        channel; //   Champs RGB/YUA de l'image

    uint8_t *pixels; // Pointeur sur nos pixels compressés

} EdImage;

typedef struct QuantizationTable
{
    int dest; // Taille total de notre matrix X
    uint8_t *matrix;
    /* data */
}QuantizationTable;

/**
 * @brief Définir le chams de résolution de notre image JPEG ( height,width, channel)
 */
EdImage *findResolution(FILE *f);

/** 
 * @brief Définir la table de décodage de notre image JPEG ( Zone DB)
 */
void decodageMCU(FILE *file,EdImage* image);
#endif