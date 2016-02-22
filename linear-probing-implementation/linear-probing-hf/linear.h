/*
 * Linear Probing Implementation
 *
 * Author: Qiaobin Fu <qiaobinf@bu.edu>
 *
 * The implementation is based on the website:
 * http://opendatastructures.org/versions/edition-0.1e/ods-java/5_2_LinearHashTable_Linear_.html#SECTION00821000000000000000
 *
 */

#ifndef _LINEAR_H_
#define _LINEAR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_function.h"

//#define LINEAR_DEBUG
#define HASH_FUNC_NUM 16
#define XIP_VXT_TABLE_SIZE  128

enum { AVAILABLE, OCCUPIED, DELETED};

struct hash_entry {
    int key;
    int value;
    int status;
};

struct hash_table {
    struct hash_entry *items;

    int table_size;
    int num_items;
};

double load_factor();
void show_hash_table();
void linear_hash_table_init(int size);
void linear_insert(int key, int val);
void linear_remove(int key);
int linear_lookup(int key);

#endif
