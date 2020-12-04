#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "symstack.h"
#include "precedent.h"
#include "str.h"

int precedent_analys(Token* help, Token* tokeng, TokenType* Type){

	symStack *stack = malloc(sizeof(symStack)); 	
	symstackInit(stack); 	
	symstackPush(stack, TYPE_EOL);	
	int a;							// pushnuti prvni hodnoty EOL

	if(help != NULL){

		if((a = idkfunkce(stack, help, Type))){
			return a;
		}
	}
	else
	{

	}
  
	if((a = idkfunkce(stack, tokeng, Type))){
		return a;	
	}
	

	while(!(StackTopTerm(stack) == TYPE_EOL && (tokeng->type == TYPE_EOL || tokeng->type == TYPE_SEMICOLON || tokeng->type == TYPE_COMMA)))							// cyklus pobezi dokud nenarazi na znak konce radku, streniku, nebo carky
	{	

		getToken(tokeng);					// ziskame token
		a = idkfunkce(stack, tokeng, Type);				// zavolame funkci pro zpracovani token
		if (a != SYNTAX_OK){
			return SEM_ERROR_OTHER;
		}
	}
	return SYNTAX_OK;
}

int idkfunkce(symStack *stack, Token* token, TokenType* Type){
		TokenType top = StackTopTerm(stack);						// do top nahrajeme nejvyssi terminal ze stacku

		if (token->type == TYPE_IDENTIFIER || token->type == TYPE_INT || token->type == TYPE_STRING  || token->type == TYPE_FLOAT64 || token->type == TYPE_BOOL)							//TODO je potreba zjitit co vse jsou identifikatory
		{ 

			if (top == TYPE_RIGHT_BRACKET || top == TYPE_IDENTIFIER || top == TYPE_FLOAT64 || top == TYPE_INT || top == TYPE_STRING || top == TYPE_BOOL)				//pokud je na vrcholu ) nebo identifikator, tak se vypise chyba, protoze nejsou kompatibilni
			{
				
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else
			{	
				if (token->type == TYPE_INT)
				{
					stackPushOpen(stack);				// jinak posle zacatek rozvoje
					symstackPush(stack,TYPE_INT);				// a znak identifikatoru
					stack->top->inte = token->attribute.integer;
				}
				else if (token->type == TYPE_STRING)
				{
					stackPushOpen(stack);				// jinak posle zacatek rozvoje
					symstackPush(stack,TYPE_STRING);				// a znak identifikatoru
					string str;
					strInit(&str);					
					strCopyString(stack->top->string, &token->attribute.string);
				}
				else if (token->type == TYPE_FLOAT64)
				{
					stackPushOpen(stack);				// jinak posle zacatek rozvoje
					symstackPush(stack,TYPE_FLOAT64);				// a znak identifikatoru
					stack->top->flt = token->attribute.float64;
				}
				else if (token->type == TYPE_IDENTIFIER) //musi se predelat 
				{/*
					tBSTNodePtr promnena;
					int g = in_function2;
					if(g == 1)
					{
						promnena = symtableSearch(local, token->attribute.string->string);

						if(promnena == NULL)
						{
							promnena = symtableSearch(global, token->attribute.string->string);	

							if(promnena == NULL)
								return SEM_ERROR_DEF;
						}
						
					}
					else
					{
						promnena = symtableSearch(global, token->attribute.string->string);	

						if(promnena == NULL)
							return SEM_ERROR_DEF;
					}
					

					tInsideVariable* var_content;
					var_content = promnena->content;
					if(var_content->dataType == 0){
						if(var_content->integer != 0){
							var_content->dataType = 1;
						}else if(var_content->string->length != 0){
							var_content->dataType = 3;
						}else if(var_content->flt != 0){
							var_content->dataType = 2;
						}
					}

					int typ = var_content->dataType;


					if (typ == 0) //pokud neni zatim nic
					{
						return SYNTAX_ERROR;
					}
					else if (typ == 1)	//pokud je integer
					{
						stackPushOpen(stack);				// jinak posle zacatek rozvoje
						symstackPush(stack,TYPE_INT);				// a znak identifikatoru
						stack->top->inte = var_content->integer;
						
					}
					else if (typ == 2)	//pokud je float
					{
						stackPushOpen(stack);				// jinak posle zacatek rozvoje
						symstackPush(stack,TYPE_FLOAT64);				// a znak identifikatoru
						stack->top->flt = var_content->flt;
						
					}
					else if (typ == 3)	// pokud je string
					{
						stackPushOpen(stack);				// jinak posle zacatek rozvoje
						symstackPush(stack,TYPE_STRING);				// a znak identifikatoru
						stack->top->string = var_content->string;
						
					}
					else
					{
						return SYNTAX_ERROR;
					}
					char* lastidcko = malloc(token->attribute.string->length);
					strcpy(lastidcko, token->attribute.string->string);
					stack->top->nazev = lastidcko;
					*/
				}
			}
		}
		else if (token->type == TYPE_LEFT_BRACKET)		// jako token dojde (
		{
			if (top == TYPE_RIGHT_BRACKET || top == TOKEN_PREC_ID)				//pokud je na vrcholu ) nebo identifikator, tak se vypise chyba, protoze nejsou kompatibilni
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else
			{
				stackPushOpen(stack);				// jinak posle zacatek rozvoje
				symstackPush(stack,TYPE_LEFT_BRACKET);					// a znak identifikatoru
			}
		}
		else if (token->type == TYPE_RIGHT_BRACKET)		// jako token dojde )
		{
			if (top == TYPE_EOL || top == TYPE_SEMICOLON )							//pokud je na vrcholu EOL, tak se vypise chyba, protoze nejsou kompatibilni
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else if (top == TYPE_LEFT_BRACKET )						//pokud na na vrchu (
			{
				symstackPush(stack,TYPE_RIGHT_BRACKET);					// pushni )
				symstackPush(stack,TOKEN_PREC_CLOSE);				// pushni konec rozvoje >
				reduction(stack);					// a proved redukci
			}
			else									//pokud je cokoliv jineho
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, token, Type);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}
			}
		}
		else if (token->type == TYPE_EOL || token->type == TYPE_SEMICOLON || TYPE_COMMA)			// jako token dojde EOL TODO ZEPTAT SE NA EOL
		{
			if (top == TYPE_EOL )							//pokud je na vrcholu eol tak jsme uspesne zredukovali celej vyraz
			{
				if (stack->top->tokenType == TOKEN_PREC_INTEGER)
				{
					*Type = TYPE_INT;
				}
				else if (stack->top->tokenType == TOKEN_PREC_FLOAT)
				{
					*Type = TYPE_FLOAT64;
				}
				else if (stack->top->tokenType == TOKEN_PREC_STRING)
				{
					*Type = TYPE_STRING;
				}
				else if (stack->top->tokenType == TOKEN_PREC_BOOL)
				{
					*Type = TYPE_BOOL;
				}
				return SYNTAX_OK;
			}
			else if (top == TYPE_LEFT_BRACKET)						//pokud je na vrcholu ( tak nastane chyba
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else									// rekurzivne tocime dokud nenastane stav $E$ nebo nenastane chyba
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, token, Type);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}
			}
		}
		else if (token->type == TYPE_PLUS || token->type == TYPE_MINUS)					// jako token dojde +-
		{
			if (top == TYPE_PLUS || top == TYPE_MINUS || top == TYPE_MULTIPLY || top == TYPE_DIVIDE || top == TYPE_INT || top == TYPE_STRING || top == TYPE_FLOAT64 || top == TYPE_BOOL || top == TYPE_RIGHT_BRACKET) //pokud je top + - * / // i )
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, token, Type);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}
			}
			else
			{
				stackPushOpen(stack);				// jinak posle zacatek rozvoje
				if (token->type == TYPE_PLUS)
				{
					symstackPush(stack,TYPE_PLUS);					// a znak identifikatoru
				}
				else
				{
					symstackPush(stack,TYPE_MINUS);					// a znak identifikatoru
				}
								
			}
		}
		else if (token->type == TYPE_MULTIPLY || token->type == TYPE_DIVIDE)					// jako token dojde * / //
		{
			if (top == TYPE_RIGHT_BRACKET || top == TYPE_INT || top == TYPE_STRING || top == TYPE_FLOAT64 || top == TYPE_MULTIPLY || top == TYPE_DIVIDE)
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, token, Type);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}
			}
			else
			{
				stackPushOpen(stack);
				if (token->type == TYPE_MULTIPLY)
				{
					symstackPush(stack,TYPE_MULTIPLY);
				}
				else if (token->type == TYPE_DIVIDE)
				{
					symstackPush(stack,TYPE_DIVIDE);
				}
			}
		}
		else if (token->type == TYPE_LESSER || token->type == TYPE_LESSER_OR_EQUAL || token->type == TYPE_GREATER || token->type == TYPE_GREATER_OR_EQUAL || token->type == TYPE_EQUALS || token->type == TYPE_NOT_EQUALS)					// jako token dojde < <= > >= == !=	=
		{
			if (top >= TYPE_EQUALS && top <= TYPE_LESSER_OR_EQUAL)
			{
				return SEM_ERROR_OTHER; //nekompatibilni terminaly
			}
			else if (top == TYPE_LEFT_BRACKET || top == TYPE_EOL)
			{
				stackPushOpen(stack);
				if (token->type == TYPE_LESSER)
				{
					symstackPush(stack,TYPE_LESSER);
				}
				else if (token->type == TYPE_LESSER_OR_EQUAL)
				{
					symstackPush(stack,TYPE_LESSER_OR_EQUAL);
				}
				else if (token->type == TYPE_GREATER)
				{
					symstackPush(stack,TYPE_GREATER);
				}
				else if (token->type == TYPE_GREATER_OR_EQUAL)
				{
					symstackPush(stack,TYPE_GREATER_OR_EQUAL);
				}
				else if (token->type == TYPE_EQUALS)
				{
					symstackPush(stack,TYPE_EQUALS);
				}
				else if (token->type == TYPE_NOT_EQUALS)
				{
					symstackPush(stack,TYPE_NOT_EQUALS);
				}
			}
			else if (token->type == TYPE_AND || token->type == TYPE_OR || token->type == TYPE_NOT)
			{

			}
			else
			{

				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >

				reduction(stack);					// proved redukci
				int b = idkfunkce(stack, token, Type);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci   (E ==

				if (b != SYNTAX_OK)
				{
					return SEM_ERROR_OTHER;
				}	
			}
		}

		return SYNTAX_OK;	
}

TokenType StackTopTerm (symStack *stack){

	symStack *temp = malloc(sizeof(symStack)); 
	temp->top = stack->top;
	TokenType tokenhelp;
	if (temp->top == NULL)
	{
		return TOKEN_ERROR;
	}
	else
	{
		tokenhelp = stack->top->tokenType;
		while(tokenhelp => Type_EOF && tokenhelp < TYPE_ASSIGN)
		{
	    	symStackItem* out = temp->top;
	    	if (out->next != NULL)
	    	{
	    	  	temp->top = out->next;
	    		tokenhelp = symstackTop(temp);
	    	}
	    	else
	    	{
	    		return TOKEN_ERROR;
	    	}
		}

		if (tokenhelp > TYPE_NOT)
		{
			return tokenhelp;
		}
		else
		{
			return TOKEN_ERROR;
		}
	}
}

int stackPushOpen(symStack *stack){
	if(StackTopTerm(stack) == symstackTop(stack))
	{

		symstackPush(stack,TOKEN_PREC_OPEN);
		return 0;

	}else
	{

		symStackItem* temp = NULL;
		symStackItem* prev = NULL;
		symStackItem* nitem = (symStackItem*)malloc(sizeof(symStackItem));
		TokenType topterm=StackTopTerm(stack);
		TokenType help=symstackTop(stack);

		temp = stack->top;
		while(topterm != help)
		{

			
			help = temp->tokenType;

			if (topterm == help)
			{
				nitem->tokenType = TOKEN_PREC_OPEN;
  				nitem->next = temp;
    			prev->next = nitem;
    			return 0;

			}else
			{

				prev = temp;
				temp = prev->next;

			}
		}
	}
	return 0;
}


symStackItem* stackPosition(symStack *stack, int j){

	symStackItem* temp = stack->top;

	for (int i = 0; i < j; ++i)
	{
		temp = temp->next;
	}

	return temp;
}

int reduction(symStack *stack){
	symStackItem* temp = stack->top;
	TokenType topterm=StackTopTerm(stack);
	TokenType help=symstackTop(stack);
	int i=1;


	while(temp->tokenType != 26 || i >10) // zjisteni kolik zaznamu se nachazi v redukci pr < id + id > => 5
	{
		i++;
		temp = temp->next;
	}



	if(topterm == TYPE_INT || topterm == TYPE_FLOAT64 || topterm == TYPE_STRING) // nutno dodelat nevim jak na to potreba roylisit lokalni a globalni promenou
	{/*

		if (i == 3)
		{
			symstackPop(stack);
			help=symstackTop(stack);
			symStackItem* temp2 = malloc(sizeof(symStackItem));
			if(stack->top->nazev != NULL){
				if(in_function2){
					if(symtableSearch(local, stack->top->nazev) != NULL)
						printf("PUSHS LF@%s\n", stack->top->nazev);
					else if(symtableSearch(global, stack->top->nazev) != NULL)
						printf("PUSHS GF@%s\n", stack->top->nazev);
					else
						return SEM_ERROR_DEF;
				}else{
					printf("PUSHS GF@%s\n", stack->top->nazev);
				}
			}
			if (help == TYPE_INT)
			{
				if(stack->top->nazev == NULL)
					printf("PUSHS int@%d\n", stack->top->inte);
				temp2->inte = stack->top->inte;
				symstackPopMore(stack, 2);
				symstackPush(stack,TOKEN_PREC_INTEGER);
				stack->top->inte = temp2->inte;
				
			}
			else if (help == TYPE_FLOAT64)
			{
				if(stack->top->nazev == NULL)
					printf("PUSHS float@%a\n", stack->top->flt);
				stack->top->flt = temp2->flt;
				symstackPopMore(stack, 2);
				symstackPush(stack,TOKEN_PREC_FLOAT);
				stack->top->flt = temp2->flt;
			}
			else if (help ==TYPE_STRING)
			{
				if(stack->top->nazev == NULL)
					printf("PUSHS string@%s\n", stack->top->string->string);
				temp2->string = stack->top->string;
				symstackPopMore(stack, 2);
				symstackPush(stack,TOKEN_PREC_STRING);
				stack->top->string = temp2->string;
			}
			else
			{
				free(temp2);
				return SYNTAX_ERROR;	
			}
			free(temp2);
			
		}
		else
		{

			return SYNTAX_ERROR;
		}*/
	}
	else if (topterm == TYPE_RIGHT_BRACKET) // hotovo provede  redukci zavorek
	{

		if (i==5)
		{
			symStackItem* prvni = stackPosition(stack, 0);
			symStackItem* druhej = stackPosition(stack, 1);
			symStackItem* treti = stackPosition(stack, 2);
			symStackItem* ctvrtej = stackPosition(stack, 3);
			symStackItem* patej = stackPosition(stack, 4);
			symStackItem* temp2 = malloc(sizeof(symStackItem));;

			if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && druhej->tokenType == TYPE_RIGHT_BRACKET && ctvrtej->tokenType == TYPE_LEFT_BRACKET)
			{
				temp2->tokenType = treti->tokenType;
				temp2->inte = treti->inte;
				temp2->flt = treti->flt;
				temp2->string = treti->string;
				symstackPopMore(stack, 5);
				symstackPush(stack, temp2->tokenType);
				stack->top->inte = temp2->inte;
				stack->top->flt = temp2->flt;
				stack->top->string = temp2->string;
				free(temp2);
				return 0;
			}
			else
			{
				free(temp2);
				return SYNTAX_ERROR;
			}

		}
		else
		{
			return SYNTAX_ERROR;
		}
	}
	else if (topterm == TYPE_LEFT_BRACKET || topterm == TYPE_EOL) //hotovo vyhodi chybu protoze to je zakazany stav
	{

		return SYNTAX_ERROR;
	}
	else if (topterm >= TYPE_PLUS && topterm <= TYPE_NOT_EQUALS)
	{
		if (topterm == TYPE_PLUS)
		{
			if (i == 5)
			{	
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);


				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TYPE_PLUS)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 420;
						printf("ADDS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{

						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 420.0;
						printf("ADDS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_STRING && ctvrtej->tokenType ==TOKEN_PREC_STRING)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_STRING);
						printf("POPS GF@?AX?\n");
						printf("POPS GF@?BX?\n");
						printf("CONCAT GF@?CX? GF@?BX? GF@?AX?\n");
						printf("PUSHS GF@?CX?\n");
					}
					else
					{
						return SYNTAX_ERROR;
					}
				}
				else
				{
					return SYNTAX_ERROR;
				}

			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TYPE_MINUS)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TYPE_MINUS)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 420;
						printf("SUBS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 42.0;
						printf("SUBS\n");
					}
					else
					{
						return SEM_ERROR_TYPE;
					}
				}
				else
				{
					return SYNTAX_ERROR;
				}

			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TYPE_DIVIDE)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TYPE_DIVIDE)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && druhej->inte == 0 )
					{
						return ZERO_DIVISION;
					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && druhej->flt == 0)
					{
						return ZERO_DIVISION;
					}

					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT); 
						stack->top->flt = 42.0;
						printf("INT2FLOATS\n");
						printf("POPS GF@?AX?\n");
						printf("INT2FLOATS\n");
						printf("PUSHS GF@?AX?\n");
						printf("DIVS\n");



					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT); 
						stack->top->flt = 42.0;
						printf("DIVS\n");

					}
					else
					{
						return SEM_ERROR_TYPE;	
					}
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TYPE_MULTIPLY)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TYPE_MULTIPLY)
				{
					if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 420;
						printf("MULS\n");

					}
					else if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 42.0;
						printf("MULS\n");
					}
					else
					{
						return SEM_ERROR_TYPE;
					}

				}
				else
				{
					return SYNTAX_ERROR;
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TYPE_GREATER_OR_EQUAL)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TYPE_GREATER_OR_EQUAL)
				{
					if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("POPS GF@?BX?\n");
						printf("GT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else if (druhej->tokenType== TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("POPS GF@?BX?\n");
						printf("GT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else if (druhej->tokenType== TOKEN_PREC_STRING && ctvrtej->tokenType == TOKEN_PREC_STRING )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("POPS GF@?BX?\n");
						printf("GT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else 
					{
						return SEM_ERROR_TYPE;
					}

				}
				else
				{
					return SYNTAX_ERROR;
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TYPE_GREATER)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TYPE_GREATER)
				{
					if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("GTS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("GTS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_STRING && ctvrtej->tokenType == TOKEN_PREC_STRING )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("GTS\n");
					}
					else 
					{
						return SEM_ERROR_TYPE;
					}
				}
				else
				{
					return SYNTAX_ERROR;
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TYPE_LESSER_OR_EQUAL)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TYPE_LESSER_OR_EQUAL)
				{
					if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("POPS GF@?BX?\n");
						printf("LT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("POPS GF@?BX?\n");
						printf("LT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_STRING && ctvrtej->tokenType == TOKEN_PREC_STRING )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("POPS GF@?AX?\n");
						printf("POPS GF@?BX?\n");
						printf("LT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else 
					{
						return SEM_ERROR_TYPE;
					}
				}
				else
				{
					return SYNTAX_ERROR;
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TYPE_LESSER)
		{
			
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TYPE_LESSER)
				{
					if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("LTS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("LTS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_STRING && ctvrtej->tokenType == TOKEN_PREC_STRING )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("LTS\n");
					}
					else 
					{
						
						return SEM_ERROR_TYPE;
					}
				}
				else
				{
					
					return SYNTAX_ERROR;
				}
			}
			else
			{
				
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TYPE_EQUALS)
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TYPE_EQUALS)
				{
					if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_STRING && ctvrtej->tokenType == TOKEN_PREC_STRING )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_BOOL && ctvrtej->tokenType == TOKEN_PREC_BOOL)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
					}
					else
					{
						return SEM_ERROR_TYPE;
					}
				}
				else
				{
					return SYNTAX_ERROR;
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else if (topterm == TYPE_NOT_EQUALS)	//done
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->tokenType == TOKEN_PREC_CLOSE && patej->tokenType == TOKEN_PREC_OPEN && treti->tokenType == TYPE_NOT_EQUALS)
				{
					if (druhej->tokenType == TOKEN_PREC_FLOAT && ctvrtej->tokenType == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
						printf("NOTS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_INTEGER && ctvrtej->tokenType == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
						printf("NOTS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_STRING && ctvrtej->tokenType == TOKEN_PREC_STRING )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
						printf("NOTS\n");
					}
					else if (druhej->tokenType == TOKEN_PREC_BOOL && ctvrtej->tokenType == TOKEN_PREC_BOOL )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
						printf("NOTS\n");
					}
					else 
					{
						return SEM_ERROR_TYPE;
					}
				}
				else
				{
					return SYNTAX_ERROR;
				}
			}
			else
			{
				return SYNTAX_ERROR;
			}
		}
		else
		{
			return SYNTAX_ERROR;
		}
	}
	else
	{
		return SYNTAX_ERROR;
	}
	return 0;
}

    

