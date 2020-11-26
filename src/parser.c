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

// use only with functions returning eRC type!
// gets next token + checks if lexical error or internal error occured
#define getToken(returnVar,tokenVar)                                                    \
    do{                                                                                 \
        returnVar = getToken(tokenVar);                                                 \
        if (returnVar == 1) {                                                           \
	        iPrint(RC_ERR_LEXICAL_ANALYSIS, true, "invalid character read from input"); \
	        return RC_ERR_LEXICAL_ANALYSIS;                                             \
	    } else if (returnVar == 2 || returnVar == 99) {                                 \
	        iPrint(RC_ERR_INTERNAL, true, "internal error (malloc, other)");            \
	        return RC_ERR_INTERNAL;                                                     \
	    }                                                                               \
	}while(0)                                                                           \

// maximum size of error message
#define MAX_ERR_MSG 255
//
#define setErrMsg(msg) do{ strncpy(errText, msg, MAX_ERR_MSG);  }while(0)

// DEBUG MACROS

#ifdef DEBUG
// print debug message
#define debugPrint(msg, ...) fprintf(stderr, "[DBG] "msg"\n", ##__VA_ARGS__);
// mark some point in code
#define debugPoint fprintf(stderr, "[DBG] ---> marked point <---\n");
// mark more points in code
#define debugPoints(ptNum) fprintf(stderr, "[DBG] --> %d - marked point - %d <---\n", ptNum, ptNum);

#else
// if not compiled with DEBUG, define as empty macros
#define debugPrint(msg, ...)
#define debugPoint
#define debugPoints
#endif

// GLOBAL VARIABLES

stNodePtr stFunctions;      /**< Symboltable for functions (only fucntions are global) */
stStack stack;              /**< Global stack for symtables                            */
stID currentFnc;            /**< To remember current function (for lookup)             */
Token* tk;                  /**< Token store - global                                  */
int token;                  /**< Token return code store - global                      */
int mainFound = 0;          /**< Was `function main` found in program?                 */
char errText[MAX_ERR_MSG];  /**< Error text msg (if error occurs)                      */
bool printLastToken;        /**< For error message - if last token may be printed      */
bool argRet = false;        /**< Arguments and returns switch for function parsing     */
//InstructionsList* il;

// FUNCTIONS - RULES

eRC returnStatement() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;
    getToken(token, tk);

    // rule: <return_stat> -> <expression>
    // TODO -> handle expression

    // rule: <return_stat> -> eps
    // TODO -> if not expression -> else do nothing
    return result;
}

eRC returnCommand() {
    debugPrint("rule %s", __func__);
    // rule: <return_cmd> -> return <return_stat>
    eRC result = RC_OK;

    if (tk->type != TYPE_KEYWORD || tk->attribute.keyword != KEYWORD_RETURN) {
        setErrMsg("expected keyword 'return'");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    result = returnStatement();
    if (result != RC_OK) return result;

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

eRC statementMul() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;
    getToken(token, tk);

    // rule: <stat_mul> -> <expression>
    // TODO -> handle <expression>

    // rule: <stat_mul> -> ID ( <arguments> )
    if (tk->type != TYPE_IDENTIFIER) {
        setErrMsg("expected identifier [multiple statement]");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    getToken(token, tk);
    if (tk->type != TYPE_LEFT_BRACKET) {
        setErrMsg("expected '(' after identifier");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    result = arguments();
    if (result != RC_OK) return result;
    if (tk->type != TYPE_RIGHT_BRACKET) {
        setErrMsg("expected ')' after argument types");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    return result;
}

eRC variableIdNext() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    // rule: <var_id_n> -> , ID <var_id_n>
    if (tk->type == TYPE_COMMA) {
        getToken(token, tk);
        if (tk->type != TYPE_IDENTIFIER) {
            setErrMsg("expected next identifier after ','");
            return RC_ERR_SYNTAX_ANALYSIS;
        }
        getToken(token, tk);
        result = variableIdNext(); // TODO -> i dont like this
        if (result != RC_OK) return result;
    }
    // else rule: <var_id_n> -> eps

    return result;
}

eRC assignment() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;
    switch (tk->type) {
        case TYPE_IDENTIFIER: // rule: <assignment> -> ID ( <arguments> )
            getToken(token, tk);
            if (tk->type != TYPE_LEFT_BRACKET) {
                setErrMsg("expected '(' after identifier");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            result = arguments();
            if (result != RC_OK) return result;
            if (tk->type != TYPE_RIGHT_BRACKET) {
                setErrMsg("expected ')' after function arguments");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            break;
        default:
            // rule: <assignment> -> <expression>
            // TODO -> handle expression
            break;
    }

    return result;
}

eRC unary() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;
    switch (tk->type) {
        case TYPE_PLUS_ASSIGN: // rules: <unary> -> += or -= or *= or /=
        case TYPE_MINUS_ASSIGN:
        case TYPE_MULTIPLY_ASSIGN:
        case TYPE_DIVIDE_ASSIGN:
            break;
        default:
            setErrMsg("expected unary assignment token '+=', '-=', '*=' or '/='");
            result = RC_ERR_SYNTAX_ANALYSIS;
            break;
    }

    return result;
}

eRC statement() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    switch (tk->type) {
        case TYPE_ASSIGN: // rule: <statement> -> = <assignment>
            getToken(token, tk);
            result = assignment();
            if (result != RC_OK) return result;
            break;
        case TYPE_LEFT_BRACKET: // rule: <statement> -> ( <arguments> )
            result = arguments();
            if (result != RC_OK) return result;
            if (tk->type != TYPE_RIGHT_BRACKET) {
                setErrMsg("expected ')' after arguments [of func-call]");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            break;
        case TYPE_DECLARATIVE_ASSIGN: // rule: <statement> -> := <assignment>
            getToken(token, tk);
            result = assignment();
            if (result != RC_OK) return result;
            break;
        default: // rule: <statement> -> <unary> <expression>
            result = unary();
            if (result != RC_OK) return result;
            getToken(token, tk);
            // TODO -> handle expression
            break;
    }

    return result;
}

eRC ifElseExpanded() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    if (tk->attribute.keyword == KEYWORD_IF) {
        // rule: <if_else_st> -> if <cmd_block> <if_else_st_n>
        // <cmd_block>
        getToken(token, tk);
        result = commandBlock();
        if (result != RC_OK) return result;

        // <if_else_st_n>
        result = ifElse();
        if (result != RC_OK) return result;
    } else {
        // else rule: <if_else_st> -> <cmd_block>
        result = commandBlock();
        if (result != RC_OK) return result;
    }

    return result;
}

eRC ifElse() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;
    switch (tk->attribute.keyword) {
        case KEYWORD_ELSE: // rule: <if_else> -> else <if_else_st>
            getToken(token, tk);
            result = ifElseExpanded();
            if (result != RC_OK) return result;
        default:
            break;
    }

    return RC_OK;
}

// TODO -> too big function, maybe separate into smaller ones ? (e.g. if_handle(), for_handle(), ..)
eRC command() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    // rule: <cmd> -> ID <statement>
    if (tk->type == TYPE_IDENTIFIER) {
        getToken(token, tk);
        result = statement();
        if (result != RC_OK) return result;
        getToken(token, tk);
    } else if (tk->type == TYPE_KEYWORD) {
        switch (tk->attribute.keyword) {
            case KEYWORD_IF: // rule: <cmd> -> if <expression> <cmd_block> <if_else>
                // <expression>
                getToken(token, tk);
                // TODO -> handle <expression>

                // <cmd_block>
                getToken(token, tk);
                result = commandBlock();
                if (result != RC_OK) return result;

                // <if_else>
                result = ifElse();
                if (result != RC_OK) return result;
                break;
            case KEYWORD_FOR: // rule: <cmd> -> for ...
                // <for_definition>
                result = forDefine();
                if (result != RC_OK) return result;

                // ;
                if (tk->type != TYPE_SEMICOLON) {
                    setErrMsg("expected ';' after for cycle definition");
                    return RC_ERR_SYNTAX_ANALYSIS;
                }

                // <expression>
                // TODO -> handle <expression>

                // <for_assignment>
                result = forAssign();
                if (result != RC_OK) return result;

                // <cmd_block>
                result = commandBlock();
                if (result != RC_OK) return result;
                break;
            case KEYWORD_RETURN: // rule: <cmd> -> <return_cmd>
                result = returnCommand();
                if (result != RC_OK) return result;
                break;
            default:
                setErrMsg("expected keyword 'if', 'for' or 'return'");
                result = RC_ERR_SYNTAX_ANALYSIS;
                break;
        }
        //getToken(token, tk);
    }
    // else rule: <return_cmd> -> eps => <cmd> -> eps

    return result;
}

eRC commands() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    // rule: <commands> -> <cmd> EOL <commands>
    // <cmd>
    result = command();
    if (result != RC_OK) return result;

    // EOL
    if (tk->type != TYPE_EOL) {
        setErrMsg("expected end-of-line after (one) command");
        return RC_ERR_SYNTAX_ANALYSIS;
    } else {
        // allow indefinite EOLs
        while (tk->type == TYPE_EOL)
            getToken(token, tk);
    }

    // <commands> -> eps
    if (tk->type == TYPE_RIGHT_CURLY_BRACKET)
        return result;

    // <commands> -> <cmd> EOL <commands>
    result = commands();
    if (result != RC_OK) return result;

    return result;
}

eRC commandBlock() {
    debugPrint("rule %s", __func__);
    // rule: <cmd_block> -> { EOL <commands> } EOL
    eRC result = RC_OK;

    getToken(token, tk);
    // { EOL
    if (tk->type != TYPE_LEFT_CURLY_BRACKET) {
        setErrMsg("expected beginning of command block, aka '{' ");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    getToken(token, tk);
    if (tk->type != TYPE_EOL) {
        setErrMsg("expected end-of-line after '{'");
        return RC_ERR_SYNTAX_ANALYSIS;
    } else {
        // allow indefinite EOLs
        while (tk->type == TYPE_EOL)
            getToken(token, tk);
    }

    // <commands>
    result = commands();
    if (result != RC_OK) return result;

    // } EOL
    if (tk->type != TYPE_RIGHT_CURLY_BRACKET) {
        setErrMsg("expected end of command block, aka '}'");
        return RC_ERR_SYNTAX_ANALYSIS;
    } else {
        // allow indefinite EOLs
        while (tk->type == TYPE_EOL)
            getToken(token, tk);
    }

    return result;
}

eRC typeFunctionNext() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    // rule: <r_type_n> -> , <type> <r_type_n>
    if (tk->type == TYPE_COMMA) {
        // <type>
        getToken(token, tk);
        result = type();
        if (result != RC_OK) return result;

        // <r_type_n>
        getToken(token, tk);
        result = typeFunctionNext(); // TODO -> i dont like this
        if (result != RC_OK) return result;
    }
    // else rule: <r_type_n> -> eps

    return result;
}

eRC typeFunction() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    switch (tk->type) {
        case TYPE_INT: // rule: <f_type> -> <type> <r_type_n>
        case TYPE_FLOAT64:
        case TYPE_STRING:
        case TYPE_BOOL:
            getToken(token, tk);
            result = typeFunctionNext();
            if (result != RC_OK) return result;
            if (tk->type != TYPE_RIGHT_BRACKET) {
                 setErrMsg("expected ')' after last function return type");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            getToken(token, tk);
        case TYPE_RIGHT_BRACKET: // rule: <f_type> -> eps (=> token is ')' )
            break;
        default:
            setErrMsg("expected ')' after '(' [func return type]");
            result = RC_ERR_SYNTAX_ANALYSIS;
            break;
    }

    return result;
}

eRC functionReturn() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;

    // rule: <func_return> -> ( <f_type> )
    if (tk->type == TYPE_LEFT_BRACKET) {
        // <f_type>
        getToken(token, tk);
        result = typeFunction();
        if (result != RC_OK) return result;
    }
    // else rule: <func_return> -> eps

    return result;
}

eRC argumentNext() {
    debugPrint("rule %s", __func__);
    eRC result = RC_OK;
    getToken(token, tk);

    // rule: <arguments_n> -> , ID <type> <arguments_n>
    if (tk->type == TYPE_COMMA) {
        getToken(token, tk);
        // ID
        if (tk->type != TYPE_IDENTIFIER) {
            setErrMsg("expected next identifier after ','");
            return RC_ERR_SYNTAX_ANALYSIS;
        }

        // <type>
        getToken(token, tk);
        result = type();
        if (result != RC_OK) return result;

        // <arguments_n>
        result = argumentNext();
        if (result != RC_OK) return result;
    }
    // else rule: <arguments_n> -> eps
    return result;
}

eRC type() {
    debugPrint("rule %s", __func__);
    // rule: <type> -> int || <type> -> float64 || <type> -> string
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

eRC arguments() {
    debugPrint("rule %s", __func__);
    // rule: <arguments> -> ID <type> <arguments_n>
    eRC result = RC_OK;

    // ID
    getToken(token, tk);
    if (tk->type == TYPE_IDENTIFIER) {
        // <type> <arguments_n>
        getToken(token, tk);
        result = type();
        if (result != RC_OK) return result;
        result = argumentNext();
        if (result != RC_OK) return result;
    }
    // else rule: <arguments> -> eps

    return result;
}

eRC function() {
    debugPrint("rule %s", __func__);
    // rule: <func> -> func ID ( <arguments> ) <func_return> <cmd_block>
    eRC result = RC_OK;

    // func keyword
    if (tk->type != TYPE_KEYWORD || tk->attribute.keyword != KEYWORD_FUNC) {
        setErrMsg("expected 'func' keyword");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    // ID: function identifier
    getToken(token, tk);
    if (tk->type != TYPE_IDENTIFIER) {
        setErrMsg("expected function identifier after 'func' keyword");
        return RC_ERR_SYNTAX_ANALYSIS;
    } else {
        if (tk->type == TYPE_IDENTIFIER && strCmpConstStr(&(tk->attribute.string), "main")) {
            mainFound++;
        }
        stStackInsert(&stack, strGetStr(&(tk->attribute.string)), ST_N_FUNCTION, UNKNOWN);
        currentFnc = strGetStr(&(tk->attribute.string));
    }

    // ( <arguments> )
    getToken(token, tk);
    if (tk->type != TYPE_LEFT_BRACKET) {
        setErrMsg("expected '(' after function identifier");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    argRet = false;
    result = arguments();
    if (result != RC_OK) return result;
    if (tk->type != TYPE_RIGHT_BRACKET) {
        setErrMsg("expected ')' after function arguments");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    // <func_return>
    getToken(token, tk);
    argRet = true;
    result = functionReturn();
    if (result != RC_OK) return result;

    // <cmd_block>
    result = commandBlock();
    if (result != RC_OK) return result;

    return result;
}

eRC functionNext() {
    debugPrint("rule %s", __func__);
    // rule: <function_n> -> <func> <function_n> || <function_n> -> eps
    eRC result = RC_OK;

    // in function: cmd_block: EOLs are skipped and i got next token which is not EOL
    switch (tk->type) {
        case TYPE_EOF: // <function_n> -> eps
            return result;
            break;
        case TYPE_KEYWORD: // <function_n> -> <func> <function_n>
            if (tk->type != TYPE_KEYWORD || tk->attribute.keyword != KEYWORD_FUNC) {
                setErrMsg("expected keyword 'func' after function definition");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            result = function();
            if (result != RC_OK) return result;
            result = functionNext(); // TODO -> i dont like this
            if (result != RC_OK) return result;
            break;
        default:
            setErrMsg("expected keyword 'func' or EOF token after function definition");
            result = RC_ERR_SYNTAX_ANALYSIS;
            break;
    }

    return result;
}

eRC functions() {
    debugPrint("rule %s", __func__);
    // Rule: <functions> -> <func> <function_n>
    eRC result = RC_OK;
    // Program must have at least 1 function
    if (tk->type != TYPE_KEYWORD || tk->attribute.keyword != KEYWORD_FUNC) {
        setErrMsg("expected at least one 'func' keyword after prolog");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    result = function();
    if (result != RC_OK) return result;
    result = functionNext();

    // After analysis of all functions, ONE main should be found
    if (mainFound == 0 || mainFound > 1) {
        if (mainFound) setErrMsg("multiple definitions of main");
        else setErrMsg("missing definition of main");
        printLastToken = 0;
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    return result;
}

eRC eolR() {
    debugPrint("rule %s", __func__);
    // Rule: <eol_r> -> EOL <eol_r> || Rule: <eol_r> -> eps
    eRC result = RC_OK;
    while (tk->type == TYPE_EOL || tk->type == TYPE_EMPTY) {
        getToken(token, tk);
    }
    return result;
}

eRC eolM() {
    debugPrint("rule %s", __func__);
    // Rule: <eol_m> -> EOL <eol_r>
    eRC result = RC_OK;
    getToken(token, tk);

    if (tk->type != TYPE_EOL) { //TODO: Take into account possible comments after fnc?
        setErrMsg("expected end-of-line after 'package main'");
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    result = eolR();
    return result;
}

eRC prolog() {
    debugPrint("rule %s", __func__);
    // Rule: <prolog> -> package main
    eRC result = RC_OK;

    switch (tk->type) {
        case TYPE_KEYWORD:
            if (tk->type != TYPE_KEYWORD || tk->attribute.keyword != KEYWORD_PACKAGE) {
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            getToken(token, tk);
            if (tk->type != TYPE_IDENTIFIER || strCmpConstStr(&(tk->attribute.string), "main")) {
                setErrMsg("expected identifier 'main'");
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            break;
        default:
            return RC_ERR_SYNTAX_ANALYSIS;
    }

    return result;
}

eRC program() {
    eRC result = RC_OK;
    debugPrint("rule %s", __func__);
    // Cycle 'till we are out of comments and eols
    while (tk->type == TYPE_EMPTY || tk->type == TYPE_EOL) {
        getToken(token, tk);
    }
    // Incorrect: package -> rule <program>
    if (tk->type != TYPE_KEYWORD || tk->attribute.keyword != KEYWORD_PACKAGE) {
        setErrMsg("expected 'package' in first line");
        return RC_ERR_SYNTAX_ANALYSIS;
    }
    // Next token should be <prolog>, which is KEYWORD
    switch (tk->type) {
        case TYPE_KEYWORD:
            result = prolog();
            if (result != RC_OK) return result;
            result = eolM();
            if (result != RC_OK) return result;
            result = functions();
            if (result != RC_OK) return result;
            if (tk->type != TYPE_EOF) {
                setErrMsg("expected 'EOF'");
                return RC_ERR_SYNTAX_ANALYSIS; //end of program, EOF expected
            }
            //generateInstruction(program_end,...)
            break;
        default:
            result = RC_ERR_SYNTAX_ANALYSIS;
            break;
        }

    return result;
}

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
    // Get first token
    getToken(token, tk);
    // Start parse
    result = program();
    if (result != RC_OK) {
        if (printLastToken) {
            string gotStr;
            strInit(&gotStr);

            tokenToString(tk, &gotStr);
            int strSize = strlen(errText) + strGetLength(&gotStr);

            strcat(errText, " but got '");
            strncat(errText, strGetStr(&gotStr),
                    MAX_ERR_MSG - strlen(errText) - 5); // need 4 chars for ...' in worst case + null byte

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
    return result;
}