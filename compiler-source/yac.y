%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SymTab.h"
#include "IOMngr.h"
#include "Semantics.h"
#include "CodeGen.h"

extern int yylex();
extern char * yytext;
extern int yyleng;
extern int yyparse();

extern struct SymTab * table;
extern struct SymEntry * entry;

void dumpTable();
%}

%union {
    int INT;
    char * STRING;
    
    struct ExprRes * ExprRes;
    struct ExprResList * ExprResList;  
    struct InstrSeq * InstrSeq;
    
    struct TextRes * TextRes;
}

%type <INT>               NUMERO
%type <STRING>            VARNAME
%type <TextRes>           TEXTO
%type <ExprRes>           BOOL
%type <ExprRes>           LOGICAL
%type <ExprRes>           EXPR
%type <ExprRes>           TERMO
%type <ExprRes>           EXPOENTE
%type <ExprRes>           FATOR
%type <ExprResList>       VARLIST
%type <InstrSeq>          PEDACO
%type <InstrSeq>          BODY

%token _VARNAME 		
%token _NUMERO
%token _TEXT
%token _CALL
%token _PRINTSTR
%token _RETURN	
%token _FUNCTION
%token _INT
%token _PRINT
%token _READ
%token _IF
%token _ELSE
%token _WHILE
%token _IGUAL
%token _DIFER
%token _MENOR
%token _MAIOR
%token _MAIORIGUAL
%token _MENORIGUAL
%token _BOOL
%token _AND
%token _OR
%token _TRUE
%token _FALSE
%token _PRINTLN
%token _PRINTSP

%%
PROGRAMA               : DECLARATION BODY                                       { FINISH($2); };

DECLARATION            : DEC DECLARATION                                        {};
DECLARATION            :                                                        {};

DEC                    : _FUNCTION VARNAME '(' ')' '{' BODY '}'                 { createFUNCTION($2,$6); }
DEC                    : _INT VARNAME ';'                                       { createVarINT($2); };
DEC                    : _INT VARNAME '[' NUMERO ']' ';'                        { createArrayINT($2,$4); };
DEC                    : _BOOL VARNAME ';'                                      { createVarBOOLEAN($2); };
DEC                    : _BOOL VARNAME '[' NUMERO ']' ';'                       { createArrayBOOLEAN($2,$4); };

BODY                   : PEDACO BODY                                            { $$ = AppendSeq($1, $2);  };
BODY                   :                                                        { $$ = NULL;  };

PEDACO                 : _IF '(' BOOL ')' '{' BODY '}'                          { $$ = doIf($3, $6); };
PEDACO                 : _IF '(' BOOL ')' '{' BODY '}' _ELSE '{' BODY '}'       { $$ = doIfElse($3, $6, $10); };
PEDACO                 : _WHILE '(' BOOL ')' '{' BODY '}'                       { $$ = doWhile($3,$6); };

PEDACO                 : _RETURN '(' BOOL ')' ';'                               { $$ =  doReturnFUNCTION($3); };

PEDACO                 : _READ '(' VARLIST ')' ';'                              { $$ = doReadList($3); };
PEDACO                 : _PRINT '(' VARLIST ')' ';'                             { $$ = doPrintList($3); };
PEDACO                 : _PRINTSTR '(' TEXTO ')' ';'                            { $$ = doPrintstr($3); };
PEDACO                 : _PRINTSP '(' BOOL ')' ';'                              { $$ = doPrintsp($3); };
PEDACO                 : _PRINTLN '(' ')' ';'                                   { $$ = doPrintln(); };

PEDACO                 : VARNAME '=' BOOL ';'                                   { $$ = doAssignVAR($1, $3); };
PEDACO                 : VARNAME '[' BOOL ']' '=' BOOL ';'                      { $$ = doAssignARRAY($1, $3, $6); };

PEDACO                 : _CALL VARNAME ';'                                      { $$ = doCallFUNCTION($2); };

VARLIST                : VARLIST ',' BOOL                                       { $$ = appendVarIntoVARLIST($1,$3); };
VARLIST                : BOOL                                                   { $$ = addVarIntoVARLIST($1); };
BOOL                   : BOOL _IGUAL LOGICAL                                    { $$ = doIGUAL($1, $3); };
BOOL                   : BOOL _DIFER LOGICAL                                    { $$ = doDIFER($1, $3); };
BOOL                   : BOOL _MENOR LOGICAL                                    { $$ = doMENOR($1, $3); };
BOOL                   : BOOL _MAIOR LOGICAL                                    { $$ = doMAIOR($1, $3); };
BOOL                   : BOOL _MAIORIGUAL LOGICAL                               { $$ = doMAIORIGUAL($1, $3); };
BOOL                   : BOOL _MENORIGUAL LOGICAL                               { $$ = doMENORIGUAL($1, $3); };
BOOL                   : LOGICAL                                                { $$ = $1; };
LOGICAL                : LOGICAL _AND EXPR                                      { $$ = doAND($1, $3); };
LOGICAL                : LOGICAL _OR EXPR                                       { $$ = doOR($1, $3); };
LOGICAL                : EXPR                                                   { $$ = $1; };
EXPR                   : EXPR '+' TERMO                                         { $$ = doAdd($1, $3);  };
EXPR                   : EXPR '-' TERMO                                         { $$ = doSub($1, $3); };
EXPR                   : TERMO                                                  { $$ = $1;  };
TERMO                  : TERMO '*' EXPOENTE                                     { $$ = doMul($1, $3);  };
TERMO                  : TERMO '/' EXPOENTE                                     { $$ = doDiv($1, $3);  };
TERMO                  : TERMO '%' EXPOENTE                                     { $$ = doMod($1, $3);   };
TERMO                  : EXPOENTE                                               { $$ = $1; };
EXPOENTE               : EXPOENTE '^' FATOR                                     { $$ = doExpoente($1,$3); };
EXPOENTE               : FATOR                                                  { $$ = $1; };
FATOR                  : '(' BOOL ')'                                           { $$ = $2; };
FATOR                  : '-' FATOR                                              { $$ = doInv($2); };
FATOR                  : '!' FATOR                                              { $$ = doNot($2); };

FATOR                  : VARNAME '(' ')'                                        { $$ = doRightValueFUNCTION($1); };
FATOR                  : VARNAME '[' BOOL ']'                                   { $$ = doRightValueARRAY($1,$3); };
FATOR                  : VARNAME                                                { $$ = doRightValueVAR($1); };
FATOR                  : _TRUE                                                  { $$ = createNUMERO("1",3); };
FATOR                  : _FALSE                                                 { $$ = createNUMERO("0",3); };
FATOR                  : NUMERO                                                 { $$ = createNUMERO(yytext,1); };
TEXTO                  : _TEXT                                                  { $$ = createTEXTO(yytext); }
NUMERO                 : _NUMERO                                                { $$ = atoi(yytext); };
VARNAME                : _VARNAME                                               { $$ = strdup(yytext); };
%%

yyerror(char *s)  {
    printf("TOKEN=%s\n",yytext);
    WriteIndicator(GetCurrentColumn());
    WriteMessage("Illegal char in yacc!");
}
