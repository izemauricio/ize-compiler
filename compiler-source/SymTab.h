#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define NAME_LEN 30

struct SymTab {
    int Size;
    struct SymEntry ** Contents;
};

struct SymEntry {
    char * Name;
    void * Attributes;
    struct SymEntry * Next;
};

struct SymTab * CreateSymTab(int);
void DestroySymTab(struct SymTab *);
bool EnterName(struct SymTab *, const char *, struct SymEntry **);
struct SymEntry * FindName(struct SymTab *, const char *);
void SetAttr(struct SymEntry *, void *);
void * GetAttr(struct SymEntry *);
const char * GetName(struct SymEntry *);
struct SymEntry * FirstEntry(struct SymTab *);
struct SymEntry * NextEntry(struct SymTab *, struct SymEntry *);
void list(struct SymTab *);