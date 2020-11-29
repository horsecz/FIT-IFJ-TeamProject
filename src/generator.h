/**
 * @file generator.h
 * @author Dominik Horky (xhorky32@vutbr.cz)
 * @brief generator.c header file
 * @version 0.1
 * @date 2020-11-20
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdbool.h>
#include "symtable.h"
#include "scanner.h"

/*
 * USER FUNCTIONS AND VARIABLES
 */

typedef union ValueUnion {
    int64_t int_v;
    double float64_v;
    char* string_v;
    bool bool_v;
} Value;

/**
 * @enum Datatypes
 * @brief to distinguish data-types of variables
 * @note compatible with @enum stDataTypes defined in symtable.h
 */
typedef enum Datatypes {
    STRING_T = STRING,
    INT_T = INT,
    FLOAT64_T = FLOAT64,
    BOOL_T = BOOL
} DataType;

/**
 * @brief Generates header required by ic20int and jump to main function (after 'prolog main' keywords in IFJ20 code)
 * @pre prolog syntax (rule <prolog>) must be OK (in syntax analysis)
 */
void generateHeader();

/**
 * @brief Generates beginning of function (-> func scope, args, label) or entire function code if it's internal function
 * @param fName name of function (string || string literal)
 * @param ... arguments of function
 * @pre syntax + semantic OK for function header
 */
void generateFunction(const char* fName, ...);

/**
 * @brief Generates definition of variable
 * @param name identifier of variable
 * @param scope number identifier of scope in which is this variable
 * @pre syntax + semantic OK for variable definition
 * @pre value of variable MUST be on TOP of stack
 */
void generateDefinition(char* name, unsigned int scope);

/**
 * @brief Generates function call
 * @param fName name of function to be called
 * @pre syntax + semantic OK for function call
 */
void generateFuncCall(char* fName);

/**
 * @brief Generates variable assignment
 * @param name identifier of variable
 * @param scope number ID of scope in which variable is
 * @pre syntax + semantic OK for this action
 * @pre variable MUST be defined (if not: semantic fails => this won't be called)
 * @pre value MUST be on TOP of stack
 */
void generateAssignment(char* name, unsigned int scope);

/**
 * @brief Generates end of function (return, frame pop)
 * @pre syntax + semantic OK
 * @pre MUST be used only in function (scope) end
 */
void generateFuncEnd();

// TODO {

void generate_if(int type); /** requires type of if statement -> 0 = if ; 1 = else ; -1 = else if **/
void generate_for();
//void otherGenerateFunctions ...

// } TODO

/*
 * INTERNAL FUNCTIONS AND VARIABLES
 */

/**
 * @brief used only for better clarity in deciding which internal function is going to be generated
 * @enum internalFunction
 */
typedef enum internalFunctions {
    INPUTS, // 0
    INPUTI,
    INPUTF,
    INPUTB, // 3

    PRINT, // 4

    INT2FLOAT, // 5
    FLOAT2INT, // 6

    FUNCLEN, // 7
    SUBSTR,
    ORD,
    CHR // 10
} intFC;

/** @brief generates code to stdout **/
void generateCode();

/**
 * @brief set ups code variable for generating internal function code
 * @param i from @enum internalFunction, which internal function will be generated
 */
void setUpCodeInternal(intFC i);

/** @brief used for internal function names **/
extern const char* internalFuncNames[];

/** @brief code that will be output to stdout will be stored here **/
extern char* code;


#endif //GENERATOR_H
