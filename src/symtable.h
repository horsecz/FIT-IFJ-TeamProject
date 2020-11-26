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
#include "returns.h"

/**
 * @brief Max size of the stack
 */
#define ST_MAXSTACK CHAR_MAX

/**
 * @brief Status codes for symtable
 * @enum stCodes
 */
typedef enum stExitCodes {
    ST_SUCCESS = 1,                     /**< Success                                */
    ST_ERROR = 0,                       /**< Error                                  */
    ST_ID_EXISTS = -1                   /**< Already exists                         */
} stC;

/**
 * @brief Identifier 
 */
typedef char* stID;

/**
 * @brief Enum containing node types
 * @enum stNodeType
 */
typedef enum stNodeType {
    ST_N_UNDEFINED,                     /**< Not defined (internal)                 */

    ST_N_FUNCTION,                      /**< Function node                          */
    ST_N_VARIABLE,                      /**< Variable node                          */
}stNType;

/**
 * @brief Enum containing variable data types
 * @enum stVarTypes
 */
typedef enum stVarTypes {
    UNKNOWN,                            /**< [INTERNAL] Internal only               */

    INT,                                /**< [BASIC] Integer data type              */
    STRING,                             /**< [BASIC] String data type               */
    FLOAT64,                            /**< [BASIC] Float data type                */
    BOOL                                /**< [BOOLTHEN] bool data type              */
} stVarType;

/**
 * @brief Structure holding data about a function
 * @enum stData
 */
typedef struct stFunctionData {
    stID        identifier;             /**< Function ID                            */
    int         returnNum;              /**< Number of return types/values          */
    stVarType   returnType[CHAR_MAX];   /**< Function return type                   */
    int         paramNum;               /**< Number of parameters function takes    */
    stVarType   paramTypes[CHAR_MAX];   /**< Parameter types                        */
    bool        defined;                /**< Is function defined?                   */
    struct stNode** innerSymtable;      /**< Inner symtable (is OK?)                */
} stFData;

/**
 * @brief Sturcutre holding data about a variable
 * @enum stData
 */
typedef struct stVariableData {
    stID        identifier;             /**< Variable ID                            */
    stVarType   type;                   /**< Variable type (int, string, float64)   */
    bool        defined;                /**< Is variable already defined?           */
    bool        fncCall;                /**< Not a variable but a function call     */
} stVData;

/**
 * @brief Structure for the node of the tree
 */
typedef struct stNode {
    stID        identifier;             /**< Identificator of the node              */
    struct stFunctionData* fData;       /**< Pointer to struct holding fnc data     */
    struct stVariableData* vData;       /**< Pointer to struct holding var data     */
    struct stNode* predecessor;         /**< NULL if GST (global symtable)
                                             -> functions symtable, in case of 
                                             variables symtable (scopes and subscopes)
                                             this will point to function node that
                                             owns this scope                        */
    struct stNode* LPtr;                /**< Pointer to the left subtree            */
    struct stNode* RPtr;                /**< Pointer to the right subtree           */
} *stNodePtr;

/**
 * @brief Stack structure (support structure for symtable implementation)
 */
typedef struct stStackT {
    stNodePtr*  a[ST_MAXSTACK];         /**< Stack - storing nodes from the tree    */
    int         top;                    /**< Indicate top of the stack - 0 => empty */
} stStack;

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                 GENERAL SYMTABLE FUNCTIONS                    *
 *                                                               * 
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Initialize symtable
 * @param symtable Pointer to the symtable
 * @post symtable will be initilized to NULL
 */
void stConstruct ( stNodePtr *symtable );

/**
 * @brief Destroy symtable
 * @note Destroy & free all nodes of the symtable 
 * @param symtable Pointer to the symtable
 */
void stDestruct ( stNodePtr *symtable );

/**
 * @brief Insert new node to the symtable
 * @param symtable Pointer to the symtable
 * @param identificator New node identificatior
 * @param nodeType Type of the node (function or variable)
 * @param datatype Datatype saved in the new node (variable type or function return type)
 * @return ST_SUCCESS Everything went OK, requested node was deleted
 * @return ST_ERROR Something went wrong (allocation problems or other)
 * @return ST_ID_EXISTS Node alredy exists in the symtbale
 */
stC stInsert ( stNodePtr *symtable, stID identificator, stNType nodeType, stVarType datatype );

/**
 * @brief Delete node from the symtable
 * @param symtable Pointer to the symtable
 * @param identificator Identificator of the node to be deleted
 * @return ST_SUCCESS Everything went OK, requested node was deleted
 * @return ST_ERROR Something went wrong (not found or other problems)
 */
stC stDelete ( stNodePtr *symtable, stID identificator );

/**
 * @brief Find node in the symtable 
 * @param symtable Pointer to the symtable
 * @param identificator Identificator of node that we are looking for
 * @return stNodePtr* Pointer to the note if found else NULL
 */
stNodePtr stLookUp ( stNodePtr *symtable, stID identificator );

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                ADVANCED SYMTABLE FUNCTIONS                    *
 *                                                               * 
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   SETTERS                                                     *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Add or change return type of the function \n
 *        If you are changing return type of the function you have to indicate its position
 *        as the returns are position sensitive, also you cannot change datatype furhter in
 *        the array if you have not defined earlir ones (ex. function return has only one return
 *        type and you try to change 3rd or later -> second is not defined)
 * @note Changing of the already defined return types probably won't be used 'cause of rules for IFJ20 language
 * @param stNode Pointer to the selected node of the symtable
 * @param datatype Datatype to be set from enum stDataTypes
 * @param position Position of the function return type in returnType array (-1 if you are adding new one)                 
 * @pre stNode is pointer to existing function node of the symtable
 * @pre datatype is from the enum stDataTypes
 * @pre position is -1 for new return type or withing range of defined return types
 * @post returnType array modified according to request (if everything is OK)
 * @post nothing changed if something is not OK (INFO on stderr)
 */
void stFncSetType ( stNodePtr stNode, stVarType datatype, int position );

/**
 * @brief Add or change type of the variable 
 * @param stNode Pointer to the selected node of the symtable
 * @param datatype Datatype to be set from enum stDataTypes
 * @pre stNode is pointer to existing variable node of the symtable
 * @pre datatype is from the enum stDataTypes
 */
void stVarSetType ( stNodePtr stNode, stVarType datatype );

/**
 * @brief Add new parameter to function
 * @param stNode Pointer to the selected node of the symtable (has to be function node!)
 * @param paramType Parameter type
 */
void stFncSetParam ( stNodePtr stNode, stVarType paramType );

/**
 * @brief Set function as defined or undefined inside symtable for functions
 * @param stNode Pointer to the selected node of the symtable (has to be function node!)
 * @param defined Is defined or not?
 */
void stFncSetDefined ( stNodePtr stNode, bool defined );

/**
 * @brief Assign inner symtable to function
 * @param stNode Pointer to the selected node of the symtable (has to be function node!)
 * @param symtable Pointer to symtable
 */
void stFncSetInnerSt ( stNodePtr stNode, stNodePtr* symtable );

/**
 * @brief Set/unset variable as an function call in variables symtable
 * @param stNode Pointer to the selected node of the symtable (has to be variable node!)
 * @param fncCall Is fncCall?
 */
void stVarSetFncCall ( stNodePtr stNode, bool fncCall );

 /*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
  *   GETTERS                                                     *
  *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Get type of the node \n
 *        Three types exists (UNDEFINED, VARIABLE & FUNCTION) 
 * @param stNode Pointer to the selected node of the symtable
 * @return stNodeType pointer to existing node
 */
stNType stGetNodeType ( stNodePtr stNode );

/**
 * @brief Get array of return types for the function
 * @param stNode Pointer to the selected node of the symtable
 * @return stVarType Array of return types defined for this function (position sensitive)
 * @return NULL will be returned if node is not a function node! TAKE NOTE!
 * @pre stNode is pointer to existing node of the symtable
 */
stVarType* stFncGetType ( stNodePtr stNode );

/**
 * @brief Get type of the selected node
 * @param stNode Pointer to the selected node of the symtable
 * @return stVarType Type that was saved in the node
 * @pre stNode is pointer to existing node of the symtable
 */
stVarType stVarGetType ( stNodePtr stNode );

/**
 * @brief Gets information if function was already defined or not from symtable 
 * @param stNode Pointer to the selected node of the symtable
 * @return true Function is defined
 * @return false Function is not defined
 */
bool stDefined ( stNodePtr stNode );

/**
 * @brief Get number of parameters the function has * 
 * @param stNode Pointer to the selected node of the symtable (has to be function node!)
 * @return int Number of parameters (-1 if error)
 */
int stFncGetNumParams ( stNodePtr stNode );

/**
 * @brief Get parameter types array of the function (use @see stFncGetNumParams to get number of params) 
 * @param stNode Pointer to the selected node of the symtable (has to be function node!)
 * @return stVarType* Array of parameter types
 */
stVarType* stFncGetParams ( stNodePtr stNode );

/**
 * @brief Get inner symtable of the function 
 * @param stNode Pointer to the selected node of the symtable (has to be function node!)
 * @return stNodePtr* Pointer to inner symtable
 */
stNodePtr* stFncGetInnerSt ( stNodePtr stNode);

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *               SYMBOLTABLE STACK FUNCTIONS                     *
 *                                                               *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   WORK WITH STACK AND SYMTABLE                                *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Initiate stack of symboltables
 * @param stack Pointer to the stack
 * @param symtable Pointer to the root symboltable
 * @return eRC Return code (RC_OK || RC_WRN_INTERNAL || RC_ERR_INTERNAL)
 */
eRC stackStInit ( stStack *stack, stNodePtr *symtable );

/**
 * @brief Destruct stack of symtables 
 * @param stack Pointer to the stack
 * @return eRC Return code (RC_OK || RC_WRN_INTERNAL || RC_ERR_INTERNAL)
 */
eRC stackStDesctruct ( stStack *stack );

/**
 * @brief Get pointer to the symtable on the top of the stack
 * @note This will not remove the symtable from the stack 
 * @param stack Pointer to the stack
 * @return stNodePtr* Pointer to the symtable on the top of the stack
 */
stNodePtr* stackGetTopSt ( stStack *stack );

/**
 * @brief  Get pointer to the symtable on the bottom of the stack 
 * @note This will not remove the symtable from the stack 
 * @param stack Pointer to the stack
 * @return stNodePtr* Pointer to the symtable on the bottom of the stack
 */
stNodePtr* stackGetBotSt ( stStack *stack );

/**
 * @brief Push a new symtable to the top of the stack
 * @param stack Pointer to the stack
 * @param symtable Pointer to the symtable
 * @return eRC Return code (RC_OK || RC_WRN_INTERNAL || RC_ERR_INTERNAL)
 */
eRC stackPushSt ( stStack *stack, stNodePtr *symtable );

/**
 * @brief Pop symtable from the stack
 * @note This will REMOVE symtable from the top of the stack
 * @param stack Pointer to the stack
 * @return stNodePtr* Pointer to the symtable poped from the stack
 */
stNodePtr* stackPopSt ( stStack *stack );

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   WORK SYMTABLE ON THE TOP OF THE STACK                       *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Classic symtable insert function (using stack of symtable instead of symtable itself)
 * @note This will work with symtable on the top of the stack
 * @param stack Pointer to the stack
 * @param identifier Identifier of function or variable (char*)
 * @param nodeType Type of the node (FUNCTION, VARIABLE or UNDEFINED)
 * @param datatype Datatype of the variable (use UNKNOWN in case of working with FUNCTIONS)
 * @return eRC Return code (RC_OK || RC_WRN_INTERNAL || RC_ERR_INTERNAL)
 */
eRC stStackInsert ( stStack *stack, stID identifier, stNType nodeType, stVarType datatype );

/**
 * @brief Classic symtable delete function (using stack of symtable instead of symtable itself)
 * @note This will work with symtable on the top of the stack 
 * @param stack Pointer to the stack
 * @param identifier Identifier of function or variable (char*)
 * @return eRC Return code (RC_OK || RC_WRN_INTERNAL || RC_ERR_INTERNAL)
 */
eRC stStackDelete ( stStack *stack, stID identifier );

/**
 * @brief Classic symtable lookup function (using stack of symtable instead of symtable itself)
 * @note This will work with symtable on the top of the stack  
 * @param stack Pointer to the stack
 * @param identificator Identifier of function or variable (char*)
 * @return stNodePtr Pointer to the node of the symtable (function or variable)
 */
stNodePtr stStackLookUp ( stStack *stack, stID identificator );

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                       STACK  HELPER                           * 
 *                                                               *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Initialize stack
 * @note This function is copied from c402 task of the IAL-DU2
 * @param S Pointer to the stack (uninitialized)
 */
void SInitP (stStack *S);

/**
 * @brief Push node to the stack
 * @note This function is copied from c402 task of the IAL-DU2
 * @param S Pointer to the stack (initialized)
 * @param ptr Pointer to the node to be put on the stack
 */
void SPushP (stStack *S, stNodePtr ptr);

/**
 * @brief Pop node from the top of the stack
 * @note This function is copied from c402 task of the IAL-DU2
 * @param S Pointer to the stack (initialized)
 * @return stNodePtr* Pointer to the node of the symtable that was saved at the top of the stack
 */
stNodePtr STopPopP (stStack *S);

/**
 * @brief Check if stack is empty
 * @note This function is copied from c402 task of the IAL-DU2
 * @param S Pointer to the stack
 * @return true Stack is empty
 * @return false Stack is not empty
 */
bool SEmptyP (stStack *S);

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                  HELPER & OTHER FUNCTIONS                     * 
 *                                                               *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Display BST
 * @note Code from IAL C402
 * @param symtable Pointer to the symtable
 */
void displayBST( stNodePtr symtable );

/**
 * @brief Display BST
 * @note Code from IAL C402
 * @param symtable Pointer to the symtable
 * @param sufix 
 * @param fromdir 
 */
void displayBST2( stNodePtr symtable, char* sufix, char fromdir );

#endif //IFJ_PROJECT_SYMTABLE_H
