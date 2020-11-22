/**
 * @file generator.h
 * @author Dominik Horky (xhorky32@vutbr.cz)
 * @brief generator.c header file
 * @version 0.1
 * @date 2020-11-20
 */

#ifndef GENERATOR_H
#define GENERATOR_H

/*
 *
 * BUILT-IN FUNCTIONS
 *
 */

// all these functions return 6 if bad datatype in function argument has been given

/* inputX functions: read 1 line (ended with EOL) from stdin
 * returns 0 if ok to second return value, 1 if error (missing input value, wrong format)
 */

// returns read line without EOL
#define inputs()    \
    LABEL _INPUTS   \
                    \
    CREATEFRAME     \
    PUSHFRAME       \
    DEFVAR LF@val   \
    DEFVAR LF@retval        \
    MOVE LF@retval int@0    \
                            \
    READ LF@val string      \
    JUMPIFEQ _INPUTS_ERR LF@val nil@nil \
    JUMP _INPUTS_END        \
                            \
    LABEL _INPUTS_ERR       \
    MOVE LF@retval int@1    \
    JUMP _INPUTS_END        \
                            \
    LABEL _INPUTS_END       \
    PUSHS LF@val    \
    PUSHS LF@retval \
    POPFRAME        \
                    \
    RETURN          \

// returns read line without all white space characters (incl. EOL)
// wrong characters before number mean wrong format => second return value = 1
// hexadecimal numbers are supported
// reads and returns int number
#define inputi()    \
    LABEL _INPUTI   \
                    \
    CREATEFRAME     \
    PUSHFRAME       \
    DEFVAR LF@val   \
    DEFVAR LF@retval        \
    MOVE LF@retval int@0    \
                    \
    READ LF@val int \
    JUMPIFEQ _INPUTI_ERR LF@val nil@nil \
    JUMP _INPUTI_END        \
                    \
    LABEL _INPUTI_ERR       \
    MOVE LF@retval int@1    \
    JUMP _INPUTI_END        \
                    \
    LABEL _INPUTI_END       \
    PUSHS LF@val    \
    PUSHS LF@retval         \
    POPFRAME        \
                    \
    RETURN          \

// reads and returns float64 number
#define inputf()    \
    LABEL _INPUTF   \
                    \
    CREATEFRAME     \
    PUSHFRAME       \
    DEFVAR LF@val   \
    DEFVAR LF@retval        \
    MOVE LF@retval int@0    \
                    \
    READ LF@val float \
    JUMPIFEQ _INPUTF_ERR LF@val nil@nil \
    JUMP _INPUTF_END        \
                    \
    LABEL _INPUTF_ERR       \
    MOVE LF@retval int@1    \
    JUMP _INPUTF_END        \
                    \
    LABEL _INPUTF_END       \
    PUSHS LF@val    \
    PUSHS LF@retval         \
    POPFRAME        \
                    \
    RETURN          \

// reads and returns bool value
#define inputb()    \
    LABEL _INPUTB   \
                    \
    CREATEFRAME     \
    PUSHFRAME       \
    DEFVAR LF@val   \
    DEFVAR LF@retval        \
    MOVE LF@retval int@0    \
                    \
    READ LF@val bool \
    JUMPIFEQ _INPUTF_ERR LF@val nil@nil \
    JUMP _INPUTF_END        \
                    \
    LABEL _INPUTF_ERR       \
    MOVE LF@retval int@1    \
    JUMP _INPUTF_END        \
                    \
    LABEL _INPUTF_END       \
    PUSHS LF@val    \
    PUSHS LF@retval         \
    POPFRAME        \
                    \
    RETURN          \

/* print function
 * prints to stdout values of arguments given (not separated), no return value
 * count ... the count of pushes / arguments that will print work with
 */
void print(int count);
#define print_start() \
    LABEL _PRINT      \
                      \
    CREATEFRAME       \
    PUSHFRAME         \
    DEFVAR LF@out     \

#define print_process()  \
    POPS LF@out             \
    WRITE LF@out            \

#define print_end() \
    POPFRAME        \
    RETURN          \

/* number datatypes conversion */

// i ... int
// returns float value of argument i
#define int2float() \
    LABEL _INT2FLOAT \
                     \
    CREATEFRAME      \
    PUSHFRAME        \
    DEFVAR LF@val    \
    POPS LF@val     \
    DEFVAR LF@result  \
                    \
    DEFVAR LF@typecheck \
    TYPE LF@typecheck LF@val \
    JUMPIFNEQ _INT2FLOAT_TYPE_OK LF@typecheck string@int \
    JUMP _INT2FLOAT_TYPE               \
                    \
    LABEL _INT2FLOAT_TYPE    \
    MOVE LF@result float@0x6.0p+0    \
    JUMP _INT2FLOAT_END \
                    \
    LABEL _INT2FLOAT_TYPE_OK                \
    INT2FLOAT LF@result LF@val                          \
    JUMP _INT2FLOAT_END \
                    \
    LABEL _INT2FLOAT_END                \
    PUSHS LF@result  \
    POPFRAME         \
                     \
    RETURN           \

// f .. float64
// returns int value of argument f
#define float2int() \
    LABEL _FLOAT2INT \
                     \
    CREATEFRAME      \
    PUSHFRAME        \
    DEFVAR LF@val    \
    POPS LF@val      \
                     \
    DEFVAR LF@result    \
                    \
    DEFVAR LF@typecheck \
    TYPE LF@typecheck LF@val \
    JUMPIFNEQ _FLOAT2INT_TYPE LF@typecheck string@float   \
    JUMP _FLOAT2INT_TYPE_OK  \
            \
    LABEL _FLOAT2INT_TYPE    \
    MOVE LF@result int@6    \
    JUMP _INT2FLOAT_END \                \
                    \
    LABEL _FLOAT2INT_TYPE_OK     \
    FLOAT2INT LF@result LF@val                           \
    JUMP _FLOAT2INT_END \
                    \
    LABEL _FLOAT2INT_END    \
    PUSHS LF@result  \
    POPFRAME         \
                     \
    RETURN           \


/* string functions */

// s ... string
// returns (int) length (number of characters, including \n, etc.) of s string
#define funclen()  \
    LABEL _FUNCLEN  \
                    \
    CREATEFRAME     \
    PUSHFRAME       \
    DEFVAR LF@str   \
    DEFVAR LF@size  \
    POPS LF@str     \
                   \
    DEFVAR LF@typecheck \
    TYPE LF@typecheck LF@str \
    JUMPIFNEQ _FUNCLEN_TYPE LF@typecheck string@string   \
    JUMP _FUNCLEN_TYPE_OK  \               \
                   \
    LABEL _FUNCLEN_TYPE_OK               \
    STRLEN LF@size LF@str    \
    JUMP _FUNCLEN_END \
                   \
    LABEL _FUNCLEN_TYPE \
    MOVE LF@result string@6    \
    JUMP _FUNCLEN_END \ \
                   \
    LABEL _FUNCLEN_END     \
    PUSHS LF@size   \
    POPFRAME        \
                    \
    RETURN          \

// s ... string
// i ... int
// n ... int
// first return value (string) is substring of string s, substring starts
// from i-th character of s and is n characters long, or
// all characters from i-th character (including) if n > len(s)-i
// second return value (int) is 0 if ok, or 1 if i is not in interval <0, len(s)>,
// or n < 0
#define substr() \
    LABEL _SUBSTR   \
                \
    CREATEFRAME \
    PUSHFRAME   \
    DEFVAR LF@str   \
    POPS LF@str     \
    DEFVAR LF@pos   \
    POPS LF@pos \
    DEFVAR LF@subsize   \
    POP LF@subsize     \
    DEFVAR LF@retval    \
    DEFVAR LF@retval2   \
    MOVE LF@retval2 int@0 \
                 \
    DEFVAR LF@typecheck \
    TYPE LF@typecheck LF@str \
    JUMPIFNEQ _SUBSTR_TYPE LF@typecheck string@string \
    TYPE LF@typecheck LF@pos    \
    JUMPIFNEQ _SUBSTR_TYPE LF@typecheck string@int    \
    TYPE LF@typecheck LF@subsize            \
    JUMPIFNEQ _SUBSTR_TYPE LF@typecheck string@int \
                \
    DEFVAR LF@newstr    \
    DEFVAR LF@cmp   \
    LT LF@cmp LF@pos int@0  \
    JUMPIFEQ _SUBSTR_ERR LF@cmp bool@true   \
                     \
    JUMPIFEQ _SUBSTR_ERR LF@str nil@nil \
    JUMPIFEQ _SUBSTR_ERR LF@subsize int@0   \
                \
    DEFVAR LF@size  \
    STRLEN LF@size LF@str   \
    GT LF@cmp LF@pos LF@size    \
    JUMPIFEQ _SUBSTR_ERR LF@cmp bool@true   \
    DEFVAR LF@tmpsize   \
    MOVE LF@tmpsize LF@subsize  \
    ADD LF@tmpsize LF@tmpsize LF@pos    \
    GT LF@cmp LF@tmpsize LF@size    \
    JUMPIFEQ _SUBSTR_ERR LF@cmp bool@true   \
                    \
    DEFVAR LF@character \
    DEFVAR LF@mov_pos   \
    MOVE LF@newstr LF@str   \
    MOVE LF@mov_pos LF@pos  \
                    \
    DEFVAR LF@cycle \
    MOVE LF@cycle int@0 \
    JUMP _SUBSTR_CYCLE  \
                    \
    LABEL _SUBSTR_CYCLE \
    GETCHAR LF@character LF@str LF@mov_pos  \
    SETCHAR LF@newstr LF@cycle LF@character \
    ADD LF@cycle LF@cycle int@1     \
    ADD LF@mov_pos LF@mov_pos int@1 \
    JUMPIFNEQ _SUBSTR_CYCLE LF@cycle LF@subsize \
    JUMP _SUBSTR_CYCLE2 \
                    \
    LABEL _SUBSTR_CYCLE2    \
    JUMPIFEQ _SUBSTR_END LF@cycle LF@size   \
    SETCHAR LF@newstr LF@cycle string@\000  \
    ADD LF@cycle LF@cycle int@1 \
    JUMPIFNEQ _SUBSTR_CYCLE2 LF@cycle LF@size   \
    JUMP _SUBSTR_END    \
                    \
    LABEL _SUBSTR_ERR   \
    MOVE LF@newstr string@nil  \
    MOVE LF@retval2 int@1   \
    JUMP _SUBSTR_END        \
                 \
    LABEL _SUBSTR_TYPE  \
    MOVE LF@newstr string@nil               \
    MOVE LF@retval2 int@6   \
    JUMP _SUBSTR_END \
                 \
    LABEL _SUBSTR_END   \
    MOVE LF@retval LF@newstr    \
    PUSHS LF@retval \
    PUSHS LF@retval2    \
                \
    POPFRAME    \
    RETURN      \

// s ... string
// i ... int
// first return value (int) is ordinal value (ASCII) of character
// on position i in string s
// second return value (int) is 0 if ok or 1 if i is not in interval <0, len(s)-1>
#define ord()    \
    LABEL _ORD      \
                \
    CREATEFRAME \
    PUSHFRAME   \
    DEFVAR LF@str   \
    POPS LF@str \
    DEFVAR LF@pos   \
    POPS LF@pos     \
    DEFVAR LF@retval    \
    DEFVAR LF@retval2   \
    MOVE LF@retval2 int@0   \
                 \
    DEFVAR LF@typecheck \
    TYPE LF@typecheck LF@str \
    JUMPIFNEQ _ORD_TYPE LF@typecheck string@string \
    TYPE LF@typecheck LF@pos    \
    JUMPIFNEQ _ORD_TYPE LF@typecheck string@int \
                 \
    DEFVAR LF@cmp           \
    LT LF@cmp LF@pos int@0  \
    JUMPIFEQ _ORD_ERR LF@cmp bool@true  \
                    \
    JUMPIFEQ _ORD_ERR LF@str nil@nil    \
                    \
    DEFVAR LF@size  \
    STRLEN LF@size LF@str   \
    SUB LF@size LF@size int@1   \
    GT LF@cmp LF@pos LF@size    \
    JUMPIFEQ _ORD_ERR LF@cmp bool@true  \
                    \
    STRI2INT LF@retval LF@str LF@pos    \
    JUMP _ORD_END   \
                    \
    LABEL _ORD_ERR  \
    MOVE LF@retval int@0  \
    MOVE LF@retval2 int@1   \
    JUMP _ORD_END       \
                 \
    LABEL _ORD_TYPE \
    MOVE LF@retval int@0  \
    MOVE LF@retval2 int@6   \
    JUMP LABEL _ORD_END \
                 \
    LABEL _ORD_END  \
    PUSHS LF@retval \
    PUSHS LF@retval2    \
                    \
    POPFRAME    \
    RETURN      \

// i ... int
// first return value (string) is one-char string containing character,
// which ASCII code is given in argument i
// second return value (int) is 0 if ok, or 1 if i is not in interval <0, 255>
#define chr()  \
    LABEL _CHR  \
                \
    CREATEFRAME \
    PUSHFRAME   \
    DEFVAR LF@retval    \
    DEFVAR LF@ch        \
    POPS LF@ch  \
    DEFVAR LF@retval2   \
    MOVE LF@retval2 int@0   \
               \
    DEFVAR LF@typecheck \
    TYPE LF@typecheck LF@ch \
    JUMPIFNEQ _CHR_TYPE LF@typecheck string@int   \
                        \
    DEFVAR LF@cmp       \
    LT LF@cmp LF@ch int@0   \
    JUMPIFEQ _CHR_ERR LF@cmp bool@true  \
                        \
    GT LF@cmp LF@ch int@255 \
    JUMPIFEQ _CHR_ERR LF@cmp bool@true  \
                        \
    INT2CHAR LF@retval LF@ch    \
    JUMP _CHR_END   \
                    \
    LABEL _CHR_ERR  \
    MOVE LF@retval string@nil  \
    MOVE LF@retval2 int@1   \
    JUMP _CHR_END           \
               \
    LABEL _CHR_TYPE     \
    MOVE LF@retval string@nil                     \
    MOVE LF@retval2 int@6   \
    JUMP _CHR_END \
                    \
    LABEL _CHR_END  \
    PUSHS LF@retval     \
    PUSHS LF@retval2    \
                    \
    POPFRAME    \
    RETURN      \

/*
 *
 * CODE GENERATING
 *
 */

// TODO {

void generate_header();
void generate_builtin_functions();

void generate_function();
void generate_cmd_block();
void generate_if(int type); /** requires type of if statement -> 0 = if ; 1 = else ; -1 = else if **/
void generate_for();

void generate_definition();
void generate_assignment();
void generate_expression();
void generate_func_call();


// } TODO


#endif //GENERATOR_H
