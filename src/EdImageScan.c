#include "../libs/EdImageStruct.h"

//========================================================
/**
 * @brief Inspection de la section de Quantization Table (FFDB) pour la collecte
 */
QuantizationTable *ReadDQT(FILE *file, int *nb_tables)
{
    QuantizationTable *tables = NULL;
    *nb_tables = 0;

    unsigned char byte, marker;

    while (fread(&byte, 1, 1, file))
    {
        if (byte == 0xFF)
        {
            fread(&marker, 1, 1, file);

            if (marker == 0xDB)
            {
                uint16_t length = LENGTH(file);

                length -= 2;

                while (length > 0)
                {
                    QuantizationTable *tmp =
                        realloc(tables,
                                (*nb_tables + 1) * sizeof(QuantizationTable));

                    if (!tmp)
                        exit(EXIT_FAILURE);

                    tables = tmp;

                    QuantizationTable *qt = &tables[*nb_tables];

                    uint8_t info = fgetc(file);

                    qt->precision = info >> 4;
                    qt->id = info & 0x0F;

                    int count = (qt->precision == 0) ? 64 : 128;

                    for (int i = 0; i < 64; i++)
                    {
                        if (qt->precision == 0)
                            qt->values[i] = fgetc(file);
                        else
                            qt->values[i] =
                                (fgetc(file) << 8) | fgetc(file);
                    }
                    length -= 1 + count;
                    (*nb_tables)++;
                }
            }
        }
    }
    printf("\nTailler : %ls", (nb_tables));
    return tables;
}

//========================================================
/**
 * @brief Inspection de la section de Start of frame (FFC0) pour prendre les dimension de l'image et l enombre de canal
 **/
void *ReadSize(FILE *file, EdImage *image)
{
    image = malloc(sizeof(EdImage));
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

            // printf("Marker : %02X - Indice :%02X\n", bit, marker);

            if (marker == 0xC0 || marker == 0xC2)
            {
                int b1, b2;

                int length = LENGTH(file);
                // (void)length; // si pas utilisé

                int precision = fgetc(file);
                // (void)precision;

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
    printf(" ________Résolution de l'image________ \nLargeur : %d\nhauteur : %d\n ", image->width, image->height);
    // free(image);
    // return image;
}

//========================================================
/**
 * @brief Inspection de la section des huffman table (FFC4) pour la collecte
 */
void Readhuff(FILE *file, Huffman *ht)
{
    unsigned char marker, bit;

    while (fread(&marker, 1, 1, file))
    {
        if (marker == 0xFF)
        {
            if (fread(&bit, 1, 1, file))
            {
                if (bit == 0xC4)
                {
                    uint16_t lenght = LENGTH(file);

                    lenght -= 2;

                    while (lenght > 0)
                    {
                        uint8_t info = fgetc(file);

                        ht->tc = (info >> 4) & 0x0F;
                        ht->th = info & 0x0F;

                        lenght--;

                        ht->totalSymbols = 0;

                        for (int j = 0; j < 16; j++)
                        {
                            ht->lengthByCode[j] = fgetc(file);
                            ht->totalSymbols += ht->lengthByCode[j];
                        }

                        lenght -= 16;

                        ht->huffval = malloc(
                            ht->totalSymbols * sizeof(uint8_t));

                        fread(ht->huffval, 1,
                              ht->totalSymbols, file);

                        lenght -= ht->totalSymbols;
                    }
                }
            }
        }
    }
    printf("_____Huffan table____\nht->tc :%u\nht->th :%u\nht->huffval :%d\n", ht->tc, ht->th, ht->totalSymbols);

    // return ht;
}

//========================================================
/**
 * @brief Inspection de la section de Start of Scan (FFDA) pour la collecte
 */
StartOfScan ReadSOS(FILE *file)
{
    StartOfScan sos = {0};

    uint8_t marker, code;

    while (fread(&marker, 1, 1, file))
    {
        if (marker == 0xFF)
        {
            if (fread(&code, 1, 1, file))
            {
                if (code == 0xDA)
                {
                    uint16_t length = LENGTH(file);

                    sos.numberOfComponents = fgetc(file);

                    sos.components = malloc(
                        sos.numberOfComponents *
                        sizeof(SOSComponent));

                    for (int i = 0;
                         i < sos.numberOfComponents;
                         i++)
                    {
                        sos.components[i].componentId =
                            fgetc(file);

                        uint8_t tableInfo = fgetc(file);

                        sos.components[i].dcTable =
                            (tableInfo >> 4) & 0x0F;

                        sos.components[i].acTable =
                            tableInfo & 0x0F;
                    }

                    sos.Ss = fgetc(file);
                    sos.Se = fgetc(file);

                    uint8_t approx = fgetc(file);

                    sos.Ah = (approx >> 4) & 0x0F;
                    sos.Al = approx & 0x0F;

                    return sos;
                }
            }
        }
    }

    return sos;
}

//========================================================
/**
 * @brief Inspection de la section de Image DATA de la section Image DATA
 */
ImageData ReadImageData(FILE *file)
{
    ImageData img = {0};

    size_t capacity = 1024;

    img.data = malloc(capacity);

    uint8_t current;
    uint8_t next;

    while (fread(&current, 1, 1, file))
    {
        if (current == 0xFF)
        {
            if (!fread(&next, 1, 1, file))
            {
                break;
            }

            // byte stuffing
            if (next == 0x00)
            {
                if (img.size >= capacity)
                {
                    capacity *= 2;

                    uint8_t *tmp =
                        realloc(img.data,
                                capacity);

                    if (!tmp)
                    {
                        free(img.data);

                        img.data = NULL;
                        img.size = 0;

                        return img;
                    }

                    img.data = tmp;
                }

                img.data[img.size++] = 0xFF;
            }

            // restart markers
            else if (next >= 0xD0 &&
                     next <= 0xD7)
            {
                continue;
            }

            else
            {
                // vrai marqueur JPEG

                fseek(file, -2, SEEK_CUR);

                break;
            }
        }
        else
        {
            if (img.size >= capacity)
            {
                capacity *= 2;

                uint8_t *tmp =
                    realloc(img.data,
                            capacity);

                if (!tmp)
                {
                    free(img.data);

                    img.data = NULL;
                    img.size = 0;

                    return img;
                }

                img.data = tmp;
            }

            img.data[img.size++] = current;
        }
    }

    return img;
}

//===================Décodeur binaire =====================================
