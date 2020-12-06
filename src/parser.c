/**
 * @file parser.c
 * @author Roman Janiczek (xjanic25@vutbr.cz)
 * @author Dominik Horky (xhorky32@vutbr.cz)
 * @brief Parser implementation
 * @version 0.420
 * @date 2020-11-15
 *
 * @note after handling <expression>, getting next token is expected (aka I've got 1 token after expression)
 */
#include "parser.h"

/**
 * @brief Gets next token + checks if lexical error or internal error occured
 * @pre function where this is used is of eRC return type!
 */
#define getToken(returnVar,tokenVar)                                                    \
    do {                                                                                \
        returnVar = getToken(tokenVar);                                                 \
        if (returnVar == 1) {                                                           \
	        iPrint(RC_ERR_LEXICAL_ANALYSIS, true, "invalid character read from input"); \
	        return RC_ERR_LEXICAL_ANALYSIS;                                             \
	    } else if (returnVar == 2 || returnVar == 99) {                                 \
	        iPrint(RC_ERR_INTERNAL, true, "internal error (malloc, other)");            \
	        return RC_ERR_INTERNAL;                                                     \
	    }                                                                               \
	} while (tokenVar->type == TYPE_EMPTY)                                              \

/**
 * @brief Error msg maximum size 
 */
#define MAX_ERR_MSG 255

/**
 * @brief Sets error string to msg specified
 * @param msg Message to be set as an error string
 */
#define setErrMsg(msg)                                                                  \
    do {                                                                                \
        strncpy(errText, msg, MAX_ERR_MSG);                                             \
    } while (0)                                                                         \

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                         DEBUG MACROS                          *
 *                                                               * 
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

#ifdef DEBUG

/**
 * @brief Print debug message
 */
#define debugPrint(msg, ...) fprintf(stderr, "[DBG] "msg"\n", ##__VA_ARGS__);

/**
 * @brief Mark some point in the code
 */
#define debugPoint fprintf(stderr, "[DBG] ---> marked point <---\n");

/**
 * @brief Mark some point in the code (can be distingushed by number -> when marking more points)
 */
#define debugPoints(ptNum) fprintf(stderr, "[DBG] --> %d - marked point - %d <---\n", ptNum, ptNum);

#else
#define debugPrint(msg, ...)
#define debugPoint
#define debugPoints
#endif

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                      GLOBAL VARIABLES                         *
 *                                                               * 
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

stNodePtr stFunctions = NULL;      /**< Symboltable for functions (only fucntions are global) */
stStack stack;              /**< Global stack for symtables                            */
stID currentFnc;            /**< To remember current function (for lookup)             */
stID currentVar;            /**< To remember current variable (for type set and lookup)*/
Token* tk;                  /**< Token store - global                                  */
int token;                  /**< Token return code store - global                      */
int mainFound = 0;          /**< Was `function main` found in program?                 */
char errText[MAX_ERR_MSG];  /**< Error text msg (if error occurs)                      */
bool printLastToken;        /**< For error message - if last token may be printed      */
bool argRet = false;        /**< Arguments and returns switch for function parsing     */
int numberOfIDs = 0;        /**< Counter of IDs on the left side of the assignment, this is needed to check correct number of expressions on the left side or correct number of returns from function call */
//InstructionsList* il;

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                       MAIN PARSER LOGIC                       *
 *                                                               * 
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

eRC parser(Token* tkn) {
    debugPrint("-> Syntax analysis (parsing) started.");

    // Symtable & stack setup (this will probably cause serious trouble and segfault)
    stConstruct(&stFunctions);
    stInsert(&stFunctions, "__funcRoot__", ST_N_UNDEFINED, UNKNOWN);
    stackStInit(&stack , &stFunctions);

    // Variables setup
    eRC result = RC_OK;
    tk = tkn;
    setErrMsg("");  // Default : error message is empty
    printLastToken = true;

    // Start parse -> call program()
    result = program();
    if (result != RC_OK) {
        if (printLastToken) {
            string gotStr;
            strInit(&gotStr);

            tokenToString(tk, &gotStr);
            int strSize = strlen(errText) + strGetLength(&gotStr);

            strcat(errText, " but got '");
            strncat(errText, strGetStr(&gotStr),
                    MAX_ERR_MSG - strlen(errText) - 5); // Need 4 chars for ...' in worst case + null byte

            if (strSize < MAX_ERR_MSG) {
                errText[strlen(errText)] = '\'';
            } else {
                strcat(errText, "...'");
            }
            strFree(&gotStr);
        }
        iPrint(result, true, errText);
    }

    #ifdef DEBUG
    displayBST(stFunctions);
    #endif
    debugPrint("-> Syntax analysis %s.", (result ? "failed" : "OK"));

    stackStDesctruct(&stack);
    return result;
}

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *               BASIC PROGRAM STRUCTURE CHECKS                  *
 *                                                               * 
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

eRC program() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    // Cycle 'till we are out of eols (comments are ignored by default)
    do {
        getToken(token, tk);
    } while (tk->type == TYPE_EOL);

    switch (tk->type) {
        case TYPE_KEYWORD:
            result = prolog();                      // <prolog> part of the rule <program>
            if (result != RC_OK) return result;
            generateHeader();                       // generate ifjcode20 header and program body
            result = eolM();                        // <eol_m> part of the rule for <program>
            if (result != RC_OK) return result;
            result = functionsBlock();              // <functions> part of the rule for <program>
            if (result != RC_OK) return result;
            if (tk->type != TYPE_EOF) {             // We expect 'EOF' at the end of the file ofc! 
                setErrMsg("expected 'EOF'");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            generateUsedInternalFunctions();
            return result;
        default:
            setErrMsg("expected TYPE_KEYWORD");
            return RC_ERR_SYNTAX_ANALYSIS;
    }

    return result;                                  // Should be unreachable but just in case!
}

eRC prolog() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    switch (tk->type) {
        case TYPE_KEYWORD:
            if (tk->attribute.keyword != KEYWORD_PACKAGE) {
                setErrMsg("expected 'package' in first line");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            getToken(token, tk);                    // Get next token to check for 'main' part (nothing else is supported in IFJ20)
            if (tk->type != TYPE_IDENTIFIER || strCmpConstStr(&(tk->attribute.string), "main")) {
                setErrMsg("expected identifier 'main'");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            break;
        default:
            setErrMsg("expected TYPE_KEYWORD");
            return RC_ERR_SYNTAX_ANALYSIS;
    }

    return result;
}

eRC eolM() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    getToken(token, tk);                            // Get new token (this is needed as the token was not prepared before calling this function)

    if (tk->type != TYPE_EOL) {
        setErrMsg("expected end-of-line after 'package main'");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    result = eolR();
    return result;
}

eRC eolR() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    // Cycle through all EOLs
    do {
        getToken(token, tk);
    } while (tk->type == TYPE_EOL);

    return result;
}

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                   FUNCTION RELATED CHECKS                     *
 *                                                               * 
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   GENERAL FUNCTION CHECKS                                     *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

eRC functionsBlock() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;
    
    // There has to be at least one function in source code!
    // - Token was already prepared by eolR() function
    if (tk->type != TYPE_KEYWORD || tk->attribute.keyword != KEYWORD_FUNC) {
        setErrMsg("expected at least one 'func' keyword after prolog");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    result = function();                            // Check function and save it to symtable of functions
    if (result != RC_OK) return result;
    result = functionNext();                        // Check for next function that could appear in the code
    if (result != RC_OK) return result;

    // There has to be 'main' function defined (only once!)
    // - Check that main function was found
    if (mainFound == 0 || mainFound > 1) {
        if (mainFound) setErrMsg("multiple definitions of main");
        else setErrMsg("missing definition of main");
        printLastToken = 0;
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    return result;
}

eRC function() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    // There should be a function keyword to indicate beggining of the new function
    // - During first call this part is already checked in functions()
    // - TODO: Check who prepares token for processing in later calls (check functionNext())
    if (tk->type != TYPE_KEYWORD || tk->attribute.keyword != KEYWORD_FUNC) {
        setErrMsg("expected 'func' keyword");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    getToken(token, tk);                                // Get new token that should contain function identifier
    if (tk->type != TYPE_IDENTIFIER) {
        setErrMsg("expected function identifier after 'func' keyword");
        return RC_ERR_SYNTAX_ANALYSIS;
    } else {
        // Generate beginning of function
        generateFunction(strGetStr(&tk->attribute.string));
        // Add function GST (functions symtable -> always at the bottom of the symtable stack)
        stStackInsert(&stack, strGetStr(&(tk->attribute.string)), ST_N_FUNCTION, UNKNOWN);
        currentFnc = strGetStr(&(tk->attribute.string));// Save what is the identifier of the function we are currently parsing
        // Check if function identifier is 'main' (this is so that we can check later that this was defined)
        if (!strcmp(strGetStr(&(tk->attribute.string)), "main")) {
            mainFound++;
        }
    }

    getToken(token, tk);                                // Get new token - looking for arguments
    if (tk->type != TYPE_LEFT_BRACKET) {                // Arguments section starts with '('
        setErrMsg("expected '(' after function identifier");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    argRet = false;                                     // Parsing arguments -> argRet = false
    result = arguments();                               // Parse function arguments
    if (result != RC_OK) return result;
    if (tk->type != TYPE_RIGHT_BRACKET) {               // Arguments section ends with ')'
        setErrMsg("expected ')' after function arguments");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    getToken(token, tk);
    argRet = true;                                      // Parsing arguments -> argRet = true
    result = functionReturn();                          // Parse function return types
    if (result != RC_OK) return result;

    result = commandBlock();                            // Parse command block of the function
    if (result != RC_OK) return result;
    generateFuncEnd();

    return result;
}

eRC functionNext() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    // Token should be prepared by the commandBlock() function that is preceding call of this function
    // - Prepared meaning skipped EOLs
    switch (tk->type) {
        case TYPE_EOF:                                  // <function_n> -> eps
            return result;
            break;
        case TYPE_KEYWORD:                              // <function_n> -> <func> <function_n>
            if (tk->type != TYPE_KEYWORD || tk->attribute.keyword != KEYWORD_FUNC) {
                setErrMsg("expected keyword 'func' after function definition");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            result = function();                        // Parse function
            if (result != RC_OK) return result;
            result = functionNext();                    // Check if there is another function following the last one
            if (result != RC_OK) return result;
            break;
        default:
            setErrMsg("expected keyword 'func' or EOF token after function definition");
            result = RC_ERR_SYNTAX_ANALYSIS;
            break;
    }

    return result;
}

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   TYPE FUNCTION CHECKS                                        *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

eRC arguments() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    getToken(token, tk);                                // Get the token with the ID or others (eps)
    if (tk->type == TYPE_IDENTIFIER) {
        generateFuncArgument(strGetStr(&tk->attribute.string));
        getToken(token, tk);                            // Get the token with the type
        result = type();                                // Parse type
        if (result != RC_OK) return result;
        result = argumentNext();                        // Parse next argument
        if (result != RC_OK) return result;
    }

    return result;
}

eRC argumentNext() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    getToken(token, tk);                                // Get the token with the comma or others (eps)
    if (tk->type == TYPE_COMMA) {
        getToken(token, tk);                            // Get the token with ID
        if (tk->type != TYPE_IDENTIFIER) {
            setErrMsg("expected next identifier after ','");
            return RC_ERR_SYNTAX_ANALYSIS;
        }

        generateFuncArgument(strGetStr(&tk->attribute.string));
        getToken(token, tk);                            // Get the token with the type
        result = type();                                // Parse type
        if (result != RC_OK) return result;
        result = argumentNext();                        // Parse next argument
        if (result != RC_OK) return result;
    }

    return result;
}

eRC type() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;
    
    if (tk->type == TYPE_KEYWORD) {
        switch (tk->attribute.keyword) {
            case KEYWORD_INT:
                if (!argRet) {
                    stFncSetParam(stStackLookUp(&stack, currentFnc), INT);
                } else if (argRet) {
                    stFncSetType(stStackLookUp(&stack, currentFnc), INT, -1);
                }
                break;
            case KEYWORD_FLOAT64:
                if (!argRet) {
                    stFncSetParam(stStackLookUp(&stack, currentFnc), FLOAT64);
                } else if (argRet) {
                    stFncSetType(stStackLookUp(&stack, currentFnc), FLOAT64, -1);
                }
                break;
            case KEYWORD_STRING:
                if (!argRet) {
                    stFncSetParam(stStackLookUp(&stack, currentFnc), STRING);
                } else if (argRet) {
                    stFncSetType(stStackLookUp(&stack, currentFnc), STRING, -1);
                }
                break;
            case KEYWORD_BOOL:
                if (!argRet) {
                    stFncSetParam(stStackLookUp(&stack, currentFnc), BOOL);
                } else if (argRet) {
                    stFncSetType(stStackLookUp(&stack, currentFnc), BOOL, -1);
                }
                break;
            default:
                setErrMsg("expected datatype 'int', 'float64', 'string' or 'bool'");
                result = RC_ERR_SYNTAX_ANALYSIS;
                break;
        }
    }

    return result;
}

eRC functionReturn() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    if (tk->type == TYPE_LEFT_BRACKET) {
        getToken(token, tk);                            // Get the next token (should be KEYWORD or RIGHT BRACKET)
        result = functionReturnType();                  // Parse return type
        if (result != RC_OK) return result;
    }

    return result;
}

eRC functionReturnType() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    switch (tk->type) {
        case TYPE_KEYWORD:
            result = type();                            // Parse type
            getToken(token, tk);                        // Get the next token (should be COMMA or RIGHT BRACKET)
            result = functionReturnTypeNext();          // Parse next return type
            if (result != RC_OK) return result;
            if (tk->type != TYPE_RIGHT_BRACKET) {
                 setErrMsg("expected ')' after last function return type");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            break;
        case TYPE_RIGHT_BRACKET:                        // Closing bracket
            break;
        default:
            setErrMsg("expected ')' after '(' [func return type]");
            result = RC_ERR_SYNTAX_ANALYSIS;
            break;
    }

    return result;
}

eRC functionReturnTypeNext() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    if (tk->type == TYPE_COMMA) {
        getToken(token, tk);                        // Get the next token (should be KEYWORD)
        if (tk->type != TYPE_KEYWORD) {             // Has to be keyword as we already found comma
            setErrMsg("expected KEYWORD (int, float64, string, bool) after ',' in the function return values");
            return RC_ERR_SYNTAX_ANALYSIS;
        }
        result = type();                            // Parse type
        if (result != RC_OK) return result;

        getToken(token, tk);                        // Get the next token (should be COMMA or RIGHT BRACKET)
        result = functionReturnTypeNext();          // Parse next return type
        if (result != RC_OK) return result;
    }

    return result;
}

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   COMMANDS FUNCTION CHECK                                     *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

eRC commandBlock() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    if (tk->type != TYPE_LEFT_CURLY_BRACKET){       // In case that return values were eps
        getToken(token, tk);                        // Get next token (should be LEFT CURLY BRACKET)
    }
    if (tk->type != TYPE_LEFT_CURLY_BRACKET) {      // Start command block with the LEFT CURLY BRACKET
        setErrMsg("expected beginning of command block, aka '{' ");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    getToken(token, tk);                            // Get next token (should be EOL)    
    if (tk->type != TYPE_EOL) {                     // LEFT CURLY BRACKET is followed by EOL!
        setErrMsg("expected end-of-line after '{'");
        return RC_ERR_SYNTAX_ANALYSIS;
    }
    do {                                            // Allow indefinite EOLs
        getToken(token, tk);
    } while (tk->type == TYPE_EOL);

    if (tk->type != TYPE_RIGHT_CURLY_BRACKET) {
        result = commands();                            // Parse commands
        if (result != RC_OK) return result;

        if (tk->type != TYPE_RIGHT_CURLY_BRACKET) {     // End command block with the RIGHT CURLY BRACKET
            setErrMsg("expected end of command block, aka '}'");
            return RC_ERR_SYNTAX_ANALYSIS;
        }
    }

    getToken(token, tk);                            // Get next token (should be EOL)
    if (tk->type != TYPE_EOL) {                     // RIGHT CURLY BRACKET is followed by EOL!
        setErrMsg("expected end-of-line after '}'");
        return RC_ERR_SYNTAX_ANALYSIS;
    }
    do {                                            // Allow indefinite EOLs
        getToken(token, tk);
    } while (tk->type == TYPE_EOL);

    return result;
}

eRC commands() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    result = command();                             // Parse command
    if (result != RC_OK) return result;

    if (tk->type != TYPE_EOL) {                     // Line of command ends with EOL!
        setErrMsg("expected end-of-line after (one) command");
        return RC_ERR_SYNTAX_ANALYSIS;
    }
    do {                                            // Allow indefinite EOLs
        getToken(token, tk);
    } while (tk->type == TYPE_EOL);

    if (tk->type == TYPE_RIGHT_CURLY_BRACKET) {     // Detected end of the command block
        return result;
    }

    result = commands();                            // Parse another commans
    if (result != RC_OK) return result;

    return result;
}

eRC command() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;


    switch (tk->type) {
        case TYPE_IDENTIFIER:                           // ID <statement>
            debugPrint("<%s> -> ID <statement>", __func__);
            generatorSaveID(strGetStr(&tk->attribute.string));
            getToken(token, tk);                        // Get next token for statement
            result = statement();                       // Parse statement
            if (result != RC_OK) return result;
            break;
        case TYPE_KEYWORD:                              // Other commands (not a variable or function)(if, for)
             switch (tk->attribute.keyword) {
                case KEYWORD_IF:                        // if <expression> <cmd_block> <if_else>
                    debugPrint("<%s> -> if <expression> <cmd_block> <if_else>", __func__);
                    getToken(token, tk);                // Step over IF

                    result = precedent_analys(tk, NULL, &stack); // Evaluate expression
                    if (result != RC_OK) return result;
                    // if (expression == true) generateIfScope();

                    result = commandBlock();            // Parse block of commands (inside of if)
                    if (result != RC_OK) return result;

                    result = ifElse();                  // Parse if else
                    if (result != RC_OK) return result;
                    break;
                case KEYWORD_FOR:                       // for <for_definition> ; <expression> ; <for_assignment>
                    debugPrint("<%s> -> for <for_definition> ; <expression> ; <for_assignment>", __func__);

                    result = forDefine();               // Parse definiton section of for
                    if (result != RC_OK) return result;

                    if (tk->type != TYPE_SEMICOLON) {   // After definition must be ';'
                        setErrMsg("expected ';' after for cycle definition");
                        return RC_ERR_SYNTAX_ANALYSIS;
                    }

                    // TODO -> handle <expression> -> requires semantic analysis

                    result = forAssign();               // Parse assignment section of for
                    if (result != RC_OK) return result;

                    result = commandBlock();            // Parse block of commands (inside of for)
                    if (result != RC_OK) return result;
                    break;
                case KEYWORD_RETURN:                    // <<return_cmd>
                    debugPrint("<%s> -> <return_cmd>", __func__);

                    result = returnCommand();           // Parse return
                    if (result != RC_OK) return result;
                    break;
                default:
                    setErrMsg("expected keyword 'if', 'for' or 'return'");
                    result = RC_ERR_SYNTAX_ANALYSIS;
                    break;
            }
            break;
        default:
            break;
    }

    return result;
}

eRC statement() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    switch (tk->type) {
        case TYPE_LEFT_BRACKET:                         // ( <arguments> )
            debugPrint("<%s> -> ( <arguments> )", __func__);

            result = funcCallArguments();                       // Parse arguments
            if (result != RC_OK) return result;
            if (tk->type != TYPE_RIGHT_BRACKET) {
                setErrMsg("expected ')' after arguments [of func-call]");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            char* funcID = generatorGetID();
            if (strcmp(funcID, "print"))
                generateFuncCall(funcID);
            // else
            //  print(datatype_arg1); print(datatype_arg2); (...)
            getToken(token, tk);
            break;
        case TYPE_ASSIGN:                               // = <assignment>   => <id_mul> = <assignment> where <id_mul> is eps
            getToken(token, tk);                        // Get the next token (move past = to <assignment>)
            result = assignment();                      // Parse assignment
            if (result != RC_OK) return result;
            break;
        case TYPE_DECLARATIVE_ASSIGN:                   // := <expression>
            generateDefinitions();
            getToken(token, tk);                        // Get the next token (move past := to <assignment>)
            result = assignment();                      // Parse assignment
            if (result != RC_OK) return result;
            break;
        case TYPE_PLUS_ASSIGN:
        case TYPE_MINUS_ASSIGN:
        case TYPE_MULTIPLY_ASSIGN:
        case TYPE_DIVIDE_ASSIGN:                        // <unary> <expression>
            result = unary();                           // Parse unary (just do a re-check) TODO: consider removing
            if (result != RC_OK) return result;
            getToken(token, tk);
            // TODO -> handle <expression> -> requires semantic analysis
            // generateAssignment(identifier);
            break;
        default:                                        // <id_mul> = <assignment>
            result = multipleID();                      // Parse multiple IDs
            if (result != RC_OK) return result;
            if (tk->type != TYPE_ASSIGN) {
                setErrMsg("expected '=' after multiple IDs");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            getToken(token, tk);                        // Get the next token (move past = to <assignment>)
            result = assignment();                      // Parse assignment
            if (result != RC_OK) return result;
            break;
    }

    return result;
}

eRC multipleID() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    if (tk->type == TYPE_COMMA) {                   // , ID <id_mul>
        numberOfIDs++;                              // There are 2 IDs now, need 2 expressions or fnc with 2 return types
        getToken(token, tk);                        // Get the next token (move past , )
        if (tk->type != TYPE_IDENTIFIER) {
            setErrMsg("expected ID after ','");
            return RC_ERR_SYNTAX_ANALYSIS;
        }
        generatorSaveID(strGetStr(&tk->attribute.string));
        getToken(token, tk);                        // Prepare token for another multipleID call
        result = multipleID();                      // Parse multiple IDs
        if (result != RC_OK) return result;
    }

    return result;
}

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   ASSIGN & DEFINE CHECK                                       *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

eRC assignment() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    switch (tk->type) {
        case TYPE_IDENTIFIER:                       // ID ( <arguments> )
            generatorSaveID(strGetStr(&tk->attribute.string));
            getToken(token, tk);                    // Get the next token (move past '(')
            if (tk->type != TYPE_LEFT_BRACKET) {
                setErrMsg("expected '(' after identifier");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            result = funcCallArguments();                   // Parse arguments
            if (result != RC_OK) return result;
            if (tk->type != TYPE_RIGHT_BRACKET) {
                setErrMsg("expected ')' after function arguments");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            generateFuncCall(generatorGetID());
            generateAssignments();
            getToken(token, tk);                    // This function promises that it'll prepare functions for other processing
            break;
        default:                                    // <expression> <expr_n>
            // TODO -> handle <expression> -> requires semantic analysis
            getToken(token, tk); // temporary: after expression is done, I got 1 token after expression
            result = expressionNext();
            if (result != RC_OK) return result;
            generateAssignments();
            break;
    }

    return result;
}

eRC unary() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    switch (tk->type) {
        case TYPE_PLUS_ASSIGN:                      // +=
        case TYPE_MINUS_ASSIGN:                     // -=
        case TYPE_MULTIPLY_ASSIGN:                  // *=
        case TYPE_DIVIDE_ASSIGN:                    // /=
            break;
        default:
            setErrMsg("expected unary assignment token '+=', '-=', '*=' or '/='");
            result = RC_ERR_SYNTAX_ANALYSIS;
            break;
    }

    return result;
}

eRC expressionNext() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    if (tk->type == TYPE_COMMA) {
        if(--numberOfIDs < 0) {
            setErrMsg("expected less expressions on the right side of the assignment");
            result = RC_ERR_SYNTAX_ANALYSIS;
        }
        // TODO -> handle <expression> -> requires semantic analysis
        getToken(token, tk); // temporary: for dev purposes -> to simulate handling (1) number-only expression
        getToken(token, tk); // temporary: expressions returns 1 token after expression (ends) -> remove after expression handling is done
        result = expressionNext();
        if (result != RC_OK) return result;
    }

    return result;
}

eRC funcCallArguments() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    getToken(token, tk);                                    // Get the token with the ID or others (eps)
    if (tk->type == TYPE_IDENTIFIER) {
        generateFuncArgument(strGetStr(&tk->attribute.string));
        getToken(token, tk);                                 // Get the token with ',' if next argument present or ')' if this was last argument
        if (tk->type != TYPE_COMMA) {
            if (tk->type == TYPE_RIGHT_BRACKET)
                return result;
            else {
                setErrMsg("expected ')' or ',' after (function call argument) identifier");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
        }
        result = funcCallArguments();                        // Parse next argument
        if (result != RC_OK) return result;
    }

    return result;
}

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   IF ELSE CHECK                                               *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

eRC ifElse() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;
    switch (tk->attribute.keyword) {
        case KEYWORD_ELSE: // rule: <if_else> -> else <if_else_st>
            getToken(token, tk);
            result = ifElseExpanded();
            if (result != RC_OK) return result;
        default:
            generateIfScopeEnd();
            break;
    }

    return RC_OK;
}

eRC ifElseExpanded() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    if (tk->attribute.keyword == KEYWORD_IF) {
        // rule: <if_else_st> -> if <expression> <cmd_block> <if_else_st_n>
        // <expression>
        // TODO: handle expression
        // <cmd_block>
        getToken(token, tk);
        // if (expression == true) generateIfScope();   // expression ... there should be 'else if <expression> <cmdblock>'
        result = commandBlock();
        if (result != RC_OK) return result;

        // <if_else_st_n>
        result = ifElse();
        if (result != RC_OK) return result;
    } else {
        // else rule: <if_else_st> -> <cmd_block>
        // if (expression == false) generateIfScope();      // expression ... in line 591 in command(); or upper (804)
        result = commandBlock();
        if (result != RC_OK) return result;
    }

    return result;
}

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   FOR CHECK                                                   *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

eRC forDefine() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;
    getToken(token, tk);

    // rule: <for_definition> -> ID := <expression>
    if (tk->type == TYPE_IDENTIFIER) {
        // := <expression>
        getToken(token, tk);
        if (tk->type != TYPE_DECLARATIVE_ASSIGN) {
            setErrMsg("expected ':=' after identifier [for definition]");
            return RC_ERR_SYNTAX_ANALYSIS;
        }
        // TODO -> handle <expression>
    }
    // else rule: <for_definition> -> eps

    return result;
}

eRC forAssign() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;
    getToken(token, tk);

    // rule: <for_assignment> -> ID = <expression>
    if (tk->type == TYPE_IDENTIFIER) {
        getToken(token, tk);
        if (tk->type != TYPE_ASSIGN) {
            setErrMsg("expected '=' after identifier [for assignment]");
            return RC_ERR_SYNTAX_ANALYSIS;
        }
        // TODO -> handle <expression>
    }

    return result;
}

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *   RETURN CHECK                                                *
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

eRC returnCommand() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    if (tk->type != TYPE_KEYWORD || tk->attribute.keyword != KEYWORD_RETURN) {
        setErrMsg("expected keyword 'return'");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    result = returnStatement();
    if (result != RC_OK) return result;

    return result;
}

eRC returnStatement() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;
    getToken(token, tk);

    // rule: <return_stat> -> <expression>
    // TODO -> handle expression
    // note: after expression handle, its result will be (pushed) on top of stack (last expr. -> top)

    // rule: <return_stat> -> eps
    // TODO -> if not expression -> else do nothing
    getToken(token, tk); // temporary: after expression is handled, we must have 1 token after that expression
    return result;
}