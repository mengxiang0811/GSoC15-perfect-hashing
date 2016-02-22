/*
 * Chained Hash Table Implementation
 *
 * Author: Qiaobin Fu <qiaobinf@bu.edu>
 *
 * The implementation is based on the website:
 *  http://opendatastructures.org/versions/edition-0.1e/ods-java/5_1_ChainedHashTable_Hashin.html
 */

#include "chain.h"
#include "hash_function.h"

static struct hash_table chained_hash_table;

static uint (* hash_func[HASH_FUNC_NUM])(const unsigned char * str, uint len) = 
{BOB1, JSHash, OCaml, OAAT, PJWHash, RSHash,  SDBM, Simple, SML, STL,
    APHash, BKDR, DEKHash, DJBHash, FNV32, Hsieh};


void chained_hash_table_init(int size) {
    chained_hash_table.table_size = size;
    chained_hash_table.buckets = (struct hash_entry **) calloc(size, sizeof(struct hash_entry *));
}

double load_factor() {
    return 1.0;
}

struct hash_entry * create_entry(int key, int val) {

    struct hash_entry *new_entry;

    new_entry = (struct hash_entry *) malloc(sizeof(*new_entry));

    if (!new_entry) {
        printf("Error: malloc failed!\n");
        return NULL;
    }

    new_entry->key = key;
    new_entry->value = val;
    
    new_entry->next = NULL;

    return new_entry;
}

static struct hash_entry *chained_hash_table_lookup(struct hash_table *htbl, int key) {

    //int hash_index = key % htbl->table_size;
    unsigned int hash_index = hash_func[0]((const unsigned char *)&key, sizeof(key)) % htbl->table_size;

    struct hash_entry *cur = htbl->buckets[hash_index];

    while (cur) {

        if (cur->key == key)
            return cur;

        cur = cur->next;
    }

    return NULL;
}

struct hash_entry *chained_lookup(int key) {
    return chained_hash_table_lookup(&chained_hash_table, key);
}

static void release_hash_table(struct hash_table *htbl) {
    int i = 0;
    struct hash_entry *cur, *pre;

    if (!htbl) return;

    for (i = 0; i < htbl->table_size; i++) {

        if (!htbl->buckets[i]) {
            continue;
        }
        
        pre = htbl->buckets[i];
        cur = pre->next;

        free(pre);
        
        while (cur) {
            pre = cur;
            cur = cur->next;

            free(pre);
        }
    }
}

static int chained_hash_table_put(struct hash_table *htbl, int key, int val) {

    if (chained_hash_table_lookup(htbl, key) != NULL) {
        printf("Error: the key=%d exists!\n", key);
        return -1;
    }

#if 0
    if (htbl->num_items > htbl->table_size)
        resize();
#endif

    //int hash_index = key % htbl->table_size;
    unsigned int hash_index = hash_func[0]((const unsigned char *)&key, sizeof(key)) % htbl->table_size;

    struct hash_entry *entry =  create_entry(key, val);

    /* the head is empty now */
    if (!htbl->buckets[hash_index]) {
        htbl->buckets[hash_index] = entry;
        htbl->num_items++;

        return 1;
    }

    /* add the entry to the head */
    entry->next = (htbl->buckets[hash_index]);
    htbl->buckets[hash_index] = entry;
    htbl->num_items++;

    return 1;
}

int chained_insert(int key, int val) {
    return chained_hash_table_put(&chained_hash_table, key, val);
}

void resize() {
    // TODO:
    struct hash_table ntbl;

    ntbl.table_size = chained_hash_table.table_size * 2;
    ntbl.buckets = (struct hash_entry **) calloc(ntbl.table_size, sizeof(struct hash_entry *));

    int i = 0;
    struct hash_entry *cur;

    for (i = 0; i < chained_hash_table.table_size; i++) {
        cur = chained_hash_table.buckets[i];

        while (cur) {
            struct hash_entry *entry = create_entry(cur->key, cur->value);
            chained_hash_table_put(&ntbl, cur->key, cur->value);

            cur = cur->next;
        }
    }

    release_hash_table(&chained_hash_table);

    chained_hash_table.table_size = ntbl.table_size;
    chained_hash_table.buckets = ntbl.buckets;
}


int chained_remove(int key) {

    //int hash_index = key % chained_hash_table.table_size;
    unsigned int hash_index = hash_func[0]((const unsigned char *)&key, sizeof(key)) % chained_hash_table.table_size;
    
    struct hash_entry *cur, *pre;

    cur = chained_hash_table.buckets[hash_index];

    if (!cur) {
        printf("The key=%d is not in the hash table!\n", key);
        return -1;
    }

    if (cur->key == key) {
        chained_hash_table.buckets[hash_index] = cur->next;
        free(cur);
        chained_hash_table.num_items--;

        return 1;
    }

    pre = cur;
    cur = cur->next;

    while (cur) {
        if (cur->key == key) {
            pre->next = cur->next;
            free(cur);
            chained_hash_table.num_items--;
            
            return 1;
        }

        pre = cur;
        cur = cur->next;
    }
    
    printf("The key=%d is not in the hash table!\n", key);

    return -1;
}

void show_hash_table() {
    
    int i = 0;
    struct hash_entry *cur;

    printf("Show all entries in the chained hash table [bucket_index]: [key, value, status]\n");

    for (i = 0; i < chained_hash_table.table_size; i++) {
        printf("Bucket[%d]: ", i);

        if (!chained_hash_table.buckets[i]) {
            printf("empty!\n");
            continue;
        }
        
        cur = chained_hash_table.buckets[i];

        while (cur) {
        printf("[%04x, %d] ", cur->key, cur->value);
            cur = cur->next;
        }

        printf("\n");
    }
}
