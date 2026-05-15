/**
 * @brief Ficheir de monitoring
 * @date 14 mai 2026
 * Ce  fichier contien que des fonction de debuggage de nos code source à déployer pour examinier leur contenu/tye
 */
// #include <string.h>
#include "../../libs/EdImageStruct.h"

/**
 * @brief Fonction d'affihcage des valeur contenue dans notre table de quantization
 */
void print_quantization_tables(QuantizationTable *tables, int nb_tables)
{
    for (int t = 0; t < nb_tables; t++)
    {
        printf("\n=============================\n");
        printf("Table %d\n", t);
        printf("ID        : %u\n", tables[t].id);
        printf("Precision : %u bits\n",
               tables[t].precision == 0 ? 8 : 16);

        printf("Coefficients :\n\n");

        for (int i = 0; i < 64; i++)
        {
            printf("%4u ", tables[t].values[i]);

            if ((i + 1) % 8 == 0)
                printf("\n");
        }

        printf("\n");
    }
}

//================================================================================

/**Affoicher une section spécifique  du JPEG */
void printf_ShowSection(unsigned char section, FILE *file)
{
    unsigned char marker, next, bit;
    int nb_sector = 0;
    int line = 0;

    while (fread(&marker, 1, 1, file))
    {
        line++;
        fread(&next, 1, 1, file);
        if (next == section)
            nb_sector++;
    }
    printf("\nLine :%d - Sector Number :%d\n", line, nb_sector);
}

/**Affoicher toute les sections du JPEG */

void showAllMark()
{
    FILE *file = fopen("../img/image.jpeg", "rb");

    if (!file)
    {
        fprintf(stderr, "Introuvable\n");
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
        }
    }
    fclose(file);
}
