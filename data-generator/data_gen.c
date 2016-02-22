#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ELE 128
#define DTA_SZ  1000000
#define DATA_SET "insert_data"
#define TEST_SET "read_data"

/*  ./gen loadfactor
 *
 * */

int main(int argc, char **argv) {
    srand(time(NULL));

    int i = 0;
    int item_num = DTA_SZ;

    if (argc != 2) {
        printf("Wrong parameters: ./gen loadfactor!\n");
        return 0;
    }

    float loadfactor = atof(argv[1]);

    char dataset[128];
    char testset[128];

    unsigned int elements[MAX_ELE];

    memset(dataset, 0, sizeof(dataset));
    memset(testset, 0, sizeof(testset));

    sprintf(dataset, "%s_%.1f.txt", DATA_SET, loadfactor);
    sprintf(testset, "%s_%.1f.txt", TEST_SET, loadfactor);

    FILE *df = fopen(dataset, "w");
    FILE *rdf = fopen(testset, "w");

    while (i < (int)ceil(MAX_ELE * loadfactor)) {
        elements[i] = rand();
        fprintf(df, "%u\n", elements[i]);
        i++;
    }
    fclose(df);
    
    i = 0;
    while(i < DTA_SZ) {
        fprintf(rdf, "%u\n", elements[rand()%((int)ceil(MAX_ELE * loadfactor))]);
        i++;
    }

    fclose(rdf);

    return 0;
}
