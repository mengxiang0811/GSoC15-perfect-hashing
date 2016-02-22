/*
 * Linear Probing Implementation
 *
 * Author: Qiaobin Fu <qiaobinf@bu.edu>
 *
 * The implementation is based on the website:
 * http://opendatastructures.org/versions/edition-0.1e/ods-java/5_2_LinearHashTable_Linear_.html#SECTION00821000000000000000
 *
 */

#include "linear.h"

static struct hash_table linear_hash_table;

static uint (* hash_func[HASH_FUNC_NUM])(const unsigned char * str, uint len) = 
{BOB1, JSHash, OCaml, OAAT, PJWHash, RSHash,  SDBM, Simple, SML, STL,
    APHash, BKDR, DEKHash, DJBHash, FNV32, Hsieh};


double load_factor() {
    return 1.0 * linear_hash_table.num_items / (linear_hash_table.table_size + 0.0);
}

void show_hash_table() {
    int i;
    if (linear_hash_table.num_items == 0) {
        printf("The hash table is empty!!\n");
        return;
    }

    printf("Show all entries in the linear hash table [item_index, key, value, status]:\n");
    
    for (i = 0; i < linear_hash_table.table_size; i++) {
        printf("[%d, %04x, %02x, %d]\n", i, linear_hash_table.items[i].key, linear_hash_table.items[i].value, linear_hash_table.items[i].status);
    }

    printf("\n");
}
void linear_hash_table_init(int size) {
    linear_hash_table.table_size = size;
    linear_hash_table.items = (struct hash_entry *) calloc(size, sizeof(struct hash_entry));
}

void linear_insert(int key, int val) {

    //int hash_index = key % linear_hash_table.table_size;

    unsigned int hash_index = hash_func[0]((const unsigned char *)&key, sizeof(key)) % linear_hash_table.table_size;

    if (linear_hash_table.table_size == linear_hash_table.num_items) {
        printf("Error: the hash table is full!!\n");
        return;
    }

    while (linear_hash_table.items[hash_index].status == OCCUPIED) {
        hash_index = (hash_index + 1) % linear_hash_table.table_size;
    }

    linear_hash_table.items[hash_index].key = key;
    linear_hash_table.items[hash_index].value = val;
    linear_hash_table.items[hash_index].status = OCCUPIED;
    linear_hash_table.num_items ++;
    
    return;
}

void linear_remove(int key) {
    //int hash_index = key % linear_hash_table.table_size;
    unsigned int hash_index = hash_func[0]((const unsigned char *)&key, sizeof(key)) % linear_hash_table.table_size;
    int count = 0, flag = 0;
    
    if (linear_hash_table.num_items == 0) {
        printf("The hash table is empty!!\n");
        return;
    }

    while (linear_hash_table.items[hash_index].status != AVAILABLE && count <= linear_hash_table.table_size) {
        if (linear_hash_table.items[hash_index].key == key) {
            linear_hash_table.items[hash_index].key = 0;

            /* set status to DELETED during deletion operation*/
            linear_hash_table.items[hash_index].status = DELETED;
            linear_hash_table.items[hash_index].value = 0;
            
            linear_hash_table.num_items--;
            flag = 1;
            
            break;
        }

        hash_index = (hash_index + 1) % linear_hash_table.table_size;
        count++;
    }

#ifdef LINEAR_DEBUG
    if (flag)
        printf("Delete key = %d successfully!\n", key);
    else
        printf("key = %d is not available in the hash table!\n", key);
#endif
}

int linear_lookup(int key) {
    //int hash_index = key % linear_hash_table.table_size;
    unsigned int hash_index = hash_func[0]((const unsigned char *)&key, sizeof(key)) % linear_hash_table.table_size;
    int count = 0;
    
    if (linear_hash_table.num_items == 0) {
        printf("The hash table is empty!!\n");
        return -1;
    }

    while (linear_hash_table.items[hash_index].status != AVAILABLE && count <= linear_hash_table.table_size) {
        if (linear_hash_table.items[hash_index].key == key) {
            return linear_hash_table.items[hash_index].value;
        }

        hash_index = (hash_index + 1) % linear_hash_table.table_size;
    }

    return -1;
}
