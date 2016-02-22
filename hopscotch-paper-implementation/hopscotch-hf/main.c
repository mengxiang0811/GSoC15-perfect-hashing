#include <sys/time.h>
#include "hopscotch.h"
#include "load_data.h"

#define LOOP    1000
//#define SHOW

int main(int argc, char **argv) {
    int i = 0;
    int j = 0;

    if (argc != 2) {
        printf("Wrong parameters: ./hop loadfactor!\n");
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
    printf("Step 1: Initialize the hopscotch hashing table!\n");
	B = (struct xip_vxt_entry *) calloc(XIP_VXT_TABLE_SIZE, sizeof(struct xip_vxt_entry));
    printf("****************************************************\n\n");

    printf("\n****************************************************\n");
    printf("Step 2: Insert %d items into the hopscotch hashing table!\n", ele_num);
    
    for (i = 0; i < ele_num; i++) {
        //printf("insert key = %u\n", elements[i]);
        vxt_register_xidty(elements[i], i);
    }
    
    printf("****************************************************\n\n");
    
#ifdef SHOW
    show_hash_table();
#endif

    printf("\n****************************************************\n");
    printf("Step 3: check the hopscotch hashing table!\n");
    
    for (i = 0; i < ele_num; i++) {
        if (xt_to_vxt_rcu(elements[i]) != i) {
            printf("Error key-value: %u, %d!!!\n", xt_to_vxt_rcu(elements[i]), i);
            return -1;
        }
    }
    
    printf("All the key-value pairs are correct!\n");
    printf("****************************************************\n\n");

    printf("\n****************************************************\n");
    printf("Step 4: begin to test the performance of hopscotch hashing\n");

    struct timeval start, end;
    gettimeofday(&start, 0);
    for (j = 0; j < LOOP; j++) {
        for (i = 0; i < DTA_SZ; i++) {
            xt_to_vxt_rcu(readele[i]);
        }
    }
    gettimeofday(&end, 0);

    long elapsed = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
    printf("The performance of hopscotch hashing is: %f MLPS\n", (LOOP * DTA_SZ)/(elapsed * 1.0));
    printf("****************************************************\n\n");

    printf("\n****************************************************\n");
    printf("Step 5: remove all items into the hopscotch hashing table!\n");
    for (i = 0; i < ele_num; i++) {
        vxt_unregister_xidty(elements[i]);
    }

#ifdef SHOW
    show_hash_table();
#endif
    printf("****************************************************\n\n");

    return 0;
}
