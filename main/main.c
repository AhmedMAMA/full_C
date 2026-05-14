#include "../src/EdImageScan.c"

int main(int argc, char const *argv[])
{
    FILE *f = fopen("../img/test.jpeg", "rb");
    if (!f)
    {
        fprintf(stderr, "Impossible d'ouvrir le fichier\n");
        return EXIT_FAILURE;
    }
    EdImage *img = malloc(sizeof(EdImage));

    // img = findResolution(f);
    decodageMCU(f,img);

    // fclose(f);
    return EXIT_SUCCESS;
}