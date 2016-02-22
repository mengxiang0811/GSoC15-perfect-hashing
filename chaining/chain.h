/*
 * Chained Hash Table Implementation
 *
 * Author: Qiaobin Fu <qiaobinf@bu.edu>
 *
 * The implementation is based on the website:
 *  http://opendatastructures.org/versions/edition-0.1e/ods-java/5_1_ChainedHashTable_Hashin.html
 */

#ifndef _CHAINING_H_
#define _CHAINING_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHAIN_DEBUG
#define HASH_FUNC_NUM   16
#define XIP_VXT_TABLE_SIZE  128

struct hash_entry {
    int key;
    int value;

    struct hash_entry *next;
};

struct hash_table {
    struct hash_entry **buckets;

    int table_size;
    int num_items;
};

double load_factor();
void show_hash_table();
void chained_hash_table_init(int size);

void resize();
int chained_insert(int key, int val);
int chained_remove(int key);
struct hash_entry *chained_lookup(int key);

#endif
