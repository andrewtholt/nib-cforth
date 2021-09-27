#include <stdlib.h>
#include <errno.h>
#include "forth.h"

void allocate() {
    int size = (int)pop();

    void *ptr = calloc( size, sizeof(CELL));
    push((CELL)ptr);

    if ( ptr == NULL) {
        push(errno);
        errno=0;
    } else {
        push(0);
    }
}

void resize() {
    int size = pop();
    void *ptr = (void *)pop();

    push( (CELL)realloc(ptr, size));
}


void memFree() {
    void *ptr = (void *)pop();

    free( ptr );

    push(0);
}

void extend_memory() {
    create_codeword("allocate",allocate,0);
    create_codeword("resize",resize,0);
    create_codeword("free",memFree,0);
}

