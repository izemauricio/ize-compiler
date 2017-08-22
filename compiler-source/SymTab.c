#include "SymTab.h"

int getHash(const char * str, int size) {
    int i = 0;
    int sum = 0;
    for (i = 0; i < strlen(str); i++) {
        sum += str[i];
    }
    return (sum % size);
}

struct SymTab * CreateSymTab(int size) {
    struct SymTab * tab = (struct SymTab *) malloc(sizeof ( struct SymTab));

    if (tab != NULL)
        {
        tab->Size = size;
        tab->Contents = (struct SymEntry **) malloc(size * sizeof ( struct SymEntry *));
        if (tab->Contents == NULL) {
            printf("ERROR: malloc returned zero @ CreateSymTab()");
        }
    }
    else
     {
        printf("ERROR: malloc returned zero @ CreateSymTab()");
    }

    return tab;
}

void DestroySymTab(struct SymTab * tab) {
    if (tab != NULL) {
        int hash = 0;
        while (hash < tab->Size) {
            if (tab->Contents[hash] != NULL) {
                struct SymEntry * now = tab->Contents[hash];
                struct SymEntry * future = now->Next;

                tab->Contents[hash] = NULL;

                free(now);
                while (future != NULL) {
                    now = future;
                    future = now->Next;
                    free(now);
                }
            }
            hash++;
        }
        free(tab->Contents);
        tab->Contents = NULL;

        free(tab);
        tab = NULL;
    }
}

bool EnterName(struct SymTab * tab, const char * name, struct SymEntry ** returningEntry) {
    if (tab != NULL) {
        if (tab->Contents != NULL) {

            struct SymEntry * temp = NULL;

            temp = FindName(tab, name);

            int hash = getHash(name, tab->Size);

            if (temp == NULL) {
                if (tab->Contents[hash] == NULL) {
                    // Alloc
                    struct SymEntry * entry = (struct SymEntry *) malloc(sizeof ( struct SymEntry));

                    // Fill
                    entry->Attributes = NULL;
                    entry->Name = malloc(sizeof ( strlen(name)));
                    strcpy(entry->Name, name);
                    entry->Next = NULL;

                    // Link
                    tab->Contents[hash] = entry;

                    // Save parameter
                    *returningEntry = entry;
                } else {
                    // Alloc
                    struct SymEntry * entry = (struct SymEntry *) malloc(sizeof ( struct SymEntry));

                    // Fill
                    entry->Attributes = NULL;
                    entry->Name = malloc(sizeof ( strlen(name)));
                    strcpy(entry->Name, name);

                    // Link
                    entry->Next = tab->Contents[hash];
                    tab->Contents[hash] = entry;

                    // Save parameter
                    *returningEntry = entry;
                }

                return true;
            } else {
                *returningEntry = temp;
                return false;
            }
        }
    }
}

struct SymEntry * FindName(struct SymTab * tab, const char * name) {
    if (tab != NULL) {
        if (tab->Contents != NULL) {
            int hash = getHash(name, tab->Size);

            struct SymEntry * entry = tab->Contents[hash];

            while (entry != NULL) {
                if (strcmp(entry->Name, name) == 0) {
                    return entry;
                } else {
                    entry = entry->Next;
                }
            }

            return NULL;
        }
    }
}

void SetAttr(struct SymEntry * entry, void * attr) {
    entry->Attributes = attr;
}

void * GetAttr(struct SymEntry * entry) {

    if (entry != NULL) {

        return entry->Attributes;
    } else {
        return NULL;
    }
}

const char * GetName(struct SymEntry * entry) {
    return entry->Name;
}

struct SymEntry * FirstEntry(struct SymTab * tab) {
    if (tab != NULL) {
        if (tab->Contents != NULL) {
            int hash = 0;

            while (hash < tab->Size) {
                if (tab->Contents[hash] != NULL) {
                    return tab->Contents[hash];
                }

                hash++;
            }

            return NULL;
        }
    }
}

int getHashOfEntty(struct SymTab * Tab, struct SymEntry * Entry) {
    if (Tab != NULL) {
        if (Tab->Contents != NULL) {
            int hash = 0;

            while (hash < Tab->Size) {
                if (Tab->Contents[hash] != NULL) {
                    struct SymEntry * _entry = Tab->Contents[hash];

                    while (_entry != NULL) {
                        if (_entry == Entry) {
                            return hash;
                        }

                        _entry = _entry->Next;
                    }
                }

                hash++;
            }
        } else printf("ERROR: contents is null\n");
    } else printf("ERROR: tab is null\n");
}

struct SymEntry * NextEntry(struct SymTab * tab, struct SymEntry * entry) {
    if (entry->Next != NULL) {

        return entry->Next;
    } else {
        int hash = getHashOfEntty(tab, entry);

        hash++;

        while (hash < tab->Size) {
            if (tab->Contents[hash] != NULL) {

                return tab->Contents[hash];
            }

            hash++;
        }

        return NULL;
    }
}
