/*
 * Portable Forth in C ===================
 *
 *
 * (c) 1993,1996 Nigel Bromley: nib@dial.pipex.com or +44 7010 700NIB (700642).
 *
 * No warranties of any sort! Free use for non-commercial purposes only.
 * Author's copyright notice should appear with your own on any derived
 * versions.
 *
 * Forth.c - contains the global data, the headerless code bodies, and all of
 * the start-up and support functions.
 *
 * nib 1992-03-24 update nib 1993-06-16, 1996-01-21
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <unistd.h>

#include "forth.h"
#include "words.h"

char scratchString[SCRATCH_SIZE][255];

extern char    *optarg;
char           *prompt;

/*
 * -------------------------------------------------------------------------
 * Globals.
 */
int             go_flag, break_flag;
CODE_FIELD      cf_asdotq = c_asdotq;
CODE_FIELD    **ip;
CODE_FIELD      cf_aemit = c_aemit, cf_akey = c_akey, cf_afkey =
c_afkey,        cf_afemit = c_afemit, cf_ado = c_ado, cf_aloop =
c_aloop,        cf_aploop = c_aploop, cf_aslloop = c_aslloop, cf_aabortq =
c_aabortq,      cf_adotq = c_adotq, cf_lit = c_lit, cf_branch =
c_branch,       cf_zbranch = c_zbranch, cf_exit = c_exit;
BYTE           *latest, *dp, *k_dict, *k_last, *k_safe, *tib;
CELL           *sp, *rsp, *up, *pp, *stack, *rstack, init_forth, init_vocab;
jmp_buf         environment;
char            s[100];
/*
static char     buffer[255];
*/

#pragma weak extend_file
void extend_file() {
}

#pragma weak extend_memory
void extend_memory() {
}

#pragma weak extend_from_file
void extend_from_file() {
}

/*
 * -------------------------------------------------------------------------
 * Headerless code bodies.
 */

void 
c_docol()
{
	rpush((CELL) ip);
	ip = (CODE_FIELD **) pp;
}

void
c_stop()
{
	go_flag = FALSE;
}

void
c_dodoes()
{
	rpush((CELL) ip);
	ip = (CODE_FIELD **) * pp;
	push((CELL) (pp + 1));
}

void
c_dovar()
{
	push((CELL) pp);
}

void
c_douser()
{
	push((CELL) (up + *pp));
}

void
c_doconst()
{
	push(*pp);
}

void
c_dovoc()
{
	*puser(CONTEXT) = (CELL) (pp + 1);
}

/*
 * --------------------------------------------------------------------------
 * Support functions.
 */
CELL
pop(void)
{
	if ((BYTE *) sp >= (BYTE *) stack + STACK_SIZE) {
		uabort("** stack underflow");
		return 0;
	} else
		return *sp++;
}

void
push(CELL n)
{
	if (sp <= stack)
		uabort("** stack overflow");
	else
		*--sp = n;
}

CELL
rpop(void)
{
	if ((BYTE *) rsp >= (BYTE *) rstack + RSTACK_SIZE) {
		uabort("** return stack underflow");
		return 0;
	} else
		return *rsp++;
}

void
rpush(CELL n)
{
	if (rsp <= rstack)
		uabort("** return stack overflow");
	else
		*--rsp = n;
}

void
error(void)
{
	go_flag = FALSE;
	break_flag = TRUE;
	longjmp(environment, 6);
}

void
uabort(const char *mess)
{
	c_rpstor();
	c_spstor();
	c_lbrak();
	c_where();
	printf("\r\n%s", mess);
	c_endall();
	error();
}

void
user(CELL offset)
{
	push((CELL) (up + offset));
}

CELL           *
puser(CELL offset)
{
	return (CELL *) (up + offset);
}

void
align(BYTE ** p)
{
	while ((UCELL) * p & ALIGN)
		*(*p)++ = 0;
}

/*
 * --------------------------------------------------------------------------
 * Kernel dictionary creation functions.
 */
void
create_header(char *name, CODE_FIELD code, BYTE flags)
{
	BYTE           *t;
	CELL           *xx;

	t = latest;
	latest = dp;
	*dp++ = (BYTE) strlen(name) | NAME_END | flags;
	if (*name == 0)
		*dp++ = 0;	/* fiddle for null word */
	while (*name)
		*dp++ = *name++;
	*(dp - 1) |= NAME_END;
	align(&dp);
	xx = (CELL *) dp;
	*xx++ = (CELL) t;
	*xx++ = (CELL) code;
	dp = (BYTE *) xx;
}

void create_codeword(char *name, CODE_FIELD code, BYTE flags) {
	create_header(name, code, flags);
}

void
create_user(char *name, CELL offset, CELL init)
{
	CELL           *xx;

	create_header(name, c_douser, 0);
	xx = (CELL *) dp;
	*xx++ = offset;
	dp = (BYTE *) xx;
	*(up + offset) = init;
}

void
create_constant(char *name, CELL value)
{
	CELL           *xx;

	create_header(name, c_doconst, 0);
	xx = (CELL *) dp;
	*xx++ = value;
	dp = (BYTE *) xx;
}

void
create_vocab(char *name)
{
	CELL           *xx;

	create_header(name, c_dovoc, IMMED);
	*dp++ = 0x81;
	*dp++ = 0xa0;
	align(&dp);
	init_forth = (CELL) dp;
	xx = (CELL *) dp;
	*xx++ = (CELL) latest;
	init_vocab = (CELL) xx;
	*xx++ = 0;
	dp = (BYTE *) xx;
}

/*
 * --------------------------------------------------------------------------
 * General initialisation.
 */
void
init(void)
{
	break_flag = FALSE;
	go_flag = FALSE;
}

/*
 * --------------------------------------------------------------------------
 * Initialisation for building the kernel dictionary.
 */
void
init_for_build(void)
{
	latest = 0;
	dp = k_dict;
}

/*
 * --------------------------------------------------------------------------
 * Special quit routine. This is the only infinite loop, doing get line and
 * then execute it. The only 'quit' available to the user sets flags to come
 * back into this one. Only way out of this loop is 'exit' done by 'bye'.
 */
void
outer_quit(char *fname)
{
	extern int      verbose;
	int             init = 0;
	char           *ptr;
    int ret=0;

	c_cold();
	setjmp(environment);
    
	for (;;) {
		c_rpstor();
		if (verbose) {
			//printf("\r\nok> ");
			printf("\r\n%s ", prompt);
		}


		if (init > 0) {
			c_query();
		}
#ifdef LINUX
#warning "LINUX"
		if (init == 0) {
			ptr = (char *) getenv("FORTHRC");
			if (!ptr) {
				ptr = "/dev/null";
			} else {
				c_string_getf(ptr);
			}

			if (strlen(fname) > 0)
				c_string_getf(fname);

//			init++;
		}
#endif
        init=1;

		c_interpret();
	}
}

/*
    Usage
*/

void
usage()
{
	printf("cforth <-h|-?> <-v> <-q> <-f fname>\n");
}

/*
 * --------------------------------------------------------------------------
 * Main.
 */
int
main(int argc, char **argv)
{
	int             ch;
	extern int      verbose;
	char            fname[255];

	prompt = (char *) malloc(32);
	fname[0] = (char) NULL;

	while ((ch = getopt(argc, argv, "vh?af:q")) != -1) {
		switch (ch) {
		case 'h':
		case '?':
			usage();
			exit(0);
			break;
		case 'q':
			verbose = 0;
			break;
		case 'f':
			strcpy(fname, optarg);
			break;
		case 'v':
			c_banner();
			exit(0);
		}
	}
    //
	//ATH
    //
    strcpy(prompt, "ok->");

	k_dict = (BYTE *) malloc(KERNEL_SIZE);
	if (k_dict == NULL) {
		printf("Can't allocate workspace (%u)\n", KERNEL_SIZE);
		return 1;
	}
	stack = (CELL *) malloc(STACK_SIZE);
	if (stack == NULL) {
		printf("Can't allocate stack (%u)\n", STACK_SIZE);
		return 2;
	}
	rstack = (CELL *) malloc(RSTACK_SIZE);
	if (rstack == NULL) {
		printf("Can't allocate rstack (%u)\n", RSTACK_SIZE);
		return 3;
	}
	up = (CELL *) malloc(USER_SIZE);
	if (up == NULL) {
		printf("Can't allocate user area (%u)\n", USER_SIZE);
		return 3;
	}
	tib = (BYTE *) malloc(TIB_SIZE);
	if (tib == NULL) {
		printf("Can't allocate tib (%u)\n", TIB_SIZE);
		return 4;
	}
	init();
	init_for_build();
	create_kernel();
    extend_file();
    extend_memory();

    extend_from_file();
#if UNIX
	SET_RAW;
#endif

	outer_quit(fname);

	return 0;
}
