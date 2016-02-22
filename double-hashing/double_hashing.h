#ifndef _DHASH_
#define _DHASH_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "hash_function.h"

#define DHT_DEBUG
#define HASH_FUNC_NUM   16
#define MAX_KICK_OUT    32
#define XIP_VXT_TABLE_SIZE 128

struct hash_entry {
    int key;
    int value;
};

struct hash_table {
	int hf1;
	int hf2;

    int table_size;
    int num_items;
	
    struct hash_entry *items;
};

double load_factor();
void show_hash_table();
void double_hash_table_init(int size);

int DHT_insert(uint32_t key, uint32_t value);
int DHT_remove(uint32_t key);
int DHT_lookup(uint32_t key);

#endif
