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
 * if function is internal, its label begins with '_' otherwise with '$'
 * @param fName name of function (string || string literal)
 * @pre syntax + semantic OK for function header
 */
void generateFunction(const char* fName);

/**
 * @brief Pushes 1 argument to stack
 * @param argName name of the argument (identifier)
 * @post At the top of stack is last argument of function
 */
void generateFuncArgument(char* argName);

/**
 * @brief Generates function call
 * @param fName name of function to be called
 * @pre syntax + semantic OK for function call
 */
void generateFuncCall(char* fName);

/**
 * @brief Pushe 1 return value to stack
 * @param retName name of return value (identifier)
 * @post At the top of stack is last return value
 */
void generateFuncReturn(char* retName);

/**
 * @brief Generates end of function (return to last instruction before func-call, frame pop)
 * @pre syntax + semantic OK
 * @pre MUST be used only in function (scope) end
 */
void generateFuncEnd();

/**
 * @brief Generates definition of variable
 * @param name identifier of variable
 * @pre syntax + semantic OK for variable definition
 * @pre value of variable MUST be on TOP of stack
 * @note variable name contains 'name' + '$' + 'number', where number is automatically assigned
 * (as count of variables in program)
 */
void generateDefinition(char* name);

/**
 * @brief Generates variable assignment
 * @param name identifier of variable
 * @pre syntax + semantic OK for this action
 * @pre variable MUST be defined (if not: semantic fails => this won't be called)
 * @pre value MUST be on TOP of stack
 */
void generateAssignment(char* name);

/**
 * @brief Internal function print (cant be written as macro), outputs 1 argument to stdout
 * @note In intended to be called repeatedly
 * @param type datatype of argument to print
 */
void generatePrint(DataType type);

/**
 * @brief Generates beginning of if (else if, else) scope
 */
void generateIfScope();

/**
 * @brief Generates end of if (else if, else) scope
 */
void generateIfScopeEnd();

// TODO {

void generateForScope();

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

/** @brief generates internal code (includes: internal function code, header) to stdout **/
void generateCodeInternal();

/**
 * @brief set ups code variable for generating internal function code
 * @param i from @enum internalFunction, which internal function will be generated
 */
void setUpCodeInternal(intFC i);

/**
 * @brief checks if function with fName is internal or not
 * @param counter counter of array internalFuncNames (used in setUpCodeInternal(...) function)
 * @param fName name, identifier of function
 * @return returns 0 if function is internal function or not 0 value if not (see strcmp return values)
 */
int isFuncInternal(intFC* counter, const char* fName);

/** @brief used for internal function names **/
extern const char* internalFuncNames[];

/** @brief internal function code that will be output to stdout will be stored here **/
extern char* code;

/** @brief (internal) if scope counter **/
extern int if_cnt;

/** @brief (internal) else scope counter **/
extern int else_cnt;

/** @brief (internal) else if scope counter **/
extern int elseif_cnt;

/** @brief (internal) for scope counter **/
extern int for_cnt;


#endif //GENERATOR_H
