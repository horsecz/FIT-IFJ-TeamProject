/**
 * @file parser.h
 * @author Roman Janiczek (xjanic25@vutbr.cz)
 * @brief Header for parser
 * @version 0.1
 * @date 2020-11-14
 */
#ifndef IFJ_PROJECT_PARSER_H
#define IFJ_PROJECT_PARSER_H
// General 
#include <stdio.h>
#include <stdlib.h>
// Project specific
#include "scanner.h"
#include "returns.h"

/**
 * @brief Main parser logic 
 */
eRC parser(BTNode_t** SymbolTable);
//*** ACCORDING TO LL1-GRAMAR FUNCTION TO BE CALLED
// PROGRAM
eRC program();
eRC prolog();
// EOLs
eRC eolM();
eRC eolR();
// FUNCTION RELATED
eRC functions();
eRC function();
eRC functionNext();
eRC functionReturn();
// FUNCTION ARGUMENTS
eRC arguments();
eRC argumentNext();
// TYPE
eRC type();
eRC typeFunction();
eRC typeFunctionNext();
// COMMANDS / CODE
eRC commands();
eRC command();
eRC commandBlock();
// STATEMENTS
eRC statement();
eRC statementMul();
eRC variableIdNext();
// FOR
eRC forDefine();
eRC forAssign();
// RETURNS
eRC returnCommand();
eRC returnStatement();

#endif