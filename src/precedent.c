/**
 * @file precedent.c
 * @author Jan Pospisil (xpospi94@vutbr.cz)
 * @brief Precedent & part semantic implementation
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "symstack.h"
#include "precedent.h"
#include "str.h"



int Translate_string(string *s){
	char pismenko;

	int len = strGetLength(s);

	for (int i = 0; i < len; ++i)
	{
		pismenko = s->str[i];
		if (pismenko<33 || pismenko == 35 || pismenko == 92)
		{
			if (pismenko<10)
			{
				printf("\\00%d",  pismenko);
			}else
			{
				printf("\\0%d",  pismenko);
			}
			
		}
		else
		{
			printf("%c", pismenko);
		}
		

	}
	printf("\n");
	return 0;
}

////{ { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { { {



int precedent_analys(Token* tokeng, TokenType* Type, stStack* Vars){			// if(porovnej(a,b))
	symStack *stack = malloc(sizeof(symStack)); 	
	symstackInit(stack); 	
	symstackPush(stack, TYPE_EOL);	
	int a;							// pushnuti prvni hodnoty EOL
	int f;

	if (tokeng->type !=2)
	{	
		if((a = idkfunkce(stack, tokeng, Type, Vars))){
		return a;	
		}
	}
	if (tokeng->type == TYPE_SEMICOLON || tokeng->type == TYPE_LEFT_CURLY_BRACKET || tokeng->type == TYPE_EOL || tokeng->type == TYPE_COMMA)
	{
		return 2;
	}
	

	while(!(StackTopTerm(stack) == TYPE_EOL && (tokeng->type == TYPE_EOL || tokeng->type == TYPE_SEMICOLON || tokeng->type == TYPE_COMMA || tokeng->type == TYPE_LEFT_CURLY_BRACKET)))							// cyklus pobezi dokud nenarazi na znak konce radku, streniku, nebo carky
	{

		f = getToken(tokeng);
		if (f == 1)
		{
			return f;
		}
		else if (f == 2 || f == 99)
		{
			return INTERNAL_ERROR;
		}					
		if (tokeng->type !=2)
		{	

			a = idkfunkce(stack, tokeng, Type, Vars);				// zavolame funkci pro zpracovani token

			if (a != SYNTAX_OK){
				printf("%d\n", a);
				return a;
			}
		}
		
	}

	return SYNTAX_OK;
}

int idkfunkce(symStack *stack, Token* token, TokenType* Type, stStack* Vars){
		TokenType top = StackTopTerm(stack);						// do top nahrajeme nejvyssi terminal ze stacku



		
		
 

		if (token->type == TYPE_IDENTIFIER || token->type == TYPE_INT || token->type == TYPE_STRING  || token->type == TYPE_FLOAT64 || token->type == TYPE_BOOL)							//TODO je potreba zjitit co vse jsou identifikatory
		{ 

			if (top == TYPE_RIGHT_BRACKET || top == TYPE_IDENTIFIER || top == TYPE_FLOAT64 || top == TYPE_INT || top == TYPE_STRING || top == TYPE_BOOL)				//pokud je na vrcholu ) nebo identifikator, tak se vypise chyba, protoze nejsou kompatibilni
			{
				
				return 2;				//nekompatibilni terminaly
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
					strCopyString(&stack->top->string, &token->attribute.string);			
				}
				else if (token->type == TYPE_BOOL)
				{
					stackPushOpen(stack);				// jinak posle zacatek rozvoje
					symstackPush(stack,TYPE_BOOL);				// a znak identifikatoru
					stack->top->boolen = token->attribute.boolean;						
				}
				else if (token->type == TYPE_FLOAT64)
				{
					stackPushOpen(stack);				// jinak posle zacatek rozvoje
					symstackPush(stack,TYPE_FLOAT64);				// a znak identifikatoru
					stack->top->flt = token->attribute.float64;					
				}
				else if (token->type == TYPE_IDENTIFIER) //musi se predelat 
				{
					
					stVarType help2;
					help2 = stVarTypeLookUp(Vars, strGetStr(&token->attribute.string));

					if (help2 == INT)
					{
						stackPushOpen(stack);				
						symstackPush(stack, TYPE_INT);				
						stack->top->inte = token->attribute.integer;
						strInit(&stack->top->nazev); 
						strCopyString(&stack->top->nazev, &token->attribute.string);

					}
					else if (help2 == STRING)
					{
						stackPushOpen(stack);				
						symstackPush(stack, TYPE_STRING);			
						strCopyString(&stack->top->string, &token->attribute.string);
						strInit(&stack->top->nazev); 
						strCopyString(&stack->top->nazev, &token->attribute.string);
					}
					else if (help2 == FLOAT64)
					{
						stackPushOpen(stack);				
						symstackPush(stack, TYPE_FLOAT64);				
						stack->top->flt = token->attribute.float64;
						strInit(&stack->top->nazev); 
						strCopyString(&stack->top->nazev, &token->attribute.string);
					}
					else if (help2 == BOOL)
					{
						stackPushOpen(stack);				
						symstackPush(stack, TYPE_BOOL);				
						stack->top->boolen = token->attribute.boolean;
						strInit(&stack->top->nazev); 
						strCopyString(&stack->top->nazev, &token->attribute.string);
					}
					else
					{
						return 3;
					}
				}
			}
		}
		else if (token->type == TYPE_LEFT_BRACKET)		// jako token dojde (
		{
			if (top == TYPE_RIGHT_BRACKET || top == TYPE_IDENTIFIER || top == TYPE_INT || top == TYPE_STRING || top == TYPE_FLOAT64 || top == TYPE_BOOL)				//pokud je na vrcholu ) nebo identifikator, tak se vypise chyba, protoze nejsou kompatibilni
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
			if (top == TYPE_EOL && precRightBrace == true)							//pokud je na vrcholu EOL, tak se vypise chyba, protoze nejsou kompatibilni
			{
				precRightBrace = false;			//nekompatibilni terminaly
			}
			else if (top == TYPE_EOL || top == TYPE_SEMICOLON)							//pokud je na vrcholu EOL, tak se vypise chyba, protoze nejsou kompatibilni
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
			}
			else if (top == TYPE_LEFT_BRACKET )						//pokud na na vrchu (
			{
				symstackPush(stack,TYPE_RIGHT_BRACKET);					// pushni )
				symstackPush(stack,TOKEN_PREC_CLOSE);				// pushni konec rozvoje >
				int c = reduction(stack);					// a proved redukci
				if (c != SYNTAX_OK)
				{
					return c;
				}	
			}
			else									//pokud je cokoliv jineho
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				int c = reduction(stack);
				if (c != SYNTAX_OK)
				{
					return c;
				}					// proved redukci
				int b = idkfunkce(stack, token, Type, Vars);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return b;
				}
			}
		}
		else if (token->type == TYPE_EOL || token->type == TYPE_SEMICOLON || token->type == TYPE_COMMA || token->type == TYPE_LEFT_CURLY_BRACKET)			// jako token dojde EOL TODO ZEPTAT SE NA EOL
		{	
			
			if (top == TYPE_EOL )							//pokud je na vrcholu eol tak jsme uspesne zredukovali celej vyraz
			{
				

				if (stack->top->token_Type == TOKEN_PREC_INTEGER)
				{
					*Type = TYPE_INT;
					return SYNTAX_OK;
				}
				else if (stack->top->token_Type == TOKEN_PREC_FLOAT)
				{
					*Type = TYPE_FLOAT64;
					return SYNTAX_OK;
				}
				else if (stack->top->token_Type == TOKEN_PREC_STRING)
				{
					*Type = TYPE_STRING;
					return SYNTAX_OK;
				}
				else if (stack->top->token_Type == TOKEN_PREC_BOOL)
				{
					*Type = TYPE_BOOL;
					return SYNTAX_OK;
				}
				
				return SYNTAX_OK;
			}
			else if (top == TYPE_LEFT_BRACKET)						//pokud je na vrcholu ( tak nastane chyba
			{
				return 2;				//nekompatibilni terminaly				
			}
			else									// rekurzivne tocime dokud nenastane stav $E$ nebo nenastane chyba
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				int c = reduction(stack);					// proved redukci
				if (c != SYNTAX_OK)
				{
					return c;
				}	
				int b = idkfunkce(stack, token, Type, Vars);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return b;
				}
			}
		}
		else if (token->type == TYPE_PLUS || token->type == TYPE_MINUS)					// jako token dojde +-
		{
			if (top == TYPE_PLUS || top == TYPE_MINUS || top == TYPE_MULTIPLY || top == TYPE_DIVIDE || top == TYPE_INT || top == TYPE_STRING || top == TYPE_FLOAT64 || top == TYPE_BOOL || top == TYPE_RIGHT_BRACKET) //pokud je top + - * / // i )
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				int c = reduction(stack);					// proved redukci
				if (c != SYNTAX_OK)
				{
					return c;
				}

				int b = idkfunkce(stack, token, Type, Vars);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return b;
				}
			}
			else if (top == TYPE_NOT)
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
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
			if (top == TYPE_RIGHT_BRACKET || top == TYPE_INT || top == TYPE_STRING || top == TYPE_FLOAT64 || top == TYPE_BOOL || top == TYPE_MULTIPLY || top == TYPE_DIVIDE)
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >
				int c = reduction(stack);					// proved redukci
				if (c != SYNTAX_OK)
				{
					return c;
				}	

				int b = idkfunkce(stack, token, Type, Vars);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci
				if (b != SYNTAX_OK)
				{
					return b;
				}
			}
			else if (top == TYPE_NOT)
			{
				return SEM_ERROR_OTHER;				//nekompatibilni terminaly
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
			else if (top == TYPE_LEFT_BRACKET || top == TYPE_EOL || top == TYPE_AND || top == TYPE_OR)
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
			else if (top == TYPE_NOT)
			{
				if (token->type == TYPE_EQUALS || token->type == TYPE_NOT_EQUALS)
				{
					symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >

					int c = reduction(stack);					// proved redukci
					if (c != SYNTAX_OK)
				{
					return c;
				}	

					int b = idkfunkce(stack, token, Type, Vars);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci   (E ==

					if (b != SYNTAX_OK)
					{
						return b;
					}	
				}
				else
				{
					return SEM_ERROR_OTHER;
				}
			}
			else
			{

				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >

				int c = reduction(stack);					// proved redukci
				if (c != SYNTAX_OK)
				{
					return c;
				}	

				int b = idkfunkce(stack, token, Type, Vars);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci   (E ==

				if (b != SYNTAX_OK)
				{
					return b;
				}	
			}
		}
		else if (token->type == TYPE_AND || token->type == TYPE_OR)
		{
			if (top == TYPE_LEFT_BRACKET || top == TYPE_EOL)
			{
				stackPushOpen(stack);
				if (token->type == TYPE_AND)
				{
					symstackPush(stack,TYPE_AND);
				}
				else if (token->type == TYPE_OR)
				{
					symstackPush(stack,TYPE_OR);
				}
			}
			else
			{
				symstackPush(stack,TOKEN_PREC_CLOSE);				//pushni konec rozvoje >

				int c = reduction(stack);					// proved redukci
				if (c != SYNTAX_OK)
				{
					return c;
				}	

				int b = idkfunkce(stack, token, Type, Vars);		// zavola sebe sama rekurzivne pro vyhodnoceni dalsich redukci   (E ==

				if (b != SYNTAX_OK)
				{
					return b;
				}
			}
		}
		else if (token->type == TYPE_NOT)
		{
			if (top == TYPE_EQUALS || top == TYPE_NOT_EQUALS || top == TYPE_EOL || top == TYPE_LEFT_BRACKET || top == TYPE_AND || top == TYPE_OR || top == TYPE_NOT)
			{
				stackPushOpen(stack);
				symstackPush(stack,TYPE_NOT);
			}
			else
			{
				return SEM_ERROR_OTHER;
			}
		}
		else
		{
			return 2;
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
		tokenhelp = stack->top->token_Type;
		while(tokenhelp > TYPE_SEMICOLON)
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


		if (tokenhelp < TYPE_ASSIGN)
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

			help = temp->token_Type;
			if (topterm == help)
			{
				nitem->token_Type = TOKEN_PREC_OPEN;
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
	int i=1;


	while(temp->token_Type != TOKEN_PREC_OPEN || i >10) // zjisteni kolik zaznamu se nachazi v redukci pr < id + id > => 5
	{
		i++;
		temp = temp->next;
	}



	if(topterm == TYPE_INT || topterm == TYPE_FLOAT64 || topterm == TYPE_STRING || topterm == TYPE_BOOL) // pokud to je ID
	{

		if (i == 3)
		{
			symStackItem* prvni = stackPosition(stack, 0);
			symStackItem* druhej = stackPosition(stack, 1);
			symStackItem* treti = stackPosition(stack, 2);

			if (prvni->token_Type == TOKEN_PREC_CLOSE && treti->token_Type == TOKEN_PREC_OPEN)	//kontrola ze se provadi redukce spravne
			{
				if(strGetLength(&druhej->nazev) != 0)		//pokud neni NULL znamena to ze to je promnnena 
				{
					if (druhej->token_Type == TYPE_INT)
					{
						printf("PUSHS LF@%s\n", druhej->nazev.str);
						symstackPopMore(stack, 3);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 420;						//nahodna hodnota aby hodnota nebyla 0 dela to pak bordel pri deleni						
					}
					if (druhej->token_Type == TYPE_FLOAT64)
					{
						printf("PUSHS LF@%s\n", druhej->nazev.str);
						symstackPopMore(stack, 3);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->inte = 420.0;						//nahodna hodnota aby hodnota nebyla 0 dela to pak bordel pri deleni
					}
					if (druhej->token_Type == TYPE_STRING)
					{
						printf("PUSHS LF@%s\n", druhej->nazev.str);
						symstackPopMore(stack, 3);
						symstackPush(stack, TOKEN_PREC_STRING);
					}
					if (druhej->token_Type == TYPE_BOOL)
					{
						printf("PUSHS LF@%s\n", druhej->nazev.str);
						symstackPopMore(stack, 3);
						symstackPush(stack, TOKEN_PREC_BOOL);					}
				}
				else if (strGetLength(&druhej->nazev) == 0)	//pokud je null znamena to ze to neni promnenna
				{
					if (druhej->token_Type == TYPE_INT)
					{
						if (druhej->inte == 0)	// zjistime jestli je hodnota 0
						{
							printf("PUSHS int@%d\n", druhej->inte);
							symstackPopMore(stack, 3);
							symstackPush(stack, TOKEN_PREC_INTEGER);
							stack->top->inte = 0;						// pokud je tak ji posleme dal kvuli deleni
						}
						else if (druhej->inte != 0)
						{
							printf("PUSHS int@%d\n", druhej->inte);
							symstackPopMore(stack, 3);
							symstackPush(stack, TOKEN_PREC_INTEGER);
							stack->top->inte = 420;						//pokud to neni nula tak na hodnote jiz dale nezalezi a pushneme nahodne cislo, pokud vas zajima proc to delam takle, tak protoze to je jednodussi jak definovat pomocnou promenou abych to cislo actualy prevedl 
						}
						
					}
					if (druhej->token_Type == TYPE_FLOAT64)
					{
						if (druhej->flt == 0.0)
						{
							printf("PUSHS float@%a\n", druhej->flt);
							symstackPopMore(stack, 3);
							symstackPush(stack, TOKEN_PREC_FLOAT);
							stack->top->inte = 0;						// pokud je tak ji posleme dal kvuli deleni
						}
						else if (druhej->flt != 0.0)
						{
							printf("PUSHS float@%a\n", druhej->flt);
							symstackPopMore(stack, 3);
							symstackPush(stack, TOKEN_PREC_FLOAT);
							stack->top->inte = 420;						//pokud to neni nula tak na hodnote jiz dale nezalezi a pushneme nahodne cislo, pokud vas zajima proc to delam takle, tak protoze to je jednodussi jak definovat pomocnou promenou abych to cislo actualy prevedl 
						}
					}
					if (druhej->token_Type == TYPE_STRING)
					{
						printf("PUSHS string@");
						Translate_string(&druhej->string);
						symstackPopMore(stack, 3);
						symstackPush(stack, TOKEN_PREC_STRING);
					}
					if (druhej->token_Type == TYPE_BOOL)
					{
						if (druhej->boolen== true)
						{
							printf("PUSHS bool@true\n");
						}
						else if (druhej->boolen== false)
						{
							printf("PUSHS bool@false\n");
						}
						symstackPopMore(stack, 3);
						symstackPush(stack, TOKEN_PREC_BOOL);
					}
				}
			}

			
		}
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

			if (prvni->token_Type == TOKEN_PREC_CLOSE && patej->token_Type == TOKEN_PREC_OPEN && druhej->token_Type == TYPE_RIGHT_BRACKET && ctvrtej->token_Type == TYPE_LEFT_BRACKET)
			{
				temp2->token_Type = treti->token_Type;
				temp2->inte = treti->inte;
				temp2->flt = treti->flt;
				temp2->string = treti->string;
				symstackPopMore(stack, 5);
				symstackPush(stack, temp2->token_Type);
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
	else if (topterm >= TYPE_PLUS && topterm <= TYPE_NOT)
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


				if (prvni->token_Type == TOKEN_PREC_CLOSE && patej->token_Type == TOKEN_PREC_OPEN && treti->token_Type == TYPE_PLUS)
				{
					if (druhej->token_Type == TOKEN_PREC_INTEGER && ctvrtej->token_Type == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 420;
						printf("ADDS\n");
					}
					else if (druhej->token_Type == TOKEN_PREC_FLOAT && ctvrtej->token_Type == TOKEN_PREC_FLOAT)
					{

						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 420.0;
						printf("ADDS\n");
					}
					else if (druhej->token_Type == TOKEN_PREC_STRING && ctvrtej->token_Type ==TOKEN_PREC_STRING)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_STRING);
						printf("POPS GF@?BX?\n");
						printf("POPS GF@?AX?\n");
						printf("CONCAT GF@?CX? GF@?BX? GF@?AX?\n");
						printf("PUSHS GF@?CX?\n");
					}
					else
					{
						return SEM_ERROR_TYPECOMP;
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
			
				if (prvni->token_Type == TOKEN_PREC_CLOSE && patej->token_Type == TOKEN_PREC_OPEN && treti->token_Type == TYPE_MINUS)
				{	
					if (druhej->token_Type == TOKEN_PREC_INTEGER && ctvrtej->token_Type == TOKEN_PREC_INTEGER)
					{	
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 420;
						printf("SUBS\n");
					}	
					else if (druhej->token_Type == TOKEN_PREC_FLOAT && ctvrtej->token_Type == TOKEN_PREC_FLOAT)
					{
						
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 42.0;
						printf("SUBS\n");
					}
					else
					{	
						return SEM_ERROR_TYPECOMP;
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

				if (prvni->token_Type == TOKEN_PREC_CLOSE && patej->token_Type == TOKEN_PREC_OPEN && treti->token_Type == TYPE_DIVIDE)
				{
					if (druhej->token_Type == TOKEN_PREC_INTEGER && druhej->inte == 0 )
					{
						return ZERO_DIVISION;
					}
					else if (druhej->token_Type == TOKEN_PREC_FLOAT && druhej->flt == 0.0)
					{
						return ZERO_DIVISION;
					}

					if (druhej->token_Type == TOKEN_PREC_INTEGER && ctvrtej->token_Type == TOKEN_PREC_INTEGER)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER); 
						stack->top->flt = 42;
						printf("IDIVS\n");



					}
					else if (druhej->token_Type == TOKEN_PREC_FLOAT && ctvrtej->token_Type == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT); 
						stack->top->flt = 42.0;
						printf("DIVS\n");

					}
					else
					{
						return SEM_ERROR_TYPECOMP;	
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

				if (prvni->token_Type == TOKEN_PREC_CLOSE && patej->token_Type == TOKEN_PREC_OPEN && treti->token_Type == TYPE_MULTIPLY)
				{
					if (druhej->token_Type == TOKEN_PREC_INTEGER && ctvrtej->token_Type == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_INTEGER);
						stack->top->inte = 420;
						printf("MULS\n");

					}
					else if (druhej->token_Type == TOKEN_PREC_FLOAT && ctvrtej->token_Type == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_FLOAT);
						stack->top->flt = 42.0;
						printf("MULS\n");
					}
					else
					{
						return SEM_ERROR_TYPECOMP;
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

				if (prvni->token_Type == TOKEN_PREC_CLOSE && patej->token_Type == TOKEN_PREC_OPEN && treti->token_Type == TYPE_GREATER_OR_EQUAL)
				{
					if (druhej->token_Type == TOKEN_PREC_FLOAT && ctvrtej->token_Type == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("POPS GF@?BX?\n");
						printf("POPS GF@?AX?\n");
						printf("GT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else if (druhej->token_Type== TOKEN_PREC_INTEGER && ctvrtej->token_Type == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("POPS GF@?BX?\n");
						printf("POPS GF@?AX?\n");
						printf("GT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else if (druhej->token_Type== TOKEN_PREC_STRING && ctvrtej->token_Type == TOKEN_PREC_STRING )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("POPS GF@?BX?\n");
						printf("POPS GF@?AX?\n");
						printf("GT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else 
					{
						return SEM_ERROR_TYPECOMP;
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

				if (prvni->token_Type == TOKEN_PREC_CLOSE && patej->token_Type == TOKEN_PREC_OPEN && treti->token_Type == TYPE_GREATER)
				{
					if (druhej->token_Type == TOKEN_PREC_FLOAT && ctvrtej->token_Type == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("GTS\n");
					}
					else if (druhej->token_Type == TOKEN_PREC_INTEGER && ctvrtej->token_Type == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("GTS\n");
					}
					else if (druhej->token_Type == TOKEN_PREC_STRING && ctvrtej->token_Type == TOKEN_PREC_STRING )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("GTS\n");
					}
					else 
					{
						return SEM_ERROR_TYPECOMP;
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

				if (prvni->token_Type == TOKEN_PREC_CLOSE && patej->token_Type == TOKEN_PREC_OPEN && treti->token_Type == TYPE_LESSER_OR_EQUAL)
				{
					if (druhej->token_Type == TOKEN_PREC_FLOAT && ctvrtej->token_Type == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("POPS GF@?BX?\n");
						printf("POPS GF@?AX?\n");
						printf("LT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else if (druhej->token_Type == TOKEN_PREC_INTEGER && ctvrtej->token_Type == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("POPS GF@?BX?\n");
						printf("POPS GF@?AX?\n");
						printf("LT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else if (druhej->token_Type == TOKEN_PREC_STRING && ctvrtej->token_Type == TOKEN_PREC_STRING )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("POPS GF@?BX?\n");
						printf("POPS GF@?AX?\n");
						printf("LT GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("EQ GF@?CX? GF@?AX? GF@?BX?\n");
						printf("PUSHS GF@?CX?\n");
						printf("ORS\n");
					}
					else 
					{
						return SEM_ERROR_TYPECOMP;
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

				if (prvni->token_Type == TOKEN_PREC_CLOSE && patej->token_Type == TOKEN_PREC_OPEN && treti->token_Type == TYPE_LESSER)
				{
					if (druhej->token_Type == TOKEN_PREC_FLOAT && ctvrtej->token_Type == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("LTS\n");
					}
					else if (druhej->token_Type == TOKEN_PREC_INTEGER && ctvrtej->token_Type == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("LTS\n");
					}
					else if (druhej->token_Type == TOKEN_PREC_STRING && ctvrtej->token_Type == TOKEN_PREC_STRING )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("LTS\n");
					}
					else 
					{
						
						return SEM_ERROR_TYPECOMP;
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

				if (prvni->token_Type == TOKEN_PREC_CLOSE && patej->token_Type == TOKEN_PREC_OPEN && treti->token_Type == TYPE_EQUALS)
				{
					if (druhej->token_Type == TOKEN_PREC_FLOAT && ctvrtej->token_Type == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
					}
					else if (druhej->token_Type == TOKEN_PREC_INTEGER && ctvrtej->token_Type == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
					}
					else if (druhej->token_Type == TOKEN_PREC_STRING && ctvrtej->token_Type == TOKEN_PREC_STRING )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
					}
					else if (druhej->token_Type == TOKEN_PREC_BOOL && ctvrtej->token_Type == TOKEN_PREC_BOOL)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
					}
					else
					{
						return 5;
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

				if (prvni->token_Type == TOKEN_PREC_CLOSE && patej->token_Type == TOKEN_PREC_OPEN && treti->token_Type == TYPE_NOT_EQUALS)
				{
					if (druhej->token_Type == TOKEN_PREC_FLOAT && ctvrtej->token_Type == TOKEN_PREC_FLOAT)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
						printf("NOTS\n");
					}
					else if (druhej->token_Type == TOKEN_PREC_INTEGER && ctvrtej->token_Type == TOKEN_PREC_INTEGER )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
						printf("NOTS\n");
					}
					else if (druhej->token_Type == TOKEN_PREC_STRING && ctvrtej->token_Type == TOKEN_PREC_STRING )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
						printf("NOTS\n");
					}
					else if (druhej->token_Type == TOKEN_PREC_BOOL && ctvrtej->token_Type == TOKEN_PREC_BOOL )
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;
						printf("EQS\n");
						printf("NOTS\n");
					}
					else 
					{
						return SEM_ERROR_TYPECOMP;
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
		else if (topterm == TYPE_AND)	//done
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->token_Type == TOKEN_PREC_CLOSE && patej->token_Type == TOKEN_PREC_OPEN && treti->token_Type == TYPE_AND)
				{
					if (druhej->token_Type == TYPE_BOOL && ctvrtej->token_Type == TYPE_BOOL)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;

						printf("ANDS\n");
					}
					else 
					{
						return SEM_ERROR_TYPECOMP;
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
		else if (topterm == TYPE_OR)	//done
		{
			if (i == 5)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);
				symStackItem* patej = stackPosition(stack, 4);

				if (prvni->token_Type == TOKEN_PREC_CLOSE && patej->token_Type == TOKEN_PREC_OPEN && treti->token_Type == TYPE_OR)
				{
					if (druhej->token_Type == TYPE_BOOL && ctvrtej->token_Type == TYPE_BOOL)
					{
						symstackPopMore(stack, 5);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->inte = 1;

						printf("ORS\n");
					}
					else 
					{
						return SEM_ERROR_TYPECOMP;
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
		else if (topterm == TYPE_NOT)	//done
		{
			if (i == 4)
			{
				symStackItem* prvni = stackPosition(stack, 0);
				symStackItem* druhej = stackPosition(stack, 1);
				symStackItem* treti = stackPosition(stack, 2);
				symStackItem* ctvrtej = stackPosition(stack, 3);

				if (prvni->token_Type == TOKEN_PREC_CLOSE && ctvrtej->token_Type == TOKEN_PREC_OPEN && treti->token_Type == TYPE_NOT)
				{
					if (druhej->token_Type == TOKEN_PREC_BOOL)
					{
						symstackPopMore(stack, 4);
						symstackPush(stack, TOKEN_PREC_BOOL);
						stack->top->boolen = 1;
						printf("NOTS\n");
					}
					else 
					{
						return SEM_ERROR_TYPECOMP;
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

stVarType precTypeToSymtableType (TokenType prec) {
	if (prec == TYPE_INT) {
		return INT;
	}
	if (prec == TYPE_FLOAT64) {
		return FLOAT64;
	}
	if (prec == TYPE_STRING) {
		return STRING;
	}
	if (prec == TYPE_BOOL) {
		return BOOL;
	}
	return UNKNOWN;
}    

