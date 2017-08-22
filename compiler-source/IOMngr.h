#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024

bool OpenFiles(const char *, const char *);
void CloseFiles();
char GetSourceChar();
void WriteIndicator(int);
void WriteMessage(const char *);
int GetCurrentLine();
int GetCurrentColumn();
