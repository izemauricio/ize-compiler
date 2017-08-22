#include <stdio.h>

#include "SymTab.h"
#include "IOMngr.h"
#include "Semantics.h"
#include "CodeGen.h"

extern int yyparse();

FILE *aFile;

struct SymTab *table;
struct SymTab *ProcSymTab;
struct SymEntry *entry;
int inProc = 0;

int main(int argc, char * argv[]) 
{
    table = CreateSymTab(33);

    OpenFiles(argv[1], argv[2]);

    if (argc == 4)
        aFile = fopen(argv[3], "w");
    else
        aFile = stdout;

    yyparse();
}

