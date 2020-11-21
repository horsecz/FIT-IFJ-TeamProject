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
#include "symtable.h"
#include "returns.h"

/**
 * @brief Main parser logic
 * @param SymbolTable Symbol table (implemented as BST)
 */
eRC parser(Token *tk, BTNodePtr* SymbolTable);

/** ---------------------------------------------------- **
 *  List of functions generated according to LL1-grammar  *
 ** ---------------------------------------------------- **/

/**
 * @brief Rule '<program> -> <prolog> <eol_m> <functions>'
 * @return eRC @see @enum returnCodes
 */
eRC program();

/**
 * @brief Rule '<prolog> -> package main'
 * @return eRC @see @enum returnCodes
 */
eRC prolog();

/**
 * @brief Rule '<eol_m> -> EOL <eol_r>'
 * @return eRC @see @enum returnCodes
 */
eRC eolM();

/**
 * @brief Rule '<eol_r> -> EOL <eol_r>' or '<eol_r> -> eps'
 * @return eRC @see @enum returnCodes
 */
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
eRC assignment();
eRC unary();
eRC variableIdNext();
// IF
eRC ifElse();
eRC ifElseExpanded();
// FOR
eRC forDefine();
eRC forAssign();
// RETURNS
eRC returnCommand();
eRC returnStatement();

#endif