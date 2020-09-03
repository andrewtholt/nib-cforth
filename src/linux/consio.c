#include <stdio.h>

#include "consio.h"
#include "forth.h"

void c_akey() {
    push(getchar());
}

void c_qterminal() {
    uabort((const char *)"?terminal for Unix/Linux not yet implemented");
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

