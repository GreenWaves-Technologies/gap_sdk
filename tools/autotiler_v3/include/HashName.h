/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 */

#ifndef __HASH_NAME_H__
#define __HASH_NAME_H__

#include <stddef.h>

typedef struct ANameT NameT;
typedef struct ANameT {
        char *Name;
	unsigned int Len;
        NameT *Next;
} NameT;

typedef struct {
	unsigned int Size;
	NameT **Table;
} HashTableT;

NameT *HashInsertName(char *Name);
HashTableT *CreateHashTable(int Size);
void SetActiveHashTable(HashTableT *HashTable);
void FreeHashTable(HashTableT *HashTable);
#endif
