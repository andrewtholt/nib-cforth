/*
		Portable Forth in C
		===================


(c) 1993,1996 Nigel Bromley: nib@dial.pipex.com or +44 7010 700NIB (700642).

No warranties of any sort! Free use for non-commercial purposes only.
Author's copyright notice should appear with your own on any derived
versions.

Forth.h - contains the main switches for various versions, prototypes for
all external functions other than the code bodies, global variables.

Notes:
	1) CELL must be signed.
	2) CELL, code pointers (CODE_FIELD) and data pointers (CELL *)
		must all be the same size.
	3) for vocabularies to work (around the area of building '81a0'
		correctly on big/little-endian and different word sizes)
		alignment must be the same as the CELL size.

nib 1992-03-20
update nib 1993-07-03, 1996-01-21
*/

#define INTEL16 0	/* 16-bit data, all near pointers, small model  */
#define INTEL32 1	/* 32-bit data, all far pointers, large model     */
#define CLEAN32 0	/* proper 32-bit data, 32-bit address             */
#define DOS 0
#define UNIX 1

#ifdef UNIX
/*
#define FILE_ACCESS 1
*/
#endif

#if INTEL16
	typedef int CELL;
	typedef unsigned int UCELL;
	typedef unsigned int WORD;
	typedef unsigned char BYTE;
	typedef void (*CODE_FIELD)(void);
	#define ALIGN 0x1                   /* mask of bits to be zero */
	#define CELL_SIZE 2                 /* bytes in the Forth single */
	#define KERNEL_SIZE 40000u
	#define STACK_SIZE 1024*CELL_SIZE
	#define RSTACK_SIZE 1024*CELL_SIZE
	#define USER_SIZE 256*CELL_SIZE
	#define TIB_SIZE 200
#endif

#if INTEL32
	typedef long CELL;
	typedef unsigned long UCELL;
	typedef unsigned int WORD;
	typedef unsigned char BYTE;
	typedef void (*CODE_FIELD)(void);
	#define ALIGN 0x3                   /* mask of bits to be zero */
	#define CELL_SIZE 4                 /* bytes in the Forth single */
	#define KERNEL_SIZE 60000u
	#define STACK_SIZE 1024*CELL_SIZE
	#define RSTACK_SIZE 1024*CELL_SIZE
	#define USER_SIZE 256*CELL_SIZE
	#define TIB_SIZE 200
#endif

#if CLEAN32
	typedef int CELL;
	typedef unsigned int UCELL;
	typedef unsigned short WORD;
	typedef unsigned char BYTE;
	typedef void (*CODE_FIELD)(void);
	#define ALIGN 0x3                   /* mask of bits to be zero */
	#define CELL_SIZE 4                 /* bytes in the Forth single */
	#define KERNEL_SIZE 100000
	#define STACK_SIZE 1024*CELL_SIZE
	#define RSTACK_SIZE 1024*CELL_SIZE
	#define USER_SIZE 256*CELL_SIZE
	#define TIB_SIZE 200
#endif

#if UNIX
#define SET_RAW system("stty raw -echo isig")
#define SET_NORMAL system("stty sane")
#define FILE_READ_MODE "r"
#define FILE_WRITE_MODE "w"
#define FILE_APPEND_MODE "a"
// #define DB
#endif
#if DOS
#define FILE_READ_MODE "rb"
#define FILE_WRITE_MODE "wb"
#define FILE_APPEND_MODE "ab"
#endif

/* -------------------------------------------------------------------------
Definitions.
*/

#define SAFETY 256
#define FALSE 0
#define TRUE 1
#define SMUDGE 0x20
#define IMMED 0x40
#define NAME_END 0x80
#define BL 0x20
#define BCKSP 0x08
#define BCKCUR 0x08
#define BELL 0x07
#define CR 0x0d
#define LF 0x0a
#define RUBOUT 0x7f

#define SP0 0
#define R0 1
#define TIB 2
#define WIDTH 3
#define WARNING 4
#define FENCE 5
#define DP 6
#define VOC_LINK 7
#define TEMIT 8
#define TKEY 9
#define PIN 10
#define BASE 11
#define CONTEXT 12
#define CSP 13
#define CURRENT 14
#define DPL 15
#define HLD 16
#define OUT 17
#define STATE 18

/* -------------------------------------------------------------------------
Globals
*/
extern int go_flag, break_flag;
extern CODE_FIELD **ip;
extern CELL *sp, *rsp, *up, *pp, *stack, *rstack, init_forth, init_vocab;
extern BYTE *dp, *tib, *latest, *k_dict;
extern CODE_FIELD cf_asdotq;
extern CODE_FIELD cf_aemit, cf_akey, cf_afkey, cf_afemit,
	cf_ado, cf_aloop, cf_aploop, cf_aslloop, cf_aabortq,
	cf_adotq, cf_lit, cf_branch, cf_zbranch, cf_exit;

#define SCRATCH_SIZE 4 /* this must be a power of 2 e.g. 2,4,8,16,... */
char scratchString[SCRATCH_SIZE][255];


/* -------------------------------------------------------------------------
Function prototypes
*/
void c_docol(void);
void c_stop(void);
void c_dodoes(void);
void c_dovar(void);
void c_douser(void);
void c_doconst(void);
void c_dovoc(void);
void push(CELL n);
CELL pop(void);
void rpush(CELL n);
CELL rpop(void);
void align(BYTE **p);
void align_ptr(void);
void create_var(char *name, CELL init);
void create_constant(char *name, CELL value);
void create_user(char *name, CELL offset, CELL init);
void create_codeword(char *name, CODE_FIELD func, BYTE flags);
void create_vocab(char *name);
void create_kernel(void);
void uabort(char *mess);
void error(void);
void user(CELL offset);
CELL *puser(CELL offset);
void string(void);
void countedString(void);
void c_dump(void);
/*
#ifdef DB
void dbBucketStats(void);
void dbCreate(void);
void dbGrow(void);
void dbOccupancy(void);
void dbSetAlarmLevel(void);
void dbGetAlarmLevel(void);
void dbSetFlags(void);
void dbSetMaxRecords(void);
void dbSetNameSize(void);
void dbSetDefSize(void);
void dbGetName(void);
void dbGetDef(void);
void dbSetName(void);
void dbStatus(void);
void dbSetDef(void);
void dbUpdate(void);
void dbLookup(void);
void dbDump(void);
void dbLoad(void);
void dbInstall(void);
void dbDisplayRecord(void);
void dbDebugDump(void);
void dbFindFirst(void);
void dbFindNext(void);
void dbFindFirstDef(void);
void dbFindNextDef(void);
void dbDelete(void);

#endif
*/
//
// ATH
// 
#if EMBEDDED
#define PUTC putc
#define GETC getc
#define UDELAY udelay
#define STRTOL simple_strtol
#else
#define PUTC putchar
#endif



