#include <stdio.h>

#include "consio.h"
#include "forth.h"
#include "words.h"


extern int depth;
extern FILE *ifp[];

void c_akey() {
    push(getchar());
}

void c_qterminal() {
    uabort((const char *)"?terminal for Unix/Linux not yet implemented");
}

void c_afkey() {
    extern int      verbose;

    char            ch;
    if (depth < 0)
        uabort("no file open");
    ch = getc(ifp[depth]);

    if (ch == EOF) {
        if (verbose)
            printf("end of file");
        c_endf();
        push('\r');
        return;
    }   

    if (ch == '\n')
        ch = '\r';

    push(ch);
}

void c_aemit() {
    extern int verbose;

    if (verbose) {
        // putchar((BYTE) pop());
        PUTC((BYTE) pop());
    } else {
        pop();
    }

    push(1);
    user(OUT);
    c_pstor();
}

void c_endall() {
    while (depth >= 0)
        c_endf();
}

void c_endf(void) {
    if (depth < 0)
        return;
    fclose(ifp[depth]);

    if (depth == 0) {    
        *puser(TKEY) = (CELL) & cf_akey;
    }   
    depth--;
}


