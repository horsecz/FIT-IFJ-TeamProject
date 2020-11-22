#include "symtable.h"
#define testmsg(text) fprintf(stderr, "--> %s\n", text)

bool detectError = false;

int main(void)
{
    
    /*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
     *                                                               *
     *                 BASIC TESTS FOR SYMTABLE                      *
     *                                                               * 
     *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

    // TEST OF SYMTABLE
    fprintf(stderr, "------------- SYMTABLE TEST -------------");

    // CONSTRUCT & DESTRUCT TEST (EMPTY SYMTABLE)
    fprintf(stderr, "\n-------------> CONSTRUCT, DESTRUCT\n");

    stNodePtr mysym = NULL;
    // -> Construct
    stConstruct(&mysym);
    if (mysym == NULL)
        testmsg("OK: &mysym symtable constructed");
    else
    {
        testmsg("ERR: error in construction of &mysym");
        detectError = true;
    }
    // -> Destruct
    stDestruct(&mysym);
    if (mysym == NULL)
        testmsg("OK: &mysym destructed (nothing happened rly)");
    else
    {
        testmsg("ERR: error in destruction");
        detectError = true;
    }

    // OPERATIONS ON EMPTY SYMTABLE
    fprintf(stderr, "\n-------------> OPERATIONS ON EMPTY SYMTABLE\n");

    // -> Look for non-existing key
    stNodePtr n_res = stLookUp(&mysym, "ABCDE");
    if (n_res == NULL)
        testmsg("OK: string ABCDE not found");
    else
    {
        testmsg("ERR: string ABCDE found and something is rly bad");
        detectError = true;
    }
    // -> Delete non-existing key
    int res = stDelete(&mysym, "ABCDE");
    if (!res)
        testmsg("OK: string ABCDE not found and not deleted");
    else
    {
        testmsg("ERR: string ABCDE found, deleted and something is rly bad");
        detectError = true;
    }

    // VARIABLES
    fprintf(stderr, "\n-------------> VARIABLES\n");

    // FILLING SYMTABLE (VARIABLES)
    // -> Insert one key
    fprintf(stderr, "\n-------------> INSERTING FIRST KEY\n");

    res = stInsert(&mysym, "ABCDE", ST_N_VARIABLE, INT);
    if (res > 0)
        testmsg("OK: id ABCDE with type INT inserted into &mysym symtable");
    else if (res == 0)
    {
        testmsg("ERR: something is wrong, cant insert ABCDE");
        detectError = true;
    }
    else
    {
        testmsg("ERR: cant insert ABCDE because it exists (even it doesnt)");
        detectError = true;
    }
    // -> Insert more keys
    fprintf(stderr, "\n-------------> INSERTING MORE KEYS\n");

    res = stInsert(&mysym, "AAAAA", ST_N_VARIABLE, FLOAT64);
    if (res > 0)
        testmsg("OK: id AAAAA with type FLOAT64 inserted into &mysym symtable");
    else if (res == 0)
    {
        testmsg("ERR: something is wrong, cant insert AAAAA");
        detectError = true;
    }
    else
    {
        testmsg("ERR: cant insert AAAAA because it exists (despite fact that it doesnt)");
        detectError = true;
    }

    res = stInsert(&mysym, "SSSSS", ST_N_VARIABLE, STRING);
    if (res > 0)
        testmsg("OK: id SSSSS with type STRING inserted into &mysym symtable");
    else if (res == 0)
    {
        testmsg("ERR: something is wrong, cant insert SSSSS");
        detectError = true;
    }
    else
    {
        testmsg("ERR: cant insert SSSSS because it exists (despite fact that it doesnt)");
        detectError = true;
    }

    res = stInsert(&mysym, "BCDEF", ST_N_VARIABLE, 0);
    if (res > 0)
        testmsg("OK: id BCDEF with type UNKNOWN inserted into &mysym symtable");
    else if (res == 0)
    {
        testmsg("ERR: cant insert BCDEF due to malloc error");
        detectError = true;
    }
    else
    {
        testmsg("ERR: cant insert BCDEF because it exists (despite fact that it doesnt)");
        detectError = true;
    }

    // OPERATIONS ON NON-EMPTY SYMTABLE (VARIABLES FILLED)
    // -> Look-up
    fprintf(stderr, "\n-------------> LOOKUP OPERATIONS\n");

    n_res = stLookUp(&mysym, "BCDEF");
    if (n_res)
        printf("---> OK: id BCDEF found in symtable with type %d\n", stGetType(n_res));
    else
    {
        testmsg("ERR: something went wrong, id BCDEF not found (even it's there)");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "SSSSS");
    if (n_res)
        printf("---> OK: id SSSSS found in symtable with type %d\n", stGetType(n_res));
    else
    {
        testmsg("ERR: something went wrong, id SSSSS not found (even it's there)");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "AAAAA");
    if (n_res)
        printf("---> OK: id AAAAA found in symtable with type %d\n", stGetType(n_res));
    else
    {
        testmsg("ERR: something went wrong, id AAAAA not found (even it's there)");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "ABCDE");
    if (n_res)
        printf("---> OK: id ABCDE found in symtable with type %d\n", stGetType(n_res));
    else
    {
        testmsg("ERR: something went wrong, id ABCDE not found (even it's there)");
        detectError = true;
    }

    // -> Get/Set
    fprintf(stderr, "\n-------------> GET / SET TYPE\n");

    char resultGetSet[CHAR_MAX];
    sprintf(resultGetSet, "DONE: id ABCDE has type %d", stGetType(n_res));
    testmsg(resultGetSet);
    stSetType(n_res, STRING);
    sprintf(resultGetSet, "DONE: id ABCDE has now type %d", stGetType(n_res));
    testmsg(resultGetSet);

    // -> Insert existing
    fprintf(stderr, "\n-------------> RE-INSERTION\n");

    res = stInsert(&mysym, "ABCDE", ST_N_VARIABLE, 0);
    if (res < 0)
        testmsg("OK: re-insert of ABCDE (which is in table) failed");
    else if (res > 0)
    {
        testmsg("ERR: something went really wrong because ABCDE was inserted again.");
        detectError = true;
    }
    else
    {
        testmsg("ERR: malloc error in insert");
        detectError = true;
    }

    // -> Delete
    fprintf(stderr, "\n-------------> DELETE OF EXISTING KEYS\n");

    res = stDelete(&mysym, "ABCDE");
    if (res)
        testmsg("OK: deleted ABCDE from table");
    else if (!res)
    {
        testmsg("ERR: cant delete ABCDE because it isnt in symtable (even it is)");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "SSSSS");
    if (n_res != NULL)
        testmsg("OK: SSSSS was again found in table and not removed");
    else
    {
        testmsg("ERR: SSSSS wasnt found in table, but it wasnt removed yet");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "BCDEF");
    if (n_res != NULL)
        testmsg("OK: BCDEF was again found in table and not removed");
    else
    {
        testmsg("ERR: BCDEF wasnt found in table, but it wasnt removed yet");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "AAAAA");
    if (n_res != NULL)
        testmsg("OK: AAAAA was again found in table and not removed (ofc, bcs its root)");
    else
    {
        testmsg("ERR: AAAAA wasnt found in table, but it wasnt removed yet");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "ABCDE");
    if (n_res != NULL)
    {
        testmsg("ERR: ABCDE was again found in table but its was removed");
        detectError = true;
    }
    else
        testmsg("OK: ABCDE wasnt found in table");

    res = stDelete(&mysym, "SSSSS");
    if (res)
        testmsg("OK: deleted SSSSS from table");
    else if (!res)
    {
        testmsg("ERR: cant delete SSSSS because it isnt in symtable (even it is)");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "AAAAA");
    if (n_res != NULL)
        testmsg("OK: AAAAA was again found in table and not removed (ofc, bcs its root)");
    else
    {
        testmsg("ERR: AAAAA wasnt found in table, but it wasnt removed yet");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "BCDEF");
    if (n_res != NULL)
        testmsg("OK: BCDEF was again found in table and not removed");
    else
    {
        testmsg("ERR: BCDEF wasnt found in table, but it wasnt removed yet");
        detectError = true;
    }

    res = stDelete(&mysym, "ABCDE");
    if (!res)
        testmsg("OK: not deleted ABCDE from table again, its gone already");
    else if (res)
    {
        testmsg("ERR: deleted ABCDE again (even it was already removed)");
        detectError = true;
    }

    res = stDelete(&mysym, "AAAAA");
    if (res)
        testmsg("OK: deleted AAAAA from table");
    else if (!res)
    {
        testmsg("ERR: cant remove AAAAA (it is not in table)");
        detectError = true;
    }

    res = stDelete(&mysym, "BCDEF");
    if (res)
        testmsg("OK: deleted BCDEF from table");
    else if (!res)
    {
        testmsg("ERR: cant remove BCDEF (it is not in table)");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "BCDEF");
    if (n_res == NULL)
        testmsg("OK: BCDEF was not found in table");
    else
    {
        testmsg("ERR: BCDEF was found in table, it was removed !");
        detectError = true;
    }
    if (mysym == NULL)
        testmsg("OK: &mysym is NULL => empty");
    else
    {
        testmsg("ERR: &mysym is NOT NULL, but it's empty");
        detectError = true;
    }

    // -> Destruct
    fprintf(stderr, "\n-------------> TABLE DESTRUCT\n");
    stDestruct(&mysym);
    if (mysym == NULL)
        testmsg("OK: symtable &mysym destructed - check valgrind if memory is ok");
    else
    {
        testmsg("ERR: destructed symtable &mysym isnt NULL");
        detectError = true;
    }

    // FUNCTIONS
    fprintf(stderr, "\n-------------> FUNCTIONS\n");
    stConstruct(&mysym);
    if (mysym == NULL)
        testmsg("OK: &mysym symtable constructed");
    else
    {
        testmsg("ERR: error in construction of &mysym");
        detectError = true;
    }
    
    // FILLING SYMTABLE (FUNCTIONS)
    // -> Insert one key
    fprintf(stderr, "\n-------------> INSERTING FIRST KEY\n");

    res = stInsert(&mysym, "ABCDE", ST_N_FUNCTION, INT);
    if (res > 0)
        testmsg("OK: id ABCDE with type INT inserted into &mysym symtable");
    else if (res == 0)
    {
        testmsg("ERR: something is wrong, cant insert ABCDE");
        detectError = true;
    }
    else
    {
        testmsg("ERR: cant insert ABCDE because it exists (even it doesnt)");
        detectError = true;
    }
    // -> Insert more keys
    fprintf(stderr, "\n-------------> INSERTING MORE KEYS\n");

    res = stInsert(&mysym, "AAAAA", ST_N_FUNCTION, FLOAT64);
    if (res > 0)
        testmsg("OK: id AAAAA with type FLOAT64 inserted into &mysym symtable");
    else if (res == 0)
    {
        testmsg("ERR: something is wrong, cant insert AAAAA");
        detectError = true;
    }
    else
    {
        testmsg("ERR: cant insert AAAAA because it exists (despite fact that it doesnt)");
        detectError = true;
    }

    res = stInsert(&mysym, "SSSSS", ST_N_FUNCTION, STRING);
    if (res > 0)
        testmsg("OK: id SSSSS with type STRING inserted into &mysym symtable");
    else if (res == 0)
    {
        testmsg("ERR: something is wrong, cant insert SSSSS");
        detectError = true;
    }
    else
    {
        testmsg("ERR: cant insert SSSSS because it exists (despite fact that it doesnt)");
        detectError = true;
    }

    res = stInsert(&mysym, "BCDEF", ST_N_FUNCTION, 0);
    if (res > 0)
        testmsg("OK: id BCDEF with type UNKNOWN inserted into &mysym symtable");
    else if (res == 0)
    {
        testmsg("ERR: cant insert BCDEF due to malloc error");
        detectError = true;
    }
    else
    {
        testmsg("ERR: cant insert BCDEF because it exists (despite fact that it doesnt)");
        detectError = true;
    }

    // OPERATIONS ON NON-EMPTY SYMTABLE (FUNCTIONS FILLED)
    // -> Look-up
    fprintf(stderr, "\n-------------> LOOKUP OPERATIONS\n");

    n_res = stLookUp(&mysym, "BCDEF");
    if (n_res)
        printf("---> OK: id BCDEF found in symtable with type %d\n", stGetType(n_res));
    else
    {
        testmsg("ERR: something went wrong, id BCDEF not found (even it's there)");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "SSSSS");
    if (n_res)
        printf("---> OK: id SSSSS found in symtable with type %d\n", stGetType(n_res));
    else
    {
        testmsg("ERR: something went wrong, id SSSSS not found (even it's there)");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "AAAAA");
    if (n_res)
        printf("---> OK: id AAAAA found in symtable with type %d\n", stGetType(n_res));
    else
    {
        testmsg("ERR: something went wrong, id AAAAA not found (even it's there)");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "ABCDE");
    if (n_res)
        printf("---> OK: id ABCDE found in symtable with type %d\n", stGetType(n_res));
    else
    {
        testmsg("ERR: something went wrong, id ABCDE not found (even it's there)");
        detectError = true;
    }

    // -> Get/Set
    fprintf(stderr, "\n-------------> GET / SET TYPE\n");

    sprintf(resultGetSet, "DONE: id ABCDE has type %d", stGetType(n_res));
    testmsg(resultGetSet);
    stSetType(n_res, STRING);
    sprintf(resultGetSet, "DONE: id ABCDE has now type %d", stGetType(n_res));
    testmsg(resultGetSet);

    // -> Insert existing
    fprintf(stderr, "\n-------------> RE-INSERTION\n");

    res = stInsert(&mysym, "ABCDE", ST_N_VARIABLE, 0);
    if (res < 0)
        testmsg("OK: re-insert of ABCDE (which is in table) failed");
    else if (res > 0)
    {
        testmsg("ERR: something went really wrong because ABCDE was inserted again.");
        detectError = true;
    }
    else
    {
        testmsg("ERR: malloc error in insert");
        detectError = true;
    }

    // -> Delete
    fprintf(stderr, "\n-------------> DELETE OF EXISTING KEYS\n");

    res = stDelete(&mysym, "ABCDE");
    if (res)
        testmsg("OK: deleted ABCDE from table");
    else if (!res)
    {
        testmsg("ERR: cant delete ABCDE because it isnt in symtable (even it is)");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "SSSSS");
    if (n_res != NULL)
        testmsg("OK: SSSSS was again found in table and not removed");
    else
    {
        testmsg("ERR: SSSSS wasnt found in table, but it wasnt removed yet");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "BCDEF");
    if (n_res != NULL)
        testmsg("OK: BCDEF was again found in table and not removed");
    else
    {
        testmsg("ERR: BCDEF wasnt found in table, but it wasnt removed yet");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "AAAAA");
    if (n_res != NULL)
        testmsg("OK: AAAAA was again found in table and not removed (ofc, bcs its root)");
    else
    {
        testmsg("ERR: AAAAA wasnt found in table, but it wasnt removed yet");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "ABCDE");
    if (n_res != NULL)
    {
        testmsg("ERR: ABCDE was again found in table but its was removed");
        detectError = true;
    }
    else
        testmsg("OK: ABCDE wasnt found in table");

    res = stDelete(&mysym, "SSSSS");
    if (res)
        testmsg("OK: deleted SSSSS from table");
    else if (!res)
    {
        testmsg("ERR: cant delete SSSSS because it isnt in symtable (even it is)");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "AAAAA");
    if (n_res != NULL)
        testmsg("OK: AAAAA was again found in table and not removed (ofc, bcs its root)");
    else
    {
        testmsg("ERR: AAAAA wasnt found in table, but it wasnt removed yet");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "BCDEF");
    if (n_res != NULL)
        testmsg("OK: BCDEF was again found in table and not removed");
    else
    {
        testmsg("ERR: BCDEF wasnt found in table, but it wasnt removed yet");
        detectError = true;
    }

    res = stDelete(&mysym, "ABCDE");
    if (!res)
        testmsg("OK: not deleted ABCDE from table again, its gone already");
    else if (res)
    {
        testmsg("ERR: deleted ABCDE again (even it was already removed)");
        detectError = true;
    }

    res = stDelete(&mysym, "AAAAA");
    if (res)
        testmsg("OK: deleted AAAAA from table");
    else if (!res)
    {
        testmsg("ERR: cant remove AAAAA (it is not in table)");
        detectError = true;
    }

    res = stDelete(&mysym, "BCDEF");
    if (res)
        testmsg("OK: deleted BCDEF from table");
    else if (!res)
    {
        testmsg("ERR: cant remove BCDEF (it is not in table)");
        detectError = true;
    }

    n_res = stLookUp(&mysym, "BCDEF");
    if (n_res == NULL)
        testmsg("OK: BCDEF was not found in table");
    else
    {
        testmsg("ERR: BCDEF was found in table, it was removed !");
        detectError = true;
    }
    if (mysym == NULL)
        testmsg("OK: &mysym is NULL => empty");
    else
    {
        testmsg("ERR: &mysym is NOT NULL, but it's empty");
        detectError = true;
    }

    // -> Destruct
    fprintf(stderr, "\n-------------> TABLE DESTRUCT\n");
    stDestruct(&mysym);
    if (mysym == NULL)
        testmsg("OK: symtable &mysym destructed - check valgrind if memory is ok");
    else
    {
        testmsg("ERR: destructed symtable &mysym isnt NULL");
        detectError = true;
    }

    fprintf(stderr, "\n\n------------- TEST ENDED-------------\n");
    return (detectError) ? 1 : 0;
}