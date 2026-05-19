
#include "debugFunction.h"

// ================= Mes fonctions de debug ====================

/**
 * @brief Affiche le contenu d'une table de quantification chargée
 */
void afficherDQTDebug(EdImage *image)
{
    printf("\n=================== TABLES DQT CHARGÉES ===================\n");
    for (int id = 0; id < 4; id++) {
        // On vérifie si la table a été initialisée (id concordant)
        if (image->qTables[id].id == id) {
            printf("-> Table de Quantification [%d] (Précision: %s):\n", 
                   id, (image->qTables[id].precision == 0) ? "8 bits" : "16 bits");
            
            // Affichage de la matrice 8x8 brute
            for (int i = 0; i < 64; i++) {
                printf("%4u ", image->qTables[id].values[i]);
                if ((i + 1) % 8 == 0) printf("\n");
            }
            printf("\n");
        }
    }
    printf("===========================================================\n");
}

/**
 * @brief Affiche une table de Huffman au format matrice
 */
void afficherHuffmanMatriceDebug(Huffman *h, const char* type)
{
    if (h->totalSymbols == 0) return; // Table vide

    printf("-> Table Huffman %s [ID: %u] (%d symboles):\n", type, h->th, h->totalSymbols);
    int index = 0;
    for (int i = 0; i < 16; i++) {
        int nbCodes = h->lengthByCode[i];
        if (nbCodes > 0) {
            printf("  Longueur %2d : ", i + 1);
            for (int j = 0; j < nbCodes; j++) {
                if (index < h->totalSymbols) {
                    printf("%02X ", h->huffval[index]);
                    index++;
                }
            }
            printf("\n");
        }
    }
    printf("\n");
}

/**
 * @brief Parcourt et affiche toutes les tables de Huffman chargées
 */
void afficherToutesLesTablesHuffman(EdImage *image)
{
    printf("\n=================== TABLES DHT CHARGÉES ===================\n");
    for (int i = 0; i < 4; i++) {
        afficherHuffmanMatriceDebug(&image->dcTables[i], "DC");
        afficherHuffmanMatriceDebug(&image->acTables[i], "AC");
    }
    printf("===========================================================\n");
}

/**
 * @brief Libère proprement toute la mémoire dynamique stockée dans EdImage
 */
void libererMemoireImage(EdImage *image)
{
    if (!image) return;

    // Libération des symboles des tables Huffman DC et AC
    for (int i = 0; i < 4; i++) {
        if (image->dcTables[i].huffval) {
            free(image->dcTables[i].huffval);
        }
        if (image->acTables[i].huffval) {
            free(image->acTables[i].huffval);
        }
    }

    // Libération des composants SOS
    if (image->sos.components) {
        free(image->sos.components);
    }

    // Libération du flux binaire compressé
    if (image->bitstream) {
        free(image->bitstream);
    }

    // Libération de la structure globale elle-même
    free(image);
}

// ================= Mon main =====================
int main(int argc, char const *argv[])
{
    const char *jpeg_path = "/home/ahmed/Documents/next/nigma_conseil/img/test.jpeg";

    printf("Début du parsing du fichier JPEG : %s\n", jpeg_path);
    printf("-----------------------------------------------------------\n");

    // 1. Lancement du chef d'orchestre
    EdImage *image = ParseJPEG(jpeg_path);

    // Vérification de la réussite du parsing
    if (!image) {
        fprintf(stderr, "Erreur fatale : Impossible de parser le fichier JPEG.\n");
        return EXIT_FAILURE;
    }

    // 2. Affichage des structures de debug collectées en mémoire
    printf("\n--- Vérification des données stockées dans la structure ---\n");
    
    // Test des dimensions (déjà affiché par ReadSize, mais stocké ici)
    printf("Vérification globale : Largeur = %d px, Hauteur = %d px, Canaux = %d\n", 
           image->width, image->height, image->channel);

    // Test des tables de Quantification
    afficherDQTDebug(image);

    // Test des tables de Huffman
    afficherToutesLesTablesHuffman(image);

    // Test de l'ImageData (Bitstream)
    printf("\n=================== STATISTIQUES BITSTREAM ===================\n");
    printf("Taille du bitstream compressé récupéré : %zu octets\n", image->bitstreamSize);
    if (image->bitstreamSize > 0) {
        printf("Premiers octets du flux : %02X %02X %02X %02X\n", 
               image->bitstream[0], image->bitstream[1], image->bitstream[2], image->bitstream[3]);
    }
    printf("==============================================================\n");

    // 3. Nettoyage de la mémoire avant de quitter
    libererMemoireImage(image);
    printf("\nMémoire nettoyée avec succès. Fin des tests de collecte.\n");

    return EXIT_SUCCESS;
}