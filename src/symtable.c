/**
 *  @note   IFJ: Implementace interpreta jazyka IFJ20
 *	@file	symtable.c
 *	@author Dominik Horky (xhorky32@stud.fit.vutbr.cz)
 *  @author Roman Janiczek (xjanic25@vutbr.cz)
 *	@brief  Modul symtable je hlavnim modulem pracujicicm s binarnim stromem, ktery je ADT pro tabulku symbolu.
 *	@note	Reseni IFJ-proj, tabulka symbolu
 *	@note   Pro vypracovani tohoto modulu byly vyuzity funkce z IAL-DU2 (modul C402 - preddefinovane i mnou vypracovane)
 */

#include "symtable.h"

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                 GENERAL SYMTABLE FUNCTIONS                    *
 *                                                               * 
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

#define freeData(stVar) if(!(*stVar)->fData) {          \
                            free((*stVar)->fData);      \
                        }                               \
                        if(!(*stVar)->vData) {          \
                            free((*stVar)->vData);      \
                        }                               

void stConstruct ( stNodePtr *symtable ) {
    if (!(*symtable)) {    // Symtable is alredy NULL, no need to do anything
        return;
    }
    (*symtable) = NULL;    // Symtable contained some mem garbage, set to NULL
}

/**
 * @brief Helper function for destructor
 * @param ptr Pointer to the node
 * @param stack Pointer to the stack
 * @post Fills stack with nodes -> all leftmost nodes pushed onto stack
 */
void addLeftmost ( stNodePtr ptr, stStack* stack ) {
    if(!ptr) {	// Not found - NULL protection
		return;
	}

    while(ptr) {// Go through the whole tree to the left
		SPushP(stack, ptr);
		ptr = ptr->LPtr;
	}
}

void stDestruct ( stNodePtr *symtable ) {
    stStack* stack = (stStack*) calloc(sizeof(stStack), 1);
    if (!stack) {
        iPrint(RC_ERR_INTERNAL, true, "Cannot destruct symtable due to internal error. [malloc-stack]");
        return;
    }

    SInitP(stack);
    addLeftmost((*symtable), stack);

    while (!SEmptyP(stack)) {                       // Process every node in stack
        stNodePtr deleting = STopPopP(stack);
		addLeftmost(deleting->RPtr, stack);	// Check right subtree
		free(deleting);
    }
    free(stack);    // Free stack that we allocated
}

/**
 * @brief Helper function for going through the tree according to stID
 * @param fst First string
 * @param snd Second string
 * @return int 0 -> Strings are equal,
 *            -N -> First string is less than second
 *             N -> First string is greater than second
 */
int sortStrings ( stID fst, stID snd ) {
    return strcmp(fst, snd);
}

int stInsert ( stNodePtr *symtable, stID identificator, stNType nodeType, stVarType datatype ) {
    stNodePtr new = (stNodePtr) calloc(sizeof(struct stNode), 1);
    if (!new) { 
        return ST_ERROR; 
    }

    new->RPtr = NULL;
    new->LPtr = NULL;
    new->identifier = identificator;
    if (nodeType == ST_N_FUNCTION) {
        new->fData = calloc(sizeof(stFData), 1);
        new->vData = NULL;
        new->fData->identifier = identificator;
        new->fData->returnType = datatype;
        new->fData->defined = false;
        new->fData->paramNum = 0;
        new->fData->innerSymtable = NULL;
    } else if (nodeType == ST_N_VARIABLE) {
        new->fData = NULL;
        new->vData = calloc(sizeof(stVData), 1);
        new->vData->type = datatype;
        new->vData->defined = false;
        new->vData->fncCall = false;
    } else {
        new->fData = NULL;
        new->vData = NULL;
    }

    if (!(*symtable)) {
        (*symtable) = new;
        return ST_SUCCESS;
    }

    stNodePtr temp = (*symtable);

    while (temp) {
        if (sortStrings(temp->identifier, identificator) > 0) {
            if (!temp->LPtr) {
                temp->LPtr = new;
                return ST_SUCCESS;
            }
            temp = temp->LPtr;
        } else if (sortStrings(temp->identifier, identificator) < 0) {
            if (!temp->RPtr) {
                temp->RPtr = new;
                return ST_SUCCESS;
            }
            temp = temp->RPtr;
        } else {
            freeData(&new);
            free(new);
            return ST_ID_EXISTS;
        }
    }

    return ST_SUCCESS;
}

stNodePtr stLookUp ( stNodePtr *symtable, stID identificator ) {
    if (!(*symtable)) {
        return NULL;
    }

    stNodePtr temp = (*symtable);

    while (temp) {
        if (sortStrings(temp->identifier, identificator) > 0) {         //left
            temp = temp->LPtr;
        } else if (sortStrings(temp->identifier, identificator) < 0) {  //right
            temp = temp->RPtr;
        } else {
            return temp;
        }
    }

    return NULL;
}

/**
 * @brief Helper function - find, replace and free rightmost
 * @param PtrReplaced Node that will be replaced
 * @param RootPtr From this node we look for rightmost
 */
void deleteReplaceByRightmost ( stNodePtr PtrReplaced, stNodePtr *RootPtr ) {
    // Stop conditon - END // STOP SOMETHING WENT HORRIBLY WRONG
	if (!(*RootPtr)) {
		return;
	}

	stNodePtr rm = NULL;

	if (!(*RootPtr)->RPtr) {	// IS THE RIGHTMOST (NO RIGHT SUB TREE EXISTS)
		rm = (*RootPtr);
		PtrReplaced->identifier = rm->identifier;
        if(rm->fData) {
            PtrReplaced->fData = rm->fData;
        }
        if(rm->vData) {
            PtrReplaced->vData = rm->vData;
        }
        // DO A PROPER FREE
        freeData(RootPtr);
		free((*RootPtr));
        // SET NODE
		(*RootPtr) = rm->LPtr;
		return;
	}
	deleteReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);	// LOOK MORE TO THE RIGHT
}

int stDelete ( stNodePtr *symtable, stID identificator ) {
    // Stop conditon - NOT FOUND
	if (!(*symtable)) {
		return ST_ERROR;
	}

	if (sortStrings((*symtable)->identifier, identificator) > 0) {			// Key in left subtree
		return stDelete(&(*symtable)->LPtr, identificator);
	} else if (sortStrings((*symtable)->identifier, identificator) < 0) {	// Key in right subtree
		return stDelete(&(*symtable)->RPtr, identificator);
	} else { // Found key
		if (!(*symtable)->LPtr && !(*symtable)->RPtr) {	// No subtrees in key node
            freeData(symtable);
			free((*symtable));
			(*symtable) = NULL;
			return ST_SUCCESS;
		}
		if (!(*symtable)->LPtr &&  (*symtable)->RPtr) {	// Only right subtree
			stNodePtr child = (*symtable)->RPtr;
            freeData(symtable);
			free((*symtable));
			(*symtable) = child;
			return ST_SUCCESS;
		}
		if (!(*symtable)->RPtr &&  (*symtable)->LPtr) {	// Only left subtree
			stNodePtr child = (*symtable)->LPtr;
            freeData(symtable);
			free((*symtable));
			(*symtable) = child;
			return ST_SUCCESS;
		}
		if ((*symtable)->LPtr && (*symtable)->RPtr) {		// Both subtrees
			deleteReplaceByRightmost((*symtable), &(*symtable)->LPtr);
			return ST_SUCCESS;
		}
	}
    return ST_ERROR;
}

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                ADVANCED SYMTABLE FUNCTIONS                    *
 *                                                               * 
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

void stSetType ( stNodePtr idNode, stVarType datatype ) {
    if(!idNode) {
        return;
    }
    if (idNode->fData) {
        idNode->fData->returnType = datatype;
    }
    if (idNode->vData) {
       idNode->vData->type = datatype;
    }
}

void stFncSetParam ( stNodePtr stNode, stVarType paramType ) {
    if (stNode && stNode->fData) {
        stNode->fData->paramTypes[stNode->fData->paramNum + 1] = paramType;
        stNode->fData->paramNum++;
    }
}

void stFncSetDefined ( stNodePtr stNode, bool defined ) {
    if (stNode) {
        if (stNode->fData) {
            stNode->fData->defined = defined;
        } else if (stNode->vData) {
            stNode->vData->defined = defined;
        }
    }
}

void stFncSetInnerSt ( stNodePtr stNode, stNodePtr* symtable ) {
    if (stNode && stNode->fData) {
        stNode->fData->innerSymtable = symtable;
    }
}

void stVarSetFncCall ( stNodePtr stNode, bool fncCall ) {
    if (stNode && stNode->vData) {
        stNode->vData->fncCall = fncCall;
    }
}

stVarType stGetType ( stNodePtr idNode ) {
    if (!idNode) {
        return UNKNOWN;
    }
    if (idNode->fData) {
        return idNode->fData->returnType;
    }
    if (idNode->vData) {
        return idNode->vData->type;
    }
    return UNKNOWN;
}

bool stDefined ( stNodePtr stNode ) {
    if (stNode) {
        if (stNode->fData) {
            return stNode->fData->defined;
        } else if (stNode->vData) {
            return stNode->vData->defined;
        }
    }
    return false;
}

int stFncGetNumParams ( stNodePtr stNode ) {
    if (stNode && stNode->fData) {
        return stNode->fData->paramNum;
    }
    return -1;
}

stVarType* stFncGetParams ( stNodePtr stNode ) {
    if (stNode && stNode->fData) {
        return stNode->fData->paramTypes;
    }
    return NULL;
}

stNodePtr* stFncGetInnerSt ( stNodePtr stNode) {
    if (stNode && stNode->fData) {
        return stNode->fData->innerSymtable;
    }
    return NULL;
}

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                       STACK  HELPER                           * 
 *                                                               *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

void SInitP (stStack *S)
{
    S->top = 0;
}

void SPushP (stStack *S, stNodePtr ptr)
{
    /* Při implementaci v poli může dojít k přetečení zásobníku. */
    if (S->top == ST_MAXSTACK) {
        iPrint(RC_ERR_INTERNAL, true, "Pointer stack overflow.");
        return;
    } else {
        S->top++;
        S->a[S->top]=&ptr;
    }
}

stNodePtr STopPopP (stStack *S)
{
    /* Operace nad prázdným zásobníkem způsobí chybu. */
    if (S->top==0)  {
        iPrint(RC_ERR_INTERNAL, true, "Pointer stack overflow.");
        return(NULL);
    }
    else {
        return *(S->a[S->top--]);
    }
}

bool SEmptyP (stStack *S)
{
    return(S->top==0);
}
