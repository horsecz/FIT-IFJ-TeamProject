/**
 * @file parser.c
 * @author Roman Janiczek (xjanic25@vutbr.cz)
 * @author Dominik Horky (xhorky32@vutbr.cz)
 * @brief Parser implementation
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
#define debugPoints(ptNum)
#endif

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                      GLOBAL VARIABLES                         *
 *                                                               * 
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

stNodePtr stFunctions = NULL;   /**< Symboltable for functions (only fucntions are global) */
stStack stack;                  /**< Global stack for symtables                            */
stID currentFnc;                /**< To remember current function (for lookup)             */
stID currentVar;                /**< To remember current variable (for type set and lookup)*/
stID currentVarMul[MAX_SIZE];   /**< To remember current variable (for type set and lookup)*/
Token* tk;                      /**< Token store - global                                  */
TokenType precType;             /**< For work with precedent analysis                      */
int token;                      /**< Token return code store - global                      */
int mainFound = 0;              /**< Was `function main` found in program?                 */
char errText[MAX_ERR_MSG];      /**< Error text msg (if error occurs)                      */
bool printLastToken;            /**< For error message - if last token may be printed      */
bool fncDef = true;             /**< Parsing function definiton (for arguments)            */
bool argRet = false;            /**< Arguments and returns switch for function parsing     */
int scope = -1;                 /**< Scope lvl for variables                               */
int numberOfIDs = 0;            /**< Counter of IDs on the left side of the assignment, this is needed to check correct number of expressions on the left side or correct number of returns from function call */
int numberOfExp = 0;            /**< Temp helper value for expression compare              */
bool funcCall = false;          /**< Detection of function call used in deciding whether do expression (count) check or not */
bool precRightBrace = false;    /**< For precedent: check if right brace of func call was detected (prevent semantic errors) */
bool afterIf = false;           /**< Dont check EOLs after if command block */

/*** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***
 *                                                               *
 *                       MAIN PARSER LOGIC                       *
 *                                                               * 
 *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ***/

eRC parser(Token* tkn) {
    debugPrint("-> Syntax analysis (parsing) started.");

    // Symtable & stack setup (this will probably cause serious trouble and segfault)
    stConstruct(&stFunctions);
    stInsert(&stFunctions, "__funcRoot__", ST_N_UNDEFINED, UNKNOWN, scope);
    stackStInit(&stack , &stFunctions);
    // INBUILT FUNCTIONS HARDCODE
    // - func inputs()(string, int)
    stInsert(&stFunctions, "inputs", ST_N_FUNCTION, UNKNOWN, 0);
    stFncSetType(stLookUp(&stFunctions, "inputs"), STRING);
    stFncSetType(stLookUp(&stFunctions, "inputs"), INT);
    // - func inputi()(int, int)
    stInsert(&stFunctions, "inputi", ST_N_FUNCTION, UNKNOWN, 0);
    stFncSetType(stLookUp(&stFunctions, "inputi"), INT);
    stFncSetType(stLookUp(&stFunctions, "inputi"), INT);
    // - func inputf()(float64, int)
    stInsert(&stFunctions, "inputf", ST_N_FUNCTION, UNKNOWN, 0);
    stFncSetType(stLookUp(&stFunctions, "inputf"), FLOAT64);
    stFncSetType(stLookUp(&stFunctions, "inputf"), INT);
    // - func inputb()(bool, int)
    stInsert(&stFunctions, "inputb", ST_N_FUNCTION, UNKNOWN, 0);
    stFncSetType(stLookUp(&stFunctions, "inputb"), BOOL);
    stFncSetType(stLookUp(&stFunctions, "inputb"), INT);
    // - func int2float(i int)(float64)
    stInsert(&stFunctions, "int2float", ST_N_FUNCTION, UNKNOWN, 0);
    stFncSetParam(stLookUp(&stFunctions, "int2float"), INT);
    stFncSetType(stLookUp(&stFunctions, "int2float"), FLOAT64);
    // - func float2int(f float64)(int)
    stInsert(&stFunctions, "float2int", ST_N_FUNCTION, UNKNOWN, 0);
    stFncSetParam(stLookUp(&stFunctions, "float2int"), FLOAT64);
    stFncSetType(stLookUp(&stFunctions, "float2int"), INT);
    // - func len(s string, i1 int, i2 int)(string, int)
    stInsert(&stFunctions, "len", ST_N_FUNCTION, UNKNOWN, 0);
    stFncSetParam(stLookUp(&stFunctions, "len"), STRING);
    stFncSetParam(stLookUp(&stFunctions, "len"), INT);
    stFncSetParam(stLookUp(&stFunctions, "len"), INT);
    stFncSetType(stLookUp(&stFunctions, "len"), STRING);
    stFncSetType(stLookUp(&stFunctions, "len"), INT);
    // - func substr(s string, i int)(string, int)
    stInsert(&stFunctions, "substr", ST_N_FUNCTION, UNKNOWN, 0);
    stFncSetParam(stLookUp(&stFunctions, "substr"), STRING);
    stFncSetParam(stLookUp(&stFunctions, "substr"), INT);
    stFncSetType(stLookUp(&stFunctions, "substr"), STRING);
    stFncSetType(stLookUp(&stFunctions, "substr"), INT);
    // - func ord(s string, i int)(int, int)
    stInsert(&stFunctions, "ord", ST_N_FUNCTION, UNKNOWN, 0);
    stFncSetParam(stLookUp(&stFunctions, "ord"), STRING);
    stFncSetParam(stLookUp(&stFunctions, "ord"), INT);
    stFncSetType(stLookUp(&stFunctions, "ord"), INT);
    stFncSetType(stLookUp(&stFunctions, "ord"), INT);
    // - func chr(i int)(string, int)
    stInsert(&stFunctions, "chr", ST_N_FUNCTION, UNKNOWN, 0);
    stFncSetParam(stLookUp(&stFunctions, "chr"), INT);
    stFncSetType(stLookUp(&stFunctions, "chr"), STRING);
    stFncSetType(stLookUp(&stFunctions, "chr"), INT);
    
    // Variables setup
    eRC result = RC_OK;
    tk = tkn;
    setErrMsg("");  // Default : error message is empty
    printLastToken = true;

    // Start parse -> call program()
    result = program();
    if (result != RC_OK && result != RC_WRN_INTERNAL && result != RC_ERR_INTERNAL && result != RC_ERR_SEMANTIC_PROG_FUNC) {
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
    displayBST(stackGetTopSt(&stack));
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
            result = checkFunctions(stFunctions);   // Check for undefined function calls
            if (result != RC_OK) return result;
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
        return RC_ERR_SEMANTIC_PROG_FUNC;
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
        result = stStackInsert(&stack, strGetStr(&tk->attribute.string), ST_N_FUNCTION, UNKNOWN, scope);
        if (result != RC_OK) {
            setErrMsg("redefinition attempt");
            return RC_ERR_SEMANTIC_PROG_FUNC;
        }
        currentFnc = strGetStr(&tk->attribute.string);// Save what is the identifier of the function we are currently parsing
        stFncSetDefined(stLookUp(&stFunctions, currentFnc), true);
        // Check if function identifier is 'main' (this is so that we can check later that this was defined)
        if (!strcmp(strGetStr(&tk->attribute.string), "main")) {
            mainFound++;
        }
    }

    getToken(token, tk);                                // Get new token - looking for arguments
    if (tk->type != TYPE_LEFT_BRACKET) {                // Arguments section starts with '('
        setErrMsg("expected '(' after function identifier");
        return RC_ERR_SYNTAX_ANALYSIS;
    }
    // TODO: Check this
    scope++;
    stNodePtr stVars = NULL;
    stConstruct(&stVars);
    stInsert(&stVars, "__varsRoot__", ST_N_UNDEFINED, UNKNOWN, scope);
    stInsert(&stVars, "_", ST_N_VARIABLE, UNDERSCORE, scope);
    stackPushSt(&stack, &stVars);                       // Entering new scope (function)	

    fncDef = true;
    argRet = false;                                     // Parsing arguments -> argRet = false
    result = arguments();                               // Parse function arguments
    if (result != RC_OK) return result;
    if (tk->type != TYPE_RIGHT_BRACKET) {               // Arguments section ends with ')'
        setErrMsg("expected ')' after function arguments");
        return RC_ERR_SYNTAX_ANALYSIS;
    }
    generateFuncArguments();

    getToken(token, tk);
    argRet = true;                                      // Parsing arguments -> argRet = true
    result = functionReturn();                          // Parse function return types
    if (result != RC_OK) return result;
    fncDef = false;

    result = commandBlock();                            // Parse command block of the function
    if (result != RC_OK) return result;
    generateFuncEnd();

    do {                                            // Allow indefinite EOLs
        getToken(token, tk);
    } while (tk->type == TYPE_EOL);

    // TODO: Symtable check	
    #ifdef DEBUG
    displayBST(stackGetTopSt(&stack));
    #endif
    stNodePtr destructed = stackPopSt(&stack);
    stDestruct(&destructed);                     // End of the scope (function)

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

    getToken(token, tk);
    // Get the token with the ID or others (eps)
    if (tk->type == TYPE_IDENTIFIER) {
        generatorSaveID(strGetStr(&tk->attribute.string));
        result = stStackInsert(&stack, strGetStr(&tk->attribute.string), ST_N_VARIABLE, UNKNOWN, scope);// Save variable as defined	
        if (result != RC_OK) {
            setErrMsg("redefinition attempt");
            return RC_ERR_SEMANTIC_PROG_FUNC;
        }
        currentVar = strGetStr(&tk->attribute.string);
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

        generatorSaveID(strGetStr(&tk->attribute.string));
        result = stStackInsert(&stack, strGetStr(&tk->attribute.string), ST_N_VARIABLE, UNKNOWN, scope);// Save variable as defined	
        if (result != RC_OK) {
            setErrMsg("redefinition attempt");
            return RC_ERR_SEMANTIC_PROG_FUNC;
        }
        currentVar = strGetStr(&tk->attribute.string);
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
                if (fncDef) {
                    if (!argRet) {
                        stFncSetParam(stLookUp(&stFunctions, currentFnc), INT);
                        stVarSetType(stStackLookUp(&stack, currentVar), INT);
                    } else if (argRet) {
                        stFncSetType(stLookUp(&stFunctions, currentFnc), INT);
                    }
                }
                break;
            case KEYWORD_FLOAT64:
                if (fncDef) {
                    if (!argRet) {
                        stFncSetParam(stLookUp(&stFunctions, currentFnc), FLOAT64);
                        stVarSetType(stStackLookUp(&stack, currentVar), FLOAT64);
                    } else if (argRet) {
                        stFncSetType(stLookUp(&stFunctions, currentFnc), FLOAT64);
                    }
                }
                break;
            case KEYWORD_STRING:
                if (fncDef) {
                    if (!argRet) {
                        stFncSetParam(stLookUp(&stFunctions, currentFnc), STRING);
                        stVarSetType(stStackLookUp(&stack, currentVar), STRING);
                    } else if (argRet) {
                        stFncSetType(stLookUp(&stFunctions, currentFnc), STRING);
                    }
                }
                break;
            case KEYWORD_BOOL:
                if (fncDef) {
                    if (!argRet) {
                        stFncSetParam(stLookUp(&stFunctions, currentFnc), BOOL);
                        stVarSetType(stStackLookUp(&stack, currentVar), BOOL);
                    } else if (argRet) {
                        stFncSetType(stLookUp(&stFunctions, currentFnc), BOOL);
                    }
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
    if (tk->type != TYPE_EOL && !afterIf) {                     // RIGHT CURLY BRACKET is followed by EOL!
        setErrMsg("expected end-of-line after '}'");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

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
            currentVar = strGetStr(&tk->attribute.string);
            getToken(token, tk);                        // Get next token for statement
            result = statement();                       // Parse statement
            if (result != RC_OK) return result;
            break;
        case TYPE_KEYWORD:                              // Other commands (not a variable or function)(if, for)
             switch (tk->attribute.keyword) {
                case KEYWORD_IF:                        // if <expression> <cmd_block> <if_else>
                    debugPrint("<%s> -> if <expression> <cmd_block> <if_else>", __func__);
                    getToken(token, tk);                // Step over IF
                    // TODO: Check functionality of this part	
                    result = precedent_analys(tk, &precType, &stack); // Evaluate expression
                    if (result != RC_OK) return result;
                    if (precType != TYPE_BOOL) {	
                        iPrint(RC_ERR_SEMANTIC_TYPECOMP, true, "result of IF expression is not bool");	
                        return RC_ERR_SEMANTIC_TYPECOMP;	
                    }
                    generateIfScope();
                    // TODO: Check this
                    scope++;
                    stNodePtr stVarsIf = NULL;
                    stInsert(&stVarsIf, "__varsRoot__", ST_N_UNDEFINED, UNKNOWN, scope);
                    stackPushSt(&stack, &stVarsIf);       // Entering new scope (if 1)

                    afterIf = true;
                    result = commandBlock();            // Parse block of commands (inside of if)
                    if (result != RC_OK) return result;

                    // TODO: Symtable check	
                    #ifdef DEBUG
                    displayBST(stackGetTopSt(&stack));
                    #endif
                    stNodePtr destructedIf = stackPopSt(&stack);
                    stDestruct(&destructedIf);     // End of the scope (if 1)

                    result = ifElse();                  // Parse if else
                    if (result != RC_OK) return result;
                    break;
                case KEYWORD_FOR:                       // for <for_definition> ; <expression> ; <for_assignment>
                    debugPrint("<%s> -> for <for_definition> ; <expression> ; <for_assignment>", __func__);

                    // TODO: Check this
                    scope++;
                    stNodePtr stVarsFor = NULL;
                    stInsert(&stVarsFor, "__varsRoot__", ST_N_UNDEFINED, UNKNOWN, scope);
                    stackPushSt(&stack, &stVarsFor);       // Entering new scope (for 1)

                    generateForBeginning();
                    result = forDefine();               // Parse definiton section of for
                    if (result != RC_OK) return result;

                    if (tk->type != TYPE_SEMICOLON) {   // After definition must be ';'
                        setErrMsg("expected ';' after for cycle definition");
                        return RC_ERR_SYNTAX_ANALYSIS;
                    }

                    getToken(token, tk);                // Step over FOR
                    generateForExpression();
                    // TODO: Check functionality of this part	
                    result = precedent_analys(tk, &precType, &stack); // Evaluate expression
                    if (result != RC_OK) return result;
                    // TODO -> handle <expression> -> requires semantic analysis
                    if (precTypeToSymtableType(precType) != BOOL) {
                        setErrMsg("result of for condition is not bool");
                        return RC_ERR_SEMANTIC_TYPECOMP;
                    }

                     if (tk->type != TYPE_SEMICOLON) {   // After definition must be ';'
                         setErrMsg("expected ';' after for cycle expression");
                         return RC_ERR_SYNTAX_ANALYSIS;
                     }

                    generateForCondition();
                    generateForAssignment();
                    result = forAssign();               // Parse assignment section of for
                    if (result != RC_OK) return result;
                    generateForAssignmentEnd();

                    generateForScope();
                    result = commandBlock();            // Parse block of commands (inside of for)
                    if (result != RC_OK) return result;
                    generateForScopeDefinitions();
                    generateForScopeEnd();

                    // TODO: Symtable check	
                    #ifdef DEBUG
                    displayBST(stackGetTopSt(&stack));
                    #endif
                    stNodePtr destructedFor = stackPopSt(&stack);
                    stDestruct(&destructedFor);     // End of the scope (if 1)

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
            // Try to inser function into GST (if already present run checks, if not insert and add arguments)
            stC result_s = stInsert(&stFunctions, currentVar, ST_N_FUNCTION, UNKNOWN, scope);
            if (result_s == ST_ERROR) return RC_ERR_INTERNAL;

            result = funcCallArguments();                       // Parse arguments
            if (result != RC_OK) return result;
            if (tk->type != TYPE_EOL) {
                setErrMsg("expected EOL after assignment with func-call");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            char* funcID = generatorGetID();
            if (strcmp(funcID, "print"))
                generateFuncCall(funcID);

            if (!funcCall)
                getToken(token, tk);
            funcCall = false;
            break;
        case TYPE_ASSIGN:                               // = <assignment>   => <id_mul> = <assignment> where <id_mul> is eps
            numberOfIDs++;
            getToken(token, tk);                        // Get the next token (move past = to <assignment>)
            result = assignment();                      // Parse assignment
            if (result != RC_OK) return result;
            break;
        case TYPE_DECLARATIVE_ASSIGN:;                  // := <expression>
            stNodePtr vars = stackGetTopSt(&stack);
            if (stLookUp(&vars, currentVar)) {
                iPrint(RC_ERR_SEMANTIC_PROG_FUNC, true, "redefinition attempt!");
                return RC_ERR_SEMANTIC_PROG_FUNC;
            }
            result = stStackInsert(&stack, currentVar, ST_N_VARIABLE, UNKNOWN, scope);	
            if (result != RC_OK) {
                setErrMsg("redefinition attempt");
                return RC_ERR_SEMANTIC_PROG_FUNC;
            }
            getToken(token, tk);                        // Get the next token (move past := to <expression>)
            result = precedent_analys(tk, &precType, &stack);// Evaluate expression
            if (result != RC_OK) return result;
            debugPrint("Declaring as: %d, received: %d", precTypeToSymtableType(precType), precType)
            generateDefinitions();
            if (stVarGetType(stStackLookUp(&stack, currentVar)) != UNDERSCORE) {
                stVarSetType(stStackLookUp(&stack, currentVar), precTypeToSymtableType(precType));// Set variable type
            }
            break;
        case TYPE_PLUS_ASSIGN:
        case TYPE_MINUS_ASSIGN:
        case TYPE_MULTIPLY_ASSIGN:
        case TYPE_DIVIDE_ASSIGN:                        // <unary> <expression>
            result = unary();                           // Parse unary (just do a re-check)
            if (result != RC_OK) return result;
            break;
        default:                                        // <id_mul> = <assignment>
            numberOfIDs++;
            result = multipleID();                      // Parse multiple IDs
            if (result != RC_OK) return result;
            if (tk->type != TYPE_ASSIGN) {
                setErrMsg("expected '=' after multiple IDs");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            getToken(token, tk);                        // Get the next token (move past = to <assignment>)
            numberOfExp = numberOfIDs;
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
        currentVarMul[numberOfIDs - 1] = strGetStr(&tk->attribute.string);
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
    result = precedent_analys(tk, &precType, &stack);
    debugPrint("%d", numberOfIDs);
    if (result != RC_OK) { // ID ( ... ) -> funccall, precedent not found function ID in variable table
        if (result == RC_ERR_SEMANTIC_OTHER)  {
            generatorSaveID(strGetStr(&tk->attribute.string));
            result = semantic_analysis (strGetStr(&tk->attribute.string), stFunctions, stack, currentVar, currentVarMul);
            if (result != RC_OK) {
                printLastToken = 0;
                return result;
            }
            currentVar = strGetStr(&tk->attribute.string);
            getToken(token, tk);                    // Get the next token (move past '(')
            if (tk->type != TYPE_LEFT_BRACKET) {
                setErrMsg("expected '(' after identifier");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            result = funcCallArguments();                   // Parse arguments
            if (result != RC_OK) return result;
            if (!funcCall && tk->type != TYPE_RIGHT_BRACKET) {
                setErrMsg("expected ')' after function arguments");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            generateFuncCall(generatorGetID());
            generateAssignments();

            if (!funcCall)
                getToken(token,tk);                    // This function promises that it'll prepare functions for other processing
            funcCall = false;
            return result;
        } else {
            return result;
        }
    }
    if (stVarTypeLookUp(&stack, currentVar) != UNDERSCORE && stVarTypeLookUp(&stack, currentVar) != precTypeToSymtableType(precType)) {
        if (stVarTypeLookUp(&stack, currentVar) == UNKNOWN) {
            iPrint(RC_ERR_SEMANTIC_PROG_FUNC, true, "undefined variable");
            return RC_ERR_SEMANTIC_PROG_FUNC;
        }
        debugPrint("Found var type: %d, assigning: %d, received: %d", stVarTypeLookUp(&stack, currentVar), precTypeToSymtableType(precType), precType)
        iPrint(RC_ERR_SEMANTIC_TYPECOMP, true, "assigning wrong type");
        return RC_ERR_SEMANTIC_TYPECOMP;
    }
    numberOfIDs--;
    result = expressionNext();
    if (result != RC_OK) return result;
    debugPrint("%d", numberOfIDs);
    if (numberOfIDs != 0) {
        iPrint(RC_ERR_SEMANTIC_OTHER, true, "invalid number of values on the left side of assignment");
        return RC_ERR_SEMANTIC_OTHER;
    }

    generateAssignments();
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
            generatorUnaryPrepare(tk->type, currentVar);
            getToken(token, tk); // move past unary assign token

            result = precedent_analys(tk, &precType, &stack); // Evaluate expression
            if (result != RC_OK) return result;
            if (stVarTypeLookUp(&stack, currentVar) != precTypeToSymtableType(precType)) {
                debugPrint("Found var type: %d, assigning: %d, received: %d", stVarTypeLookUp(&stack, currentVar), precTypeToSymtableType(precType), precType)
                iPrint(RC_ERR_SEMANTIC_TYPECOMP, true, "assigning wrong type");
                return RC_ERR_SEMANTIC_TYPECOMP;
            }
            generateUnaryExpression();
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
        numberOfIDs--;
        if(!funcCall && numberOfIDs < 0) {
            setErrMsg("expected less expressions on the right side of the assignment");
            result = RC_ERR_SYNTAX_ANALYSIS;
        }
        debugPrint("get token follows");
        getToken(token, tk);                        // Step over COMMA	

        result = precedent_analys(tk, &precType, &stack);// Evaluate expression	
        if (result != RC_OK) return result;
        if (!funcCall && stVarTypeLookUp(&stack, currentVar) != UNDERSCORE && stVarTypeLookUp(&stack, currentVarMul[numberOfExp - numberOfIDs - 1]) != precTypeToSymtableType(precType)) {
            if (stVarTypeLookUp(&stack, currentVarMul[numberOfExp - numberOfIDs - 1]) == UNKNOWN) {
                iPrint(RC_ERR_SEMANTIC_PROG_FUNC, true, "undefined variable");
                return RC_ERR_SEMANTIC_PROG_FUNC;
            }
            iPrint(RC_ERR_SEMANTIC_TYPECOMP, true, "assigning wrong type");
            return RC_ERR_SEMANTIC_TYPECOMP;	
        }
        generatorPrintCheck((DataType)precType);
        result = expressionNext();
        if (result != RC_OK) return result;
    }

    return result;
}

eRC funcCallArguments() {
    debugPrint("rule %s", __func__);
    // TODO: Rework of this function to parse arguments against existing function or create new one
    // rule <arguments_fc> -> <expression
    eRC result = RC_OK;
    getToken(token, tk);                                    // Get the token with the ID or others (eps)
    precRightBrace = true;
    result = precedent_analys(tk, &precType, &stack); // Evaluate expression
    if (result != RC_OK) return result;
    stNodePtr func = stLookUp(&stFunctions, currentVar);

    if (func == NULL) { // function not in GST
        stC result_s = stInsert(&stFunctions, currentVar, ST_N_FUNCTION, UNKNOWN, scope);
        if (result_s != ST_SUCCESS) return RC_ERR_INTERNAL;
        func = stLookUp(&stFunctions, currentVar);
        func->fData->defined = false;
    }

    if (strcmp(currentVar, "print") && !func->fData->defined) { // do not check types if it is print(...) function
        stFncSetParam(func, precTypeToSymtableType(precType));
    } else {
        // TODO: Check arguments -> Add cycle through params (recursion call on this function or in expressionNext)
        if (strcmp(currentVar, "print") && stFncGetParams(func)[0] != precTypeToSymtableType(precType)) {
            setErrMsg("type of parameters of the function doesn't match");
            printLastToken = false;
            return RC_ERR_SEMANTIC_PARAM;
        }
    }
    generatorPrintCheck((DataType) precType);

    funcCall = true;
    result = expressionNext();  // TODO: This will need to be changed and reworked most probably
    if (result != RC_OK) return result;

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
            break;
        default:
            generateIfScopeEnd();
            afterIf = false;
            if (tk->type != TYPE_EOL) {
                setErrMsg("expected end-of-line after last if command block");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            break;
    }

    return RC_OK;
}

eRC ifElseExpanded() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    if (tk->attribute.keyword == KEYWORD_IF) {
        getToken(token, tk);                              // Step over IF
        // TODO: Check functionality of this part	
        result = precedent_analys(tk, &precType, &stack); // Evaluate expression
        if (result != RC_OK) return result;
        if (precType != TYPE_BOOL) {	
            iPrint(RC_ERR_SEMANTIC_TYPECOMP, true, "result of IF expression is not bool");	
            return RC_ERR_SEMANTIC_TYPECOMP;	
        }

        // TODO: Check this
        scope++;
        stNodePtr stVarsIf = NULL;
        stInsert(&stVarsIf, "__varsRoot__", ST_N_UNDEFINED, UNKNOWN, scope);
        stackPushSt(&stack, &stVarsIf);       // Entering new scope (if 2)
        
        generateIfScope();

        afterIf = true;
        result = commandBlock();
        if (result != RC_OK) return result;

        // TODO: Symtable check	
        #ifdef DEBUG
        displayBST(stackGetTopSt(&stack));
        #endif
        stNodePtr destructedIf = stackPopSt(&stack);
        stDestruct(&destructedIf);     // End of the scope (if 2)

        result = ifElse();
        if (result != RC_OK) return result;
    } else {
        // TODO: Check this
        scope++;
        stNodePtr stVarsElse = NULL;
        stInsert(&stVarsElse, "__varsRoot__", ST_N_UNDEFINED, UNKNOWN, scope);
        stackPushSt(&stack, &stVarsElse);       // Entering new scope (else)
        // else rule: <if_else_st> -> <cmd_block>
        // if (expression == false) generateIfScope();      // expression ... in line 591 in command(); or upper (804)
        afterIf = false;
        result = commandBlock();
        if (result != RC_OK) return result;
        generateIfScopeEnd();

        // TODO: Symtable check	
        #ifdef DEBUG
        displayBST(stackGetTopSt(&stack));
        #endif
        stNodePtr destructedElse = stackPopSt(&stack);
        stDestruct(&destructedElse);     // End of the scope (else)
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
        currentVar = strGetStr(&tk->attribute.string);
        result = stStackInsert(&stack, currentVar, ST_N_VARIABLE, UNKNOWN, scope);
        if (result != RC_OK) {
            setErrMsg("redefinition attempt");
            return RC_ERR_SEMANTIC_PROG_FUNC;
        }
        generatorSaveID(strGetStr(&tk->attribute.string));
        // := <expression>
        getToken(token, tk);
        if (tk->type != TYPE_DECLARATIVE_ASSIGN) {
            setErrMsg("expected ':=' after identifier [for definition]");
            return RC_ERR_SYNTAX_ANALYSIS;
        }
         // TODO: Check functionality of this part
        getToken(token, tk);                        // Step over DECLR ASSIGN	

        result = precedent_analys(tk, &precType, &stack);// Evaluate expression	
        if (result != RC_OK) return result;
        stVarSetType(stStackLookUp(&stack, currentVar), precTypeToSymtableType(precType));// Set variable type
        generateDefinitions();
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
        generatorSaveID(strGetStr(&tk->attribute.string));
        getToken(token, tk);
        if (tk->type != TYPE_ASSIGN) {
            setErrMsg("expected '=' after identifier [for assignment]");
            return RC_ERR_SYNTAX_ANALYSIS;
        }
         // TODO: Check functionality of this part
        getToken(token, tk);                        // Step over ASSIGN	

        result = precedent_analys(tk, &precType, &stack);// Evaluate expression	
        if (result != RC_OK) return result;
        if (stVarTypeLookUp(&stack, currentVar) != precTypeToSymtableType(precType)) {
            debugPrint("Found var type: %d, assigning: %d, received: %d", stVarTypeLookUp(&stack, currentVar), precTypeToSymtableType(precType), precType)
            iPrint(RC_ERR_SEMANTIC_TYPECOMP, true, "assigning wrong type");
            return RC_ERR_SEMANTIC_TYPECOMP;
        }
        generateAssignments();
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
    int returns = 0;
    stNodePtr GST = stackGetBotSt(&stack);;
    stNodePtr func = NULL;

    // rule: <return_stat> -> <expression>
    // note: after expression handle, its result will be (pushed) on top of stack (last expr. -> top)
    do {
        getToken(token, tk);

        if (tk->type == TYPE_EOL) // <return_stat> -> eps [return EOL]
            return result;

        // <return_stat> -> <expression>
        result = precedent_analys(tk, &precType, &stack);// Evaluate expression	
        if (result != RC_OK) return result;

        func = stLookUp(&GST, currentFnc);
        if ((returns < func->fData->returnNum) && (stFncGetType(func)[returns] != precTypeToSymtableType(precType))) {
            iPrint(RC_ERR_SEMANTIC_PARAM, true, "wrong return type");
            return RC_ERR_SEMANTIC_PARAM;
        }

        returns++;
        if (tk->type != TYPE_COMMA && returns != func->fData->returnNum) {
            char str[90] = "function ";
            char ret[60] = { 0 };
            snprintf(ret, 59,"expected %d return value(s) but got %d", func->fData->returnNum, returns);
            strcat(str, ret);
            iPrint(RC_ERR_SEMANTIC_PARAM, true, str);
            return RC_ERR_SEMANTIC_PARAM;
        }
    } while (returns != stLookUp(&stFunctions, currentFnc)->fData->returnNum);

    // rule: <return_stat> -> eps
    // TODO -> if not expression -> else do nothing
    return result;
}