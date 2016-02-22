#ifndef _QUAD_HASH_
#define _QUAD_HASH_

/*
 * Quadratic Probing Implementation
 *
 * Author: Qiaobin Fu <qiaoibnf@bu.edu>
 *
 * The implementation is based on the website:
 *  https://en.wikipedia.org/wiki/Quadratic_probing
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "hash_function.h"

#define QUAD_DEBUG
#define HASH_FUNC_NUM   16
#define MAX_KICK_OUT    32
#define XIP_VXT_TABLE_SIZE 128

struct hash_entry {
    int key;
    int value;
};

struct hash_table {
    int hf1;

    int table_size;
    int num_items;

    struct hash_entry *items;
};

double load_factor();
void show_hash_table();
void quadratic_hash_table_init(int size);

int QUAD_insert(uint32_t key, uint32_t value);
int QUAD_remove(uint32_t key);
int QUAD_lookup(uint32_t key);

#endif
