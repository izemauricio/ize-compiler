%{
#include "y.tab.h"

#include "SymTab.h"
#include "IOMngr.h"

#define YY_INPUT(buf,result,max_size) { int c = GetSourceChar(); result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); }
%}

letra [A-Za-z]
numero [0-9]

%%
true                            { return _TRUE;}
false                           { return _FALSE; }
int				{ return _INT; }
boolean                         { return _BOOL; }
function                        { return _FUNCTION; }
return                          { return _RETURN; }
call                            { return _CALL; }
if				{ return _IF; }
else                            { return _ELSE; };
while                           { return _WHILE; };
print				{ return _PRINT; }
read                            { return _READ; }
printstr                        { return _PRINTSTR; };
println                         { return _PRINTLN; };
printsp                         { return _PRINTSP; };

{letra}({letra}|{numero})*	{ return _VARNAME; }
{numero}{numero}*		{ return _NUMERO; }
\"((.)*)\"                      { return _TEXT; }

\=\=				{ return _IGUAL; }
\!\=				{ return _DIFER; }
\<				{ return _MENOR; }
\>				{ return _MAIOR; }
\>\=				{ return _MAIORIGUAL; }
\<\=				{ return _MENORIGUAL; }
\&\&                            { return _AND; }
\|\|                            { return _OR; }
\!                              { return '!'; }

\=      			{ return '='; }
\+				{ return '+'; }
\-				{ return '-'; }
\*				{ return '*'; }
\/                              { return '/'; }
\%                              { return '%'; }
\^                              { return '^'; }

\[				{ return '['; }
\]				{ return ']'; }
\,                              { return ','; }
\;				{ return ';'; }
\{				{ return '{'; }
\}				{ return '}'; }
\(				{ return '('; }
\)				{ return ')'; }

[ ]				{  }
\n				{  }
\t				{  }
\r				{  }

.				{ WriteIndicator(GetCurrentColumn()); WriteMessage("Illegal char in lex! >>"); }
%%

yywrap () {
}