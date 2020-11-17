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


void stConstruct ( BTNodePtr *symtable ) {
    if (!(*symtable)) {    // Symtable is alredy NULL, no need to do anything
        return;
    }
    (*symtable) = NULL;    // Symtable contained some mem garbage, set to NULL
}

/**
 * @brief Helper function for destructor
 * @param ptr Pointer to the node
 * @param stack Pointer to the stack
 * @post Fills stack with nodes -> all leftmost nodes
 */
void destructLeftmost ( BTNodePtr ptr, tStackP* stack ) {
    if(!ptr) {	// Not found - NULL protection
		return;
	}

    while(ptr) {// Go through the whole tree to the left
		SPushP(stack, ptr);
		ptr = ptr->LPtr;
	}
}

void stDestruct ( BTNodePtr *symtable ) {
    tStackP* stack = (tStackP*) calloc(sizeof(tStackP), 1);
    if (!stack) {
        fprintf(stderr, "Cannot destruct symtable due to internal error. [malloc-stack]\n");
        return;
    }

    SInitP(stack);
    destructLeftmost((*symtable), stack);

    while (!SEmptyP(stack)) {                       // Process every node in stack
        BTNodePtr deleting = STopPopP(stack);
		destructLeftmost(deleting->RPtr, stack);	// Check right subtree
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

int stInsert ( BTNodePtr *symtable, stID identificator, stType datatype ) {
    BTNodePtr new = (BTNodePtr) calloc(sizeof(struct BTNode), 1);
    if (!new) { 
        return ST_ERROR; 
    }

    new->RPtr = NULL;
    new->LPtr = NULL;
    new->type = datatype;
    new->id = identificator;

    if (!(*symtable)) {
        (*symtable) = new;
        return ST_SUCCESS;
    }

    BTNodePtr temp = (*symtable);

    while (temp) {
        if (sortStrings(temp->id, identificator) > 0) {
            if (!temp->LPtr) {
                temp->LPtr = new;
                return ST_SUCCESS;
            }
            temp = temp->LPtr;
        } else if (sortStrings(temp->id, identificator) < 0) {
            if (!temp->RPtr) {
                temp->RPtr = new;
                return ST_SUCCESS;
            }
            temp = temp->RPtr;
        } else {
            free(new);
            return ST_ID_EXISTS;
        }
    }

    return ST_SUCCESS;
}

BTNodePtr stLookUp ( BTNodePtr *symtable, stID identificator ) {
    if (!(*symtable)) {
        return NULL;
    }

    BTNodePtr temp = (*symtable);

    while (temp) {
        if (sortStrings(temp->id, identificator) > 0) {         //left
            temp = temp->LPtr;
        } else if (sortStrings(temp->id, identificator) < 0) {  //right
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
void deleteReplaceByRightmost ( BTNodePtr PtrReplaced, BTNodePtr *RootPtr ) {
    // Stop conditon - END // STOP SOMETHING WENT HORRIBLY WRONG
	if (!(*RootPtr)) {
		return;
	}

	BTNodePtr rm = NULL;

	if (!(*RootPtr)->RPtr) {	// IS THE RIGHTMOST (NO RIGHT SUB TREE EXISTS)
		rm = (*RootPtr);
		PtrReplaced->id = rm->id;
        PtrReplaced->type = rm->type;
		free((*RootPtr));
		(*RootPtr) = rm->LPtr;
		return;
	}
	deleteReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);	// LOOK MORE TO THE RIGHT
}

int stDelete ( BTNodePtr *symtable, stID identificator ) {
    // Stop conditon - NOT FOUND
	if (!(*symtable)) {
		return ST_ERROR;
	}

	if (sortStrings((*symtable)->id, identificator) > 0) {			// Key in left subtree
		return stDelete(&(*symtable)->LPtr, identificator);
	} else if (sortStrings((*symtable)->id, identificator) < 0) {	// Key in right subtree
		return stDelete(&(*symtable)->RPtr, identificator);
	} else { // Found key
		if (!(*symtable)->LPtr && !(*symtable)->RPtr) {	// No subtrees in key node
			free((*symtable));
			(*symtable) = NULL;
			return ST_SUCCESS;
		}
		if (!(*symtable)->LPtr &&  (*symtable)->RPtr) {	// Only right subtree
			BTNodePtr child = (*symtable)->RPtr;
			free((*symtable));
			(*symtable) = child;
			return ST_SUCCESS;
		}
		if (!(*symtable)->RPtr &&  (*symtable)->LPtr) {	// Only left subtree
			BTNodePtr child = (*symtable)->LPtr;
			free((*symtable));
			(*symtable) = child;
			return ST_SUCCESS;
		}
		if ((*symtable)->LPtr && (*symtable)->LPtr) {		// Both subtrees
			deleteReplaceByRightmost((*symtable), &(*symtable)->LPtr);
			return ST_SUCCESS;
		}
	}
    return ST_ERROR;
}

stType stGetType ( BTNodePtr idNode ) {
    if(!idNode) {
        return UNKNOWN;
    }
    return idNode->type;
}

void stSetType ( BTNodePtr idNode, stType datatype ) {
    if(!idNode) {
        return;
    }
    idNode->type = datatype;
    return;
}

void SInitP (tStackP *S)
{
    S->top = 0;
}

void SPushP (tStackP *S, BTNodePtr ptr)
{
    /* Při implementaci v poli může dojít k přetečení zásobníku. */
    if (S->top == ST_MAXSTACK)
        printf("ERROR: Pointer stack overflow.\n");
    else {
        S->top++;
        S->a[S->top]=&ptr;
    }
}

BTNodePtr STopPopP (tStackP *S)
{
    /* Operace nad prázdným zásobníkem způsobí chybu. */
    if (S->top==0)  {
        printf("ERROR: Pointer stack overflow.\n");
        return(NULL);
    }
    else {
        return *(S->a[S->top--]);
    }
}

bool SEmptyP (tStackP *S)
{
    return(S->top==0);
}
