/**
 *  @note   IFJ: Implementace interpreta jazyka IFJ20
 *	@file	symtable.h
 *	@author Dominik Horky (xhorky32@stud.fit.vutbr.cz)
 *  @author Roman Janiczek (xjanic25@vutbr.cz)
 *	@brief  Hlavickovy soubor slouzici pro symtable.c
 *	@note	Reseni IFJ-proj, tabulka symbolu
 */

#ifndef IFJ_PROJECT_SYMTABLE_H
#define IFJ_PROJECT_SYMTABLE_H
// General
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

/**
 * @brief Max size of the stack
 */
#define ST_MAXSTACK CHAR_MAX

/**
 * @brief Status codes for symtable
 * @enum stCodes
 */
typedef enum stCodes {
    ST_SUCCESS = 1,     /**< Success */
    ST_ERROR = 0,       /**< Error */
    ST_ID_EXISTS = -1   /**< Already exists */
} stC;

typedef char* stID;

/**
 * @brief Enum containing data types
 * @enum stDataTypes
 */
typedef enum stDataTypes {
    UNKNOWN,    /**< [INTERNAL] Internal only */

    INT,        /**< [BASIC] Integer data type */
    STRING,     /**< [BASIC] String data type */
    FLOAT64,    /**< [BASIC] Float data type */

} stType;

/**
 * @brief Structure for the node of the tree
 */
typedef struct BTNode {
    stID id;                /**< Identificator of the node */
    stType type;            /**< Saved type - refer to enum stDateTypes */
    struct BTNode* LPtr;    /**< Pointer to the left subtree */
    struct BTNode* RPtr;    /**< Pointer to the right subtree */
} *BTNodePtr;

/**
 * @brief Stack structure (support structure for symtable implementation)
 */
typedef struct {
    BTNodePtr* a[ST_MAXSTACK];   /**< Stack - storing nodes from the tree */
    int top;                    /**< Indicate top of the stack - 0 => empty */
} tStackP;

/**
 * @brief Initialize symtable
 * @param symtable Pointer to the symtable
 * @post symtable will be initilized to NULL
 */
void stConstruct ( BTNodePtr *symtable );

/**
 * @brief Destroy symtable
 * @note Destroy & free all nodes of the symtable 
 * @param symtable Pointer to the symtable
 */
void stDestruct ( BTNodePtr *symtable );

/**
 * @brief Insert new node to the symtable
 * @param symtable Pointer to the symtable
 * @param identificator New node identificatior
 * @param datatype Datatype saved in the new node
 * @return ST_SUCCESS Everything went OK, requested node was deleted
 * @return ST_ERROR Something went wrong (allocation problems or other)
 * @return ST_ID_EXISTS Node alredy exists in the symtbale
 */
stC stInsert ( BTNodePtr *symtable, stID identificator, stType datatype );

/**
 * @brief Delete node from the symtable
 * @param symtable Pointer to the symtable
 * @param identificator Identificator of the node to be deleted
 * @return ST_SUCCESS Everything went OK, requested node was deleted
 * @return ST_ERROR Something went wrong (not found or other problems)
 */
stC stDelete ( BTNodePtr *symtable, stID identificator );

/**
 * @brief Find node in the symtable 
 * @param symtable Pointer to the symtable
 * @param identificator Identificator of node that we are looking for
 * @return BTNodePtr* Pointer to the note if found else NULL
 */
BTNodePtr stLookUp ( BTNodePtr *symtable, stID identificator );

/**
 * @brief Get type of the selected node
 * @param stNode Pointer to the selected node of the symtable
 * @return stType Type that was saved in the node
 * @pre stNode is pointer to existing node of the symtable
 */
stType stGetType ( BTNodePtr stNode );

/**
 * @brief Set new datatype to selected node
 * @param stNode Pointer to the selected node of the symtable
 * @param datatype Datatype to be set from enum stDataTypes
 * @pre stNode is pointer to existing node of the symtable
 * @pre datatype is from the enum stDataTypes
 */
void stSetType ( BTNodePtr stNode, stType datatype );

/**
 * @brief Initialize stack
 * @note This function is copied from c402 task of the IAL-DU2
 * @param S Pointer to the stack (uninitialized)
 */
void SInitP (tStackP *S);

/**
 * @brief Push node to the stack
 * @note This function is copied from c402 task of the IAL-DU2
 * @param S Pointer to the stack (initialized)
 * @param ptr Pointer to the node to be put on the stack
 */
void SPushP (tStackP *S, BTNodePtr ptr);

/**
 * @brief Pop node from the top of the stack
 * @note This function is copied from c402 task of the IAL-DU2
 * @param S Pointer to the stack (initialized)
 * @return BTNodePtr* Pointer to the node of the symtable that was saved at the top of the stack
 */
BTNodePtr STopPopP (tStackP *S);

/**
 * @brief Check if stack is empty
 * @note This function is copied from c402 task of the IAL-DU2
 * @param S Pointer to the stack
 * @return true Stack is empty
 * @return false Stack is not empty
 */
bool SEmptyP (tStackP *S);

#endif //IFJ_PROJECT_SYMTABLE_H
