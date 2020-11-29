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

/** macros for converting 'C code' into 'string literal' (used in generating internal functions) **/
#define toString(code) #code
#define convertToString(code) toString(code)

/** set ups code variable and outputs it to stdout via generateCode function **/
#define GEN_CODE(codeToGen) \
    code = codeToGen;       \
    generateCode();         \

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
 *
 * DEFINITIONS OF IFJ20 BUILT-IN FUNCTIONS
 *
 */

// all these functions return 6 if bad datatype in function argument has been given (via semantic analysis)

/* inputX functions: read 1 line (ended with EOL) from stdin
 * returns 0 if ok to second return value, 1 if error (missing input value, wrong format)
 */

// returns read line without EOL
#define inputs()  \n \
    LABEL _INPUTS    \n \
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
    LABEL _INPUTI   \n \
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
    LABEL _INPUTF   \n \
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
    LABEL _INPUTB   \n \
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
 * prints to stdout values of arguments given (not separated), no return value
 * count ... the count of pushes / arguments that will print work with
 */
void print(int count) {
    //somewhere before print: generateCode(pushPrintArguments())


    //generateCode(print_start())
    //for (int i = 0; i < count; i++) {
    //generateCode(print_process(arguments))
    //generateCode(print_end())
    //}
}

#define print_start() \n \
    LABEL _PRINT      \n \
                      \n \
    CREATEFRAME       \n \
    PUSHFRAME         \n \
    DEFVAR LF@out

#define print_process()  \n \
    POPS LF@out             \n \
    WRITE LF@out

#define print_end() \n \
    POPFRAME        \n \
    RETURN

/* number datatypes conversion */

// i ... int
// returns float value of argument i
#define int2float() \n \
    LABEL _INT2FLOAT \n \
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
    LABEL _FLOAT2INT \n \
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
    LABEL _FUNCLEN  \n \
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
    LABEL _SUBSTR   \n \
                \n \
    CREATEFRAME \n \
    PUSHFRAME   \n \
    DEFVAR LF@str   \n \
    POPS LF@str     \n \
    DEFVAR LF@pos   \n \
    POPS LF@pos \n \
    DEFVAR LF@subsize   \n \
    POP LF@subsize     \n \
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
    LABEL _ORD      \n \
                \n \
    CREATEFRAME \n \
    PUSHFRAME   \n \
    DEFVAR LF@str   \n \
    POPS LF@str \n \
    DEFVAR LF@pos   \n \
    POPS LF@pos     \n \
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
    LABEL _CHR  \n \
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
                \n \
    CALL $MAIN  \n \
    EXIT int@0  \n \

/*
 *
 * GENERATOR USER FUNCTIONS
 *
 */

void generateHeader() {
    code = convertToString(ic20int_header());
    generateCode();
}

void generateFunction(const char* fName, ...)  {
    funcName = (char*) fName;
    intFC i = INPUTS;
    int res = 1;

    // check if fName isnt internal function name
    while(internalFuncNames[i] != NULL) {
        res = strcmp(funcName, internalFuncNames[i++]);
        if (!res) {
            i--;
            break;
        }
    }

    // push func arguments to stack
    // (...) TODO

    // not 0 -> strcmp diff in all variations -> not generating internal function
    if (res) {
        char label[7+MAX_FNAME] = { 0 };

        // if generated function is main
        if (!strcmp(funcName, "main")) {
            strcpy(label, "\nLABEL $MAIN");
        } else {
            strcpy(label, "\nLABEL $");
            strcat(label, funcName);
        }

        char frame[] = "CREATEFRAME\nPUSHFRAME\n";
        GEN_CODE(label);
        GEN_CODE(frame);
    } else {
        setUpCodeInternal(i);
        generateCode();
    }
}

void generateDefinition(char* name, unsigned int scope) {
    // DEFVAR LF@name
    char new_variable[10+MAX_IDNAME+1+10] = "DEFVAR LF@";
    char scopeChar[10] = { 0 };
    sprintf(scopeChar, "%u", scope);

    strncat(new_variable, name, MAX_IDNAME);
    strcat(new_variable, "$");
    strcat(new_variable, scopeChar);

    GEN_CODE(new_variable);

    generateAssignment(name, scope);
}

void generateFuncCall(char* fName) {
    // CALL fName
    funcName = fName;
    char func_call[6+MAX_FNAME] = "CALL ";

    // Check if fName isnt internal func name
    intFC i = INPUTS;
    int res = 1;

    while(internalFuncNames[i] != NULL) {
        res = strcmp(funcName, internalFuncNames[i++]);
        if (!res) {
            i--;
            break;
        }
    }

    // To upper case chars
    char funcInt[MAX_FNAME] = { 0 };
    strncat(funcInt, fName, MAX_FNAME);
    for (int j = 0; funcInt[j] != '\0'; j++)
        funcInt[j] = toupper(funcInt[j]);

    if (res) // user function
        strcat(func_call, "$");
    else // internal function
        strcat(func_call, "_");

    strncat(func_call, funcInt, MAX_FNAME);
    GEN_CODE(func_call);
}

void generateAssignment(char* name, unsigned int scope) {
    // POP LF@name
    char pop_variable[7+MAX_IDNAME+2] = "POPS LF@";
    char scopeChar[10] = { 0 };
    sprintf(scopeChar, "%u", scope);

    strncat(pop_variable, name, MAX_IDNAME);
    strcat(pop_variable, "$");
    strcat(pop_variable, scopeChar);

    GEN_CODE(pop_variable);
}

void generateFuncEnd() {
    char popReturn[] = "POPFRAME\n\nRETURN";
    GEN_CODE(popReturn);
}

/*
 *
 * GENERATOR INTERNAL FUNCTIONS
 *
 */

void generateCode() {
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
        case PRINT:
            code = convertToString(print()); /**< todo not working now **/
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
            break;
    }
}