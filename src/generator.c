/**
 * @file generator.c
 * @author Dominik Horky (xhorky32@vutbr.cz)
 * @brief Code generator implementation
 * @version 0.1
 * @date 2020-11-20
 */

#include "generator.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>

/** macros for converting 'C code' into 'string literal' (used in generating internal functions) **/
#define toString(code) #code
#define convertToString(code) toString(code)

/** generates pop code **/
#define GEN_POP(name) fprintf(stdout, "POPS LF@%s\n", name);

/** generates push code **/
#define GEN_PUSH(name) fprintf(stdout, "PUSHS LF@%s\n", name);

/** maximum number of characters 1 function identifier can contain **/
#define MAX_FNAME 255

/** maximum number of characters 1 non function identifieer can contain **/
#define MAX_IDNAME MAX_FNAME

/** for generateFunction to compare if funcName is internal function **/
const char* internalFuncNames[] = {
        "inputs",
        "inputi",
        "inputf",
        "inputb",
        "print",
        "int2float",
        "float2int",
        "funclen",
        "substr",
        "ord",
        "chr",
};

/** (raw) code that will be output to stdout**/
char* code = NULL;

/** function name **/
char* funcName = NULL;

/*
 * SCOPE COUNTERS
 */

unsigned int if_cnt = 0;
unsigned int for_cnt = 0;

/** will be if scope printed or not? **/
bool ifelse_ignore = false;

/** if-elseif-else scope is open **/
bool ifelse_open = false;

/** used internal functions **/
intFC internalFuncsUsed[11] = { 0 };

IDList* id_list = NULL;

/*
 *
 * DEFINITIONS OF IFJ20 BUILT-IN FUNCTIONS
 *
 */

/* inputX functions: read 1 line (ended with EOL) from stdin
 * returns 0 if ok to second return value, 1 if error (missing input value, wrong format)
 */

// returns read line without EOL
#define inputs()  \n \
    LABEL _inputs    \n \
                     \n \
    CREATEFRAME      \n \
    PUSHFRAME        \n \
    DEFVAR LF@val    \n \
    DEFVAR LF@retval        \n \
    MOVE LF@retval int@0    \n \
                            \n \
    READ LF@val string      \n \
    JUMPIFEQ _INPUTS_ERR LF@val nil@nil \n \
    JUMP _INPUTS_END        \n \
                            \n \
    LABEL _INPUTS_ERR       \n \
    MOVE LF@retval int@1    \n \
    JUMP _INPUTS_END        \n \
                            \n \
    LABEL _INPUTS_END       \n \
    PUSHS LF@val    \n \
    PUSHS LF@retval \n \
    POPFRAME        \n \
                    \n \
    RETURN

// returns read line without all white space characters (incl. EOL)
// wrong characters before number mean wrong format => second return value = 1
// hexadecimal numbers are supported
// reads and returns int number
#define inputi()    \n \
    LABEL _inputi   \n \
                    \n \
    CREATEFRAME     \n \
    PUSHFRAME       \n \
    DEFVAR LF@val   \n \
    DEFVAR LF@retval        \n \
    MOVE LF@retval int@0    \n \
                    \n \
    READ LF@val int \n \
    JUMPIFEQ _INPUTI_ERR LF@val nil@nil \n \
    JUMP _INPUTI_END        \n \
                    \n \
    LABEL _INPUTI_ERR       \n \
    MOVE LF@retval int@1    \n \
    JUMP _INPUTI_END        \n \
                    \n \
    LABEL _INPUTI_END       \n \
    PUSHS LF@val    \n \
    PUSHS LF@retval         \n \
    POPFRAME        \n \
                    \n \
    RETURN


// reads and returns float64 number
#define inputf()    \n \
    LABEL _inputf   \n \
                    \n \
    CREATEFRAME     \n \
    PUSHFRAME       \n \
    DEFVAR LF@val   \n \
    DEFVAR LF@retval        \n \
    MOVE LF@retval int@0    \n \
                    \n \
    READ LF@val float \n \
    JUMPIFEQ _INPUTF_ERR LF@val nil@nil \n \
    JUMP _INPUTF_END        \n \
                    \n \
    LABEL _INPUTF_ERR       \n \
    MOVE LF@retval int@1    \n \
    JUMP _INPUTF_END        \n \
                    \n \
    LABEL _INPUTF_END       \n \
    PUSHS LF@val    \n \
    PUSHS LF@retval         \n \
    POPFRAME        \n \
                    \n \
    RETURN

// reads and returns bool value
#define inputb()    \n \
    LABEL _inputb   \n \
                    \n \
    CREATEFRAME     \n \
    PUSHFRAME       \n \
    DEFVAR LF@val   \n \
    DEFVAR LF@retval        \n \
    MOVE LF@retval int@0    \n \
                    \n \
    READ LF@val bool \n \
    JUMPIFEQ _INPUTF_ERR LF@val nil@nil \n \
    JUMP _INPUTF_END        \n \
                    \n \
    LABEL _INPUTF_ERR       \n \
    MOVE LF@retval int@1    \n \
    JUMP _INPUTF_END        \n \
                    \n \
    LABEL _INPUTF_END       \n \
    PUSHS LF@val    \n \
    PUSHS LF@retval         \n \
    POPFRAME        \n \
                    \n \
    RETURN

/* print function
 * prints to stdout values of arguments given (in stack), no return value
 */
void print(DataType arg_type) {
    char* type = NULL;
    switch (arg_type) {
        case INT_T:
            type = "INT";
            break;
        case FLOAT64_T:
            type = "FLOAT";
            break;
        case STRING_T:
            type = "STRING";
            break;
        case BOOL_T:
            type = "BOOL";
            break;
        default:
            fprintf(stderr, "ERROR: CODE GENERATOR: unknown type in internal function print()\n");
            break;
    }

    fprintf(stdout, "POPS GF@?PRINT_%s?\nWRITE GF@?PRINT_%s?\n", type, type);
}

/* number datatypes conversion */

// i ... int
// returns float value of argument i
#define int2float() \n \
    LABEL _int2float \n \
                     \n \
    CREATEFRAME      \n \
    PUSHFRAME        \n \
    DEFVAR LF@val    \n \
    POPS LF@val     \n \
    DEFVAR LF@result  \n \
                    \n \
    DEFVAR LF@typecheck \n \
    TYPE LF@typecheck LF@val \n \
    JUMPIFNEQ _INT2FLOAT_TYPE_OK LF@typecheck string@int \n \
    JUMP _INT2FLOAT_TYPE               \n \
                    \n \
    LABEL _INT2FLOAT_TYPE    \n \
    MOVE LF@result float@0x6.0p+0    \n \
    JUMP _INT2FLOAT_END \n \
                    \n \
    LABEL _INT2FLOAT_TYPE_OK                \n \
    INT2FLOAT LF@result LF@val                          \n \
    JUMP _INT2FLOAT_END \n \
                    \n \
    LABEL _INT2FLOAT_END                \n \
    PUSHS LF@result  \n \
    POPFRAME         \n \
                     \n \
    RETURN

// f .. float64
// returns int value of argument f
#define float2int() \n \
    LABEL _float2int \n \
                     \n \
    CREATEFRAME      \n \
    PUSHFRAME        \n \
    DEFVAR LF@val    \n \
    POPS LF@val      \n \
                     \n \
    DEFVAR LF@result    \n \
                    \n \
    DEFVAR LF@typecheck \n \
    TYPE LF@typecheck LF@val \n \
    JUMPIFNEQ _FLOAT2INT_TYPE LF@typecheck string@float   \n \
    JUMP _FLOAT2INT_TYPE_OK  \n \
            \n \
    LABEL _FLOAT2INT_TYPE    \n \
    MOVE LF@result int@6    \n \
    JUMP _INT2FLOAT_END \n \
                    \n \
    LABEL _FLOAT2INT_TYPE_OK     \n \
    FLOAT2INT LF@result LF@val \n \
    JUMP _FLOAT2INT_END \n \
                    \n \
    LABEL _FLOAT2INT_END    \n \
    PUSHS LF@result  \n \
    POPFRAME         \n \
                     \n \
    RETURN

/* string functions */

// s ... string
// returns (int) length (number of characters, including \n \n, etc.) of s string
#define funclen()  \n \
    LABEL _funclen  \n \
                    \n \
    CREATEFRAME     \n \
    PUSHFRAME       \n \
    DEFVAR LF@str   \n \
    DEFVAR LF@size  \n \
    POPS LF@str     \n \
                   \n \
    DEFVAR LF@typecheck \n \
    TYPE LF@typecheck LF@str \n \
    JUMPIFNEQ _FUNCLEN_TYPE LF@typecheck string@string   \n \
    JUMP _FUNCLEN_TYPE_OK  \n \
                   \n \
    LABEL _FUNCLEN_TYPE_OK               \n \
    STRLEN LF@size LF@str    \n \
    JUMP _FUNCLEN_END \n \
                   \n \
    LABEL _FUNCLEN_TYPE \n \
    MOVE LF@result string@6    \n \
    JUMP _FUNCLEN_END \n \
                   \n \
    LABEL _FUNCLEN_END     \n \
    PUSHS LF@size   \n \
    POPFRAME        \n \
                    \n \
    RETURN

// s ... string
// i ... int
// n ... int
// first return value (string) is substring of string s, substring starts
// from i-th character of s and is n characters long, or
// all characters from i-th character (including) if n > len(s)-i
// second return value (int) is 0 if ok, or 1 if i is not in interval <0, len(s)>,
// or n < 0
#define substr() \n \
    LABEL _substr   \n \
                \n \
    CREATEFRAME \n \
    PUSHFRAME   \n  \
    DEFVAR LF@subsize   \n \
    POP LF@subsize     \n  \
    DEFVAR LF@pos   \n \
    POPS LF@pos \n \
    DEFVAR LF@str   \n \
    POPS LF@str     \n \
    DEFVAR LF@retval    \n \
    DEFVAR LF@retval2   \n \
    MOVE LF@retval2 int@0 \n \
                 \n \
    DEFVAR LF@typecheck \n \
    TYPE LF@typecheck LF@str \n \
    JUMPIFNEQ _SUBSTR_TYPE LF@typecheck string@string \n \
    TYPE LF@typecheck LF@pos    \n \
    JUMPIFNEQ _SUBSTR_TYPE LF@typecheck string@int    \n \
    TYPE LF@typecheck LF@subsize            \n \
    JUMPIFNEQ _SUBSTR_TYPE LF@typecheck string@int \n \
                \n \
    DEFVAR LF@newstr    \n \
    DEFVAR LF@cmp   \n \
    LT LF@cmp LF@pos int@0  \n \
    JUMPIFEQ _SUBSTR_ERR LF@cmp bool@true   \n \
                     \n \
    JUMPIFEQ _SUBSTR_ERR LF@str nil@nil \n \
    JUMPIFEQ _SUBSTR_ERR LF@subsize int@0   \n \
                \n \
    DEFVAR LF@size  \n \
    STRLEN LF@size LF@str   \n \
    GT LF@cmp LF@pos LF@size    \n \
    JUMPIFEQ _SUBSTR_ERR LF@cmp bool@true   \n \
    DEFVAR LF@tmpsize   \n \
    MOVE LF@tmpsize LF@subsize  \n \
    ADD LF@tmpsize LF@tmpsize LF@pos    \n \
    GT LF@cmp LF@tmpsize LF@size    \n \
    JUMPIFEQ _SUBSTR_ERR LF@cmp bool@true   \n \
                    \n \
    DEFVAR LF@character \n \
    DEFVAR LF@mov_pos   \n \
    MOVE LF@newstr LF@str   \n \
    MOVE LF@mov_pos LF@pos  \n \
                    \n \
    DEFVAR LF@cycle \n \
    MOVE LF@cycle int@0 \n \
    JUMP _SUBSTR_CYCLE  \n \
                    \n \
    LABEL _SUBSTR_CYCLE \n \
    GETCHAR LF@character LF@str LF@mov_pos  \n \
    SETCHAR LF@newstr LF@cycle LF@character \n \
    ADD LF@cycle LF@cycle int@1     \n \
    ADD LF@mov_pos LF@mov_pos int@1 \n \
    JUMPIFNEQ _SUBSTR_CYCLE LF@cycle LF@subsize \n \
    JUMP _SUBSTR_CYCLE2 \n \
                    \n \
    LABEL _SUBSTR_CYCLE2    \n \
    JUMPIFEQ _SUBSTR_END LF@cycle LF@size   \n \
    SETCHAR LF@newstr LF@cycle string@\n \000  \n \
    ADD LF@cycle LF@cycle int@1 \n \
    JUMPIFNEQ _SUBSTR_CYCLE2 LF@cycle LF@size   \n \
    JUMP _SUBSTR_END    \n \
                    \n \
    LABEL _SUBSTR_ERR   \n \
    MOVE LF@newstr string@nil  \n \
    MOVE LF@retval2 int@1   \n \
    JUMP _SUBSTR_END        \n \
                 \n \
    LABEL _SUBSTR_TYPE  \n \
    MOVE LF@newstr string@nil               \n \
    MOVE LF@retval2 int@6   \n \
    JUMP _SUBSTR_END \n \
                 \n \
    LABEL _SUBSTR_END   \n \
    MOVE LF@retval LF@newstr    \n \
    PUSHS LF@retval \n \
    PUSHS LF@retval2    \n \
                \n \
    POPFRAME    \n \
    RETURN

// s ... string
// i ... int
// first return value (int) is ordinal value (ASCII) of character
// on position i in string s
// second return value (int) is 0 if ok or 1 if i is not in interval <0, len(s)-1>
#define ord()    \n \
    LABEL _ord      \n \
                \n \
    CREATEFRAME \n \
    PUSHFRAME   \n  \
    DEFVAR LF@pos   \n \
    POPS LF@pos     \n \
    DEFVAR LF@str   \n \
    POPS LF@str \n \
    DEFVAR LF@retval    \n \
    DEFVAR LF@retval2   \n \
    MOVE LF@retval2 int@0   \n \
                 \n \
    DEFVAR LF@typecheck \n \
    TYPE LF@typecheck LF@str \n \
    JUMPIFNEQ _ORD_TYPE LF@typecheck string@string \n \
    TYPE LF@typecheck LF@pos    \n \
    JUMPIFNEQ _ORD_TYPE LF@typecheck string@int \n \
                 \n \
    DEFVAR LF@cmp           \n \
    LT LF@cmp LF@pos int@0  \n \
    JUMPIFEQ _ORD_ERR LF@cmp bool@true  \n \
                    \n \
    JUMPIFEQ _ORD_ERR LF@str nil@nil    \n \
                    \n \
    DEFVAR LF@size  \n \
    STRLEN LF@size LF@str   \n \
    SUB LF@size LF@size int@1   \n \
    GT LF@cmp LF@pos LF@size    \n \
    JUMPIFEQ _ORD_ERR LF@cmp bool@true  \n \
                    \n \
    STRI2INT LF@retval LF@str LF@pos    \n \
    JUMP _ORD_END   \n \
                    \n \
    LABEL _ORD_ERR  \n \
    MOVE LF@retval int@0  \n \
    MOVE LF@retval2 int@1   \n \
    JUMP _ORD_END       \n \
                 \n \
    LABEL _ORD_TYPE \n \
    MOVE LF@retval int@0  \n \
    MOVE LF@retval2 int@6   \n \
    JUMP LABEL _ORD_END \n \
                 \n \
    LABEL _ORD_END  \n \
    PUSHS LF@retval \n \
    PUSHS LF@retval2    \n \
                    \n \
    POPFRAME    \n \
    RETURN

// i ... int
// first return value (string) is one-char string containing character,
// which ASCII code is given in argument i
// second return value (int) is 0 if ok, or 1 if i is not in interval <0, 255>
#define chr()  \n \
    LABEL _chr  \n \
                \n \
    CREATEFRAME \n \
    PUSHFRAME   \n \
    DEFVAR LF@retval    \n \
    DEFVAR LF@ch        \n \
    POPS LF@ch  \n \
    DEFVAR LF@retval2   \n \
    MOVE LF@retval2 int@0   \n \
               \n \
    DEFVAR LF@typecheck \n \
    TYPE LF@typecheck LF@ch \n \
    JUMPIFNEQ _CHR_TYPE LF@typecheck string@int   \n \
                        \n \
    DEFVAR LF@cmp       \n \
    LT LF@cmp LF@ch int@0   \n \
    JUMPIFEQ _CHR_ERR LF@cmp bool@true  \n \
                        \n \
    GT LF@cmp LF@ch int@255 \n \
    JUMPIFEQ _CHR_ERR LF@cmp bool@true  \n \
                        \n \
    INT2CHAR LF@retval LF@ch    \n \
    JUMP _CHR_END   \n \
                    \n \
    LABEL _CHR_ERR  \n \
    MOVE LF@retval string@nil  \n \
    MOVE LF@retval2 int@1   \n \
    JUMP _CHR_END           \n \
               \n \
    LABEL _CHR_TYPE     \n \
    MOVE LF@retval string@nil                     \n \
    MOVE LF@retval2 int@6   \n \
    JUMP _CHR_END \n \
                    \n \
    LABEL _CHR_END  \n \
    PUSHS LF@retval     \n \
    PUSHS LF@retval2    \n \
                    \n \
    POPFRAME    \n \
    RETURN

// IFJcode20 header for interpret + beginning of program (jumps to $MAIN function)
#define ic20int_header() .IFJcode20 \n \
                    \n\
    DEFVAR GF@?AX?  \n\
    DEFVAR GF@?BX?  \n\
    DEFVAR GF@?CX?  \n                 \
    DEFVAR GF@?PRINT_INT? \n                     \
    DEFVAR GF@?PRINT_FLOAT? \n        \
    DEFVAR GF@?PRINT_STRING? \n       \
    DEFVAR GF@?PRINT_BOOL?    \n       \
    DEFVAR GF@?FOR_RESULT? \n \
    \n\
    CALL $main       \n\
    EXIT int@0      \n\


/*
 *
 * GENERATOR USER FUNCTIONS
 *
 */

void generateHeader() {
    code = convertToString(ic20int_header());
    generateCodeInternal();
}

void generateFunction(const char* fName)  {
    funcName = (char*) fName;

    // not 0 -> strcmp diff in all variations -> not generating internal function
    fprintf(stdout, "\nLABEL $%s\n\nCREATEFRAME\nPUSHFRAME\n", fName);
}

void generateFuncArgument(char* argName) {
    GEN_PUSH(argName);
}

void generateFuncCall(char* fName) {
    // CALL $fName or CALL _fname
    char fName_sep = '\0';

    // Check if fName isnt internal func name
    intFC i = INPUTS;
    int res = isFuncInternal(&i, fName);

    if (res) // user function
        fName_sep = '$';
    else // internal function
        fName_sep = '_';

    fprintf(stdout, "CALL %c%s\n", fName_sep, fName);
}

void generateFuncReturn(char* retName) {
    GEN_PUSH(retName);
}

void generateFuncEnd() {
    fprintf(stdout, "POPFRAME\n\nRETURN\n");
}

void generateDefinitions() {
    char* name = generatorGetID();

    while (name != NULL) {
        fprintf(stdout, "DEFVAR LF@%s\n", name);
        fprintf(stdout, "POPS LF@%s\n", name);
        name = generatorGetID();
    }
}

void generateAssignments() {
    char* name = generatorGetID();
    while (name != NULL) {
        GEN_POP(name);
        name = generatorGetID();
    }
}

void generatePrint(DataType type) {
    print(type);
}

void generateIfScope() {
    if (!ifelse_ignore) {
        fprintf(stdout, "JUMP IF$%d\n\n", if_cnt);
        fprintf(stdout, "LABEL IF$%d\nCREATEFRAME\nPUSHFRAME\n", if_cnt);
        if_cnt++;
        ifelse_open = true;
    }
}

void generateIfScopeEnd() {
    if (!ifelse_ignore && ifelse_open) {
        fprintf(stdout, "POPFRAME\n\n");
        ifelse_open = false;
    }
}

void generateForBeginning() {
    fprintf(stdout, "\nLABEL FOR_START$%d\n", for_cnt);
}

void generateForExpression() {
    fprintf(stdout, "\nLABEL FOR_EXPR$%d\n", for_cnt);
}

void generateForCondition() {
    fprintf(stdout, "POPS GF@?FOR_RESULT?\nJUMPIFEQ FOR$%d GF@?FOR_RESULT? bool@true\nJUMP FOR_END$%d", for_cnt, for_cnt);
}

void generateForAssignment() {
    fprintf(stdout, "\n\nLABEL FOR_ASSIGN$%d\n", for_cnt);
}

void generateForAssignmentEnd() {
    fprintf(stdout, "JUMP FOR_EXPR$%d\n", for_cnt);
}

void generateForScope() {
    fprintf(stdout, "\nLABEL FOR$%d\nCREATEFRAME\nPUSHFRAME\n", for_cnt);
}

void generateForScopeEnd() {
    fprintf(stdout, "POPFRAME\nJUMP FOR_ASSIGN$%d\n\nLABEL FOR_END$%d\n\n", for_cnt, for_cnt);
    for_cnt++;
}

void ignoreIfScope(int ignore) {
    if (ignore)
        ifelse_ignore = true;
    else
        ifelse_ignore = false;
}

void generatorSaveID(char* id) {
    IDList* temp = id_list;

    temp = (IDList*) malloc(sizeof(IDList));
    if (temp == NULL) {
        fprintf(stderr, "ERROR: CODE GENERATOR: unable to alloc memory for ID list structure.\n");
        return;
    }

    temp->identifier = (char*) calloc(sizeof(char)+(strlen(id)+1), 1);
    if (temp->identifier == NULL) {
        fprintf(stderr, "ERROR: CODE GENERATOR: unable to alloc memory for ID (ID list struct data).\n");
        free(temp);
        return;
    }

    strcpy(temp->identifier, id);

    temp->next = id_list;
    id_list = temp;
}

char* generatorGetID() {
    if (id_list == NULL)
        return NULL;

    char* id = id_list->identifier;

    IDList* temp = id_list;
    id_list = id_list->next;
    free(temp);

    return id;
}

void generateUsedInternalFunctions() {
    for (intFC i = INPUTS; i <= CHR; i++) {
        if (internalFuncsUsed[i]) {
            setUpCodeInternal(i);
            generateCodeInternal();
        }
    }
}

/*
 *
 * GENERATOR INTERNAL FUNCTIONS
 *
 */

void generateCodeInternal() {
    // get rid of spaces created by macros
    char* output = (char*) calloc(sizeof(char)*strlen(code)+2, 1);
    if (code != NULL && output != NULL) {
        strcpy(output, code);
        for (int i = 0; code[i] != '\0'; i++) {
            if (output[i] == '\n' && output[i + 1] == ' ') {
                output[i] = ' ';
                output[i + 1] = '\n';
                i++;
            }
        }
    }

    // check if last character before null byte is \n
    // output to stdout
    if (output == NULL) {
        fprintf(stderr, "INTERNAL ERROR: CODE GENERATOR: unable to generate code of '%s' (malloc error)\n",
                (funcName != NULL ? funcName : "ic20int header"));
        return;
    }

    fprintf(stdout, "%s\n", output);
    free(output);
}

void setUpCodeInternal(intFC i) {
    switch(i) {
        case INPUTS:
            code = convertToString(inputs());
            break;
        case INPUTI:
            code = convertToString(inputi());
            break;
        case INPUTF:
            code = convertToString(inputf());
            break;
        case INPUTB:
            code = convertToString(inputb());
            break;
        case INT2FLOAT:
            code = convertToString(int2float());
            break;
        case FLOAT2INT:
            code = convertToString(float2int());
            break;
        case FUNCLEN:
            code = convertToString(funclen());
            break;
        case SUBSTR:
            code = convertToString(substr());
            break;
        case ORD:
            code = convertToString(ord());
            break;
        case CHR:
            code = convertToString(chr());
            break;
        default:
            code = NULL;
            break;
    }
}

int isFuncInternal(intFC* counter, const char* fName) {
    int i = *counter;
    int res = 1;
    while(internalFuncNames[i] != NULL) {
        res = strcmp(fName, internalFuncNames[i++]);
        if (!res) { // strcmp = 0 <=> no difference
            i--; // need to go by 1 back because of i++ upper
            break;
        }
    }

    internalFuncsUsed[i]++; // marks internal function as used so it will be generated
    *counter = i;
    return res;
}