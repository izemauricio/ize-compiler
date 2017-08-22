#include "IOMngr.h"

FILE * filein = NULL;
FILE * fileout = NULL;

int lin = 0;
int col = 0;
char line[MAX_LINE];
char sout[MAX_LINE];

bool
OpenFiles(const char * source, const char * listining) {
    filein = fopen(source, "r");

    if (listining == NULL) {
        fileout = stdout;
    } else {
        fileout = fopen(listining, "w");
    }

    if (filein == NULL || fileout == NULL) {
        return false;
    }

    return true;
}

void
CloseFiles() {

    if (filein != NULL) {
        fclose(filein);
    }

    if (fileout != NULL) {
        fclose(fileout);
    }
}

/*
char GetSourceChar() {
    char c = 0;

    if (filein != NULL) {

        if (col == 0) {

            if (!fgets(line, MAX_LINE, filein)
                    || ferror(filein) || feof(filein)) {
                return EOF;
            }
            lin++;

            fprintf(fileout, "\n--------------------------------------------------------------->>> LINE %d: %s", lin, line);

            c = line[col];
            col++;
        } else {
            c = line[col];

            if (c == 10)
                col = 0;
            else
                col++;
        }
    }

    if(c == 0) {
        c = '\n';
      }
    printf(" [%c %d] ",c,c);
    return c;
}
 */

char
GetSourceChar() {

    if (col == 0) {

        fgets(line, MAX_LINE, filein);

        sout[0] = '\0';

        char convNumLine[5];

        snprintf(convNumLine, 4, "%d", lin);

        int t = strlen(convNumLine);

        strcat(sout, convNumLine);

        strcat(sout, " ");

        strcat(sout, line);

        if (feof(filein)) {
            return EOF;
        }

        fputs(sout, fileout);
    }
    char r = line[col];
    int num = (int) r;
    col++;

    if (num == 10) {
        col = 0;
        lin++;
    }

    //printf ("%c(%d)",r,r);

    return r;

}

void
WriteIndicator(int col) {
    int i = 0;

    fprintf(fileout, "\n");

    while (i <= col) {
        fprintf(fileout, " ");
        i++;
    }
    fprintf(fileout, "^");
}

void
WriteMessage(const char * msg) {
    fprintf(fileout, "\n");
    fprintf(fileout, "%s", msg);
}

int
GetCurrentLine() {
    return lin;
}

int
GetCurrentColumn() {
    return col;
}