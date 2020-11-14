/**
 * @file returns.h
 * @author Roman Janiczek (xjanic25@vutbr.cz)
 * @brief Header for return codes and helper functions
 * @version 0.1
 * @date 2020-11-14
 */
#ifndef IFJ_PROJECT_RETURNS_H
#define IFJ_PROJECT_RETURNS_H
// General
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * @brief Enum containing valid return codes
 * @enum returnCodes 
 */
typedef enum returnCodes {
    RC_OK,                      /**< Everything went OK, value of 0 */
    RC_ERR_LEXICAL_ANALYSIS,    /**< Error during lexical analysis (invalid lexem structure), value of 1 */
    RC_ERR_SYNTAX_ANALYSIS,     /**< Error during syntax analysis (invalid syntax or spacing), value of 2 */
    RC_ERR_SEMANTIC_PROG_FUNC,  /**< Semantic error in program (undefined function, variable, redefinition attepmt, etc.), value of 3 */
    RC_ERR_SEMANTIC_TYPE,       /**< Semantic error during type assignment to new variable, value of 4 */
    RC_ERR_SEMANTIC_TYPECOMP,   /**< Semantic error in type compatibility (arithmetic, etc.), value of 5 */
    RC_ERR_SEMANTIC_PARAM,      /**< Semantic error in program (invalid number of params or return values), value of 6 */
    RC_ERR_SEMANTIC_OTHER,      /**< Other semantic errors, value of 7 */
    RC_ERR_SEMANTIC_ZERODIV = 9,/**< Semantic error, zero division, value of 9 */
    RC_ERR_INTERNAL = 99        /**< Internal error, value of 99 */
} eRC;

/**
 * @brief Description of returnCodes for print
 */
char *errorString[] = {
    "SUCCESS",
    "LEXICAL ANALYSIS",
    "SYNTAX ANALYSIS",
    "SEMANTIC (UNDEFINED VALUES, REDEFINITON ATTEMPTS, ETC.)",
    "SEMANTIC (TYPE ASSIGN)",
    "SEMANTIC (TYPE COMPATIBILITY)",
    "SEMANTIC (PARAMETRS & RETURN VALUES)",
    "SEMANTIC (OTHER)",
    "",
    "SEMANTIC (ZERO DIVISION)"
    "INTERNAL",
};

/**
 * @brief Nice print of errors and other things
 */
void iPrint(eRC, bool);

#endif