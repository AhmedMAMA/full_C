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

//======================Structure de stockage des données de la table de HUffman =================================
typedef struct Huffman
{
    uint8_t tc;
    uint8_t th;

    uint8_t lengthByCode[16];

    uint8_t *huffval;

    int totalSymbols;

} Huffman;

//======================Structure de stockage des données de la table de quantization =================================
typedef struct QuantizationTable
{
    uint8_t precision;
    uint8_t id;
    uint16_t values[64];
} QuantizationTable;

//======================Structure de stockage des données de la section de Image of SCAN =================================
typedef struct
{
    uint8_t componentId;
    uint8_t dcTable;
    uint8_t acTable;

} SOSComponent;

typedef struct
{
    uint8_t numberOfComponents;

    SOSComponent *components;

    uint8_t Ss;
    uint8_t Se;
    uint8_t Ah;
    uint8_t Al;

} StartOfScan;


//====================== Structure de lecture des données compressée de notre image JPEG brut ===============================
typedef struct
{
    uint8_t *data;
    size_t size;

} ImageData;

//====================== Structure de stockage des données de l'image en pixels (image décodée) =================================
typedef struct EdImage
{
    int width;
    int height;
    int channel;

    Huffman dcTables[4];
    Huffman acTables[4];

    QuantizationTable qTables[4];

    StartOfScan sos;

    uint8_t *bitstream;
    size_t bitstreamSize;

} EdImage;

//============================  Fonctions  =====================================
void *ReadSize(FILE *file, EdImage *image);

/**
 * @brief Définir la table de décodage de notre image JPEG ( Zone DB)
 */
QuantizationTable *ReadDQT(FILE *file, int *nb_tables);

void Readhuff(FILE *file, Huffman *ht);

StartOfScan ReadSOS(FILE *file);
ImageData ReadImageData(FILE *file);

#endif