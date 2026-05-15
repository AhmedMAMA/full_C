#include "../src/EdImageScan.c"
#include "../src/debug/debugFunction.c"


int main(int argc, char const *argv[])
{
    // char* filename = "";
    FILE *f = fopen("../img/image.jpeg", "rb");
    EdImage img;

    // Détection de la résolution de l'image
    // ReadSize(f,&img);

    //Définition de la table de quantization 
    int nb_table;
    // QuantizationTable *table = ReadDQT(f,&nb_table);
    // print_quantization_tables(table,nb_table);


    // Définition de la table de Huffman


    // Analyse d'une section de notre jpeg
    printf_ShowSection(0xC4,f);

}