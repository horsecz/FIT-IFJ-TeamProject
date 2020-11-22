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

// TODO {

// all these functions return 6 if bad datatype in function argument has been given

/* inputX functions: read 1 line (ended with EOL) from stdin
 * returns 0 if ok to second return value, 1 if error (missing input value, wrong format)
 */

// returns read line without EOL
#define inputs()

// returns read line without all white space characters (incl. EOL)
// wrong characters before number mean wrong format => second return value = 1
// hexadecimal numbers are supported
// reads and returns int number
#define inputi()

// reads and returns float64 number
#define inputf()

/* print function
 * prints to stdout values of arguments given (not separated), no return value
 * int argument will be output with %d, float64 with %a
 */
#define print(...)

/* number datatypes conversion */

// i ... int
// returns float value of argument i
#define int2float(i)

// f .. float64
// returns int value of argument f
#define float2int(f)

/* string functions */

// s ... string
// returns (int) length (number of characters, including \n, etc.) of s string
#define funclen(s)

// s ... string
// i ... int
// n ... int
// first return value (string) is substring of string s, substring starts
// from i-th character of s and is n characters long, or
// all characters from i-th character (including) if n > len(s)-i
// second return value (int) is 0 if ok, or 1 if i is not in interval <0, len(s)>,
// or n < 0
#define substr(s,i,n)

// s ... string
// i ... int
// first return value (int) is ordinal value (ASCII) of character
// on position i in string s
// second return value (int) is 0 if ok or 1 if i is not in interval <0, len(s)-1>
#define ord(s,i)

// i ... int
// first return value (string) is one-char string containing character,
// which ASCII code is given in argument i
// second return value (int) is 0 if ok, or 1 if i is not in interval <0, 255>
#define chr(i)

/*
 *
 * CODE GENERATING
 *
 */

void generate_header();

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
