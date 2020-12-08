//
// Created by horse on 08.12.20.
//

#include "semantic.h"
#include <stdio.h>
#include <string.h>

eRC semantic_analysis (char* functionName, stNodePtr stFunctions, stStack stack, stID currentVar, stID* currentVarMul, int numberOfIDs) {
    // check if return type(s) of this function is same as variable type(s)
    stVarType* retType = stFncGetType(stLookUp(&stFunctions, functionName));
    stVarType varType = stVarTypeLookUp(&stack, currentVar);
    stNodePtr func = stLookUp(&stFunctions, functionName);
    numberOfIDs++;

    // if function was found in GST (defined)
    if (func != NULL) {
        if (func->fData->returnNum != numberOfIDs) {   // variables != returnNum
            char str[255] = {0};
            snprintf(str, 254, "function which returns %d value(s) is assigned to %d variable(s)", func->fData->returnNum, numberOfIDs);
            setErrMsg(str);
            return RC_ERR_SEMANTIC_PARAM;
        }

        char str[255] = {0};
        char* f_returnType = NULL;
        char* v_type = NULL;

        for (int i = 0; i < func->fData->returnNum; i++) { // check types one by one
            if (i)
                varType = stVarTypeLookUp(&stack, currentVarMul[i]);

            if (retType[i] != varType) {
                f_returnType = setErrorType(retType[i]);
                v_type = setErrorType(varType);
                snprintf(str, 254, "assigning return type %s to variable with type %s", f_returnType, v_type);
                setErrMsg(str);

                if (f_returnType != NULL)
                    free(f_returnType);
                if (v_type != NULL)
                    free(v_type);

                return RC_ERR_SEMANTIC_TYPECOMP;
            }
        }
    } else { // function isnt in GST (not defined, but may be defined later)
        //saveFuncID, saveVariableTypes, checkLater
        // ?
    }

    numberOfIDs = 0;
    return RC_OK;
}

char* setErrorType (stVarType type) {
    char* text = NULL;
    switch (type) {
        case INT:
            text = "INT";
            break;
        case FLOAT64:
            text = "FLOAT64";
            break;
        case STRING:
            text = "STRING";
            break;
        case BOOL:
            text = "BOOL";
            break;
        default:
            text = "UNKNOWN";
            break;
    }

    char* stringType = (char*) calloc(sizeof(char)*10, 1);
    if (stringType == NULL) {
        fprintf(stderr, "[SEMANTIC] Internal error (unable to alloc memory for error msg)\n");
        return NULL;
    }

    strcpy(stringType, text);
    return stringType;
}