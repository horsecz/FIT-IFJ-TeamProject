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
                        }                               \

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

    new->predecessor = NULL;
    new->RPtr = NULL;
    new->LPtr = NULL;
    new->identifier = identificator;
    if (nodeType == ST_N_FUNCTION) {
        new->fData = calloc(sizeof(stFData), 1);
        new->vData = NULL;
        new->fData->identifier = identificator;
        new->fData->returnNum = 0;
        new->fData->defined = false;
        new->fData->paramNum = 0;
        new->fData->innerSymtable = NULL;
    } else if (nodeType == ST_N_VARIABLE) {
        new->fData = NULL;
        new->vData = calloc(sizeof(stVData), 1);
        new->vData->identifier = identificator;
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
        if (sortStrings(temp->identifier, identificator) > 0) {         // Left
            temp = temp->LPtr;
        } else if (sortStrings(temp->identifier, identificator) < 0) {  // Right
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
		if ((*symtable)->LPtr && (*symtable)->RPtr) {	// Both subtrees
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

void stFncSetType ( stNodePtr stNode, stVarType datatype, int position ) {
    if (stNode && stNode->fData) {
        if (position == -1) {
            stNode->fData->returnType[0] = datatype;
            return;
        } else if (stNode->fData->returnNum >= position) {
            stNode->fData->returnType[position] = datatype;
            return;
        }
    }
    iPrint(RC_WRN_INTERNAL, false, "[SYMTABLE] Invalid parameters in stFncSetType call");
}

void stVarSetType ( stNodePtr stNode, stVarType datatype ) {
    if (stNode && stNode->vData) {
        stNode->vData->type = datatype;
        return;
    }
    iPrint(RC_WRN_INTERNAL, false, "[SYMTABLE] Invalid node sent during stVarSetType call");
}

void stFncSetParam ( stNodePtr stNode, stVarType paramType ) {
    if (stNode && stNode->fData) {
        stNode->fData->paramTypes[stNode->fData->paramNum] = paramType;
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

stNType stGetNodeType ( stNodePtr stNode ) {
    if (stNode) {
        if (stNode->fData) {
            return ST_N_FUNCTION;
        } else if (stNode->vData) {
            return ST_N_VARIABLE;
        } else {
            return ST_N_UNDEFINED;
        }
    }
    iPrint(RC_WRN_INTERNAL, false, "[SYMTABLE] Invalid node sent during stGetNodeType call");
    return ST_N_UNDEFINED;
}

stVarType* stFncGetType ( stNodePtr stNode ) {
    if (stNode && stNode->fData) {
        return stNode->fData->returnType;
    }
    return NULL;
}

stVarType stVarGetType ( stNodePtr stNode ) {
    if (stNode && stNode->vData) {
        return stNode->vData->type;
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
 *               SYMBOLTABLE STACK FUNCTIONS                     * 
 *                                                               *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

eRC stackStInit ( stStack *stack, stNodePtr *symtable ) {
    if (!stack && !(*symtable)) {
        iPrint(RC_WRN_INTERNAL, false, "[SYMTABLE][STACK] Invalid arguments for stackStInit call");
        return RC_WRN_INTERNAL;
    }
    SInitP(stack);
    if (stack->top == ST_MAXSTACK) {
        return RC_ERR_INTERNAL;
    }
    SPushP(stack, (*symtable));
    return RC_OK;
}

eRC stackStDesctruct ( stStack *stack ) {
    if (!stack) {
        iPrint(RC_WRN_INTERNAL, false, "[SYMTABLE][STACK] Invalid arguments for stackStDesctruct call");
        return RC_WRN_INTERNAL;
    }
    while(stack->top >= 0) {
        stDestruct(stack->a[stack->top--]);  // Possible segfault here
    }
    return RC_OK;
}

stNodePtr* stackGetTopSt ( stStack *stack ) {
    if (!stack) {
        iPrint(RC_WRN_INTERNAL, false, "[SYMTABLE][STACK] Invalid arguments for stackGetTopSt call");
        return NULL;
    }
    return (stack->top >= 0) ? stack->a[stack->top] : NULL;
}

stNodePtr* stackGetBotSt ( stStack *stack ) {
    if (!stack) {
        iPrint(RC_WRN_INTERNAL, false, "[SYMTABLE][STACK] Invalid arguments for stackGetBotSt call");
        return NULL;
    }
    return (stack->top >= 0) ? stack->a[0] : NULL;
}

eRC stackPushSt ( stStack *stack, stNodePtr *symtable ) {
    if (!stack && !(*symtable)) {
        iPrint(RC_WRN_INTERNAL, false, "[SYMTABLE][STACK] Invalid arguments for stackPushSt call");
        return RC_WRN_INTERNAL;
    }
    if (stack->top >= 0) {
        (*symtable)->predecessor = (*stackGetTopSt(stack));
    }
    SPushP(stack, (*symtable));
    return RC_OK;
}

stNodePtr* stackPopSt ( stStack *stack ) {
    if (!stack) {
        iPrint(RC_WRN_INTERNAL, false, "[SYMTABLE][STACK] Invalid arguments for stackPopSt call");
        return NULL;
    }
    return (stack->top >= 0) ? stack->a[stack->top--] : NULL;
}

eRC stStackInsert ( stStack *stack, stID identifier, stNType nodeType, stVarType datatype ) {
    stC status = stInsert(stackGetTopSt(stack), identifier, nodeType, datatype);
    if (status != ST_SUCCESS) {
        iPrint(RC_WRN_INTERNAL, false, "[SYMTABLE][STATUS] Something went wrong during stStackInsert call");
    }
    return (status == ST_SUCCESS) ? RC_OK : RC_WRN_INTERNAL;
}

eRC stStackDelete ( stStack *stack, stID identifier ) {
    stC status = stDelete(stackGetTopSt(stack), identifier);
    if (status != ST_SUCCESS) {
        iPrint(RC_WRN_INTERNAL, false, "[SYMTABLE][STATUS] Something went wrong during stStackDelete call");
    }
    return (status == ST_SUCCESS) ? RC_OK : RC_WRN_INTERNAL;
}

stNodePtr stStackLookUp ( stStack *stack, stID identificator ) {
    return stLookUp(stackGetTopSt(stack), identificator);
}

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   WORK WITH WHOLE STACK                                       *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

stVarType stVarTypeLookUp ( stStack *stack, stID identificator ) {
    if (!stack) {
        return UNKNOWN;
    }
    for (int i = stack->top; i > 0; i--) {
        if (!stLookUp(stack->a[i], identificator)) {
            continue;
        }
        return stVarGetType(stLookUp(stack->a[i], identificator));
    }
    return UNKNOWN;
}

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                       STACK  HELPER                           * 
 *                                                               *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

void SInitP (stStack *S)
{
    S->top = -1;
}

void SPushP (stStack *S, stNodePtr ptr)
{
    /* Při implementaci v poli může dojít k přetečení zásobníku. */
    if (S->top == ST_MAXSTACK) {
        iPrint(RC_ERR_INTERNAL, true, "[STACK] Overflow");
        return;
    } else {
        S->top++;
        S->a[S->top]=&ptr;
    }
}

stNodePtr STopPopP (stStack *S)
{
    if (S->top == -1)  {
        iPrint(RC_ERR_INTERNAL, true, "[STACK] Empty");
        return NULL;
    }
    else {
        return *(S->a[S->top--]);
    }
}

bool SEmptyP (stStack *S)
{
    return (S->top == -1);
}

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                  HELPER & OTHER FUNCTIONS                     * 
 *                                                               *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

void displayBST(stNodePtr symtable) {
    fprintf(stderr, "[SYM]=================================================\n");
    fprintf(stderr, "[SYM]\t\tBinary tree structure\n");
    fprintf(stderr, "[SYM]=================================================\n\n");
    if (symtable != NULL) {
        displayBST2(symtable, "", 'X');\
    } else {
        fprintf(stderr, "[SYM]Tree is empty\n");
    }
    fprintf(stderr, "\n[SYM]=================================================\n");
}

void displayBST2( stNodePtr symtable, char* sufix, char fromdir ) {
    char* types[] = {
        "UNKNOWN",
        "INT",
        "STRING",
        "FLOAT64",
        "BOOL"
    };
    if (symtable != NULL) {
        char* suf2 = (char*) malloc(strlen(sufix) + 4);
        strcpy(suf2, sufix);
        if (fromdir == 'L') {
            suf2 = strcat(suf2, "  |");
            fprintf(stderr, "%s\n", suf2);
	    } else {
	        suf2 = strcat(suf2, "   ");
        }
        displayBST2(symtable->RPtr, suf2, 'R');
        if (symtable->fData){
            fprintf(stderr, "%s  +- func %s", sufix, symtable->identifier);
            fprintf(stderr, "(");
            for(int i = 0; i < symtable->fData->paramNum; i++) {
                fprintf(stderr, "%s ", types[symtable->fData->paramTypes[i]]);
            }
            fprintf(stderr, ")(");
            for(int i = 0; i < symtable->fData->returnNum; i++) {
                fprintf(stderr, "%s ", types[symtable->fData->returnType[i]]);
            }
            fprintf(stderr, ") {}\n");
        } else if (symtable->vData) {
            fprintf(stderr, "%s  +- %s %s", sufix, types[symtable->vData->type], symtable->identifier);
        } else {
            fprintf(stderr, "%s  +- %s", sufix, symtable->identifier);
        }
        strcpy(suf2, sufix);
        if (fromdir == 'R') {
            suf2 = strcat(suf2, "  |");
        } else {
            suf2 = strcat(suf2, "   ");
        }
        displayBST2(symtable->LPtr, suf2, 'L');
        if (fromdir == 'R') {
            fprintf(stderr, "%s\n", suf2);
        }
        free(suf2);
    }
}