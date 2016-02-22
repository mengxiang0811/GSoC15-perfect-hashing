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

#ifndef _CUCKOO_HASHING_H_
#define _CUCKOO_HASHING_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "hash_function.h"

//#define CUCKOO_DEBUG
#define ASSOC_WAY       (4)  /* 4-way association */
#define MAX_KICK_OUT    (64) /* maximum number of cuckoo kicks before claiming failure */
#define HASH_FUNC_NUM   16
#define XIP_VXT_TABLE_SIZE  128

enum { AVAILABLE, OCCUPIED, };

/* The hash entries, including the key-value,
 * and the status of the buckets
 */
struct hash_entry {
    uint32_t key;
    uint32_t status; /* AVAILABLE, OCCUPIED */
    uint32_t value;
};

struct hash_table {
    struct hash_entry **buckets;
    struct hash_entry *items;

    uint32_t bucket_num;
    uint32_t table_size;
    uint32_t num_items;
};

static inline int is_pow_of_2(uint32_t x)
{
    return !(x & (x-1));
}

static inline uint32_t upper_pow_of_2(uint32_t x)
{
    if (is_pow_of_2(x))
        return x;

    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

/*
int cuckoo_hash_put(struct hash_table *table, uint32_t key, uint32_t value);
void cuckoo_hash_remove(struct hash_table *table, uint32_t key);
*/
void cuckoo_insert(uint32_t key, uint32_t value);
void cuckoo_remove(uint32_t key);
int cuckoo_lookup(uint32_t key);

double load_factor();
void show_hash_table();
int cuckoo_hashing_init(size_t size);

#endif
