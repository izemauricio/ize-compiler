struct ExprRes {
    int RegValue;
    int RegOffsetAddr;
    int DataType;
    char * VarName;
    struct InstrSeq * InstructSeq;
};

struct ExprResList {
    struct ExprRes * ExprRes;
    struct ExprResList * next;
};

struct TextRes {
    char * VarNameTEXTO;
};

struct ATRIBUTOS {
    int type;
    struct InstrSeq * FunctionBody;
    char * texto;
    int arraysize;
};

void FINISH(struct InstrSeq *AllCode);
struct InstrSeq *printSeq(struct ExprRes *seq);
struct InstrSeq *doWhile(struct ExprRes *Expr, struct InstrSeq *whilebody);
struct InstrSeq *doIfElse(struct ExprRes *Expr, struct InstrSeq *truebody, struct InstrSeq *falsebody);
struct InstrSeq *doIf(struct ExprRes *Expr, struct InstrSeq *truebody);
struct ExprRes *doOR(struct ExprRes *Res1, struct ExprRes *Res2);
struct ExprRes *doAND(struct ExprRes *Res1, struct ExprRes *Res2);
struct ExprRes *doMENORIGUAL(struct ExprRes *Res1, struct ExprRes *Res2);
struct ExprRes *doMAIORIGUAL(struct ExprRes *Res1, struct ExprRes *Res2);
struct ExprRes *doMAIOR(struct ExprRes *Res1, struct ExprRes *Res2);
struct ExprRes *doMENOR(struct ExprRes *Res1, struct ExprRes *Res2);
struct ExprRes *doDIFER(struct ExprRes *Res1, struct ExprRes *Res2);
struct ExprRes *doIGUAL(struct ExprRes *Expr1, struct ExprRes *Expr2);
struct ExprRes *doNot(struct ExprRes *Res1);
struct ExprRes *doInv(struct ExprRes *Res1);
struct ExprRes *doMod(struct ExprRes *Res1, struct ExprRes *Res2);
struct ExprRes *doExpoente(struct ExprRes *BASE, struct ExprRes *EXPO);
struct ExprRes *doDiv(struct ExprRes *Res1, struct ExprRes *Res2);
struct ExprRes *doMul(struct ExprRes *Res1, struct ExprRes *Res2);
struct ExprRes *doSub(struct ExprRes *Res1, struct ExprRes *Res2);
struct ExprRes *doAdd(struct ExprRes *Res1, struct ExprRes *Res2);
struct InstrSeq *doCallFUNCTION(char *label);
struct InstrSeq *printSeq2(struct InstrSeq *seq);
struct InstrSeq *doReturnFUNCTION(struct ExprRes *var);
struct InstrSeq *doPrintList(struct ExprResList *list);
struct InstrSeq *doReadList(struct ExprResList *list);
struct ExprResList *appendVarIntoVARLIST(struct ExprResList *list, struct ExprRes *var);
struct ExprResList *addVarIntoVARLIST(struct ExprRes *var);
struct InstrSeq *doPrintstr(struct TextRes *text);
struct InstrSeq *doPrintln();
struct InstrSeq *doPrintsp(struct ExprRes *Res1);
struct InstrSeq *doAssignARRAY(char *ArrayName, struct ExprRes *ExprIndex, struct ExprRes *ExprValue);
struct InstrSeq *doAssignVAR(char *name, struct ExprRes *rvalue);
struct ExprRes *doRightValueFUNCTION(char *fname);
struct ExprRes *doRightValueARRAY(char *ArrayName, struct ExprRes *ExprIndex);
struct ExprRes *doRightValueVAR(char *name);
void createFUNCTION(char *name, struct InstrSeq *body);
void createArrayBOOLEAN(char *name, int size);
void createVarBOOLEAN(char *name);
void createArrayINT(char *name, int size);
void createVarINT(char *name);
char *getStringNumber();
struct TextRes *createTEXTO(char *text);
struct ExprRes *createNUMERO(char *numero, int type);
extern int count;
extern struct SymTab *table;
extern struct SymEntry *entry;