#include "debugFunction.h"

//========================================================
/**
 * @brief Lit un marqueur DQT et remplit directement le tableau de l'image
 */
void ReadDQT(FILE *file, EdImage *image)
{
    uint16_t length = LENGTH(file);
    length -= 2;

    while (length > 0)
    {
        uint8_t info = fgetc(file);
        uint8_t id = info & 0x0F;
        uint8_t precision = info >> 4;

        if (id >= 4) {
            fprintf(stderr, "Erreur: ID de table de quantification invalide (%d)\n", id);
            exit(EXIT_FAILURE);
        }

        image->qTables[id].id = id;
        image->qTables[id].precision = precision;

        int count = (precision == 0) ? 64 : 128;

        for (int i = 0; i < 64; i++)
        {
            if (precision == 0)
                image->qTables[id].values[i] = fgetc(file);
            else
                image->qTables[id].values[i] = (fgetc(file) << 8) | fgetc(file);
        }
        length -= (1 + count);
    }
}

//========================================================
/**
 * @brief Lit la section SOF (FFC0) pour les dimensions
 */
void ReadSize(FILE *file, EdImage *image)
{
    int length = LENGTH(file);
    int precision = fgetc(file); (void)precision;

    int b1 = fgetc(file);
    int b2 = fgetc(file);
    image->height = (b1 << 8) | b2;

    b1 = fgetc(file);
    b2 = fgetc(file);
    image->width = (b1 << 8) | b2;

    image->channel = fgetc(file);
    
    // On saute le reste des infos du SOF (les facteurs d'échantillonnage)
    fseek(file, length - 8, SEEK_CUR); 
    
    printf("________ Résolution de l'image ________\nLargeur : %d\nHauteur : %d\nCanaux  : %d\n", 
           image->width, image->height, image->channel);
}

//========================================================
/**
 * @brief Lit les tables de Huffman et les range au bon endroit (AC ou DC)
 */
void Readhuff(FILE *file, EdImage *image)
{
    uint16_t length = LENGTH(file);
    length -= 2;

    while (length > 0)
    {
        uint8_t info = fgetc(file);
        uint8_t tc = (info >> 4) & 0x0F; // 0 = DC, 1 = AC
        uint8_t th = info & 0x0F;        // ID de la table (0 à 3)
        length--;

        // Cibler la bonne table dans notre structure EdImage
        Huffman *ht = (tc == 0) ? &image->dcTables[th] : &image->acTables[th];
        ht->tc = tc;
        ht->th = th;
        ht->totalSymbols = 0;

        for (int j = 0; j < 16; j++)
        {
            ht->lengthByCode[j] = fgetc(file);
            ht->totalSymbols += ht->lengthByCode[j];
        }
        length -= 16;

        ht->huffval = malloc(ht->totalSymbols * sizeof(uint8_t));
        if (!ht->huffval) exit(EXIT_FAILURE);

        fread(ht->huffval, 1, ht->totalSymbols, file);
        length -= ht->totalSymbols;
        
        printf("Table Huffman chargée : %s[%d] (%d symboles)\n", 
               (tc == 0) ? "DC" : "AC", th, ht->totalSymbols);
    }
}

//========================================================
/**
 * @brief Lit le marqueur SOS (FFDA)
 */
StartOfScan ReadSOS(FILE *file)
{
    StartOfScan sos = {0};
    uint16_t length = LENGTH(file); (void)length;

    sos.numberOfComponents = fgetc(file);
    sos.components = malloc(sos.numberOfComponents * sizeof(SOSComponent));

    for (int i = 0; i < sos.numberOfComponents; i++)
    {
        sos.components[i].componentId = fgetc(file);
        uint8_t tableInfo = fgetc(file);
        sos.components[i].dcTable = (tableInfo >> 4) & 0x0F;
        sos.components[i].acTable = tableInfo & 0x0F;
    }

    sos.Ss = fgetc(file);
    sos.Se = fgetc(file);
    uint8_t approx = fgetc(file);
    sos.Ah = (approx >> 4) & 0x0F;
    sos.Al = approx & 0x0F;

    return sos;
}

//========================================================
/**
 * @brief Extrait le flux d'ImageData compressé (avec gestion du byte-stuffing)
 */
ImageData ReadImageData(FILE *file)
{
    ImageData img = {0};
    size_t capacity = 4096;
    img.data = malloc(capacity);
    if (!img.data) exit(EXIT_FAILURE);

    uint8_t current, next;

    while (fread(&current, 1, 1, file))
    {
        if (current == 0xFF)
        {
            if (!fread(&next, 1, 1, file)) break;

            if (next == 0x00) // Byte stuffing
            {
                if (img.size >= capacity) {
                    capacity *= 2;
                    img.data = realloc(img.data, capacity);
                }
                img.data[img.size++] = 0xFF;
            }
            else if (next >= 0xD0 && next <= 0xD7) // Restart Marker
            {
                continue;
            }
            else // Un vrai marqueur JPEG (probablement EOI 0xD9)
            {
                fseek(file, -2, SEEK_CUR);
                break;
            }
        }
        else
        {
            if (img.size >= capacity) {
                capacity *= 2;
                img.data = realloc(img.data, capacity);
            }
            img.data[img.size++] = current;
        }
    }
    return img;
}

//=================== Chef d'orchestre : Le Décodeur Général ====================

EdImage *ParseJPEG(const char *filename) 
{
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Impossible d'ouvrir le fichier");
        return NULL;
    }

    EdImage *image = calloc(1, sizeof(EdImage)); // calloc met tout à 0 par défaut
    if (!image) exit(EXIT_FAILURE);

    unsigned char byte, marker;

    // Vérification de l'en-tête SOI (FFD8)
    if (fgetc(file) != 0xFF || fgetc(file) != 0xD8) {
        fprintf(stderr, "Ce fichier n'est pas un JPEG valide.\n");
        free(image);
        fclose(file);
        return NULL;
    }

    // Boucle principale de lecture des marqueurs
    while (fread(&byte, 1, 1, file)) 
    {
        if (byte == 0xFF) 
        {
            fread(&marker, 1, 1, file);

            switch (marker) 
            {
                case 0xC0: // SOF0 (Baseline)
                case 0xC2: // SOF2 (Progressive)
                    ReadSize(file, image);
                    break;

                case 0xDB: // DQT
                    ReadDQT(file, image);
                    break;

                case 0xC4: // DHT
                    Readhuff(file, image);
                    break;

                case 0xDA: // SOS
                    image->sos = ReadSOS(file);
                    // L'ImageData suit IMMÉDIATEMENT le SOS dans le fichier JPEG
                    ImageData imgData = ReadImageData(file);
                    image->bitstream = imgData.data;
                    image->bitstreamSize = imgData.size;
                    break;

                case 0xD9: // EOI (End of Image)
                    printf("Fin de l'image JPEG atteinte.\n");
                    break;

                default:
                    // Marqueur inconnu ou inutile (ex: APP0, COM), on passe sa taille
                    if (marker >= 0xE0 && marker <= 0xEF) {
                        uint16_t skip_len = LENGTH(file);
                        fseek(file, skip_len - 2, SEEK_CUR);
                    }
                    break;
            }
        }
    }

    fclose(file);
    return image;
}