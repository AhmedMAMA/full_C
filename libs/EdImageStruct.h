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

#define LENGTH(XX) fgetc(XX) << 8 | fgetc(XX)

/**
 * @brief Permettre d'extraction de la table de quantification des images JPEG.
 */

typedef struct QuantizationTable
{
    uint8_t precision;
    uint8_t id;
    uint16_t values[64];
} QuantizationTable;

/**
 * @brief Permettre l'extaction de tous nos données issues des images JPEG
 */
typedef struct EdImage
{
    int height,  // Champs hauteur de l'image
        width,   //  Champs largeur de l'image
        channel; //   Champs RGB/YUA de l'image

    uint8_t *pixels; // Pointeur sur nos pixels compressés

} EdImage;

/**
 * @brief Permettre l'extaction de la table de Huffman des images JPEG
 */
typedef struct HuffmanTable
{
    
};



/**
 * @brief Définir le chams de résolution de notre image JPEG ( height,width, channel)
 */
void *ReadSize(FILE *file, EdImage *image);

/**
 * @brief Définir la table de décodage de notre image JPEG ( Zone DB)
 */
QuantizationTable *ReadDQT(FILE *file, int *nb_tables);

#endif