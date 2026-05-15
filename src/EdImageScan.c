#include "../libs/EdImageStruct.h"

//========================================================
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

            printf("Marker : %02X - Indice :%02X\n", bit, marker);

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
                printf("Résolution de l'image : \n Taille /Poid : %d\nPrecision :%d\n Largeur :%d\n Hauteur :%d\n", length, precision, image->width, image->height);

                break;
            }
        }
    }
    // free(image);
    // return image;
}

//========================================================
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
                        {
                            qt->values[i] = fgetc(file);
                        }
                        else
                        {
                            qt->values[i] =
                                (fgetc(file) << 8) | fgetc(file);
                        }
                    }

                    length -= 1 + count;

                    (*nb_tables)++;
                }
            }
        }
    }

    return tables;
}

//========================================================

