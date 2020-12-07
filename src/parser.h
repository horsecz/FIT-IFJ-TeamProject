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
#include "precedent.h"
#include "generator.h"

/**
 * @brief Variable for precedent (expression analysis)
 */
extern bool precRightBrace;

/**
 * @brief Main parser logic \n
 *        Contains mainly enviroment preparation before parsing and then calls @see program() to initiate
 *        parsing. After this it checks return value and acts upon that
 * @param tk Pointer to token //TODO: Not sure why this is needed
 * @return eRC @see returnCodes
 */
eRC parser(Token *tk);

/** ----------------------------------------------------------- **
 *  List of functions generated according to LL1-grammar         *
 ** ----------------------------------------------------------- **/

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *               BASIC PROGRAM STRUCTURE CHECKS                  *
 *                                                               * 
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Program parse \n
 *        Goes through EOLs at the beggining of the program,
 *        does basic rule check and then calls appropriate parse functions.
 * @note <program> -> <prolog> <eol_m> <functions>
 * @return eRC @see returnCodes
 * @pre Global and shared variables are initialized and setup for parsing
 * @post Parsing should be complete and prepared for code generation or code should already be generated
 */
eRC program();

/**
 * @brief Prolog parse \n
 *        Checks if program starts with 'package main'.
 * @note <prolog> -> package main
 * @return eRC @see returnCodes
 * @pre Expects token to be at the TYPE_KEYWORD
 * @post Leaves token at the TYPE_IDENTIFIER for 'main' -> next function has to call getToken!
 */
eRC prolog();

/**
 * @brief EOL after 'package main' parse \n
 *        Require EOL after 'package main'
 * @note <eol_m> -> EOL <eol_r>
 * @return eRC @see returnCodes
 * @post Next function has to call getToken
 */
eRC eolM();

/**
 * @brief EOL indefinite parse \n
 *        Go through indefinite number of EOLs after @see eolM
 * @note <eol_r> -> EOL <eol_r>
 * @note <eol_r> -> eps
 * @return eRC @see returnCodes
 * @post Token will be ready for processing (do not call getToken in function following this function)
 */
eRC eolR();

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                   FUNCTION RELATED CHECKS                     *
 *                                                               * 
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   GENERAL FUNCTION CHECKS                                     *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Parse block of functions \n
 *        According to roles this block has to contain at least one function (and at least one function
 *        with identifier 'main')
 * @note <functions> -> <func> <function_n>
 * @return eRC @see returnCodes
 * @pre Token should be prepared before calling this function @see eolR()
 * @post
 */
eRC functionsBlock();

/**
 * @brief Parse function \n
 *        Check function and its <cmd_block>. Save information about functions and its inside structure.
 *        Information about function (params, return values, ID) will be saved to GST (global symtable),
 *        additional symtables will be created for variables of different scopes
 * @note <func> -> func ID ( <arguments> ) <func_return> <cmd_block> 
 * @return eRC @see returnCodes
 * @pre Token should be prepared in functions preceding this function
 * @post Token will be prepared for other step -> @see commandBlock()
 */
eRC function();

/**
 * @brief Parse next function \n
 *        Try to parse another functions that may be in the source code
 * @note <function_n> -> <func> <function_n> 
 * @note <function_n> -> eps
 * @return eRC @see returnCodes
 * @pre Token should be prepared (skipped EOLs, etc..) -> @see commandBlock()
 * @post
 */
eRC functionNext();

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   TYPE FUNCTION CHECKS                                        *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Parse function arguments \n
 *        Gets the first identifier of the function argument and then call type() for type parse
 * @note <arguments> -> ID <type> <arguments_n>
 * @note <arguments> -> eps
 * @return eRC @see returnCodes
 * @post Token may be ready for parsing if the <arguments> -> eps rule was applied (everytime?), check for safety
 */
eRC arguments();

/**
 * @brief Parse next argument of the function \n
 *        Tries to get the token with comma, if comma is not found then it's accepted as an eps state
 * @note <arguments_n> -> , ID <type> <arguments_n>
 * @note <arguments_n> -> eps
 * @return eRC @see returnCodes
 * @post Token may be ready for parsing if the <arguments_n> -> eps rule was applied (everytime?), check for safety
 */
eRC argumentNext();

/**
 * @brief Parse type \n
 *        Parse type of the variable (in function arguments and return types)
 * @note <type> -> int
 * @note <type> -> float64
 * @note <type> -> string
 * @note <type> -> bool
 * @return eRC @see returnCodes
 * @pre Token has to be prepared before calling this function (no getToken call in this function)
 * @post Leaves token as it was -> next function has to call getToken!
 */
eRC type();

/**
 * @brief Parse function return setting \n
 *        Can be empty, () or contain (<types>) -> where <types> can be one type or more
 * @note <func_return> -> ( <f_type> <r_type_n> )
 * @note <func_return> -> eps
 * @return eRC @see returnCodes
 * @pre Expects token to be pre-prepared before calling this function
 * @post This function doesn't prepare token for the next function that will be called
 */
eRC functionReturn();

/**
 * @brief Parse function return type \n
 *        Checking inside of the brackets
 * @note <f_type> -> <type>
 * @note <f_type> -> eps
 * @return eRC @see returnCodes
 * @pre Expects token to be pre-prepared before calling this function
 * @post This function doesn't prepare token for the next function that will be called
 */
eRC functionReturnType();

/**
 * @brief Parse next function return type \n
 *        Check if there is more return types
 * @note <r_type_n> -> <type>
 * @note <r_type_n> -> eps
 * @return eRC @see returnCodes
 * @pre Expects token to be pre-prepared (call of getToken() done before calling this function)
 * @post This function doesn't prepare token for the next function that will be called
 */
eRC functionReturnTypeNext();

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   COMMANDS FUNCTION CHECK                                     *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Parse command block of the function \n
 *        Command block is composed of commands where every command is on the new line (one instruction per line)
 * @note <cmd_block> -> { EOL <commands> } EOL
 * @return eRC @see returnCodes
 * @post Token will be prepared for other processing
 */
eRC commandBlock();

/**
 * @brief Parse one line of commands
 * @note <commands> -> <cmd> EOL <commands>
 * @note <commands> -> eps
 * @return eRC @see returnCodes
 * @pre Expecting token to be pre-prepared
 * @post Token will stay at the RIGHT CURLY BRACKET
 */
eRC commands();

/**
 * @brief Parse command \n
 *        See notes for acceptable commands
 * @note <cmd> -> ID <statement>
 * @note <cmd> -> if <expression> <cmd_block> <if_else>
 * @note <cmd> -> for <for_definition> ; <expression> ; <for_assignment> <cmd_block>
 * @note <cmd> -> <return_cmd>
 * @return eRC @see returnCodes
 * @pre Expecting token to be pre-prepared
 * @post Token will be prepared for other processing
 */
eRC command();

/**
 * @brief Parse statement part of 'ID <statement>' command \n
 *        This is used for funcion call or definition & assignment for variables
 * @note <statement> -> <id_mul> = <assignment>
 * @note <statement> -> ( <arguments> )
 * @note <statement> -> := <expression>
 * @note <statement> -> <unary> <expression>
 * @return eRC @see returnCodes
 * @pre Expecting token to be pre-prepared
 * @post Token will be prepared for other processing
 */
eRC statement();

/**
 * @brief Parse multiple IDs \n
 *        If there is more IDs on the left side of the assignment
 * @note <id_mul> -> , ID <id_mul>
 * @note <id_mul> -> eps
 * @return eRC @see returnCodes
 * @pre Expecting token to be pre-prepared
 * @post Token will be prepared for other processing (or at least should be)
 */
eRC multipleID();

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   ASSIGN & DEFINE CHECK                                       *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Parse assignment (expression side) \n
 *        Check <expression> and expression count in case of multiple IDs & validates function return in case of
 *        function call
 * @note <assignment> -> <expression> <expr_n>
 * @note <assignment> -> ID ( <arguments> )
 * @return eRC @see returnCodes
 * @pre Expects token to be pre-prepared
 * @post Token will be prepared for other processing (or at least should be) 
 */
eRC assignment();

/**
 * @brief Verify unary \n
 *        Vefifies unary and does error output in case of failure
 * @note <unary> -> +=
 * @note <unary> -> -=
 * @note <unary> -> *=
 * @note <unary> -> /=
 * @return eRC @see returnCodes
 * @post Token will be same as it was before this function (no changes on token)
 */
eRC unary();

/**
 * @brief Check if there is more than one expression
 * @note <expr_n> -> <expression> <expr_n>
 * @note <expr_n> -> eps
 * @return eRC @see returnCodes
 * @pre Expects token to be pre-prepared
 * @post Token should be prepared for other processing (check may be a good idea)
 */
eRC expressionNext();

/**
 * @brief Parse arguments of function call
 * @note <arguments_fc> -> ( <arguments_fc> )
 * @note <arguments_fc> -> ( )
 * @return eRC @see returnCodes
 */
eRC funcCallArguments();

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   IF ELSE CHECK                                               *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Parse else part of the if
 * @note <if_else> -> else <if_else_st>
 * @note <if_else> -> eps
 * @return eRC @see returnCodes
 * @pre 
 * @post 
 */
eRC ifElse();

/**
 * @brief Parse another if following else
 * @note <if_else_st> -> if <cmd_block> <if_else>
 * @note <if_else_st> -> <cmd_block>
 * @return eRC @see returnCodes
 * @pre 
 * @post 
 */
eRC ifElseExpanded();

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   FOR CHECK                                                   *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Parse definitoon of variable for for (can be empty)
 * @note <for_definition> -> ID := <expression>
 * @note <for_definition> -> eps
 * @return eRC @see returnCodes
 * @pre 
 * @post 
 */
eRC forDefine();

/**
 * @brief Parse assignment to variable for for (can be empty)
 * @note <for_assignment> -> ID = <expression>
 * @note <for_assignment> -> eps
 * @return eRC @see returnCodes
 * @pre Expects token to be pre-prepared for processing
 * @post 
 */
eRC forAssign();

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   RETURN CHECK                                                *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

/**
 * @brief Parse return command
 * @note <return_cmd> -> return <return_stat>
 * @note <return_cmd> -> eps
 * @return eRC @see returnCodes
 * @pre 
 * @post 
 */
eRC returnCommand();

/**
 * @brief Parse return statement
 * @note <return_stat> -> <expression>
 * @note <return_stat> -> eps
 * @return eRC @see returnCodes
 * @pre 
 * @post 
 */
eRC returnStatement();

#endif