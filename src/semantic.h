/**
 * @file semantic.h
 * @author Dominik Horky (xhorky32@vutbr.cz)
 * @brief Semantic helper header
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include "symtable.h"

/**
 * @brief Main function for semantic analysis, checks if types for assignment (to variables) are ok when calling function.
 * @param functionName name of function which is going to be called
 * @param stFunctions @see parser.c
 * @param stack @see parser.c
 * @param currentVar @see parser.c
 * @param currentVarMul @see parser.c
 * @param numberOfIDs @see parser.c
 * @param printLastToken @see parser.c
 * @return @see eRC
 */
eRC semantic_analysis (char* functionName, stNodePtr stFunctions, stStack stack, stID currentVar, stID* currentVarMul, int numberOfIDs);

/**
 * @brief Converts type to string.
 * @param type variable type in stVarType datatype
 * @param stringType variable where will be string located
 */
char* setErrorType (stVarType type);
#endif // SEMANTIC_H
