/*
 * Quadratic Probing Implementation
 *
 * Author: Qiaobin Fu <qiaoibnf@bu.edu>
 *
 * The implementation is based on the website:
 *  https://en.wikipedia.org/wiki/Quadratic_probing
 *
 * */

#include "quadratic_probing.h"

static uint (* hash_func[HASH_FUNC_NUM])(const unsigned char * str, uint len) = 
{BOB1, JSHash, OCaml, OAAT, PJWHash, RSHash,  SDBM, Simple, SML, STL,
    APHash, BKDR, DEKHash, DJBHash, FNV32, Hsieh};

static struct hash_table quad_hash_table;

double load_factor() {
    return 1.0 * quad_hash_table.num_items / (quad_hash_table.table_size + 0.0);
}

void show_hash_table() {
    int i;
    if (quad_hash_table.num_items == 0) {
        printf("The hash table is empty!!\n");
        return;
    }

    printf("Show all entries in the double hash table [item_index, key, value]:\n");
    
    for (i = 0; i < quad_hash_table.table_size; i++) {
        if (quad_hash_table.items[i].key != 0)
            printf("[%d, %04x, %d]\n", i, quad_hash_table.items[i].key, quad_hash_table.items[i].value);
    }

    printf("\n");
}

void quadratic_hash_table_init(int size) {
    quad_hash_table.hf1 = 0;

    quad_hash_table.table_size = size;
    quad_hash_table.items = (struct hash_entry *) calloc(size, sizeof(struct hash_entry));
}

int QUAD_insert(uint32_t key, uint32_t value)
{
    if (quad_hash_table.num_items >= quad_hash_table.table_size) {
        printf("Error: insertion failed, the table is full!\n");
        return -1;
    }

    unsigned int hash_index1 = hash_func[quad_hash_table.hf1]((const unsigned char *)&key, sizeof(key));

    unsigned int idx = hash_index1 % quad_hash_table.table_size;

    int count =0;

    while((quad_hash_table.items[idx].key != 0) && (quad_hash_table.items[idx].key != key) &&  (count < MAX_KICK_OUT)) {
        count++;

        /* a good choice for table_size with a power of 2 */
        idx = (hash_index1 + (count * count + count) / 2) % quad_hash_table.table_size;
    }

    if (count == MAX_KICK_OUT) {
        printf("Error: insertion failed!\n");
        return -1;
    }
    else 
    { 
        quad_hash_table.items[idx].key = key;
        quad_hash_table.items[idx].value = value;
        quad_hash_table.num_items++;

        return 1;
    }
}

int QUAD_remove(uint32_t key)
{	
    unsigned int hash_index1 = hash_func[quad_hash_table.hf1]((const unsigned char *)&key, sizeof(key));

    unsigned int idx = hash_index1 % quad_hash_table.table_size;

    int count = 0;

    while((count < MAX_KICK_OUT) && (quad_hash_table.items[idx].key != key)) {
        count++;
        idx = (hash_index1 + (count * count + count) / 2) % quad_hash_table.table_size;
    }

    if(quad_hash_table.items[idx].key == key) {
        quad_hash_table.items[idx].key = 0;
        quad_hash_table.items[idx].value = 0;
        quad_hash_table.num_items--;

        return 1;
    }
    else {
        //printf("Warning: removed key=%d is not in the hash table!", key);

        return -1;
    }
}

int QUAD_lookup(uint32_t key)
{
    unsigned int hash_index1 = hash_func[quad_hash_table.hf1]((const unsigned char *)&key, sizeof(key));

    unsigned int idx = hash_index1 % quad_hash_table.table_size;

    int count = 0;

    while((quad_hash_table.items[idx].key != key) && (count < MAX_KICK_OUT)) {
        count++;
        idx = (hash_index1 + (count * count + count) / 2) % quad_hash_table.table_size;
    }

    if(quad_hash_table.items[idx].key == key) {
        return quad_hash_table.items[idx].value;
    }
    else {
        //printf("Warning: the key=%d is not in the hash table!\n", key);
        return -1;
    }
}
