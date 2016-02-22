/*
 * Cuckoo Hashing Implementation
 *
 * Author: Qiaobin Fu <qiaobinf@bu.edu>
 *
 * The implementation is based on the original paper:
 *  Pagh, Rasmus, and Flemming Friche Rodler. Cuckoo hashing. 
 *  Springer Berlin Heidelberg, 2001.
 * 
 * and the CoNext paper:
 *  Fan, Bin, et al. "Cuckoo Filter: Practically Better Than Bloom."
 *  Proceedings of the 10th ACM International on Conference on 
 *  emerging Networking Experiments and Technologies. ACM, 2014.
 *
 * Thanks to the online open source code:
 * 1. Cuckoo filter: https://github.com/efficient/cuckoofilter
 * 2. Cuckoo filter: https://github.com/begeekmyfriend/CuckooFilter
 * 3. Cuckoo hashing implementation: http://krokisplace.blogspot.com/2010/01/cuckoo-hashing-implementation.html
 *
 */

#include "cuckoo_hashing.h"

static struct hash_table cuckoo_hash_table;

static uint (* hash_func[HASH_FUNC_NUM])(const unsigned char * str, uint len) = 
{BOB1, JSHash, OCaml, OAAT, PJWHash, RSHash,  SDBM, Simple, SML, STL,
    APHash, BKDR, DEKHash, DJBHash, FNV32, Hsieh};

double load_factor() {
    return 1.0 * cuckoo_hash_table.num_items / (cuckoo_hash_table.table_size + 0.0);
}

void show_hash_table()
{
    int i, j;

    printf("Show all entries in the cuckoo hash table [key, value, status]:\n");
    for (i = 0; i < cuckoo_hash_table.bucket_num; i++) {
        printf("bucket[%02x]:", i);

        struct hash_entry *items = cuckoo_hash_table.buckets[i];

        for (j = 0; j < ASSOC_WAY; j++) {
            printf("\t[%04x, %02x, %x]", items[j].key, items[j].value, items[j].status);
        }

        printf("\n");
    }
}

static int cuckoo_hash_collide(struct hash_table *table, uint32_t *hash, uint32_t key, uint32_t value)
{
    int j, k, alt_cnt;
    uint32_t old_hash[2], alternate_hash;
    uint32_t old_val, old_key;
    struct hash_entry *items;

    /* Kick out the old bucket and move it to the alternative bucket. */
    //old_hash[0] = hash[0];
    //old_hash[1] = hash[1];

    items = table->buckets[hash[0]];

    /* Obtain the old (key, value), and calculate the corresponding hashes */
    old_key = items[0].key;
    old_val = items[0].value;
    
    //old_hash[0] = old_key & (table->bucket_num - 1);
    //old_hash[1] = (old_hash[0] + 11) & (table->bucket_num - 1);
    
    old_hash[0] = hash_func[0]((const unsigned char *)&old_key, sizeof(old_key)) % table->bucket_num;
    old_hash[1] = hash_func[1]((const unsigned char *)&old_key, sizeof(old_key)) % table->bucket_num;

    alternate_hash = (old_hash[0] == hash[0] ? old_hash[1] : old_hash[0]);

    /* insert the new (key, value) into the current slot */
    items[0].key = key;
    items[0].value = value;

    k = 0;
    alt_cnt = 0;

KICK_OUT:
    items = table->buckets[alternate_hash];

    for (j = 0; j < ASSOC_WAY; j++) {
        if (items[j].status == AVAILABLE) {
            items[j].status = OCCUPIED;
            items[j].key = old_key;
            items[j].value = old_val;
            break;
        }
    }

    if (j == ASSOC_WAY) {
        if (++alt_cnt > MAX_KICK_OUT) {
            if (k == ASSOC_WAY - 1) {
                /* Hash table is almost full and needs to be resized */
                return 1;
            } else {
                k++;
            }
        }

        uint32_t tmp_key = items[k].key;
        uint32_t tmp_val = items[k].value;
        items[k].key = old_key;
        items[k].value = old_val;

        old_key = tmp_key;
        old_val = tmp_val;
        //old_hash[0] = old_key & (table->bucket_num - 1);
        //old_hash[1] = (old_hash[0] + 11) & (table->bucket_num - 1);
        
        old_hash[0] = hash_func[0]((const unsigned char *)&old_key, sizeof(old_key)) % table->bucket_num;
        old_hash[1] = hash_func[1]((const unsigned char *)&old_key, sizeof(old_key)) % table->bucket_num;


        alternate_hash = (old_hash[0] == alternate_hash ? old_hash[1] : old_hash[0]);

        goto KICK_OUT;
    }

    return 0;
}

static int cuckoo_hash_get(struct hash_table *table, uint32_t key)
{
    int i, j;
    uint32_t hash[2];
    uint32_t val;

    struct hash_entry *items;

    //hash[0] = key & (table->bucket_num - 1);
    //hash[1] = (hash[0] + 11) & (table->bucket_num - 1);

    hash[0] = hash_func[0]((const unsigned char *)&key, sizeof(key)) % table->bucket_num;
    hash[1] = hash_func[1]((const unsigned char *)&key, sizeof(key)) % table->bucket_num;

#ifdef CUCKOO_DEBUG
    printf("get h0:%x h1:%x\n", hash[0], hash[1]);
#endif

    /* Filter the key. */
    items = table->buckets[hash[0]];

    for (i = 0; i < ASSOC_WAY; i++) {
        if (key == items[i].key) {
            if (items[i].status == OCCUPIED) {
                val = items[i].value;
                return val;
            }
        }
    }

    if (i == ASSOC_WAY) {
        items = table->buckets[hash[1]];
        for (j = 0; j < ASSOC_WAY; j++) {
            if (key == items[j].key) {
                if (items[j].status == OCCUPIED) {
                    val = items[j].value;
                    return val;
                }
            }
        }

        if (j == ASSOC_WAY) {
#ifdef CUCKOO_DEBUG
            printf("Key not exists!\n");
#endif
            return AVAILABLE;
        }
    }

    return -1;
}

static int cuckoo_hash_put(struct hash_table *table, uint32_t key, uint32_t value)
{
    int i, j;
    uint32_t hash[2];
    struct hash_entry *items;

    //hash[0] = key & (table->bucket_num - 1);
    //hash[1] = (hash[0] + 11) & (table->bucket_num - 1);

    hash[0] = hash_func[0]((const unsigned char *)&key, sizeof(key)) % table->bucket_num;
    hash[1] = hash_func[1]((const unsigned char *)&key, sizeof(key)) % table->bucket_num;

#ifdef CUCKOO_DEBUG
    printf("put: value:%x h0:%x h1:%x\n", value, hash[0], hash[1]);
#endif

    /* Insert new key into hash buckets. */
    items = table->buckets[hash[0]];
    for (i = 0; i < ASSOC_WAY; i++) {
        if (items[i].status == AVAILABLE) {
            items[i].status = OCCUPIED;
            items[i].key = key;
            items[i].value = value;
            break;
        }
    }

    if (i == ASSOC_WAY) {
        items = table->buckets[hash[1]];
        for (j = 0; j < ASSOC_WAY; j++) {
            if (items[j].status == AVAILABLE) {
                items[j].status = OCCUPIED;
                items[j].key = key;
                items[j].value = value;
                break;
            }
        }

        if (j == ASSOC_WAY) {
            if (cuckoo_hash_collide(table, hash, key, value)) {
#ifdef CUCKOO_DEBUG
                printf("Hash table collision!\n");
#endif
                return -1;
            }
        }
    }

#ifdef CUCKOO_DEBUG
    show_hash_table();
#endif

    return 0;
}

static void cuckoo_hash_remove(struct hash_table *table, uint32_t key)
{
    uint32_t i, j, hash[2];
    struct hash_entry *items;

    //hash[0] = key & (table->bucket_num - 1);
    //hash[1] = (hash[0] + 11) & (table->bucket_num - 1);

    hash[0] = hash_func[0]((const unsigned char *)&key, sizeof(key)) % table->bucket_num;
    hash[1] = hash_func[1]((const unsigned char *)&key, sizeof(key)) % table->bucket_num;

#ifdef CUCKOO_DEBUG
    printf("Remove: h0:%x h1:%x\n", hash[0], hash[1]);
#endif

    items = table->buckets[hash[0]];

    for (i = 0; i < ASSOC_WAY; i++) {
        if (key == items[i].key) {
            items[i].status = AVAILABLE;
            items[i].key = 0;
            items[i].value = 0;
            cuckoo_hash_table.num_items --;
            return;
        }
    }

    if (i == ASSOC_WAY) {
        items = table->buckets[hash[1]];
        for (j = 0; j < ASSOC_WAY; j++) {
            if (key == items[j].key) {
                items[j].status = AVAILABLE;
                items[j].key = 0;
                items[j].value = 0;
                cuckoo_hash_table.num_items --;
                return;
            }
        }

        if (j == ASSOC_WAY) {
#ifdef CUCKOO_DEBUG
            printf("Key not exists!\n");
#endif
        }
    }
}

static void cuckoo_rehash(struct hash_table *table)
{
    int i;
    struct hash_table old_table;

    /* Reallocate hash slots */
    old_table.items = table->items;
    old_table.table_size = table->table_size;

    table->table_size *= 2;
    table->items = calloc(table->table_size, sizeof(struct hash_entry));

    if (table->items == NULL) {
        table->items = old_table.items;
        return;
    }

    /* Reallocate hash buckets associated with slots */
    old_table.buckets = table->buckets;
    old_table.bucket_num = table->bucket_num;

    table->bucket_num *= 2;
    table->buckets = malloc(table->bucket_num * sizeof(struct hash_entry *));

    if (table->buckets == NULL) {
        free(table->items);
        table->items = old_table.items;
        table->buckets = old_table.buckets;
        return;
    }

    for (i = 0; i < table->bucket_num; i++) {
        table->buckets[i] = &table->items[i * ASSOC_WAY];
    }

    /* Rehash all hash slots */
    for (i = 0; i < old_table.bucket_num; i++) {
        /* Duplicated keys in hash table which can cause eternal
         * hashing collision! Be careful of that!
         */
        assert(!cuckoo_hash_put(table, old_table.buckets[i]->key, old_table.buckets[i]->value));
    }

    free(old_table.items);
    free(old_table.buckets);
}

void cuckoo_insert(uint32_t key, uint32_t value)
{
    if (value != 0) {
        /* Reject duplicated keys keeping from eternal collision */
        int status = cuckoo_hash_get(&cuckoo_hash_table, key);
        if (status == OCCUPIED) {
            return;
        } else {
            /* Insert into hash slots */
            if (cuckoo_hash_put(&cuckoo_hash_table, key, value) == -1) {
                //cuckoo_rehash(&cuckoo_hash_table);
                cuckoo_hash_put(&cuckoo_hash_table, key, value);
            }

            cuckoo_hash_table.num_items ++;
        }
    } else {
        /* Delete at the hash slot */
        cuckoo_hash_remove(&cuckoo_hash_table, key);
    }
}

void cuckoo_remove(uint32_t key) {
    cuckoo_hash_remove(&cuckoo_hash_table, key);
#ifdef CUCKOO_DEBUG
    show_hash_table();
#endif
}

int cuckoo_lookup(uint32_t key)
{
    return cuckoo_hash_get(&cuckoo_hash_table, key);
}

int cuckoo_hashing_init(size_t size)
{
    int i;

    /* Otain the hash table size, it must be the power of 2 */
    cuckoo_hash_table.table_size = upper_pow_of_2(size);

    /* Make rehashing happen */
    //hash_table.slot_num /= 4;
    cuckoo_hash_table.items = calloc(cuckoo_hash_table.table_size, sizeof(struct hash_entry));
    if (cuckoo_hash_table.items == NULL) {
        return -1;
    }

    /* Allocate hash buckets associated with slots */
    cuckoo_hash_table.bucket_num = cuckoo_hash_table.table_size / ASSOC_WAY;
    cuckoo_hash_table.buckets = malloc(cuckoo_hash_table.bucket_num * sizeof(struct hash_entry *));

    if (cuckoo_hash_table.buckets == NULL) {
        free(cuckoo_hash_table.items);
        return -1;
    }

    for (i = 0; i < cuckoo_hash_table.bucket_num; i++) {
        cuckoo_hash_table.buckets[i] = &cuckoo_hash_table.items[i * ASSOC_WAY];
    }

    return 0;
}
