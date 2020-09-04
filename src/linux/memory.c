#include <stdlib.h>
#include <errno.h>
#include "forth.h"

void allocate() {
    int size = pop();

    void *ptr = calloc( size, sizeof(CELL));
    push(ptr);

    if ( ptr == NULL) {
        push(errno);
        errno=0;
    } else {
        push(0);
    }
}

void resize() {
    int size = pop();
    void *ptr = pop();

    push( realloc(ptr, size));
}


void memFree() {
    void *ptr = pop();

    free( ptr );

    push(0);
}

void extend_memory() {
    create_codeword("allocate",allocate,0);
    create_codeword("resize",resize,0);
    create_codeword("free",memFree,0);
}

