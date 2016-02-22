#include "double_hashing.h"

static uint (* hash_func[HASH_FUNC_NUM])(const unsigned char * str, uint len) = 
{BOB1, JSHash, OCaml, OAAT, PJWHash, RSHash,  SDBM, Simple, SML, STL,
    APHash, BKDR, DEKHash, DJBHash, FNV32, Hsieh};

static struct hash_table double_hash_table;

double load_factor() {
    return 1.0 * double_hash_table.num_items / (double_hash_table.table_size + 0.0);
}

void show_hash_table() {
    int i;
    if (double_hash_table.num_items == 0) {
        printf("The hash table is empty!!\n");
        return;
    }

    printf("Show all entries in the double hash table [item_index, key, value]:\n");
    
    for (i = 0; i < double_hash_table.table_size; i++) {
        if (double_hash_table.items[i].key != 0)
            printf("[%d, %04x, %02x]\n", i, double_hash_table.items[i].key, double_hash_table.items[i].value);
    }

    printf("\n");
}

void double_hash_table_init(int size) {
    double_hash_table.hf1 = 0;
    double_hash_table.hf2 = 1;

    double_hash_table.table_size = size;
    double_hash_table.table_size = size;
    double_hash_table.items = (struct hash_entry *) calloc(size, sizeof(struct hash_entry));
}

int DHT_insert(uint32_t key, uint32_t value)
{
    if (double_hash_table.num_items >= double_hash_table.table_size) {
        printf("Error: insertion failed, the table is full!\n");
        return -1;
    }

    unsigned int hash_index1 = hash_func[double_hash_table.hf1]((const unsigned char *)&key, sizeof(key));
    unsigned int hash_index2 = hash_func[double_hash_table.hf2]((const unsigned char *)&key, sizeof(key));

    printf("key = %d, index1 = %u, index2=%u\n", key, hash_index1, hash_index2);

    unsigned int idx = hash_index1 % double_hash_table.table_size;

    int count =0;

    while((double_hash_table.items[idx].key != 0) && (double_hash_table.items[idx].key != key) &&  (count < MAX_KICK_OUT)) {
        count++;
        idx = (hash_index1 + count * hash_index2) % double_hash_table.table_size;
    }

    if (count == MAX_KICK_OUT) {
        printf("Error: insertion failed!\n");
        return -1;
    }
    else 
    { 
        double_hash_table.items[idx].key = key;
        double_hash_table.items[idx].value = value;
        double_hash_table.num_items++;

        return 1;
    }
}

int DHT_remove(uint32_t key)
{	
    unsigned int hash_index1 = hash_func[double_hash_table.hf1]((const unsigned char *)&key, sizeof(key));
    unsigned int hash_index2 = hash_func[double_hash_table.hf2]((const unsigned char *)&key, sizeof(key));

    unsigned int idx = hash_index1 % double_hash_table.table_size;

    int count = 0;

    while((count < MAX_KICK_OUT) && (double_hash_table.items[idx].key != key)) {
        count++;
        idx = (hash_index1 + count * hash_index2) % double_hash_table.table_size;
    }

    if(double_hash_table.items[idx].key == key) {
        double_hash_table.items[idx].key = 0;
        double_hash_table.items[idx].value = 0;
        double_hash_table.num_items--;

        return 1;
    }
    else {
        //printf("Warning: removed key=%d is not in the hash table!", key);

        return -1;
    }
}

int DHT_lookup(uint32_t key)
{
    unsigned int hash_index1 = hash_func[double_hash_table.hf1]((const unsigned char *)&key, sizeof(key));
    unsigned int hash_index2 = hash_func[double_hash_table.hf2]((const unsigned char *)&key, sizeof(key));

    unsigned int idx = hash_index1 % double_hash_table.table_size;

    int count = 0;

    while((double_hash_table.items[idx].key != key) && (count < MAX_KICK_OUT)) {
        count++;
        idx = (hash_index1 + count * hash_index2) % double_hash_table.table_size;
    }

    if(double_hash_table.items[idx].key == key) {
        return double_hash_table.items[idx].value;
    }
    else {
        //printf("Warning: the key=%d is not in the hash table!\n", key);
        return -1;
    }
}
