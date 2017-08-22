#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "CodeGen.h"
#include "Semantics.h"
#include "SymTab.h"
#include "IOMngr.h"

extern struct SymEntry * entry;
extern struct SymTab * table;

int count = 0;

struct ExprRes *
CreateExprRes() {
    struct ExprRes * res;
    res = (struct ExprRes *) malloc(sizeof (struct ExprRes));
    res->DataType = -1;
    res->InstructSeq = NULL;
    res->RegOffsetAddr = -1;
    res->RegValue = -1;
    res->VarName = NULL;
    return res;
}

struct ExprRes *
createNUMERO(char * numero, int type) {
    struct ExprRes * res;
    res = CreateExprRes();

    res->DataType = type;
    res->VarName = NULL;
    res->RegValue = AvailTmpReg("createNumeroLiteral [res->Reg]");
    res->InstructSeq = GenInstr(NULL, "li", TmpRegName(res->RegValue), numero, NULL);

    return res;
}

char *
generateVarNameTEXTO() {
    char b[10];
    snprintf(b, 9, "_str%d", count);
    count++;
    return strdup(b);
}

struct TextRes *
createTEXTO(char * text) {
    struct ATRIBUTOS * a;
    a = (struct ATRIBUTOS *) malloc(sizeof (struct ATRIBUTOS));

    a->type = 0;
    a->texto = strdup(text);
    a->FunctionBody = NULL;
    a->arraysize = -1;

    char * varnametexto = generateVarNameTEXTO();

    EnterName(table, varnametexto, &entry);
    entry->Attributes = (void *) a;

    struct TextRes * textres = (struct TextRes *) malloc(sizeof (struct TextRes));
    textres->VarNameTEXTO = varnametexto;
    return textres;
}

void
createVarINT(char * name) {
    struct ATRIBUTOS * a;
    a = (struct ATRIBUTOS *) malloc(sizeof (struct ATRIBUTOS));

    a->type = 1;
    a->arraysize = 1;

    EnterName(table, name, &entry);
    entry->Attributes = (void *) a;
}

void
createArrayINT(char * name, int size) {
    struct ATRIBUTOS * a;
    a = (struct ATRIBUTOS *) malloc(sizeof (struct ATRIBUTOS));

    a->type = 2;
    a->arraysize = size;

    EnterName(table, name, &entry);
    entry->Attributes = (void *) a;
}

void
createVarBOOLEAN(char * name) {
    struct ATRIBUTOS * a;
    a = (struct ATRIBUTOS *) malloc(sizeof (struct ATRIBUTOS));

    a->type = 3;
    a->arraysize = 1;

    EnterName(table, name, &entry);
    entry->Attributes = (void *) a;
}

void
createArrayBOOLEAN(char * name, int size) {
    struct ATRIBUTOS * a;
    a = (struct ATRIBUTOS *) malloc(sizeof (struct ATRIBUTOS));

    a->type = 4;
    a->arraysize = size;

    EnterName(table, name, &entry);
    entry->Attributes = (void *) a;
}

void
createFUNCTION(char * name, struct InstrSeq * body) {
    struct ATRIBUTOS * a;
    a = (struct ATRIBUTOS *) malloc(sizeof (struct ATRIBUTOS));

    a->type = 5;
    a->FunctionBody = AppendSeq(NULL, GenInstr(NULL, "addi", "$sp", "$sp", "-4"));
    AppendSeq(a->FunctionBody, GenInstr(NULL, "sw", "$ra", "0($sp)", NULL));
    AppendSeq(a->FunctionBody, body);
    AppendSeq(a->FunctionBody, GenInstr(NULL, "lw", "$ra", "0($sp)", NULL));
    AppendSeq(a->FunctionBody, GenInstr(NULL, "addi", "$sp", "$sp", "4"));
    AppendSeq(a->FunctionBody, GenInstr(NULL, "jr", "$ra", NULL, NULL));

    EnterName(table, name, &entry);
    entry->Attributes = (void *) a;
}

struct ExprRes *
doRightValueVAR(char * VarName) {
    struct SymEntry * Entry = NULL;

    if (!FindName(table, VarName)) {
        WriteIndicator(GetCurrentColumn());
        WriteMessage("Error: Undeclared variable");
    }

    Entry = FindName(table, VarName);

    struct ExprRes * res;
    res = CreateExprRes();

    res->DataType = ((struct ATRIBUTOS *) Entry->Attributes)->type;
    res->VarName = strdup(VarName);
    res->RegValue = AvailTmpReg("doRval [res->Reg]");
    res->InstructSeq = GenInstr(NULL, "lw", TmpRegName(res->RegValue), VarName, NULL);

    return res;
}

struct ExprRes *
doRightValueARRAY(char * ArrayName, struct ExprRes * ExprIndex) {
    struct SymEntry * Entry = NULL;

    if (!FindName(table, ArrayName)) {
        WriteIndicator(GetCurrentColumn());
        WriteMessage("ERROR: Undeclared variable");
    }

    Entry = FindName(table, ArrayName);

    struct ExprRes * exprRes;
    exprRes = CreateExprRes();

    char b[100];

    exprRes->RegValue = AvailTmpReg("doRvalArray [res->Reg]");
    exprRes->RegOffsetAddr = AvailTmpReg("********************************** doRvalArray [RegOffsetArray]");
    exprRes->DataType = ((struct ATRIBUTOS *) Entry->Attributes)->type;
    exprRes->VarName = strdup(ArrayName);

    exprRes->InstructSeq = ExprIndex->InstructSeq;

    int ArrayBase = AvailTmpReg("doRvalArray [RegBaseArray]");

    /*
        la	$t0,aaa		# t0 = endereco do array[0]	
        li	$t1,2		# index = 2
        mul	$t1,$t1,4
        add	$t0,$t0,$t1
        lw	$t0,($t0)	# t0 = array[2]
     */

    AppendSeq(exprRes->InstructSeq, GenInstr(NULL, "# RIGHT VALUE OF ARRAY", NULL, NULL, NULL));
    AppendSeq(exprRes->InstructSeq, GenInstr(NULL, "la", TmpRegName(ArrayBase), ArrayName, NULL));
    AppendSeq(exprRes->InstructSeq, GenInstr(NULL, "mul", TmpRegName(ExprIndex->RegValue), TmpRegName(ExprIndex->RegValue), "4"));
    AppendSeq(exprRes->InstructSeq, GenInstr(NULL, "add", TmpRegName(exprRes->RegOffsetAddr), TmpRegName(ArrayBase), TmpRegName(ExprIndex->RegValue)));

    snprintf(b, 99, "(%s)", TmpRegName(exprRes->RegOffsetAddr));
    AppendSeq(exprRes->InstructSeq, GenInstr(NULL, "lw", TmpRegName(exprRes->RegValue), b, NULL));

    ReleaseTmpReg("", ArrayBase);
    ReleaseTmpReg("", ExprIndex->RegValue);
    ReleaseTmpReg("", ExprIndex->RegOffsetAddr);

    free(ExprIndex);

    return exprRes;
} // $t0 = valor do array[index]

struct ExprRes *
doRightValueFUNCTION(char * fname) {
    if (!FindName(table, fname)) {
        WriteIndicator(GetCurrentColumn());
        WriteMessage("ERROR: Undeclared function");
    }

    struct ExprRes * res;
    res = CreateExprRes();

    res->VarName = strdup(fname);
    res->DataType = 5;
    res->RegValue = AvailTmpReg("doRvalFunction [res->Reg]");

    /*
     jal fname
     move reg,$v0
     */

    res->InstructSeq = AppendSeq(NULL, GenInstr(NULL, "# RIGHT VALUE OF FUNCTION", NULL, NULL, NULL));
    AppendSeq(res->InstructSeq, GenInstr(NULL, "jal", fname, NULL, NULL));
    AppendSeq(res->InstructSeq, GenInstr(NULL, "move", TmpRegName(res->RegValue), "$v0", NULL));

    return res;
} // Jump and when return, $t0 = $v0

struct InstrSeq *
doAssignVAR(char * name, struct ExprRes * rvalue) {
    if (!FindName(table, name)) {
        WriteIndicator(GetCurrentColumn());
        WriteMessage("ERROR: Undeclared variable");
    }

    struct InstrSeq * code;

    code = rvalue->InstructSeq;
    AppendSeq(code, GenInstr(NULL, "sw", TmpRegName(rvalue->RegValue), name, NULL));

    ReleaseTmpReg("doAssign [rvalue->Reg]", rvalue->RegValue);
    ReleaseTmpReg("doAssign [rvalue->Reg]", rvalue->RegOffsetAddr);
    free(rvalue);

    return code;
}

struct InstrSeq *
doAssignARRAY(char * ArrayName, struct ExprRes * ExprIndex, struct ExprRes * ExprValue) {
    struct InstrSeq * code;

    if (!FindName(table, ArrayName)) {
        WriteIndicator(GetCurrentColumn());
        WriteMessage("Error: Undeclared variable");
    }

    code = ExprIndex->InstructSeq;
    AppendSeq(code, ExprValue->InstructSeq);

    int baseArray = AvailTmpReg("doAssignArray");
    char b[100];

    /*
        la	$t0,aaa			
        li	$t1,2		# index = 2
        mul	$t1,$t1,4
        add	$t0,$t0,$t1
        li	$t1,555		# array[2] = 555
        sw	$t1,($t0)
     */

    AppendSeq(code, GenInstr(NULL, "la", TmpRegName(baseArray), ArrayName, NULL));
    AppendSeq(code, GenInstr(NULL, "mul", TmpRegName(ExprIndex->RegValue), TmpRegName(ExprIndex->RegValue), "4"));
    AppendSeq(code, GenInstr(NULL, "add", TmpRegName(baseArray), TmpRegName(baseArray), TmpRegName(ExprIndex->RegValue)));
    snprintf(b, 99, "(%s)", TmpRegName(baseArray));
    AppendSeq(code, GenInstr(NULL, "sw", TmpRegName(ExprValue->RegValue), b, NULL));

    ReleaseTmpReg("doAssignArray [index->reg]", ExprIndex->RegValue);
    ReleaseTmpReg("doAssignArray [index->reg]", ExprIndex->RegOffsetAddr);
    ReleaseTmpReg("doAssignArray [rvalue->reg]", ExprValue->RegValue);
    ReleaseTmpReg("doAssignArray [index->reg]", ExprValue->RegOffsetAddr);
    ReleaseTmpReg("doAssignArray [baseArray]", baseArray);

    free(ExprIndex);
    free(ExprValue);

    return code;
}

struct InstrSeq *
doPrintsp(struct ExprRes * Res1) {
    struct InstrSeq *code;
    code = (struct InstrSeq *) malloc(sizeof (struct InstrSeq));
    char* sp = GenLabel();
    char* out = GenLabel();

    AppendSeq(code, Res1->InstructSeq);
    AppendSeq(code, GenInstr(NULL, "li", "$v0", "4", NULL));
    AppendSeq(code, GenInstr(NULL, "la", "$a0", "_sp", NULL));
    AppendSeq(code, GenInstr(NULL, "blt", TmpRegName(Res1->RegValue), "0", out));
    AppendSeq(code, GenInstr(NULL, "move", TmpRegName(Res1->RegValue), TmpRegName(Res1->RegValue), NULL));
    AppendSeq(code, GenInstr(sp, "beq", TmpRegName(Res1->RegValue), "0", out));
    AppendSeq(Res1->InstructSeq, GenInstr(NULL, "sub", TmpRegName(Res1->RegValue), TmpRegName(Res1->RegValue), "1"));

    AppendSeq(code, GenInstr(NULL, "syscall", NULL, NULL, NULL));
    AppendSeq(code, GenInstr(NULL, "j", sp, NULL, NULL));
    AppendSeq(code, GenInstr(out, NULL, NULL, NULL, NULL));
    free(Res1);
    free(sp);
    free(out);
    return code;
}

struct InstrSeq *
doPrintln() {
    struct InstrSeq * code;

    code = (struct InstrSeq *) malloc(sizeof (struct InstrSeq));

    AppendSeq(code, GenInstr(NULL, "li", "$v0", "4", NULL));
    AppendSeq(code, GenInstr(NULL, "la", "$a0", "_nl", NULL));
    AppendSeq(code, GenInstr(NULL, "syscall", NULL, NULL, NULL));

    return code;
}

struct InstrSeq *
doPrintstr(struct TextRes * text) {
    struct InstrSeq * res = AppendSeq(NULL, GenInstr(NULL, "li", "$v0", "4", NULL));
    AppendSeq(res, GenInstr(NULL, "la", "$a0", text->VarNameTEXTO, NULL));
    AppendSeq(res, GenInstr(NULL, "syscall", NULL, NULL, NULL));
    return res;
}

struct ExprResList *
addVarIntoVARLIST(struct ExprRes * var) {

    struct ExprResList * list = (struct ExprResList *) malloc(sizeof (struct ExprResList));
    list->ExprRes = var;
    list->next = NULL;

    return list;
}

struct ExprResList *
appendVarIntoVARLIST(struct ExprResList * list, struct ExprRes * var) {
    if (list == NULL) {
    } else {
        struct ExprResList * tmp = list;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }

        struct ExprResList * newVARLIST = (struct ExprResList *) malloc(sizeof (struct ExprResList));
        newVARLIST->ExprRes = var;
        newVARLIST->next = NULL;

        tmp->next = newVARLIST;
    }

    return list;
}

struct InstrSeq *
doReadList(struct ExprResList * list) {
    struct InstrSeq * code = NULL;

    if (list == NULL) {
    } else {
        struct ExprResList * tmp = list;

        while (tmp != NULL) {
            if (code == NULL) {
                code = tmp->ExprRes->InstructSeq;
            } else {
                AppendSeq(code, tmp->ExprRes->InstructSeq);
            }

            int RegTmp = AvailTmpReg("");

            AppendSeq(code, GenInstr(NULL, "# GETTING VAR FROM INPUT", NULL, NULL, NULL));
            AppendSeq(code, GenInstr(NULL, "li", "$v0", "5", NULL));
            AppendSeq(code, GenInstr(NULL, "syscall", NULL, NULL, NULL));
            AppendSeq(code, GenInstr(NULL, "move", TmpRegName(RegTmp), "$v0", NULL));


            if (tmp->ExprRes->DataType == 2 || tmp->ExprRes->DataType == 4) {
                char b[20];
                snprintf(b, 19, "(%s)", TmpRegName(tmp->ExprRes->RegOffsetAddr));
                AppendSeq(code, GenInstr(NULL, "sw", TmpRegName(RegTmp), b, NULL));
                ReleaseTmpReg("doReadList [Reg]", tmp->ExprRes->RegValue);
                ReleaseTmpReg("doReadList [Reg]", tmp->ExprRes->RegOffsetAddr);
                ReleaseTmpReg("doReadList [Reg]", RegTmp);
            } else {
                AppendSeq(code, GenInstr(NULL, "sw", TmpRegName(RegTmp), tmp->ExprRes->VarName, NULL));
                ReleaseTmpReg("doReadList [Reg]", tmp->ExprRes->RegValue);

                ReleaseTmpReg("doReadList [Reg]", RegTmp);
            }

            AppendSeq(code, GenInstr(NULL, "# FIM READ VAR", NULL, NULL, NULL));




            free(tmp->ExprRes);

            tmp = tmp->next;
        }
    }

    free(list);

    return code;
}

struct InstrSeq *
doPrintList(struct ExprResList * list) {
    struct InstrSeq * code = NULL;

    if (list == NULL) {
    } else {
        struct ExprResList * tmp = list;

        while (tmp != NULL) {
            if (code == NULL) {
                code = tmp->ExprRes->InstructSeq;
            } else {
                AppendSeq(code, tmp->ExprRes->InstructSeq);
            }

            if (tmp->ExprRes->DataType == 3 || tmp->ExprRes->DataType == 4) {
                char * THELABEL = GenLabel();
                char * THELABEL2 = GenLabel();

                AppendSeq(code, GenInstr(NULL, "# PRINT VAR BOOL", NULL, NULL, NULL));

                // if $t0 == 1
                AppendSeq(code, GenInstr(NULL, "bne", TmpRegName(tmp->ExprRes->RegValue), "1", THELABEL));

                // ifbody
                AppendSeq(code, GenInstr(NULL, "la", "$a0", "_true", NULL));
                AppendSeq(code, GenInstr(NULL, "li", "$v0", "4", NULL));

                // else
                AppendSeq(code, GenInstr(NULL, "j", THELABEL2, NULL, NULL));


                //elsebody
                AppendSeq(code, GenInstr(THELABEL, NULL, NULL, NULL, NULL));
                AppendSeq(code, GenInstr(NULL, "la", "$a0", "_false", NULL));
                AppendSeq(code, GenInstr(NULL, "li", "$v0", "4", NULL));

                // fimif
                AppendSeq(code, GenInstr(THELABEL2, NULL, NULL, NULL, NULL));

                ReleaseTmpReg("doPrintList [Reg]", tmp->ExprRes->RegOffsetAddr);
                free(THELABEL);
                free(THELABEL2);
            } else {
                AppendSeq(code, GenInstr(NULL, "# PRINTING VAR", NULL, NULL, NULL));
                AppendSeq(code, GenInstr(NULL, "move", "$a0", TmpRegName(tmp->ExprRes->RegValue), NULL));
                AppendSeq(code, GenInstr(NULL, "li", "$v0", "1", NULL));
            }

            AppendSeq(code, GenInstr(NULL, "syscall", NULL, NULL, NULL));

            AppendSeq(code, GenInstr(NULL, "li", "$v0", "4", NULL));
            AppendSeq(code, GenInstr(NULL, "la", "$a0", "_sp", NULL));
            AppendSeq(code, GenInstr(NULL, "syscall", NULL, NULL, NULL));

            ReleaseTmpReg("doPrintList [Reg]", tmp->ExprRes->RegValue);


            free(tmp->ExprRes);

            tmp = tmp->next;
        }
    }

    free(list);

    return code;
}

struct InstrSeq *
doReturnFUNCTION(struct ExprRes * Expr) {
    struct InstrSeq * res = AppendSeq(NULL, Expr->InstructSeq);
    AppendSeq(res, GenInstr(NULL, "move", "$v0", TmpRegName(Expr->RegValue), NULL));
    AppendSeq(res, GenInstr(NULL, "lw", "$ra", "0($sp)", NULL));
    // RESTORE THE $ra of THE STACK

    AppendSeq(res, GenInstr(NULL, "addi", "$sp", "$sp", "4")); // +4 stack pointer go up
    AppendSeq(res, GenInstr(NULL, "jr", "$ra", NULL, NULL));
    return res;
} // Set the value of $v0 = $t0

struct InstrSeq *
doCallFUNCTION(char * label) {
    struct InstrSeq * res = AppendSeq(NULL, GenInstr(NULL, "jal", strdup(label), NULL, NULL));
    return res;
} // Just jump

struct ExprRes *
doAdd(struct ExprRes * Res1, struct ExprRes * Res2) {
    int reg;

    reg = AvailTmpReg("doAdd");

    AppendSeq(Res1->InstructSeq, Res2->InstructSeq);

    AppendSeq(Res1->InstructSeq, GenInstr(NULL, "add",
            TmpRegName(reg),
            TmpRegName(Res1->RegValue),
            TmpRegName(Res2->RegValue)));

    //Res1->numero = Res1->numero + Res2->numero;

    ReleaseTmpReg("doAdd [Res1->Reg", Res1->RegValue);
    ReleaseTmpReg("doAdd [Res2->Reg]", Res2->RegValue);
    ReleaseTmpReg("doDiv", Res1->RegOffsetAddr);
    ReleaseTmpReg("doDiv", Res2->RegOffsetAddr);
    Res1->RegValue = reg;
    free(Res2);

    return Res1;
}

struct ExprRes *
doSub(struct ExprRes * Res1, struct ExprRes * Res2) {

    int reg;

    reg = AvailTmpReg("doSub");


    AppendSeq(Res1->InstructSeq, Res2->InstructSeq);

    AppendSeq(Res1->InstructSeq, GenInstr(NULL, "sub",
            TmpRegName(reg),
            TmpRegName(Res1->RegValue),
            TmpRegName(Res2->RegValue)));

    //Res1->numero = Res1->numero - Res2->numero;

    ReleaseTmpReg("doSub", Res1->RegValue);
    ReleaseTmpReg("doSub", Res2->RegValue);
    ReleaseTmpReg("doDiv", Res1->RegOffsetAddr);
    ReleaseTmpReg("doDiv", Res2->RegOffsetAddr);

    Res1->RegValue = reg;
    free(Res2);

    return Res1;
}

struct ExprRes *
doMul(struct ExprRes * Res1, struct ExprRes * Res2) {

    int reg;

    reg = AvailTmpReg("doMul");
    AppendSeq(Res1->InstructSeq, Res2->InstructSeq);
    AppendSeq(Res1->InstructSeq, GenInstr(NULL, "mul",
            TmpRegName(reg),
            TmpRegName(Res1->RegValue),
            TmpRegName(Res2->RegValue)));

    //Res1->numero = Res1->numero * Res2->numero;

    ReleaseTmpReg("doMul", Res1->RegValue);
    ReleaseTmpReg("doMul", Res2->RegValue);
    ReleaseTmpReg("doDiv", Res1->RegOffsetAddr);
    ReleaseTmpReg("doDiv", Res2->RegOffsetAddr);

    Res1->RegValue = reg;

    free(Res2);
    return Res1;
}

struct ExprRes *
doDiv(struct ExprRes * Res1, struct ExprRes * Res2) {
    int reg;

    reg = AvailTmpReg("doDiv");

    AppendSeq(Res1->InstructSeq, Res2->InstructSeq);

    AppendSeq(Res1->InstructSeq, GenInstr(NULL, "div", TmpRegName(reg), TmpRegName(Res1->RegValue), TmpRegName(Res2->RegValue)));

    AppendSeq(Res1->InstructSeq, GenInstr(NULL, "mflo", TmpRegName(reg), NULL, NULL));

    //Res1->numero = Res1->numero / Res2->numero;

    ReleaseTmpReg("doDiv", Res1->RegValue);
    ReleaseTmpReg("doDiv", Res2->RegValue);
    ReleaseTmpReg("doDiv", Res1->RegOffsetAddr);
    ReleaseTmpReg("doDiv", Res2->RegOffsetAddr);


    Res1->RegValue = reg;

    free(Res2);

    return Res1;
}

struct ExprRes *
doExpoente(struct ExprRes * BASE, struct ExprRes * EXPO) {
    char * START = GenLabel();
    char * CONTINUE = GenLabel();
    int RESULTADO = AvailTmpReg("doExpoente");

    AppendSeq(BASE->InstructSeq, EXPO->InstructSeq);
    AppendSeq(BASE->InstructSeq, GenInstr(NULL, "blt", TmpRegName(EXPO->RegValue), "$zero", CONTINUE));
    AppendSeq(BASE->InstructSeq, GenInstr(NULL, "move", TmpRegName(RESULTADO), TmpRegName(BASE->RegValue), NULL));
    AppendSeq(BASE->InstructSeq, GenInstr(NULL, "sub", TmpRegName(EXPO->RegValue), TmpRegName(EXPO->RegValue), "1"));
    AppendSeq(BASE->InstructSeq, GenInstr(START, "beq", TmpRegName(EXPO->RegValue), "$zero", CONTINUE));
    AppendSeq(BASE->InstructSeq, GenInstr(NULL, "mul", TmpRegName(RESULTADO), TmpRegName(RESULTADO), TmpRegName(BASE->RegValue)));
    AppendSeq(BASE->InstructSeq, GenInstr(NULL, "sub", TmpRegName(EXPO->RegValue), TmpRegName(EXPO->RegValue), "1"));
    AppendSeq(BASE->InstructSeq, GenInstr(NULL, "j", START, NULL, NULL));
    AppendSeq(BASE->InstructSeq, GenInstr(CONTINUE, NULL, NULL, NULL, NULL));
    ReleaseTmpReg("doExpoente [BASE->Reg]", BASE->RegValue);
    ReleaseTmpReg("doExpoente [EXPO->Reg]", EXPO->RegValue);
    ReleaseTmpReg("doExpoente [BASE->Reg]", BASE->RegOffsetAddr);
    ReleaseTmpReg("doExpoente [EXPO->Reg]", EXPO->RegOffsetAddr);
    free(EXPO);
    free(START);
    free(CONTINUE);

    BASE->RegValue = RESULTADO;

    return BASE;
}

struct ExprRes *
doMod(struct ExprRes * Res1, struct ExprRes * Res2) {
    int reg;

    reg = AvailTmpReg("doMod");

    AppendSeq(Res1->InstructSeq, Res2->InstructSeq);

    AppendSeq(Res1->InstructSeq, GenInstr(NULL, "div", TmpRegName(reg), TmpRegName(Res1->RegValue), TmpRegName(Res2->RegValue)));

    AppendSeq(Res1->InstructSeq, GenInstr(NULL, "mfhi", TmpRegName(reg), NULL, NULL));

    //Res1->numero = Res1->numero % Res2->numero;

    ReleaseTmpReg("doMod", Res1->RegValue);
    ReleaseTmpReg("doMod", Res2->RegValue);
    ReleaseTmpReg("doMod", Res1->RegOffsetAddr);
    ReleaseTmpReg("doMod", Res2->RegOffsetAddr);

    Res1->RegValue = reg;

    free(Res2);

    return Res1;
}

struct ExprRes *
doInv(struct ExprRes * Res1) {
    int reg;

    reg = AvailTmpReg("doInv");

    AppendSeq(Res1->InstructSeq, GenInstr(NULL, "mul", TmpRegName(reg), TmpRegName(Res1->RegValue), "-1"));

    //Res1->numero = -(Res1->numero);

    ReleaseTmpReg("doInv", Res1->RegValue);
    ReleaseTmpReg("doInv", Res1->RegOffsetAddr);

    Res1->RegValue = reg;

    return Res1;
}

struct ExprRes *
doNot(struct ExprRes * Expr) {
    AppendSeq(Expr->InstructSeq, GenInstr(NULL, "xor", TmpRegName(Expr->RegValue), TmpRegName(Expr->RegValue), "1"));
    Expr->DataType = 3;

    return Expr;
}

struct ExprRes *
doIGUAL(struct ExprRes * Expr1, struct ExprRes * Expr2) {
    AppendSeq(Expr1->InstructSeq, Expr2->InstructSeq);
    AppendSeq(Expr1->InstructSeq, GenInstr(NULL, "seq", TmpRegName(Expr1->RegValue), TmpRegName(Expr1->RegValue), TmpRegName(Expr2->RegValue)));

    ReleaseTmpReg("doMAIOR", Expr2->RegValue);
    ReleaseTmpReg("doMAIOR", Expr2->RegOffsetAddr);
    free(Expr2);

    Expr1->DataType = 3;

    return Expr1;
}

struct ExprRes *
doDIFER(struct ExprRes * Expr1, struct ExprRes * Expr2) {
    AppendSeq(Expr1->InstructSeq, Expr2->InstructSeq);
    AppendSeq(Expr1->InstructSeq, GenInstr(NULL, "sne", TmpRegName(Expr1->RegValue), TmpRegName(Expr1->RegValue), TmpRegName(Expr2->RegValue)));

    ReleaseTmpReg("doMAIOR", Expr2->RegValue);
    ReleaseTmpReg("doMAIOR", Expr2->RegOffsetAddr);
    free(Expr2);

    Expr1->DataType = 3;

    return Expr1;
}

struct ExprRes *
doMENOR(struct ExprRes * Expr1, struct ExprRes * Expr2) {
    AppendSeq(Expr1->InstructSeq, Expr2->InstructSeq);
    AppendSeq(Expr1->InstructSeq, GenInstr(NULL, "slt", TmpRegName(Expr1->RegValue), TmpRegName(Expr1->RegValue), TmpRegName(Expr2->RegValue)));

    ReleaseTmpReg("doMAIOR", Expr2->RegValue);
    ReleaseTmpReg("doMAIOR", Expr2->RegOffsetAddr);
    free(Expr2);

    Expr1->DataType = 3;

    return Expr1;
}

struct ExprRes *
doMAIOR(struct ExprRes * Expr1, struct ExprRes * Expr2) {
    AppendSeq(Expr1->InstructSeq, Expr2->InstructSeq);
    AppendSeq(Expr1->InstructSeq, GenInstr(NULL, "sgt", TmpRegName(Expr1->RegValue), TmpRegName(Expr1->RegValue), TmpRegName(Expr2->RegValue)));

    ReleaseTmpReg("doMAIOR", Expr2->RegValue);
    ReleaseTmpReg("doMAIOR", Expr2->RegOffsetAddr);
    free(Expr2);

    Expr1->DataType = 3;

    return Expr1;
}

struct ExprRes *
doMAIORIGUAL(struct ExprRes * Res1, struct ExprRes * Res2) {
    AppendSeq(Res1->InstructSeq, Res2->InstructSeq);
    AppendSeq(Res1->InstructSeq, GenInstr(NULL, "sge", TmpRegName(Res1->RegValue), TmpRegName(Res1->RegValue), TmpRegName(Res2->RegValue)));

    ReleaseTmpReg("doOR", Res2->RegValue);
    ReleaseTmpReg("doOR", Res2->RegOffsetAddr);
    free(Res2);
    Res1->DataType = 3;
    return Res1;
}

struct ExprRes *
doMENORIGUAL(struct ExprRes * Res1, struct ExprRes * Res2) {
    AppendSeq(Res1->InstructSeq, Res2->InstructSeq);
    AppendSeq(Res1->InstructSeq, GenInstr(NULL, "sle", TmpRegName(Res1->RegValue), TmpRegName(Res1->RegValue), TmpRegName(Res2->RegValue)));

    ReleaseTmpReg("doOR", Res2->RegValue);
    ReleaseTmpReg("doOR", Res2->RegOffsetAddr);
    free(Res2);
    Res1->DataType = 3;
    return Res1;
}

struct ExprRes *
doAND(struct ExprRes * Res1, struct ExprRes * Res2) {
    AppendSeq(Res1->InstructSeq, Res2->InstructSeq);
    AppendSeq(Res1->InstructSeq, GenInstr(NULL, "and", TmpRegName(Res1->RegValue), TmpRegName(Res1->RegValue), TmpRegName(Res2->RegValue)));

    ReleaseTmpReg("doOR", Res2->RegValue);
    ReleaseTmpReg("doOR", Res2->RegOffsetAddr);
    free(Res2);
    Res1->DataType = 3;
    return Res1;
}

struct ExprRes *
doOR(struct ExprRes * Res1, struct ExprRes * Res2) {
    AppendSeq(Res1->InstructSeq, Res2->InstructSeq);
    AppendSeq(Res1->InstructSeq, GenInstr(NULL, "or", TmpRegName(Res1->RegValue), TmpRegName(Res1->RegValue), TmpRegName(Res2->RegValue)));

    ReleaseTmpReg("doOR", Res2->RegValue);
    ReleaseTmpReg("doOR", Res2->RegOffsetAddr);
    free(Res2);
    Res1->DataType = 3;
    return Res1;
}

struct InstrSeq *
doIf(struct ExprRes * Expr, struct InstrSeq * truebody) {
    struct InstrSeq * code;

    char * FALSE = GenLabel();

    code = Expr->InstructSeq;

    AppendSeq(code, GenInstr(NULL, "bne", TmpRegName(Expr->RegValue), "1", FALSE));
    AppendSeq(code, truebody);
    printSeq2(truebody);
    AppendSeq(code, GenInstr(FALSE, NULL, NULL, NULL, NULL));

    ReleaseTmpReg("doIf", Expr->RegValue);
    ReleaseTmpReg("doIf", Expr->RegOffsetAddr);

    free(Expr);

    return code;
}

struct InstrSeq *
doIfElse(struct ExprRes * Expr, struct InstrSeq * truebody, struct InstrSeq * falsebody) {
    struct InstrSeq * code;

    char * FALSE = GenLabel();
    char * QUIT = GenLabel();

    code = Expr->InstructSeq;

    AppendSeq(code, GenInstr(NULL, "bne", TmpRegName(Expr->RegValue), "1", FALSE));
    AppendSeq(code, truebody);
    AppendSeq(code, GenInstr(NULL, "j", QUIT, NULL, NULL));
    AppendSeq(code, GenInstr(FALSE, NULL, NULL, NULL, NULL));
    AppendSeq(code, falsebody);
    AppendSeq(code, GenInstr(QUIT, NULL, NULL, NULL, NULL));

    ReleaseTmpReg("doIf", Expr->RegValue);
    ReleaseTmpReg("doIf", Expr->RegOffsetAddr);

    free(Expr);
    free(FALSE);
    free(QUIT);

    return code;
}

struct InstrSeq *
doWhile(struct ExprRes * Expr, struct InstrSeq * whilebody) {
    struct InstrSeq * code = (struct InstrSeq *) malloc(sizeof (struct InstrSeq));

    char * START = GenLabel();
    char * QUIT = GenLabel();

    AppendSeq(code, GenInstr(START, NULL, NULL, NULL, NULL));
    AppendSeq(code, Expr->InstructSeq);
    AppendSeq(code, GenInstr(NULL, "bne", TmpRegName(Expr->RegValue), "1", QUIT));
    AppendSeq(code, whilebody);
    AppendSeq(code, GenInstr(NULL, "j", START, NULL, NULL));
    AppendSeq(code, GenInstr(QUIT, NULL, NULL, NULL, NULL));

    ReleaseTmpReg("doIf", Expr->RegValue);
    ReleaseTmpReg("doIf", Expr->RegOffsetAddr);

    free(Expr);
    free(START);
    free(QUIT);

    return code;
}

struct InstrSeq *
printSeq(struct ExprRes *seq) {
    struct InstrSeq *cmd;

    cmd = seq->InstructSeq;


    printf("\n");

    while (cmd) {
        printf("[");

        if (cmd->Label)
            printf(" (%s) ", cmd->Label);

        if (cmd->OpCode) {
            printf(" %s ", cmd->OpCode);

            if (cmd->Oprnd1)
                printf(" %s ", cmd->Oprnd1);

            if (cmd->Oprnd2)
                printf(" %s ", cmd->Oprnd2);

            if (cmd->Oprnd3)
                printf(" %s ", cmd->Oprnd3);


        }
        printf("]");

        cmd = cmd->Next;
    }

    printf("\n");
}

struct InstrSeq *
printSeq2(struct InstrSeq *seq) {
    struct InstrSeq *cmd;

    cmd = seq;

    printf("\n");

    while (cmd) {
        printf("[");

        if (cmd->Label)
            printf(" (%s) ", cmd->Label);

        if (cmd->OpCode) {
            printf(" %s ", cmd->OpCode);

            if (cmd->Oprnd1)
                printf(" %s ", cmd->Oprnd1);

            if (cmd->Oprnd2)
                printf(" %s ", cmd->Oprnd2);

            if (cmd->Oprnd3)
                printf(" %s ", cmd->Oprnd3);


        }
        printf("]");

        cmd = cmd->Next;
    }

    printf("\n");
}

void
FINISH(struct InstrSeq *AllCode) {
    struct InstrSeq *code;

    struct SymEntry *entry;
    //struct Attr * attr;


    code = GenInstr(NULL, ".text", NULL, NULL, NULL);
    //AppendSeq(code, GenInstr(NULL, ".align", "2", NULL, NULL));
    AppendSeq(code, GenInstr(NULL, ".globl", "main", NULL, NULL));
    AppendSeq(code, GenInstr("main", NULL, NULL, NULL, NULL));

    AppendSeq(code, AllCode);

    AppendSeq(code, GenInstr(NULL, "li", "$v0", "10", NULL));
    AppendSeq(code, GenInstr(NULL, "syscall", NULL, NULL, NULL));


    // FUNCOES

    entry = FirstEntry(table);
    while (entry) {
        if (((struct ATRIBUTOS *) entry->Attributes)->type == 5) {
            AppendSeq(code, GenInstr((char *) GetName(entry), NULL, NULL, NULL, NULL));
            AppendSeq(code, ((struct ATRIBUTOS *) entry->Attributes)->FunctionBody);
        }

        entry = NextEntry(table, entry);
    }

    // FIM FUNCOES

    AppendSeq(code, GenInstr(NULL, ".data", NULL, NULL, NULL));
    AppendSeq(code, GenInstr(NULL, ".align", "4", NULL, NULL));
    AppendSeq(code, GenInstr("_nl", ".asciiz", "\"\\n\"", NULL, NULL));
    AppendSeq(code, GenInstr("_sp", ".asciiz", "\" \"", NULL, NULL));
    AppendSeq(code, GenInstr("_true", ".asciiz", "\"true\"", NULL, NULL));
    AppendSeq(code, GenInstr("_false", ".asciiz", "\"false\"", NULL, NULL));

    // VARIAVEIS INTEIRAS
    entry = FirstEntry(table);
    while (entry) {
        if (((struct ATRIBUTOS *) entry->Attributes)->type == 1 || ((struct ATRIBUTOS *) entry->Attributes)->type == 3) {
            AppendSeq(code, GenInstr((char *) GetName(entry), ".word", "0", NULL, NULL));
        }

        if (((struct ATRIBUTOS *) entry->Attributes)->type == 2 || ((struct ATRIBUTOS *) entry->Attributes)->type == 4) {
            char b[100];
            snprintf(b, 99, "0:%d", ((struct ATRIBUTOS *) entry->Attributes)->arraysize);
            AppendSeq(code, GenInstr((char *) GetName(entry), ".word", b, NULL, NULL));
        }

        if (((struct ATRIBUTOS *) entry->Attributes)->type == 0) {
            AppendSeq(code, GenInstr((char *) GetName(entry), ".asciiz", ((struct ATRIBUTOS *) entry->Attributes)->texto, NULL, NULL));
        }

        entry = NextEntry(table, entry);
    }



    WriteSeq(code);

    return;
}