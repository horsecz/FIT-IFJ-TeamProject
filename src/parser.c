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

BTNode_t* symbolTable; // symboltable global var.
Token* tk; // token global var.
int token;
int mainFound = 0; // indicator if 'func main' was found in program or not
//InstructionsList* il;

eRC returnStatement() {
    eRC result = RC_OK;
    token = getToken(tk);

    // rule: <return_stat> -> <expression>
    // TODO -> handle expression

    // rule: <return_stat> -> eps
    // TODO -> if not expression -> else do nothing

    return result;
}

eRC returnCommand() {
    // rule: <return_cmd> -> return <return_stat>
    eRC result = RC_OK;

    if (tk->attribute.keyword != KEYWORD_RETURN)
        return RC_ERR_SYNTAX_ANALYSIS;

    result = returnStatement();
    if (result != RC_OK) return result;

    return result;
}

eRC forAssign() {
    eRC result = RC_OK;
    token = getToken(tk);

    // rule: <for_assignment> -> ID = <expression>
    if (tk->type == TYPE_IDENTIFIER) {
        token = getToken(tk);
        if (tk->type != TYPE_ASSIGN)
            return RC_ERR_SYNTAX_ANALYSIS;
        // TODO -> handle <expression>
    }

    return result;
}

eRC forDefine() {
    eRC result = RC_OK;
    token = getToken(tk);

    // rule: <for_definition> -> ID := <expression>
    if (tk->type == TYPE_IDENTIFIER) {
        // := <expression>
        token = getToken(tk);
        if (tk->type != TYPE_DECLARATIVE_ASSIGN)
            return RC_ERR_SYNTAX_ANALYSIS;
        // TODO -> handle <expression>
    }
    // else rule: <for_definition> -> eps

    return result;
}

eRC statementMul() {
    eRC result = RC_OK;
    token = getToken(tk);

    // rule: <stat_mul> -> <expression>
    // TODO -> handle <expression>

    // rule: <stat_mul> -> ID ( <arguments> )
    if (tk->type != TYPE_IDENTIFIER)
        return RC_ERR_SYNTAX_ANALYSIS;

    token = getToken(tk);
    if (tk->type != TYPE_LEFT_CURLY_BRACKET)
        return RC_ERR_SYNTAX_ANALYSIS;

    result = arguments();
    if (result != RC_OK) return result;
    if (tk->type != TYPE_RIGHT_CURLY_BRACKET)
        return RC_ERR_SYNTAX_ANALYSIS;

    return result;
}

eRC variableIdNext() {
    eRC result = RC_OK;

    // rule: <var_id_n> -> , ID <var_id_n>
    if (tk->type == TYPE_COMMA) {
        token = getToken(tk);
        if (tk->type != TYPE_IDENTIFIER) return RC_ERR_SYNTAX_ANALYSIS;
        token = getToken(tk);
        result = variableIdNext(); // TODO -> i dont like this
        if (result != RC_OK) return result;
    }
    // else rule: <var_id_n> -> eps

    return result;
}

eRC statement() {
    eRC result = RC_OK;

    switch (tk->type) {
        case TYPE_DECLARATIVE_ASSIGN: // rule: <statement> -> := <expression>
            token = getToken(tk);
            // TODO -> somehow handle <expression>
            break;
        case TYPE_LEFT_CURLY_BRACKET: // rule: <statement> -> ( <arguments> )
            result = arguments();
            if (result != RC_OK) return result;
            if (tk->type != TYPE_RIGHT_CURLY_BRACKET) return RC_ERR_SYNTAX_ANALYSIS;
            break;
        case TYPE_COMMA: // rule: <statement> -> <var_id_n> = <stat_mul>
            result = variableIdNext();
            if (result != RC_OK) return result;
            if (tk->type != TYPE_ASSIGN) return RC_ERR_SYNTAX_ANALYSIS;
            result = statementMul();
            if (result != RC_OK) return result;
            break;
        default:
            result = RC_ERR_SYNTAX_ANALYSIS;
            break;
    }

    return result;
}

// TODO -> too big function, maybe separate into smaller ones ? (e.g. if_handle(), for_handle(), ..)
eRC command() {
    eRC result = RC_OK;

    // rule: <cmd> -> ID <statement>
    if (tk->type == TYPE_IDENTIFIER) {
        token = getToken(tk);
        result = statement();
        if (result != RC_OK) return result;
        token = getToken(tk);
    } else if (tk->type == TYPE_KEYWORD) {
        switch (tk->attribute.keyword) {
            case KEYWORD_IF: // rule: <cmd> -> if ...
                // <expression>
                token = getToken(tk);
                // TODO -> handle <expression>

                // <cmd_block>
                token = getToken(tk);
                result = commandBlock();
                if (result != RC_OK) return result;

                // else
                if (tk->type != TYPE_KEYWORD || tk->attribute.keyword != KEYWORD_ELSE)
                    return RC_ERR_SYNTAX_ANALYSIS;

                // <cmd_block>
                token = getToken(tk);
                result = commandBlock();
                if (result != RC_OK) return result;
                break;
            case KEYWORD_FOR: // rule: <cmd> -> for ...
                // <for_definition>
                result = forDefine();
                if (result != RC_OK) return result;

                // ;
                if (tk->type != TYPE_SEMICOLON)
                    return RC_ERR_SYNTAX_ANALYSIS;

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
                result = RC_ERR_SYNTAX_ANALYSIS;
                break;
        }
        token = getToken(tk);
    }
    // else rule: <return_cmd> -> eps => <cmd> -> eps

    return result;
}

eRC commands() {
    eRC result = RC_OK;

    // rule: <commands> -> <cmd> EOL <commands>
    // <cmd>
    result = command();
    if (result != RC_OK) return result;

    // EOL
    if (tk->type != TYPE_EOL) {
        return RC_ERR_SYNTAX_ANALYSIS;
    } else {
        // allow indefinite EOLs
        while (tk->type == TYPE_EOL)
            token = getToken(tk);
    }

    // <commands> -> eps
    if (tk->type == TYPE_RIGHT_BRACKET)
        return result;

    // <commands> -> <cmd> EOL <commands>
    result = commands();
    if (result != RC_OK) return result;

    return result;
}

eRC commandBlock() {
    // rule: <cmd_block> -> { EOL <commands> } EOL
    eRC result = RC_OK;

    // { EOL
    if (tk->type != TYPE_LEFT_BRACKET)
        return RC_ERR_SYNTAX_ANALYSIS;

    token = getToken(tk);
    if (tk->type != TYPE_EOL) {
        return RC_ERR_SYNTAX_ANALYSIS;
    } else {
        // allow indefinite EOLs
        while (tk->type == TYPE_EOL)
            token = getToken(tk);
    }

    // <commands>
    result = commands();
    if (result != RC_OK) return result;

    // } EOL
    if (tk->type != TYPE_RIGHT_BRACKET) {
        return RC_ERR_SYNTAX_ANALYSIS;
    } else {
        // allow indefinite EOLs
        while (tk->type == TYPE_EOL)
            token = getToken(tk);
    }

    return result;
}

eRC typeFunctionNext() {
    eRC result = RC_OK;

    // rule: <r_type_n> -> , <type> <r_type_n>
    if (tk->type == TYPE_COMMA) {
        // <type>
        token = getToken(tk);
        result = type();
        if (result != RC_OK) return result;

        // <r_type_n>
        token = getToken(tk);
        result = typeFunctionNext(); // TODO -> i dont like this
        if (result != RC_OK) return result;
    }

    return result;
}

eRC typeFunction() {
    eRC result = RC_OK;

    switch (tk->type) {
        case TYPE_INT: // rule: <f_type> -> <type>
        case TYPE_FLOAT64:
        case TYPE_STRING:
        case TYPE_RIGHT_CURLY_BRACKET: // rule: <f_type> -> eps (=> token is ')' )
            break;
        default:
            result = RC_ERR_SYNTAX_ANALYSIS;
            break;
    }

    return result;
}

eRC functionReturn() {
    eRC result = RC_OK;

    // rule: <func_return> -> ( <f_type> <r_type_n> )
    if (tk->type == TYPE_LEFT_CURLY_BRACKET) {
        // <f_type>
        token = getToken(tk);
        result = typeFunction();
        if (result != RC_OK) return result;

        // <r_type_n>
        if (tk->type != TYPE_RIGHT_CURLY_BRACKET) {
            result = typeFunctionNext();
            if (result != RC_OK) return result;
            if (tk->type != TYPE_RIGHT_CURLY_BRACKET)
                return RC_ERR_SYNTAX_ANALYSIS;
            token = getToken(tk);
        }
    }
    // else rule: <func_return> -> eps

    return result;
}

eRC argumentsNext() {
    eRC result = RC_OK;
    token = getToken(tk);

    // rule: <arguments_n> -> , ID <type> <arguments_n>
    if (tk->type == TYPE_COMMA) {
        token = getToken(tk);
        // ID
        if (tk->type != TYPE_IDENTIFIER)
            return RC_ERR_SYNTAX_ANALYSIS;

        // <type>
        result = type();
        if (result != RC_OK) return result;

        // <arguments_n>
        result = argumentsNext();
        if (result != RC_OK) return result;
    }
    // else rule: <arguments_n> -> eps
    return result;
}

eRC type() {
    // rule: <type> -> int || <type> -> float64 || <type> -> string
    eRC result = RC_OK;

    switch (tk->type) {
        case TYPE_INT:
        case TYPE_FLOAT64:
        case TYPE_STRING:
            break;
        default:
            result = RC_ERR_SYNTAX_ANALYSIS;
            break;
    }

    return result;
}

eRC arguments() {
    // rule: <arguments> -> ID <type> <arguments_n>
    eRC result = RC_OK;

    // ID
    token = getToken(tk);
    if (tk->type == TYPE_IDENTIFIER) {
        // <type> <arguments_n>
        result = type();
        if (result != RC_OK) return result;
        result = argumentNext();
        if (result != RC_OK) return result;
    }
    // else rule: <arguments> -> eps

    return result;
}

eRC function() {
    // rule: <func> -> func ID ( <arguments> ) <func_return> <cmd_block>
    eRC result = RC_OK;

    // func keyword
    if (strCmpConstStr(strGetStr(tk->attribute.string), "func"))
        return RC_ERR_SYNTAX_ANALYSIS;

    // ID: function identificator
    token = getToken(tk);
    if (tk->type != TYPE_IDENTIFIER) {
        return RC_ERR_SYNTAX_ANALYSIS;
    } else {
        if (strCmpConstStr(strGetStr(tk->attribute.string), "main"))
            mainFound++;
    }

    // ( <arguments> )
    token = getToken(tk);
    if (tk->type != TYPE_LEFT_CURLY_BRACKET)
        return RC_ERR_SYNTAX_ANALYSIS;

    result = arguments();
    if (result != RC_OK) return result;
    if (tk->type != TYPE_RIGHT_CURLY_BRACKET)
        return RC_ERR_SYNTAX_ANALYSIS;

    // <func_return>
    token = getToken(tk);
    result = functionReturn();
    if (result != RC_OK) return result;

    // <cmd_block>
    result = commandBlock();
    if (result != RC_OK) return result;

    return result;
}

eRC functionNext() {
    // rule: <function_n> -> <func> <function_n> || <function_n> -> eps
    eRC result = RC_OK;

    // in function: cmd_block: EOLs are skipped and i got next token which is not EOL
    switch (tk->type) {
        case TYPE_EOF: // <function_n> -> eps
            return result;
            break;
        case TYPE_KEYWORD: // <function_n> -> <func> <function_n>
            if (strCmpConstStr(strGetStr(tk->attribute.string), "func"))
                return RC_ERR_SYNTAX_ANALYSIS;
            result = function();
            if (result != RC_OK) return result;
            result = functionNext(); // TODO -> i dont like this
            if (result != RC_OK) return result;
            break;
        default:
            result = RC_ERR_SYNTAX_ANALYSIS;
            break;
    }

    return result;
}

eRC functions() {
    // rule: <functions> -> <func> <function_n>
    eRC result = RC_OK;

    // program must have at least 1 function
    if (strCmpConstStr(strGetStr(tk->attribute.string), "func"))
        return RC_ERR_SYNTAX_ANALYSIS;

    result = function();
    if (result != RC_OK) return result;
    result = functionNext();

    // after analysis of all functions, ONE main should be found
    if (mainFound == 0 || mainFound > 1)
        return RC_ERR_SYNTAX_ANALYSIS;

    return result;
}

eRC eolR() {
    // rule: <eol_r> -> EOL <eol_r> || rule: <eol_r> -> eps
    eRC result = RC_OK;
    while (tk->type == TYPE_EOL)
        token = getToken(tk);

    return result;
}

eRC eolM() {
    // rule: <eol_m> -> EOL <eol_r>
    eRC result = RC_OK;
    token = getToken(tk);

    if (tk->type != TYPE_EOL)
        result = RC_ERR_SYNTAX_ANALYSIS;

    result = eolR();
    return result;
}

eRC prolog() {
    // rule: <prolog> -> package main
    eRC result = RC_OK;

    switch (tk->type) {
        case TYPE_KEYWORD:
            if (strCmpConstStr(strGetStr(tk->attribute.string), "package")) {
                return RC_ERR_SYNTAX_ANALYSIS;
            }
            token = getToken(tk);
            if (strCmpConstStr(strGetStr(tk->attribute.string), "main") || tk->type != TYPE_IDENTIFIER) {
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

    // Cycle 'till we are out of comments and eols
    while (tk->type == TYPE_EMPTY || tk->type == TYPE_EOL) {
        token = getToken(tk);
        if (token == 1) {
            iPrint(RC_ERR_LEXICAL_ANALYSIS, true);
            return RC_ERR_LEXICAL_ANALYSIS;
        } else if (token) {
            iPrint(RC_ERR_INTERNAL, true);
            return RC_ERR_INTERNAL;
        }
    }

    // incorrect: package -> rule <program>
    if (strCmpConstStr(strGetStr(tk->attribute.string), "package")) {
        return RC_ERR_SYNTAX_ANALYSIS;
    }

    // next token should be <prolog>, which is KEYWORD
    switch (tk->type) {
        case TYPE_KEYWORD:
            result = prolog();
            if (result != RC_OK) return result;
            result = eolM();
            if (result != RC_OK) return result;
            result = functions();
            if (result != RC_OK) return result;
            if (tk->type != TYPE_EOF) return RC_ERR_SYNTAX_ANALYSIS; //end of program, EOF expected
            //generateInstruction(program_end,...)
            break;
        default:
            result = RC_ERR_SYNTAX_ANALYSIS;
            break;
        }

    return result;
}

eRC parser(BTNodePtr* SymbolTable) {
    token = getToken(tk);

    switch (token) {
        // TOKEN OK
        case 0:
            return program();
        // LEX ERROR
        case 1:
            iPrint(RC_ERR_LEXICAL_ANALYSIS, true);
            return RC_ERR_LEXICAL_ANALYSIS;
        // 2+ INTERNAL in this case
        case 2:
        case 99:
        default:
            iPrint(RC_ERR_INTERNAL, true);
            return RC_ERR_INTERNAL;
    }
}