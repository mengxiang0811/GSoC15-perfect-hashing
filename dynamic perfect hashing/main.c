#include <sys/time.h>
#include "dph.h"
#include "load_data.h"

#define LOOP    1000

extern uint32_t count;
extern HashTable Hash;
extern uint32_t PRIME;

int main(int argc, char **argv) {
    int i = 0;
    int j = 0;

    if (argc != 2) {
        printf("Wrong parameters: ./dph loadfactor!\n");
        return 0;
    }

    float loadfactor = atof(argv[1]);

    printf("The loadfactor is %.1f\n", loadfactor);

    unsigned int ele_num = (unsigned int)ceil(MAX_ELE * loadfactor);

    char dataset[128];
    char testset[128];

    unsigned int elements[MAX_ELE];
    unsigned int readele[DTA_SZ];

    memset(dataset, 0, sizeof(dataset));
    memset(testset, 0, sizeof(testset));

    sprintf(dataset, "%s_%.1f.txt", DATA_SET, loadfactor);
    sprintf(testset, "%s_%.1f.txt", TEST_SET, loadfactor);

    read_data(dataset, elements, ele_num);
    read_data(testset, readele, DTA_SZ);

    printf("\n****************************************************\n");
    printf("Step 1: Initialize the dynamic perfect hashing table!\n");
    Initialize();
    printf("****************************************************\n\n");

    printf("\n****************************************************\n");
    printf("Step 2: Insert %d items into the dynamic perfect hashing table!\n", ele_num);
    
    for (i = 0; i < ele_num; i++) {
        Insert(elements[i]);
    }
    
    printf("****************************************************\n\n");
    
    printf("\n****************************************************\n");
    printf("Step 3: begin to test the performance of dynamic perfect hashing!\n");

    struct timeval start, end;
    gettimeofday(&start, 0);
    for (j = 0; j < LOOP; j++) {
        for (i = 0; i < DTA_SZ; i++) {
            Lookup(readele[i]);
        }
    }
    gettimeofday(&end, 0);

    long elapsed = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
    printf("The performance of dynamic perfect hashing with loadfactor = %.1f is: %f MLPS\n", loadfactor, (LOOP * DTA_SZ)/(elapsed * 1.0));
    printf("****************************************************\n\n");

    printf("\n****************************************************\n");
    printf("Step 5: remove all items into the dynamic perfect hashing table!\n");
    for (i = 0; i < ele_num; i++) {
        Delete(elements[i]);
    }

    printf("****************************************************\n\n");

    return 0;
}
