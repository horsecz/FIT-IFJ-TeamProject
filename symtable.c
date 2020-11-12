/**
 *  @note   IFJ: Implementace interpreta jazyka IFJ20
 *	@file	symtable.c
 *	@author Dominik Horky, FIT
 *	@brief  Modul symtable je hlavnim modulem pracujicicm s binarnim stromem, ktery je ADT pro tabulku symbolu.
 *	@note	Reseni IFJ-proj, tabulka symbolu
 *	@note   Pro vypracovani tohoto modulu byly vyuzity funkce z IAL-DU2 (modul C402 - preddefinovane i mnou vypracovane)
 */

#include "symtable.h"


void st_construct ( BTNode_t** symtable ) {
    *symtable = NULL;
}

void destruct_leftmost ( BTNode_t** ptr, tStackP* stack) {
    while (*ptr != NULL) {
        SPushP(stack, *ptr);
        *ptr = (*ptr)->LPtr;
    }
}

void st_destruct ( BTNode_t** symtable ) {
    tStackP* stack = (tStackP*) calloc(sizeof(tStackP), 1);
    if (stack == NULL) {
        fprintf(stderr, "Cannot destruct symtable due to internal error. [malloc-stack]\n");
        return;
    }

    SInitP(stack);
    destruct_leftmost(&(*symtable), stack);

    while (!SEmptyP(stack)) {
        *symtable = STopPopP(stack);
        if ((*symtable)->RPtr != NULL)
            destruct_leftmost(&(*symtable)->RPtr, stack);

        free(*symtable);
        *symtable = NULL;
    }

    free(stack);
}

int sortStrings ( st_id fst, st_id snd ) {
    int fst_s = strlen(fst);
    int snd_s = strlen(snd);
    int cmp = strcmp(fst, snd);

    if (!cmp)
        return 0;

    for(int i = 0; i < fst_s && i < snd_s; i++) {
        if (fst[i] > snd[i]) {
            return 1;
        }
        else if (fst[i] < snd[i]) {
            return -1;
        }
    }

    if (fst_s > snd_s) {
        return 1;
    }
    else if (fst_s < snd_s) {
        return -1;
    }

    return 0;
}

int st_insert ( BTNode_t** symtable, st_id identificator, st_type datatype ) {
    BTNode_t* new = (BTNode_t*) calloc(sizeof(BTNode_t), 1);
    if (new == NULL) { return ST_ERROR; }

    new->RPtr = NULL;
    new->LPtr = NULL;
    new->type = datatype;
    new->id = identificator;


    if (*symtable == NULL) {
        *symtable = new;
        return ST_SUCCESS;
    }

    if (!strcmp((*symtable)->id, new->id)) {
        free(new);
        return ST_ID_EXISTS;
    }

    BTNode_t* temp = *symtable;
    int st_sort = 0;

    while (temp->LPtr != NULL || temp->RPtr != NULL) {
        st_sort = sortStrings(temp->id, new->id);
        if (temp->LPtr != NULL && (st_sort > 0))
            temp = temp->LPtr;
        else if (temp->RPtr != NULL && (st_sort < 0 ))
            temp = temp->RPtr;
        else if (st_sort == 0) {
            free(new);
            return ST_ID_EXISTS;
        }
        else
            break;
    }

    st_sort = sortStrings(temp->id, new->id);
    if (st_sort > 0)
        temp->LPtr = new;
    else if (st_sort < 0)
        temp->RPtr = new;
    else {
        free(new);
        return ST_ID_EXISTS;
    }

    return ST_SUCCESS;
}

BTNode_t* st_lookup ( BTNode_t** symtable, st_id identificator ) {
    BTNode_t* temp = *symtable;
    int st_sort = 0;

    while (temp != NULL) {
        st_sort = sortStrings(temp->id, identificator);
        if (st_sort > 0) //left
            temp = temp->LPtr;
        else if (st_sort < 0) //right
            temp = temp->RPtr;
        else
            break;
    }

    return temp;
}

void delete_ReplaceByRightmost (BTNode_t* PtrReplaced, BTNode_t** RootPtr) {
    while ((*RootPtr) != NULL && (*RootPtr)->RPtr != NULL)
        *RootPtr = (*RootPtr)->RPtr;

    // expecting RootPtr != NULL
    BTNode_t* work = *RootPtr;
    PtrReplaced->type = (*RootPtr)->type;
    PtrReplaced->id = (*RootPtr)->id;
    free(work);
    work = NULL;
    return;
}

int st_delete ( BTNode_t** symtable, st_id identificator ) {
    BTNode_t* removal = *symtable;
    BTNode_t* removal_p = *symtable;
    int st_sort = 0;


    while (removal != NULL) {
        st_sort = sortStrings((removal)->id, identificator);
        if (st_sort == 0)
            break;

        removal_p = removal;
        if (st_sort > 0) //left
            removal = (removal)->LPtr;
        else if (st_sort < 0) //right
            removal = (removal)->RPtr;
    }

    if (removal == NULL) return ST_ERROR;

    if ((removal)->LPtr == NULL && (removal)->RPtr == NULL) {
        if (removal_p != removal) {
            if (removal_p->RPtr == removal)
                removal_p->RPtr = NULL;
            else
                removal_p->LPtr = NULL;

        } else {
            (*symtable) = NULL;
        }

        free(removal);
        removal = NULL;
    } else if ((removal)->LPtr != NULL && (removal)->RPtr != NULL) {
        BTNode_t* LPtr = removal->LPtr->LPtr;
        delete_ReplaceByRightmost((removal), &(removal)->LPtr);
        (removal)->LPtr = LPtr;
    } else {
        BTNode_t* workLPtr = (removal)->LPtr;
        BTNode_t* workRPtr = (removal)->RPtr;
        BTNode_t* newRoot = NULL;

        if ((removal)->LPtr != NULL) {
            newRoot = workLPtr;
        } else {
            newRoot = workRPtr;
        }

        if (removal_p->LPtr == removal)
            removal_p->LPtr = newRoot;
        else if (removal_p->RPtr == removal)
            removal_p->RPtr = newRoot;

        if (removal != (*symtable)) {
            free(removal);
            removal = NULL;
        } else {
            if (removal->LPtr != NULL) {
                removal_p = removal->LPtr;
            }
            else if (removal->RPtr != NULL) {
                removal_p = removal->RPtr;
            }
            else {
                removal_p = NULL;
            }

            (*symtable) = removal_p;
            free(removal);
        }
    }

    return ST_SUCCESS;
}

st_type st_get_type ( BTNode_t** id_node ) {
    return (*id_node)->type;
}

void st_set_type ( BTNode_t** id_node, st_type datatype ) {
    (*id_node)->type = datatype;
    return;
}

/*
 * FUNCTIONS FROM IAL-DU2 (STACK)
 */

void SInitP (tStackP *S)
{
    S->top = 0;
}

void SPushP (tStackP *S, BTNode_t* ptr)
{
    /* Při implementaci v poli může dojít k přetečení zásobníku. */
    if (S->top == ST_MAXSTACK)
        printf("ERROR: Pointer stack overflow.\n");
    else {
        S->top++;
        S->a[S->top]=ptr;
    }
}

BTNode_t* STopPopP (tStackP *S)
{
    /* Operace nad prázdným zásobníkem způsobí chybu. */
    if (S->top==0)  {
        printf("ERROR: Pointer stack overflow.\n");
        return(NULL);
    }
    else {
        return (S->a[S->top--]);
    }
}

bool SEmptyP (tStackP *S)
{
    return(S->top==0);
}
