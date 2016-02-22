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

int read_data(char* filename, unsigned int elements[], int n) {
 
    int i = 0;
    char buffer[128];
    FILE *df = fopen(filename, "r");

    if (!df) {
        printf("cannot open file %s\n", filename);
        return -1;
    }

    memset(elements, 0, sizeof(int) * n);
    memset(buffer, 0, sizeof(buffer));

    while(fgets(buffer, 128, df) != NULL && i < n) { 
        int number = atoi(buffer); 
        elements[i] = number; 
        i++; 
        memset(buffer, 0, sizeof(buffer));
    }

    fclose(df);

    return 1;
}

#if 0
int main(int argc, char **argv) {
    
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
    unsigned int readele[DTA_SZ];

    memset(dataset, 0, sizeof(dataset));
    memset(testset, 0, sizeof(testset));

    sprintf(dataset, "%s_%.1f.txt", DATA_SET, loadfactor);
    sprintf(testset, "%s_%.1f.txt", TEST_SET, loadfactor);

    read_data(dataset, elements, (int)ceil(MAX_ELE * loadfactor));
    read_data(testset, readele, DTA_SZ);
 

    for (i = 0; i < (int)ceil(MAX_ELE * loadfactor); i++) {
        printf("%u\n", elements[i]);
    }

    for (i = 0; i < DTA_SZ; i++) {
        printf("%u\n", readele[i]);
    }

    return 0;
}
#endif
