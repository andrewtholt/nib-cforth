#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "forth.h"

#define FILE_DEPTH 10
FILE           *ifp[FILE_DEPTH];
int             depth = -1;

typedef enum fam {
    RO=0,
    RW,
    WO
} fam_t;

void c_tst() {
    char *ptr;
    int len=0;

    printf("\r\ntst:in fileio.c\r\n");

    len=pop();
    ptr=(char *)pop();

    printf("str=%s\r\n",ptr);

}
//
// Stack: c-addr u -- x ior 
// if file exists ior is 0
// Otherwise implementation define result code.
// x is implementation defined other info.
//
void fileStatus() {
    char *fname;
    int len=0;

    len=pop();
    fname=(char *)pop();

    int ret = access(fname, F_OK);

    push(0); // x, unused present for complaince
    push(ret);
}
// 
// Stack: c-addr u fam -- fileid ior 
//
// Open the file named in the character string specified by c-addr u, 
// with file access method indicated by fam. 
// The meaning of values of fam is implementation defined.
// If the file is successfully opened, ior is zero, fileid is its identifier, 
// and the file has been positioned to the start of the file.
// Otherwise, ior is the implementation-defined I/O result code and fileid is undefined.
//
void openFile() {
    fam_t access = pop();
    int len = pop();
    char *fname = (char *)pop();

    const char *fopenMode[] = {
        "r",
        "r+",
        "w"
    };

    FILE *fp = NULL;

    fp = fopen(fname, fopenMode[access]);
    if ( fp == NULL) {
        push(-1);   // fileid
        push(errno);
    } else {
        push(fp);
        push(0);
    }
    errno=0;
}
// 
// Stack : c-addr u fam -- fileid ior 
//
// Create the file named in the character string specified by c-addr and u, and open 
// it with file access method fam. 
// The meaning of values of fam is implementation defined. 
// If a file with the same name already exists, recreate it as an empty file. 
// If the file was successfully created and opened, ior is zero, fileid is its identifier,
// and the file has been positioned to the start of the file.
// Otherwise, ior is the implementation-defined I/O result code and fileid is undefined.
//
void createFile() {
    fam_t access = pop();
    int len = pop();
    char *fname = (char *)pop();

    int fd=creat( fname, S_IRUSR|S_IWUSR);

    if( fd == -1) {
        push(-1);
        push(errno);
        errno=0;
    } else {
        close(fd);

        push(fname);
        push(len);
        push(access);
        openFile();
    }
}
// 
// Stack : c-addr u fileid -- ior
// Write u characters from c-addr to the file identified by fileid starting at its
// current position. ior is the implementation-defined I/O result code.
//
// At the conclusion of the operation, FILE-POSITION returns the next file position 
// after the last character written to the file, and FILE-SIZE returns a value greater than 
// or equal to the value returned by FILE-POSITION.
//
void writeFile() {
    FILE *fid = pop();
    int len = pop();
    char *ptr = (char *)pop();

    int res = fwrite(ptr,1, len, fid);

    push(res);
    push(errno);
}
// 
// Stack: fileid -- ior 
//
// Attempt to force any buffered information written to the file referred to by fileid 
// to be written to mass storage, and the size information for the file to be recorded 
// in the storage directory if changed. 
// If the operation is successful, ior is zero.
// Otherwise, it is an implementation-defined I/O result code.
//
void flushFile() {
    FILE *fid = pop();

    push( fflush(fid));
}
// 
// stack : c-addr u1 fileid -- u2 ior
//
// Read u1 consecutive characters to c-addr from the current position of the file
// identified by fileid.  
// If u1 characters are read without an exception, ior is zero and u2 is equal to u1. 
// If the end of the file is reached before u1 characters are read, ior is zero and u2 is 
// the number of characters actually read. 
// If the operation is initiated when the value returned by FILE-POSITION is equal to the
// value returned by FILE-SIZE for the file identified by fileid, ior is zero and u2 is zero.
// If an exception occurs, ior is the implementation-defined I/O result code, and u2 is the
// number of characters transferred to c-addr without an exception. 
// An ambiguous condition exists if the operation is initiated when the value returned by
// FILE-POSITION is greater than the value returned by FILE-SIZE for the file identified by
// fileid, or if the requested operation attempts to read portions of the file not written. 
// At the conclusion of the operation, FILE-POSITION returns the next file position after the
// last character read.
//
void readFile() {
    FILE *fid = pop();
    int len = pop();
    char *ptr = (char *)pop();

    int res = fread(ptr,1, len, fid);
    push(res);
    push(errno);
}

void readLine() {
    FILE *fid = pop();
    int len = pop();
    char *ptr = (char *)pop();

    int res = fgets(ptr,len, fid);

    push(strlen(res));  // len
    push(-1);
    push(errno);
}

void writeLine() {
    FILE *fid = pop();
    int len = pop();
    char *ptr = (char *)pop();

    char eol = '\n';

    int res = fwrite(ptr,1, len, fid);
    res = fwrite(&eol,1, 1, fid);

    push(res);
    push(errno);
}

void closeFile() {
    FILE *fp=pop();

    push( fclose(fp));
}

#define FILE_DEPTH 10
/*
FILE *ifp[FILE_DEPTH];
int depth=-1;
*/

void includeFile() {
}
 
void c_endf() {
    if (depth < 0)
        return;
    fclose(ifp[depth]);

    if (depth == 0) {                             
        *puser(TKEY) = (CELL) & cf_akey;
    }
    depth--;
}

void c_endall() {
    while (depth >= 0)
        c_endf();
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


void c_getf() {
    char           *name;

    if (depth >= FILE_DEPTH - 1)
        uabort("file include too deep");
    c_parse();
    name = (char *) pop();
    ifp[depth + 1] = fopen(name, "r");  /* same mode for all ! */

    if (ifp[depth + 1] == NULL) {
        printf("can't open %s", name);
        uabort("file open error");
    }
    if (depth < 0) {
        *puser(TKEY) = (CELL) & cf_afkey;
    }
    depth++;
}

void c_string_getf(char *name) {

    int len;
    
    if (depth >= FILE_DEPTH - 1)
        uabort("file include too deep");
    
    ifp[depth + 1] = fopen(name, "r");  /* same mode for all ! */

    if (ifp[depth + 1] == NULL) {
        printf("can't open %s", name);
        uabort("file open error");
    }

    if (depth < 0)
        *puser(TKEY) = (CELL) & cf_afkey;
    depth++;
}
// 
// Stack : ud fileid -- ior
//
// Reposition the file identified by fileid to ud.
// ior is the implementation-defined I/O result code.
// An ambiguous condition exists if the file is positioned outside the file boundaries.
// At the conclusion of the operation, FILE-POSITION returns the value ud.
//
void repositionFile() {
    FILE *fid = pop();
    int pos = pop();

    (void)fseek(fid,(long)pos, SEEK_SET);

    push(errno);
}
// 
// Stack : fileid -- ud ior 
//
// ud is the current file position for the file identified by fileid.
// ior is the implementation-defined I/O result code.
// ud is undefined if ior is non-zero.
//
void filePosition() {
    FILE *fid = pop();

    push((CELL) ftell(fid));

    push(errno);
}
// 
// Stack : fileid -- ud ior 
//
// ud is the size, in characters, of the file identified by fileid.
// ior is the implementation-defined I/O result code.
// This operation does not affect the value returned by FILE-POSITION.
// ud is undefined if ior is non-zero.
// 
void fileSize() {
    FILE *fid = pop();

    struct stat st;

    int fd = fileno(fid);
    fstat(fd, &st);

    CELL sz = (CELL)st.st_size;

    push(sz);
    push(errno);

}

void deleteFile() {
    int len = pop();
    char *fname = (char *)pop();

    int ior = unlink( fname );

    push(ior);
}

void extend_file() {
    create_codeword("tst",c_tst,0);
    create_constant("r/o", RO);
    create_constant("r/w", RW);
    create_constant("w/o", WO);
    create_codeword("file-status",fileStatus,0);
    create_codeword("open-file",openFile,0);
    create_codeword("close-file",closeFile,0);
    create_codeword("create-file",createFile,0);
    create_codeword("read-file",readFile,0);
    create_codeword("write-file",writeFile,0);

    create_codeword("read-line",readLine,0);
    create_codeword("write-line",writeLine,0);
    create_codeword("flush-file",flushFile,0);
    create_codeword("include-file",includeFile,0);
    create_codeword("include",c_getf,0);
    create_codeword("reposition-file",repositionFile,0);
    create_codeword("file-position",filePosition,0);
    create_codeword("file-size",fileSize,0);
    create_codeword("delete-file",deleteFile,0);
}
