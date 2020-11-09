/**
 *  @note   IFJ: Implementace interpreta jazyka IFJ20
 *	@file	symtable.h
 *	@author Dominik Horky, FIT
 *	@brief  Hlavickovy soubor slouzici pro symtable.c
 *	@note	Reseni IFJ-proj, tabulka symbolu
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include "str.h"

/*
 * USER FUNCTIONS, TYPES, ENUMS, ..
 */

#define ST_SUCCESS 0
#define ST_ERROR 1
#define ST_ID_EXISTS -1
#define ST_MAXSTACK CHAR_MAX

typedef string st_id;

typedef enum {
    // BASIC DATA TYPES
    INT,
    STRING,
    FLOAT64,

    // EXTENSION DATA TYPES
} st_type;

typedef struct BTNode {
    st_id id;
    st_type type;
    struct BTNode* LPtr;
    struct BTNode* RPtr;
} BTNode_t;

typedef struct {
    BTNode_t* a[ST_MAXSTACK];
    int top;
} tStackP;

#define st_construct(symtable) { \
            BTNode_t* symtable = NULL;  \
}
// Symtable must be new non-existing BTNode_t* (pointer to struct BTNode) named 'symtable'
// Otherwise (GCC) compiler prints error message (redefinition, redeclaration of 'symtable')

void st_destruct ( BTNode_t* symtable );
// Destroys symtable

int st_insert ( BTNode_t* symtable, st_id identificator, st_type datatype );
// Inserts new node with key 'identificator' and data 'datatype' in symtable 'symtable'.
// Returns >0 if ok, 0 if insert failed (malloc, ..), <0 if 'identificator' already exists in 'symtable'

int st_delete ( BTNode_t* symtable, st_id identificator );
// Returns >0 if ok, 0 if remove failed, because 'identificator' isnt present in 'symtable'

BTNode_t* st_lookup ( BTNode_t* symtable, st_id identificator );
// Looks up for 'identificator' in 'symtable'
// Returns pointer to node containing 'identificator' key or NULL if not found.

st_type st_get_type ( BTNode_t* id_node );
// Returns type of 'id_node' identificator, which is expected to be != NULL (or expect undefined behaviour)

void st_set_type ( BTNode_t* id_node, st_type datatype );
// Sets type of 'id_node" identificator to 'datatype'
// Notes: id_node should be NULL and datatype should be from D_TYPE enum (for both cases: expect undefined behaviour)


/*
 * FUNCTIONS (PROTOTYPES) FROM IAL-DU2 (STACK)
 */

void SInitP (tStackP *S);
// Initializes stack

void SPushP (tStackP *S, BTNode_t* ptr);
// Pushes 'ptr' to stack 'S'

BTNode_t* STopPopP (tStackP *S);
// Pops last pointer from stack 'S'

bool SEmptyP (tStackP *S);
// Returns bool value (Is stack empty?)

#endif //SYMTABLE_H
