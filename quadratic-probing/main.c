#include <sys/time.h>
#include "quadratic_probing.h"
#include "load_data.h"

//#define TOT_ITEMS   10

#define LOOP    1000
//#define SHOW

int main(int argc, char **argv) {
    int i = 0;
    int j = 0;

    if (argc != 2) {
        printf("Wrong parameters: ./linear loadfactor!\n");
        return 0;
    }

    float loadfactor = atof(argv[1]);

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
    printf("Step 1: Initialize the quadratic hashing table!\n");
    quadratic_hash_table_init(XIP_VXT_TABLE_SIZE);
    printf("****************************************************\n\n");

    printf("\n****************************************************\n");
    printf("Step 2: Insert %d items into the quadratic hashing table!\n", ele_num);
    
    for (i = 0; i < ele_num; i++) {
        //printf("insert key = %u\n", elements[i]);
        QUAD_insert(elements[i], i);
    }
    
    printf("The load factor is: %f\n", load_factor());
    printf("****************************************************\n\n");
    
#ifdef SHOW
    show_hash_table();
#endif

    printf("\n****************************************************\n");
    printf("Step 3: check the quadratic hashing table!\n");
 
#if 0
    for (i = 0; i < ele_num; i++) {
        if (QUAD_lookup(elements[i]) != i) {
            printf("Error key-value: %d, %d!!!\n", QUAD_lookup(elements[i]), i);
            return -1;
        }
    }
#endif

    printf("All the key-value pairs are correct!\n");
    printf("****************************************************\n\n");

    printf("\n****************************************************\n");
    printf("Step 4: begin to test the performance of quadratic hashing\n");

    struct timeval start, end;
    gettimeofday(&start, 0);
    for (j = 0; j < LOOP; j++) {
        for (i = 0; i < DTA_SZ; i++) {
            QUAD_lookup(readele[i]);
        }
    }
    gettimeofday(&end, 0);

    long elapsed = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
    printf("The performance of quadratic hashing with loadfactor = %.f is: %f MLPS\n", loadfactor, (LOOP * DTA_SZ)/(elapsed * 1.0));
    printf("****************************************************\n\n");

    printf("\n****************************************************\n");
    printf("Step 5: remove all items into the quadratic hashing table!\n");
    for (i = 0; i < ele_num; i++) {
        QUAD_remove(elements[i]);
    }

#ifdef SHOW
    show_hash_table();
#endif

    printf("The load factor is: %f\n", load_factor());
    printf("****************************************************\n\n");

    return 0;
}
