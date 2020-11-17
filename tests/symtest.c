#include "../symtable.h"
#define testmsg(text) printf("--> %s\n", text)

int main (void) {

    // TEST OF SYMTABLE

    testmsg("------------- SYMTABLE TEST -------------");


    // construct
    testmsg("\n-------------> CONSTRUCT, DESTRUCT\n");
    BTNodePtr mysym = NULL;
    stConstruct(&mysym);
    if (mysym == NULL) testmsg("OK: &mysym symtable constructed");
    else testmsg("ERR: error in construction of &mysym");

    // destruct of empty &mysym
    stDestruct(&mysym);
    if (mysym == NULL) testmsg("OK: &mysym destructed (nothing happened rly)");
    else testmsg("ERR: error in destruction");



    // operations on empty symtable
    testmsg("\n-------------> OPERATIONS ON EMPTY SYMTABLE\n");

    // lookup for nonexisting key
    BTNodePtr n_res = stLookUp(&mysym, "ABCDE");
    if (n_res == NULL) testmsg("OK: string ABCDE not found");
    else testmsg("ERR: string ABCDE found and something is rly bad");

    // delete nonexisting key
    int res = stDelete(&mysym, "ABCDE");
    if (!res) testmsg("OK: string ABCDE not found and not deleted");
    else testmsg("ERR: string ABCDE found, deleted and something is rly bad");



    // insert this key
    testmsg("\n-------------> INSERTING FIRST KEY\n");
    res = stInsert(&mysym, "ABCDE", INT);
    printf("%s",mysym->id);
    if (res > 0) testmsg("OK: id ABCDE with type INT inserted into &mysym symtable");
    else if (res == 0) testmsg("ERR: something is wrong, cant insert ABCDE");
    else testmsg("ERR: cant insert ABCDE because it exists (even it doesnt)");



    // insert more keys
    testmsg("\n-------------> INSERTING MORE KEYS\n");

    res = stInsert(&mysym, "AAAAA", FLOAT64);
    if (res > 0) testmsg("OK: id AAAAA with type FLOAT64 inserted into &mysym symtable");
    else if (res == 0) testmsg("ERR: something is wrong, cant insert AAAAA");
    else testmsg("ERR: cant insert AAAAA because it exists (despite fact that it doesnt)");

    res = stInsert(&mysym, "SSSSS", STRING);
    if (res > 0) testmsg("OK: id SSSSS with type STRING inserted into &mysym symtable");
    else if (res == 0) testmsg("ERR: something is wrong, cant insert SSSSS");
    else testmsg("ERR: cant insert SSSSS because it exists (despite fact that it doesnt)");


    res = stInsert(&mysym, "BCDEF", 0);
    if (res > 0) testmsg("OK: id BCDEF with type UNKNOWN inserted into &mysym symtable");
    else if (res == 0) testmsg("ERR: cant insert BCDEF due to malloc error");
    else testmsg("ERR: cant insert BCDEF because it exists (despite fact that it doesnt)");



    // some operations on non-empty symtable
    // LOOKUP
    testmsg("\n-------------> LOOKUP OPERATIONS\n");
    n_res = stLookUp(&mysym, "BCDEF");
    if (n_res) printf("---> OK: id BCDEF found in symtable with type %d\n", stGetType(n_res));
    else testmsg("ERR: something went wrong, id BCDEF not found (even it's there)");

    n_res = stLookUp(&mysym, "SSSSS");
    if (n_res) printf("---> OK: id SSSSS found in symtable with type %d\n", stGetType(n_res));
    else testmsg("ERR: something went wrong, id SSSSS not found (even it's there)");

    n_res = stLookUp(&mysym, "AAAAA");
    if (n_res) printf("---> OK: id AAAAA found in symtable with type %d\n", stGetType(n_res));
    else testmsg("ERR: something went wrong, id AAAAA not found (even it's there)");

    n_res = stLookUp(&mysym, "ABCDE");
    if (n_res) printf("---> OK: id ABCDE found in symtable with type %d\n", stGetType(n_res));
    else testmsg("ERR: something went wrong, id ABCDE not found (even it's there)");


    // get/set
    testmsg("\n-------------> GET / SET TYPE\n");
    printf("DONE: id ABCDE has type %d\n", stGetType(n_res));

    stSetType(n_res, STRING);
    printf("DONE: id ABCDE has now type %d\n", stGetType(n_res));



    // insert existing
    testmsg("\n-------------> RE-INSERTION\n");
    res = stInsert(&mysym, "ABCDE", 0);
    if(res < 0) testmsg("OK: re-insert of ABCDE (which is in table) failed");
    else if (res > 0) testmsg("ERR: something went really wrong because ABCDE was inserted again.");
    else testmsg("ERR: malloc error in insert");



    // delete
    testmsg("\n-------------> DELETE OF EXISTING KEYS\n");
    res = stDelete(&mysym, "ABCDE");
    if (res) testmsg("OK: deleted ABCDE from table");
    else if (!res) testmsg("ERR: cant delete ABCDE because it isnt in symtable (even it is)");
    //printf("-> INFO: Root is now key '%s'. (L: %s; R: %s)\n", (&mysym != NULL ? &mysym->id : "NULL"), (&mysym->LPtr != NULL ? &mysym->LPtr->id : "NULL"), (&mysym->RPtr != NULL ? &mysym->RPtr->id : "NULL"));

    n_res = stLookUp(&mysym, "SSSSS");
    if (n_res != NULL) testmsg("OK: SSSSS was again found in table and not removed");
    else testmsg("ERR: SSSSS wasnt found in table, but it wasnt removed yet");

    n_res = stLookUp(&mysym, "BCDEF");
    if (n_res != NULL) testmsg("OK: BCDEF was again found in table and not removed");
    else testmsg("ERR: BCDEF wasnt found in table, but it wasnt removed yet");

    n_res = stLookUp(&mysym, "AAAAA");
    if (n_res != NULL) testmsg("OK: AAAAA was again found in table and not removed (ofc, bcs its root)");
    else testmsg("ERR: AAAAA wasnt found in table, but it wasnt removed yet");

    n_res = stLookUp(&mysym, "ABCDE");
    if (n_res != NULL) testmsg("ERR: ABCDE was again found in table but its was removed");
    else testmsg("OK: ABCDE wasnt found in table");

    res = stDelete(&mysym, "SSSSS");
    if (res) testmsg("OK: deleted SSSSS from table");
    else if (!res) testmsg("ERR: cant delete SSSSS because it isnt in symtable (even it is)");
    //printf("-> INFO: Root is now key '%s'. (L: %s; R: %s)\n", (&mysym != NULL ? &mysym->id : "NULL"), (&mysym->LPtr != NULL ? &mysym->LPtr->id : "NULL"), (&mysym->RPtr != NULL ? &mysym->RPtr->id : "NULL"));

    n_res = stLookUp(&mysym, "AAAAA");
    if (n_res != NULL) testmsg("OK: AAAAA was again found in table and not removed (ofc, bcs its root)");
    else testmsg("ERR: AAAAA wasnt found in table, but it wasnt removed yet");

    n_res = stLookUp(&mysym, "BCDEF");
    if (n_res != NULL) testmsg("OK: BCDEF was again found in table and not removed");
    else testmsg("ERR: BCDEF wasnt found in table, but it wasnt removed yet");


    res = stDelete(&mysym, "ABCDE");
    if (!res) testmsg("OK: not deleted ABCDE from table again, its gone already");
    else if (res) testmsg("ERR: deleted ABCDE again (even it was already removed)");
    //printf("-> INFO: Root is now key '%s'. (L: %s; R: %s)\n", (&mysym != NULL ? &mysym->id : "NULL"), (&mysym->LPtr != NULL ? &mysym->LPtr->id : "NULL"), (&mysym->RPtr != NULL ? &mysym->RPtr->id : "NULL"));

    res = stDelete(&mysym, "AAAAA");
    if (res) testmsg("OK: deleted AAAAA from table");
    else if (!res) testmsg("ERR: cant remove AAAAA (it is not in table)");
    //printf("-> INFO: Root is now key '%s'. (L: %s; R: %s)\n",(&mysym != NULL ? &mysym->id : "NULL"), (&mysym->LPtr != NULL ? &mysym->LPtr->id : "NULL"), (&mysym->RPtr != NULL ? &mysym->RPtr->id : "NULL"));

    res = stDelete(&mysym, "BCDEF");
    if (res) testmsg("OK: deleted BCDEF from table");
    else if (!res) testmsg("ERR: cant remove BCDEF (it is not in table)");
    //printf("-> INFO: Root is now key '%s'. (L: %s; R: %s)\n", (&mysym != NULL ? &mysym->id : "NULL"), (&mysym->LPtr != NULL ? &mysym->LPtr->id : "NULL"), (&mysym->RPtr != NULL ? &mysym->RPtr->id : "NULL"));

    n_res = stLookUp(&mysym, "BCDEF");
    if (n_res == NULL) testmsg("OK: BCDEF was not found in table");
    else testmsg("ERR: BCDEF was found in table, it was removed !");
    if (mysym == NULL) testmsg("OK: &mysym is NULL => empty");
    else testmsg("ERR: &mysym is NOT NULL, but it's empty");


    // destruct
    testmsg("\n-------------> TABLE DESTRUCT\n");
    stDestruct(&mysym);
    if (mysym == NULL) testmsg("OK: symtable &mysym destructed - check valgrind if memory is ok");
    else testmsg("ERR: destructed symtable &mysym isnt NULL");



    testmsg("\n\n------------- TEST ENDED-------------\n");
    return 0;
}