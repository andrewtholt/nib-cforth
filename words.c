/*
 * Portable Forth in C ===================
 * 
 * (c) 1993,1996 Nigel Bromley: nib@dial.pipex.com or +44 7010 700NIB (700642).
 * 
 * No warranties of any sort! Free use for non-commercial purposes only.
 * Author's copyright notice should appear with your own on any derived
 * versions.
 * 
 * Words.c - contains code bodies for all functions implementing Forth words
 * contained in the kernel dictionary.
 * 
 * Update the title! It's in c_cold.
 * 
 * nib 1992-06-24 update nib 1993-07-03, 1996-01-21
 * 
 * Known bugs: c, does not work on machines requiring word/double alignment
 * (stores ok but makes dp odd so that using end of dict as temp store breaks
 * ?? ).  There's possibly no solution to this. no forget. no ?terminal on
 * unix.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <string.h>
#include <dlfcn.h>

#include "words.h"
#include "forth.h"
#include "cpu.h"
#include "os.h"

#if DOS
#include <dos.h>
#define READ_KEYB_SUBFUNC 0x00
#define STAT_KEYB_SUBFUNC 0x01
#define READ_KEYB_INT 0x16
#define Z_FLAG 0x0040
union REGS      regs;       /* for BIOS call */
#endif

#define HALF_MASK ((UCELL)(~0) >> CELL_SIZE*4)
#define HALF_SHIFT (CELL_SIZE*4)
#define TOP_BIT ((UCELL)1 << (CELL_SIZE*8 - 1))

/* file-handling globals */
#define FILE_DEPTH 10
FILE           *ifp[FILE_DEPTH];
int             depth = -1;
extern char *prompt;

/* ... ATH Other globals */
int             verbose = 1;
int selector=0;

int c_prim_evaluate(char *cmd);

void
c_bye()
{
#if UNIX
    SET_NORMAL;
#endif
//  printf("\r\nCheerio!\r\n");
    printf("\r\n");
    exit(0);
}

void
c_xor()
{
    push(pop() ^ pop());
}

void
c_uslmod()
{
    UCELL           xh, xl, yh, yl, q, n;
    yh = (UCELL) pop();
    xh = (UCELL) pop();
    xl = (UCELL) pop();
    if (yh == 0)
    {
        push(-1);
        c_dup();
        return;
    }
    n = CELL_SIZE * 8 + 1;
    while (!(yh & TOP_BIT))
    {
        yh <<= 1;
        n++;
    }
    yl = q = 0;
    while (n--)
    {
        if ((xh > yh) || (xh == yh) && (xl >= yl))
        {
            push((CELL) xl);
            push((CELL) xh);
            push((CELL) yl);
            push((CELL) yh);
            c_dnegate();
            c_dplus();
            xh = (UCELL) pop();
            xl = (UCELL) pop();
            q = (q << 1) | 1;
        } else
        {
            q <<= 1;
        }
        yl = (yl >> 1) | ((yh & 1) ? TOP_BIT : 0);
        yh >>= 1;
    }
    push((CELL) xl);
    push((CELL) q);
}

void
c_ustar()
{
    UCELL           x, y, a, b, c, d;
    x = (UCELL) pop();
    y = (UCELL) pop();
    a = (x & HALF_MASK) * (y & HALF_MASK);
    b = (x >> HALF_SHIFT) * (y & HALF_MASK);
    c = (x & HALF_MASK) * (y >> HALF_SHIFT);
    d = (x >> HALF_SHIFT) * (y >> HALF_SHIFT);
    push((CELL) a);
    push(0);
    push((CELL) (b << HALF_SHIFT));
    push((CELL) (b >> HALF_SHIFT));
    c_dplus();
    push((CELL) (c << HALF_SHIFT));
    push((CELL) (c >> HALF_SHIFT));
    c_dplus();
    push(0);
    push((CELL) d);
    c_dplus();
}

void
c_toggle()
{
    CELL            x;
    x = pop();
    *(BYTE *) pop() ^= (BYTE) x;
}

void
c_swap()
{
    CELL            x, y;
    x = pop();
    y = pop();
    push(x);
    push(y);
}

// ATH

void c_setprompt() {
    char *ptr;
    char *tmp;
    int len;

    len=(int)pop();

    if(len > 32) 
        len=32;

    bzero(prompt,32);
    ptr=(char *) pop();
    tmp=strncpy(prompt,ptr,len);

}
void
c_spat()
{
    push((CELL) sp);
}

void
c_spstor()
{
    sp = (CELL *) * puser(SP0);
}

void
c_stod()
{
    CELL            x;
    x = pop();
    push(x);
    if (x < 0)
        push(-1);
    else
        push(0);
}

void
c_rpat()
{
    push((CELL) rsp);
}

void c_rpstor() {
    rsp = (CELL *) * puser(R0);
}

void
c_rot()
{
    CELL            x, y, z;
    x = pop();
    y = pop();
    z = pop();
    push(y);
    push(x);
    push(z);
}

void c_min_rot(void)
{
    CELL x,y,z;
    x=pop();
    y=pop();
    z=pop();

    push(x);
    push(z);
    push(y);
}

void
c_rat()
{
    push(*rsp);
}

void
c_rfrom()
{
    push(rpop());
}

#if INTEL16 || INTEL32 && DOS
void
c_portat()
{
    push((CELL) inportb(pop()));
}

void
c_portstor()
{
    CELL            padd;
    padd = pop();
    outportb(padd, (BYTE) pop());
}

void
c_portwat()
{
    push(inport(pop()));
}

void
c_portwstor()
{
    CELL            padd;
    padd = pop();
    outport(padd, pop());
}
#endif

void
c_over()
{
    CELL            x, y;
    x = pop();
    y = pop();
    push(y);
    push(x);
    push(y);
}

void
c_or()
{
    push(pop() | pop());
}

void
c_not()
{
    if (pop())
        push(0);
    else
        push(1);
}

void
c_negate()
{
    push(-pop());
}

void
c_lit()
{
    CELL           *xx;
    xx = (CELL *) ip;
    push(*xx++);
    ip = (CODE_FIELD **) xx;
}

void
c_leave()
{
    CELL            t;
    t = rpop();
    rpop();
    rpush(t);
    rpush(t);
}

void
c_j()
{
    push(*(rsp + 2));
}

void
c_itick()
{
    push(*(rsp + 1));
}

void
c_i()
{
    push(*rsp);
}

void
c_exit()
{
    ip = (CODE_FIELD **) rpop();
}

void
c_execute()
{
    CODE_FIELD     *targ_cfa, stop_dummy, *exec_frag[2];

    if (go_flag)
    {
        targ_cfa = (CODE_FIELD *) pop();
        pp = (CELL *) targ_cfa + 1;
        (**targ_cfa) ();
    } else
    {
        stop_dummy = c_stop;
        exec_frag[0] = (CODE_FIELD *) pop();
        exec_frag[1] = &stop_dummy;
        ip = exec_frag;
        go_flag = TRUE;

        while (go_flag)
        {
            pp = (*(CELL **) ip + 1);
            (***ip++) ();
        }
    }
}

void c_evaluate()
{
    int len,s;
    char *cmd;

    len=(int)pop();
    cmd = (char *)pop();
    s = c_prim_evaluate(cmd);
    push(s);

}

int c_prim_evaluate(char *cmd)
{
    int len;
    int status = 0;

    char buffer[256];

    len=(int)strlen(cmd);

    buffer[0] = len & 0x7f;
    strcpy(&buffer[1], cmd);
    push((CELL)buffer);
    user(CONTEXT);
    c_at();
    c_at();
    c_afind();
    
    status=pop();
    if( status != 0 ) {
        c_drop();
        c_cfa();
        c_execute();
    }
    return(status);
}

void c_my_evaluate(char *ptr) {
//   printf("%s\n",ptr);
    /*
    
    memset(tib,255,0x20);
    push(strlen(ptr));
    strncpy(tib,ptr,255);
*/
    c_interpret();
}

/*
void c_servent()
{
    int len;
    int status = 0;
    char *cmd;
    char buffer[1024];
    FILE *data;
    
    len = pop();
    cmd = (char *)pop();
    data = popen( cmd,"r");
    while(fgets(buffer,1024,data))
    {
//        printf("%s",buffer);
        buffer[ strlen(buffer)-1 ] =0x00;
        c_prim_evaluate(buffer);
    }
    pclose(data);
    
}
*/
void
c_enclose()
{
    BYTE            ch, *addr, *c1, *c2;
    ch = pop();
    addr = (BYTE *) pop();
    push((CELL) addr);
    c1 = addr;
    while (*c1 == ch)
        c1++;
    if (*c1 == 0)
    {
        push((CELL) (c1 - addr));
        push((CELL) (c1 - addr + 1));
        push((CELL) (c1 - addr));
        return;
    }
    c2 = c1;
    while (*c2 != 0 && *c2 != ch)
        c2++;
    if (*c2 == 0)
    {
        push((CELL) (c1 - addr));
        push((CELL) (c2 - addr));
        push((CELL) (c2 - addr));
    } else
    {
        push((CELL) (c1 - addr));
        push((CELL) (c2 - addr));
        push((CELL) (c2 - addr + 1));
    }
}

void
c_dup()
{
    CELL            x;
    x = pop();
    push(x);
    push(x);
}

void
c_drop()
{
    pop();
}

void
c_dnegate()
{
    CELL            x, y;
    x = ~pop();
    y = ~pop() + 1;
    push(y);
    if (y == 0)
        push(x + 1);
    else
        push(x);
}

void
c_digit()
{
    CELL            x, y;
    x = pop();
    y = pop() & 0xff;
    if (y >= 'a' && y <= 'z')
        y -= 'a' - 'A';
    if (y < '0' || (y > '9' && y < 'A'))
    {
        push(0);
        return;
    }
    y -= '0';
    if (y > 9)
        y -= 'A' - '9' - 1;
    if (y >= x)
        push(0);
    else
    {
        push(y);
        push(1);
    }
}

void
c_ddup()
{
    CELL            x, y;
    x = pop();
    y = pop();
    push(y);
    push(x);
    push(y);
    push(x);
}

void
c_ddrop()
{
    pop();
    pop();
}

void
c_dplus()
{
    UCELL           xl, xh, yl, yh, tl;
    xh = (UCELL) pop();
    xl = (UCELL) pop();
    yh = (UCELL) pop();
    yl = (UCELL) pop();
    push((CELL) (tl = xl + yl));
    if ((xl & yl & TOP_BIT) ||
        (((xl | yl) & TOP_BIT) && !(tl & TOP_BIT)))
        push((CELL) xh + yh + 1);
    else
        push((CELL) xh + yh);
}

void
c_dstor()
{
    CELL           *xx;
    xx = (CELL *) pop();
    *xx++ = pop();
    *xx = pop();
}

void
c_cat()
{
    BYTE           *xx;
    xx = (BYTE *) pop();
    push(*xx);
}

void
c_wat()
{
    WORD           *xx;
    xx = (WORD *) pop();
    push(*xx);
}

void
c_cstor()
{
    CELL            x;
    x = pop();
    *(BYTE *) x = pop();
}

void
c_wstor()
{
    CELL            x;
    x = pop();
    *(WORD *) x = pop();
}

void
c_zbranch()
{
    if (pop() == 0)
        c_branch();
    else
        ip++;
}

void
c_branch()
{
    ip += (*(CELL *) ip) / CELL_SIZE;
}

void
c_and()
{
    push(pop() & pop());
}

void
c_at()
{
    push(*(CELL *) pop());
}

void
c_tor()
{
    rpush(pop());
}

void
c_aloop()
{
    push(1);
    c_aploop();
}

void
c_aploop()
{
    CELL            del, i, lim;
    del = pop();
    i = rpop() + del;
    lim = rpop();
    if ((del >= 0) && (i < lim) || (del < 0) && (i > lim))
    {
        rpush(lim);
        rpush(i);
        ip += (*(CELL *) ip) / CELL_SIZE;
    } else
        ip++;
}

void
c_afind()
{
    BYTE           *p, *q, y, f, l, n;
    p = (BYTE *) pop();
    q = (BYTE *) pop();
    f = 0;
    while (p)
    {
        l = *p & 0x1f;
        n = (l == 0) ? 1 : l;
        if ((*q & 0x3f) == (*p & 0x3f))
        {
            f = 1;
            for (y = 1; y <= l; y++)
                if ((*(p + y) & 0x7f) != (*(q + y) & 0x7f))
                    f = 0;
            if (f)
            {
                break;
            }
        }
        p = (BYTE *) (((CELL) (p + n + 1) + ALIGN) & ~ALIGN);
        p = (BYTE *) * (CELL *) p;
    }
    if (f)
    {
        push((((CELL) p + n + 1 + 2 * CELL_SIZE + ALIGN) & ~ALIGN));
        push(*p);
    }
    push(f);
}

void
c_afill()
{
    UCELL           y;
    BYTE            x, *z;
    x = (BYTE) pop();
    y = (UCELL) pop();
    z = (BYTE *) pop();
    while (y--)
        *z++ = x;
}

void
c_ado()
{
    CELL            x;
    x = pop();
    rpush(pop());
    rpush(x);
}

void
c_acmove()
{
    UCELL           c;
    BYTE           *f, *t;
    c = (UCELL) pop();
    t = (BYTE *) pop();
    f = (BYTE *) pop();
    while (c--)
        *t++ = *f++;
}

void
c_alcmove()
{
    UCELL           c;
    BYTE           *f, *t;
    c = (UCELL) pop();
    t = (BYTE *) pop() + c;
    f = (BYTE *) pop() + c;
    while (c--)
        *(--t) = *(--f);
}

void
c_aslloop()
{
    UCELL           del, i, lim;
    del = pop();
    i = rpop() + del;
    lim = rpop();
    if (i < lim)
    {
        rpush(lim);
        rpush(i);
        ip += (*(CELL *) ip) / CELL_SIZE;
    } else
        ip++;
}

void
c_less()
{
    CELL            x;
    x = pop();
    if (pop() < x)
        push(1);
    else
        push(0);
}

void
c_2minus()
{
    push(pop() - 2);
}

void
c_2plus()
{
    push(pop() + 2);
}

void
c_2star()
{
    push(pop() * 2);
}

void
c_1minus()
{
    push(pop() - 1);
}

void
c_1plus()
{
    push(pop() + 1);
}

void
c_zless()
{
    if (pop() < 0)
        push(1);
    else
        push(0);
}

void
c_minus()
{
    CELL            x;
    x = pop();
    push(pop() - x);
}

void
c_pstor()
{
    CELL            x;
    x = pop();
    *(CELL *) x += pop();
}

void
c_plus()
{
    push(pop() + pop());
}

void
c_stor()
{
    CELL            x;
    x = pop();
    *(CELL *) x = pop();
}

void
c_hdot()
{
#if INTEL32
    printf(" %lx ", (long) pop());
#else
    printf(" %x ", pop());
#endif
}

void
c_rbrak()
{
    push(0xc0);
    user(STATE);
    c_stor();
}

void
c_lbrak()
{
    push(0);
    user(STATE);
    c_stor();
}

void
c_traverse()
{
    c_swap();
    do
    {
        c_over();
        c_plus();
        push(0x7f);
        c_over();
        c_cat();
        c_less();
    }
    while (!pop());
    c_swap();
    c_drop();
}

void
c_s0()
{
    user(SP0);
    c_at();
}

void
c_pick()
{
    c_dup();
    push(1);
    c_less();
    if (pop())
        uabort("pick argument < 1");
    else
    {
        push(CELL_SIZE);
        c_star();
        c_spat();
        c_plus();
        c_at();
    }
}

void
c_max()
{
    c_ddup();
    c_less();
    if (pop())
        c_swap();
    c_drop();
}

void
c_mslmod()
{
    c_tor();
    push(0);
    c_rat();
    c_uslmod();
    c_rfrom();
    c_swap();
    c_tor();
    c_uslmod();
    c_rfrom();
}

void
c_mplus()
{
    c_stod();
    c_dplus();
}

void
c_lfa()
{
    push(2 * CELL_SIZE);
    c_minus();
}

void
c_latest()
{
    user(CURRENT);
    c_at();
    c_at();
}

void
c_key()
{
    (*(CODE_FIELD) * (CELL *) * puser(TKEY)) ();
}

void
c_hold()
{
    push(-1);
    user(HLD);
    c_pstor();
    user(HLD);
    c_at();
    c_cstor();
}

void
c_hex()
{
    push(16);
    user(BASE);
    c_stor();
}

void
c_here()
{
    user(DP);
    c_at();
}

void
c_emit()
{
    (*(CODE_FIELD) * (CELL *) * puser(TEMIT)) ();
}

void
c_definitions()
{
    user(CONTEXT);
    c_at();
    user(CURRENT);
    c_stor();
}

void
c_decimal()
{
    push(10);
    user(BASE);
    c_stor();
}

void
c_dplusm()
{
    c_zless();
    if (pop())
        c_dnegate();
}

void
c_count()
{
    c_dup();
    c_1plus();
    c_swap();
    c_cat();
}

void
c_convert()
{
    for (;;)
    {
        c_1plus();
        c_dup();
        c_tor();
        c_cat();
        user(BASE);
        c_at();
        c_digit();
        if (!pop())
            break;
        c_swap();
        user(BASE);
        c_at();
        c_ustar();
        c_drop();
        c_rot();
        user(BASE);
        c_at();
        c_ustar();
        c_dplus();
        user(DPL);
        c_at();
        c_1plus();
        if (pop())
        {
            push(1);
            user(DPL);
            c_pstor();
        }
        c_rfrom();
    }
    c_rfrom();
}

void
c_cmove()
{
    c_dup();
    push(1);
    c_less();
    if (pop())
    {
        c_ddrop();
        c_drop();
    } else
        c_acmove();
}

void
c_cfa()
{
    push(CELL_SIZE);
    c_minus();
}

void
c_allot()
{
    c_dup();
    if ((UCELL) * puser(DP) + pop() + SAFETY > (UCELL) k_dict + KERNEL_SIZE)
        uabort("no room in dictionary");
    user(DP);
    c_pstor();
}

void
c_qdup()
{
    c_dup();
    if (pop())
        c_dup();
}

void
c_greater()
{
    c_swap();
    c_less();
}

void
c_equal()
{
    c_minus();
    c_not();
}

void
c_lcmove()
{
    c_dup();
    push(1);
    c_less();
    if (pop())
    {
        c_ddrop();
        c_drop();
    } else
        c_alcmove();
}

void
c_zequal()
{
    c_not();
}

void
c_mtrailing()
{
    CELL            x, y;
    c_dup();
    x = pop();
    for (y = 0; y < x; y++)
    {
        c_ddup();
        c_plus();
        c_1minus();
        c_cat();
        push(BL);
        c_minus();
        if (pop())
            break;
        else
            c_1minus();
    }
}

void
c_plusm()
{
    c_zless();
    if (pop())
        c_negate();
}

void
c_star()
{
    push(pop() * pop());
}

void
c_tstream()
{
    user(TIB);
    c_at();
    user(PIN);
    c_at();
    c_plus();
}

void
c_qcomp()
{
    user(STATE);
    c_at();
    c_not();
    if (pop())
        uabort("compile only");
}

void
c_qcsp()
{
    c_spat();
    user(CSP);
    c_at();
    c_minus();
    if (pop())
        uabort("definition not finished");
}

void c_aemit() {

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

void
c_akey()
{
#if UNIX
    push(getchar());
#endif
#if DOS
    regs.h.ah = READ_KEYB_SUBFUNC;
    int86(READ_KEYB_INT, &regs, &regs);
    push(regs.h.al);
#endif
#if EMBEDDED
    push(GETC());
#endif
}

void
c_qterminal() {
#if UNIX
    uabort("?terminal for Unix not yet implemented");
#endif
#if DOS
    regs.h.ah = STAT_KEYB_SUBFUNC;
    int86(READ_KEYB_INT, &regs, &regs);
    if (regs.x.flags & Z_FLAG) {
        push(0);
    } else {
        c_key();
        c_drop();
        push(1);
    }
#endif

#if EMBEDDED
    #warning "EMBEDDED"
    uabort("?terminal for EMBEDDED not yet implemented");
#endif
}

void
c_qstream()
{
    if (pop())
        uabort("input stream exhausted");
}

void
c_qpairs()
{
    c_minus();
    if (pop())
        uabort("conditionals not paired");
}

void
c_word()
{
    c_tstream();
    c_swap();
    c_enclose();
    c_ddup();
    c_greater();
    if (pop())
    {
        c_ddrop();
        c_ddrop();
        push(0);
        c_here();
        c_stor();
    } else
    {
        user(PIN);
        c_pstor();
        c_over();
        c_minus();
        c_dup();
        c_tor();
        c_here();
        c_cstor();
        c_plus();
        c_here();
        c_1plus();
        c_rfrom();
        c_dup();
        push(0xff);
        c_greater();
        if (pop())
            uabort("input > 255");
        c_1plus();
        c_cmove();
    }
    c_here();
}

void
c_space()
{
    push(BL);
    c_emit();
}

void
c_smudge()
{
    c_latest();
    push(SMUDGE);
    c_toggle();
}

void
c_sign()
{
    c_zless();
    if (pop())
    {
        push('-');
        c_hold();
    }
}

void c_query() {
    user(TIB);
    c_at();
    push(TIB_SIZE - 20);
    c_expect();
    push(0);
    user(PIN);
    c_stor();
}

void
c_pfa()
{
    push(1);
    c_traverse();
    c_1plus();
    c_align_ptr();
    push(2 * CELL_SIZE);
    c_plus();
}

void
c_pad()
{
    c_here();
    push(0x44);
    c_plus();
}

void
c_number()
{
    push(0);
    push(0);
    c_rot();
    c_dup();
    c_1plus();
    c_cat();
    push('-');
    c_equal();
    c_dup();
    c_tor();
    c_plus();
    push(-1);
    user(DPL);
    c_stor();
    c_convert();
    c_dup();
    c_cat();
    push(BL);
    c_greater();
    if (pop())
    {
        c_dup();
        c_cat();
        push('.');
        c_equal();
        c_not();
        if (pop())
            uabort("not recognised");
        push(0);
        user(DPL);
        c_stor();
        c_convert();
        c_dup();
        c_cat();
        push(BL);
        c_greater();
        if (pop())
            uabort("not recognised");
    }
    c_drop();
    c_rfrom();
    if (pop())
        c_dnegate();
}

void
c_nfa()
{
    BYTE           *p;
    p = (BYTE *) pop() - 2 * CELL_SIZE - 1;
    while (*p == 0)
        p--;
    push((CELL) p);
    push(-1);
    c_traverse();
}

void
c_move()
{
    push(0);
    c_max();
    c_2star();
    c_acmove();
}

void
c_min()
{
    c_ddup();
    c_greater();
    if (pop())
        c_swap();
    c_drop();
}

void
c_immediate()
{
    c_latest();
    push(0x40);
    c_toggle();
}

void
c_find()
{
    c_mfind();
    if (pop())
    {
        c_drop();
        c_cfa();
    } else
        push(0);
}

void c_expect() {

    CELL            x, y;
    BYTE           *p, *q;

    x = pop();
    q = p = (BYTE *) pop();

    while (q < p + x) {
        c_key();
        y = pop();

        if (y == BCKSP || y == RUBOUT) {
            if (p == q) {
                push(BELL);
            } else {
                q--;
                push(BCKCUR);
                c_dup();
                c_emit();
                push(BL);
                c_emit();
                *puser(PIN) -= 3;
            }
        } else {
            if (y == CR) {
                push(BL);
                *q++ = 0;
                *q = 0;
                q = p + x;
            } else {
                push(y);
                *q++ = y;
                *q = 0;
            }
        }
        c_emit();
    }
}

void c_accept()
{
    char *ptr;
    int len=0;
    CELL            x, y;
    BYTE           *p, *q;

    c_over();
    c_swap();
    x = pop();
    q = p = (BYTE *) pop();
    while (q < p + x)
    {
        if(len < 0)
            len=0;

        c_key();
        y = pop();
        if (y == BCKSP || y == RUBOUT)
        {
            if (p == q)
            {
                push(BELL);
            } else
            {
                q--;
                push(BCKCUR);
                c_dup();
                c_emit();
                push(BL);
                c_emit();
                *puser(PIN) -= 3;
                len--;
            }
        } else
        {
            if (y == CR)
            {
                push(BL);
                q = p + x;
            } else
            {
                push(y);
                *q++ = y;
                len++;
            }
        }
        c_emit();
    }

    ptr=(char *)pop();
    /*
    len=strlen(ptr);
    */
    push(len);
}

void
c_dless()
{
    c_rot();
    c_ddup();
    c_equal();
    if (pop())
    {
        c_rot();
        c_rot();
        c_dnegate();
        c_dplus();
        c_zless();
    } else
    {
        c_swap();
        c_less();
        c_swap();
        c_drop();
    }
    c_swap();
    c_drop();
}

void
c_cr()
{
    push(CR);
    c_emit();
    push(LF);
    c_emit();
    push(0);
    user(OUT);
    c_stor();
}

void c_banner() {

    printf("\r\nNigel's Portable Forth (2)\r\n");
    printf("(c) 1992, 1996 Nigel Bromley +44 7010 700642\r\n");
    printf("Andrew Holt build:%s\r\n",__DATE__);
}

void c_cold()
{
    *puser(SP0) = (CELL) ((BYTE *) stack + STACK_SIZE);
    *puser(R0) = (CELL) ((BYTE *) rstack + RSTACK_SIZE);
    *puser(TIB) = (CELL) tib;
    *puser(WIDTH) = 31;
    *puser(WARNING) = 1;
    *puser(FENCE) = (CELL) dp;
    *puser(DP) = (CELL) dp;
    *puser(TEMIT) = (CELL) & cf_aemit;
    *puser(TKEY) = (CELL) & cf_akey;
    *puser(CURRENT) = init_forth;
    *puser(CONTEXT) = init_forth;
    *puser(VOC_LINK) = init_vocab;
    *(CELL *) init_forth = (CELL) latest;

    if (verbose != 0)
    {
        c_banner();
        /*
        printf("\r\nNigel's Portable Forth (2)\r\n");
        printf("(c) 1992, 1996 Nigel Bromley +44 7010 700642\r\n");
        printf("4A Solutions Ltd build:%s\r\n",__DATE__);
        */
    }

    c_spstor();
    c_lbrak();
    c_rpstor();
    c_hex();
}

void
c_then()
{
    c_qcomp();
    push(2);
    c_qpairs();
    c_here();
    c_over();
    c_minus();
    c_swap();
    c_stor();
}

void
c_ccomma()
{
    c_here();
    c_cstor();
    push(1);
    c_allot();
}

void
c_begin()
{
    c_qcomp();
    c_here();
    push(1);
}

void
c_abs()
{
    c_dup(), c_plusm();
}

void
c_zgreater()
{
    push(0);
    c_greater();
}

void
c_mfind()
{
    push(BL);
    c_word();
    user(CONTEXT);
    c_at();
    c_at();
    c_afind();
}

void
c_comma()
{
    c_here();
    c_stor();
    push(CELL_SIZE);
    c_allot();
}

void
c_lpara()
{
    push(-1);
    user(PIN);
    c_pstor();
    push(')');
    c_word();
    c_cat();
    c_1plus();
    c_here();
    c_plus();
    c_cat();
    push(')');
    c_equal();
    c_not();
    c_qstream();
}

void
c_hash()
{
    user(BASE);
    c_at();
    c_mslmod();
    c_rot();
    push(9);
    c_over();
    c_less();
    if (pop())
    {
        push('A' - '9' - 1);
        c_plus();
    }
    push('0');
    c_plus();
    c_hold();
}

void
c_dabs()
{
    c_dup();
    c_dplusm();
}

void
c_bcompile()
{
    c_qcomp();
    c_mfind();
    c_not();
    if (pop())
        uabort("not found");
    c_drop();
    c_cfa();
    c_comma();
}

void
c_uless()
{
    push(0);
    c_swap();
    push(0);
    c_dless();
}

void
c_type()
{
    CELL            n;
    BYTE           *a;
    n = pop();
    a = (BYTE *) pop();
    while (n-- > 0)
    {
        push((CELL) * a++);
        c_emit();
    }
}

void
c_spaces()
{
    CELL            n;
    n = pop();
    while (n-- > 0)
        c_space();
}

void
c_quit()
{
    c_lbrak();
    c_rpstor();
    error();
}

void
c_fill()
{
    c_over();
    c_zgreater();
    if (pop())
        c_afill();
    else
    {
        c_ddrop();
        c_drop();
    }
}

void
c_msl()
{
    c_over();
    c_tor();
    c_tor();
    c_dup();
    c_dplusm();
    c_rat();
    c_abs();
    c_uslmod();
    c_rfrom();
    c_rat();
    c_xor();
    c_plusm();
    c_swap();
    c_rfrom();
    c_plusm();
    c_swap();
}

void
c_mstarsl()
{
    c_ddup();
    c_xor();
    c_swap();
    c_abs();
    c_tor();
    c_swap();
    c_abs();
    c_tor();
    c_over();
    c_xor();
    c_rot();
    c_rot();
    c_dabs();
    c_swap();
    c_rat();
    c_ustar();
    c_rot();
    c_rfrom();
    c_ustar();
    c_rot();
    push(0);
    c_dplus();
    c_rat();
    c_uslmod();
    c_rot();
    c_rot();
    c_rfrom();
    c_uslmod();
    c_swap();
    c_drop();
    c_swap();
    c_rot();
    c_dplusm();
}

void
c_mstar()
{
    c_ddup();
    c_xor();
    c_tor();
    c_abs();
    c_swap();
    c_abs();
    c_ustar();
    c_rfrom();
    c_dplusm();
}

void
c_compile()
{
    c_qcomp();
    push(*(CELL *) ip);
    c_comma();
    ip++;
}

void
c_hashs()
{
    do
    {
        c_hash();
        c_ddup();
        c_or();
    }
    while (pop());
}

void
c_hashgreat()
{
    c_ddrop();
    user(HLD);
    c_at();
    c_pad();
    c_over(), c_minus();
}

void
c_lesshash()
{
    c_pad();
    user(HLD);
    c_stor();
}

void
c_where()
{
    UCELL           n;
    user(PIN);
    c_at();
    c_cr();
    c_here();
    c_cat();
    c_dup();
    c_tor();
    c_minus();
    c_here();
    c_rat();
    c_plus();
    c_1plus();
    c_cat();
    push(BL);
    c_equal();
    if (pop())
        c_1minus();
    c_spaces();
    c_rfrom();
    n = (UCELL) pop() + 3;  /* prompt length */
    while (n--)
    {
        push('^');
        c_emit();
    }
}

void
c_until()
{
    push(1);
    c_qpairs();
    push((CELL) & cf_zbranch);
    c_comma();
    c_here();
    c_minus();
    c_comma();
}

void
c_roll()
{
    c_dup();
    push(1);
    c_less();
    if (pop())
        uabort("roll argument <1");
    c_1plus();
    c_dup();
    c_pick();
    c_swap();
    push(CELL_SIZE);
    c_star();
    c_spat();
    c_plus();
    do
    {
        c_dup();
        push(CELL_SIZE);
        c_minus();
        c_at();
        c_over();
        c_stor();
        push(CELL_SIZE);
        c_minus();
        c_spat();
        c_over();
        c_uless();
    }
    while (pop());
    c_ddrop();
}

void
c_loop()
{
    push(3);
    c_qpairs();
    push((CELL) & cf_aloop);
    c_comma();
    c_here();
    c_minus();
    c_comma();
}

void
c_literal()
{
    user(STATE);
    c_at();
    if (pop())
    {
        push((CELL) & cf_lit);
        c_comma();
        c_comma();
    }
}

void
c_if()
{
    c_qcomp();
    push((CELL) & cf_zbranch);
    c_comma();
    c_here();
    push(0);
    c_comma();
    push(2);
}

void
c_forget()
{
    uabort("forget not yet implemented");
}

void
c_endif()
{
    c_then();
}

void
c_else()
{
    push(2);
    c_qpairs();
    push((CELL) & cf_branch);
    c_comma();
    c_here();
    push(0);
    c_comma();
    c_swap();
    push(2);
    c_then();
    push(2);
}

void
c_builds()
{
    push(0);
    c_constant();
}

void
c_does()
{
    push((CELL) ip);
    c_latest();
    c_pfa();
    c_stor();
    push((CELL) c_dodoes);
    c_latest();
    c_pfa();
    c_cfa();
    c_stor();
    c_exit();
}

void
c_do()
{
    c_qcomp();
    push((CELL) & cf_ado);
    c_comma();
    c_here();
    push(3);
}

void
c_create()
{
    push(BL);
    c_word();
    c_dup();
    c_dup();
    c_1plus();
    c_cat();
    push(0);
    c_equal();
    if (pop())
        uabort("attempt to redefine null");
    c_dup();
    user(CONTEXT);
    c_at();
    c_at();
    c_afind();
    if (pop())
    {
        c_ddrop();
        user(WARNING);
        c_at();
        if (pop())
        {
            c_dup();
            c_count();
            c_type();
            c_space();
            printf("isn't unique");
        }
    }
    c_cat();
    user(WIDTH);
    c_at();
    c_min();
    c_1plus();
    c_allot();
    c_dup();
    push(NAME_END);
    c_toggle();
    c_here();
    c_1minus();
    push(NAME_END);
    c_toggle();
    align((BYTE **) puser(DP));
    c_latest();
    c_comma();
    push((CELL) c_doconst);
    c_comma();
    user(CURRENT);
    c_at(), c_stor();
}

void
c_again()
{
    push(1);
    c_qpairs();
    push((CELL) & cf_branch);
    c_comma();
    c_here();
    c_minus();
    c_comma();
}

void
c_qstack()
{
    /* don't need - virtual machine checks stack at all times */
}

void
c_abort()
{
    c_spstor();
    c_quit();
}

void
c_adotq()
{
    push((CELL) ip);
    c_count();
    c_dup();
    c_1plus();
    push((CELL) ip);
    c_plus();
    c_align_ptr();
    ip = (CODE_FIELD **) pop();
    c_type();
}

void
c_asdotq()
{
        push((CELL) ip);
        c_count();
        c_dup();
        c_1plus();
        push((CELL) ip);
        c_plus();
        c_align_ptr();
        ip = (CODE_FIELD **) pop();
}

void
c_semi()
{
    c_qcsp();
    push((CELL) & cf_exit);
    c_comma();
    c_smudge();
    c_lbrak();
}

void
c_slmod()
{
    c_tor();
    c_stod();
    c_rfrom();
    c_msl();
}

void
c_slloop()
{
    push(3);
    c_qpairs();
    push((CELL) & cf_aslloop);
    c_comma();
    c_here();
    c_minus();
    c_comma();
}

void
c_ploop()
{
    push(3);
    c_qpairs();
    push((CELL) & cf_aploop);
    c_comma();
    c_here();
    c_minus();
    c_comma();
}

void
c_starslmod()
{
    c_tor();
    c_mstar();
    c_rfrom();
    c_msl();
}

void
c_blank()
{
    push(BL);
    c_fill();
}

void
c_while()
{
    c_if();
    c_2plus();
}

/*
 * Before I forget again, I'll try to explain the linking.  Suppose the root *
 * vocab is forth, with daughter vocab frog. As the daughter must have *  as
 * context all of forth's vocab, even any created after frog, the *  linking
 * must be dynamic. Forth contains a context pointer to the first *  search
 * point in its vocab.  When frog is created the current (definitions *
 * vocab) is forth.  The context field of frog is not pointed at the *
 * current latest, as that would be static, but at a special dummy name *
 * field in forth, immediately preceeding forth's context pointer.  This *
 * dummy name field is '81a0', i.e. a single space, which won't match *
 * anything. This method ensures that the first definition into frog *  will
 * have its link pointing to the dummy in the parent vocab, which *  has as
 * its link the up-to-date context pointer for the parent vocab. *  There is
 * another simple linked list of all of the vocabs, headed by *  voc-link,
 * which is created here but not used.
 */
void c_vocabulary() {

    c_create();
    push(0x81);
    c_ccomma();
    push(0xa0);
    c_ccomma();
    align((BYTE **) puser(DP));
    user(CURRENT);
    c_at();
    push(CELL_SIZE);
    c_minus();
    c_comma();
    c_here();
    user(VOC_LINK);
    c_at();
    c_comma();
    user(VOC_LINK);
    c_stor();
    push((CELL) c_dovoc);
    c_latest();
    c_pfa();
    c_cfa();
    c_stor();
}

void
c_variable()
{
    c_create();
    push(CELL_SIZE);
    c_allot();
    push((CELL) c_dovar);
    c_latest();
    c_pfa();
    c_cfa();
    c_stor();
}

void
c_repeat()
{
    c_tor();
    c_tor();
    c_again();
    c_rfrom();
    c_rfrom();
    c_2minus();
    c_then();
}

void
c_mod()
{
    c_slmod();
    c_drop();
}

void
c_dliteral()
{
    user(STATE);
    c_at();
    if (pop())
    {
        c_swap();
        c_literal();
        c_literal();
    }
}

void
c_constant()
{
    c_create();
    c_comma();
    push((CELL) c_doconst);
    c_latest();
    c_pfa();
    c_cfa();
    c_stor();
}

void
c_aabortq()
{
    if (pop())
    {
        c_where();
        c_cr();
        push((CELL) ip);
        c_count();
        c_type();
        c_spstor();
        c_quit();
    } else
    {
        push((CELL) ip);
        c_dup();
        c_cat();
        c_plus();
        c_1plus();
        c_align_ptr();
        ip = (CODE_FIELD **) pop();
    }
}

void
c_colon()
{
    c_spat();
    user(CSP);
    c_stor();
    user(CURRENT);
    c_at();
    user(CONTEXT);
    c_stor();
    c_create();
    c_smudge();
    c_rbrak();
    push((CELL) c_docol);
    c_latest();
    c_pfa();
    c_cfa();
    c_stor();
}

void
c_slash()
{
    CELL            t;
    t = pop();
    push(pop() / t);
}

void
c_dotq()
{
    c_tstream();
    c_cat();
    push('\"');
    c_equal();
    if (pop())
    {
        push(1);
        user(PIN);
        c_pstor();
    } else
    {
        push('\"');
        user(STATE);
        c_at();
        if (pop())
        {
            push((CELL) & cf_adotq);
            c_comma();
        }
        c_word();
        c_dup();
        c_cat();
        c_1plus();
        c_over();
        c_plus();
        c_cat();
        push('\"');
        c_equal();
        c_not();
        c_qstream();
        user(STATE);
        c_at();
        if (pop())
        {
            c_cat();
            c_1plus();
            c_allot();
            align((BYTE **) puser(DP));
        } else
        {
            c_count();
            c_type();
        }
    }
}


void
c_sdotq()
{
    c_tstream();
    c_cat();
    push('\"');
    c_equal();

    if (pop())
    {
        push(1);
        user(PIN);
        c_pstor();
    } else
    {
        push('\"');
        user(STATE);
        c_at();
        if (pop())
        {
        /* compiling */
            push((CELL) & cf_asdotq);
            c_comma();
        }
        c_word();
        c_dup();
        c_cat();
        c_1plus();
        c_over();
        c_plus();
        c_cat();
        push('\"');
        c_equal();
        c_not();
        c_qstream();
        user(STATE);
        c_at();
        
        if (pop())
        {
            c_cat();
            c_1plus();
            c_allot();
            align((BYTE **) puser(DP));
        } else
        {
            char *src;
            int cnt;
            
            src=(char *)pop();

            memset(scratchString[selector],(int)NULL,255);
            memcpy(scratchString[selector],src,*(src)+1);
            push((int)&scratchString[selector]);
            selector= (selector +1) & (SCRATCH_SIZE-1);
            c_count();
        }
    }
}


void
c_tick()
{
    c_mfind();
    c_not();
    if (pop())
        uabort("not found");
    c_drop();
    c_literal();
}

void
c_starsl()
{
    c_starslmod();
    c_swap();
    c_drop();
}

void c_interpret() {
    break_flag = FALSE;

    while (!break_flag) {
        c_qstack();
        c_mfind();
        if (pop()) {
            user(STATE);
            c_at();
            c_less();

            if (pop()) {
                c_cfa();
                c_comma();
            } else {
                c_cfa();
                c_execute();
            }
        } else {
            c_here();
            c_number();
            user(DPL);
            c_at();
            c_1plus();
            if (pop()) {
                c_dliteral();
            } else {
                c_drop();
                c_literal();
            }
        }
    }
}

void
c_abortq()
{
    c_qcomp();
    push((CELL) & cf_aabortq);
    c_comma();
    c_tstream();
    c_cat();
    push('\"');
    c_equal();
    if (pop())
    {
        push(1);
        user(PIN);
        c_pstor();
        push(0);
        c_ccomma();
    } else
    {
        push('\"');
        c_word();
        c_dup();
        c_cat();
        c_1plus();
        c_swap();
        c_over();
        c_plus();
        c_cat();
        push('\"');
        c_equal();
        c_zequal();
        c_qstream();
        c_allot();
    }
    align((BYTE **) puser(DP));
}

void
c_2slash()
{
    push(pop() / 2);
}

void
c_user()
{
    c_constant();
    push((CELL) c_douser);
    c_latest();
    c_pfa();
    c_cfa();
    c_stor();
}

void
c_depth()
{
    c_spat();
    c_s0();
    c_swap();
    c_minus();
    push(CELL_SIZE);
    c_slash();
}

void
c_ddotr()
{
    c_tor();
    c_swap();
    c_over();
    c_dup();
    c_dplusm();
    c_lesshash();
    c_hashs();
    c_rot();
    c_sign();
    c_hashgreat();
    c_rfrom();
    c_over();
    c_minus();
    c_spaces();
    c_type();
}

void
c_ddot()
{
    push(0);
    c_ddotr();
    c_space();
}

void
c_dotr()
{
    c_tor();
    c_stod();
    c_rfrom();
    c_ddotr();
}

void
c_udot()
{
    push(0);
    c_ddot();
}

void
c_dot()
{
    c_stod();
    c_ddot();
}

void
c_quest()
{
    c_at();
    c_dot();
}

void
c_align_ptr(void)
{
    push((pop() + ALIGN) & ~ALIGN);
}

void
c_endf()
{
    if (depth < 0)
        return;
    fclose(ifp[depth]);
    if (depth == 0)
    {
        *puser(TKEY) = (CELL) & cf_akey;
    }
    depth--;
}

void
c_endall()
{
    while (depth >= 0)
        c_endf();
}

void
c_afkey()
{
    extern int      verbose;

    char            ch;
    if (depth < 0)
        uabort("no file open");
    ch = getc(ifp[depth]);
    if (ch == EOF)
    {
        if (verbose)
            printf("end of file");
        c_endf();
        push('\r');
        return;
    }
    if (ch == '\n')
        ch = '\r';
#if DOS
    if (ch == 'Z' - 0x40)
        ch = ' ';
#endif
    push(ch);
}

void
c_afemit()
{
    uabort("cannot do emit to file - use fwrite!");
}

void
c_debug()
{
    BYTE           *p;
    UCELL           n;
    printf("\r\n");
    p = (BYTE *) pop();
    for (n = 0; n < 256; n++)
    {
        printf("%02x ", *(p + n));
        if (((CELL) (p + n) & 0xf) == 0xf)
            printf("\r\n");
    }
}

void
c_x()
{
    break_flag = TRUE;
}

/* -- handle (check for NULL!!) */
#ifdef LINUX
void
c_fopenr()
{
    push((CELL) fopen((char *) pop(), FILE_READ_MODE));
}

/* -- handle (check for NULL!!) */
void
c_fopenw()
{
    push((CELL) fopen((char *) pop(), FILE_WRITE_MODE));
}

/* -- handle (check for NULL!!) */
void
c_fopena()
{
    push((CELL) fopen((char *) pop(), FILE_APPEND_MODE));
}

/* handle -- 0/EOF */
void
c_fclose()
{
    push((CELL) fclose((FILE *) pop()));
}

/* handle buff number-bytes -- actual-bytes */
void
c_fread()
{
    size_t          number;
    char           *buff;
    number = (size_t) pop();
    buff = (char *) pop();
    push(fread(buff, 1, number, (FILE *) pop()));
}

/* handle buff number-bytes -- actual-bytes */
void
c_fwrite()
{
    size_t          number;
    char           *buff;
    number = (size_t) pop();
    buff = (char *) pop();
    push(fwrite(buff, 1, number, (FILE *) pop()));
}
#endif

/* -- pointer */
void
c_parse()
{
    push(BL);
    c_word();
    c_dup();
    c_dup();
    c_cat();
    c_plus();
    c_1plus();
    push(0);
    c_swap();
    c_cstor();
    c_1plus();
}

#ifdef LINUX
void
c_getf()
{
    char           *name;
    if (depth >= FILE_DEPTH - 1)
        uabort("file include too deep");
    c_parse();
    name = (char *) pop();
    ifp[depth + 1] = fopen(name, "r");  /* same mode for all ! */
    if (ifp[depth + 1] == NULL)
    {
        printf("can't open %s", name);
        uabort("file open error");
    }
    if (depth < 0)
        *puser(TKEY) = (CELL) & cf_afkey;
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
#endif
void
c_save()
{
    uabort("save not yet implemented");
}

void
c_load()
{
    uabort("load not yet implemented");
}

void
c_jsr()
{
    ((void (*) (void)) pop()) ();
}

void
c_iddot()
{
    char           *p;
    int             n;

    p = (char *) pop();
    n = *p++ & 0x1f;
    while (n--)
    {
        push(*p++ & 0x7f);
        c_emit();
    }
    push(BL);
    c_emit();
}

void c_vlist() {
    push(80);
    user(OUT);
    c_stor();
    user(CONTEXT);
    c_at();
    c_at();
    do
    {
        push(80);
        user(OUT);
        c_at();
        c_minus();
        c_over();
        c_cat();
        push(0x1f);
        c_and();
        push(4);
        c_plus();
        c_less();
        if (pop())
        {
            c_cr();
            push(0);
            user(OUT);
            c_stor();
        }
        c_dup();
        c_iddot();
        c_space();
        c_space();
        c_pfa();
        c_lfa();
        c_at();
        c_dup();
    } while (pop());
    c_drop();
}

void
c_dots()
{
    CELL           *p, *top;

    c_cr();
    c_depth();
    if (pop())
    {
        c_spat();
        top = (CELL *) pop() - 1;
        c_s0();
        p = (CELL *) pop() - 1;
        while (p != top)
        {
            push(*p--);
            push(0);
            c_ddot();
        }
    } else
    {
        printf("empty stack");
    }
}

void
c_verbose()
{
    verbose = 1;
}

void
c_quiet()
{
    verbose = 0;
}

void c_cpu()
{
    struct utsname buf;
    int res;
    int cpu = CPU_UNKNOWN;

    res = uname(&buf);
    if ((strcasecmp(buf.machine, "i686") == 0) || (strcasecmp(buf.machine, "i386") == 0) || (strcasecmp(buf.machine, "x86pc") == 0) )
    {
        cpu = CPU_X86;
    } else if ( (strcasecmp(buf.machine, "Power Macintosh") == 0) || (strcasecmp(buf.machine, "ppc") == 0) )
    {
        cpu = CPU_PPC;
    } else if (strcasecmp(buf.machine, "armv5tel") == 0)
    {
        cpu = CPU_ARM;
    } else if (strcasecmp(buf.machine, "m68knommu") == 0)
    {
        cpu = CPU_COLDFIRE;
    } else if (strcasecmp(buf.machine, "mips") == 0)
    {
        cpu = CPU_MIPS;
    }
    push(cpu);
}

void c_os()
{
    struct utsname buf;
    int res;
    int os = OS_UNKNOWN;

    res = uname(&buf);

    if (strcasecmp(buf.sysname, "linux") == 0)
    {
        os = OS_LINUX;
    } else if (strcasecmp(buf.sysname, "darwin") == 0)
    {
        os = OS_DARWIN;
    } else if (strcasecmp(buf.sysname, "uClinux") == 0)
    {
        os = OS_UCLINUX;
    }  else if (strcasecmp(buf.sysname, "QNX") == 0)
    {
        os = OS_QNX;
    }    

    push(os);
}

#ifdef DLIB
void c_dlopen(void)
{   
    char *ptr;
    int len;
    int mode;
    void *lh;

    len = (int)pop();
    ptr = (char *)pop();
    lh = dlopen( ptr, RTLD_LAZY);
    if (!lh) 
    {
        fprintf(stderr,"%s\r\n",dlerror());
    }
    push((int)lh);
}

void c_dlclose(void)
{
    void *lh;

    lh = (void *)pop();
    (void)dlclose(lh);
}
#endif

void countedString(void)
{

    char *ptr;
    push('"');
    c_word();
    
    ptr=(char *)pop();
    push((int)ptr);
    
    c_dup();
    c_dup();
    c_cat();
    c_plus();
    c_1plus();
    push(0);
    c_swap();
    c_cstor();
    c_1plus();
    c_drop();
    

    memcpy(scratchString[selector],ptr,(*ptr)+1);
    push((int)&scratchString[selector]);
    selector= (selector +1) && 1;

}

void string(void)
{
    countedString();
    c_count();
}

void c_dump(void)
{
    char *hex="0123456789abcdef";
    char *ptr;
    char buffer[17];
    char opBuffer[128];
    char tmp[5];
    char hi,lo;
    int count;

    int len;
    int i;
    int c;
    int flag=TRUE;
    int offset=0;

    count=pop();
//    ptr=(void *)pop();
    ptr=(char *)pop();

    printf("\r\n");

    while(flag)
    {
        if(offset > count)
            return;
        memset((void *)opBuffer,32,128);
        len=16;

        sprintf(opBuffer,"%08x: ",ptr+offset);
        for(i=0;i<16;i++)
        {
            buffer[i]=(char )*(ptr+offset+i);
        }
        for(i=0;i<16;i++)
        {
            if(buffer[i] < 32 || buffer[i] >=127)
                sprintf(&opBuffer[10+i],".");
            else
                sprintf(&opBuffer[10+i],"%c ",buffer[i]);
        }

        opBuffer[26]=' ';
        opBuffer[27]='|';
        opBuffer[28]=' ';

        for(i=0;i<16;i++)
        {
            hi=((buffer[i] & 0xf0) >> 4);
            lo=(buffer[i] & 0x0f);
            opBuffer[29+(i*3)]=hex[hi];
            opBuffer[30+(i*3)]=hex[lo];
        }
        opBuffer[80]='\0';
        printf("%s\r\n",opBuffer);
        offset+=16;
    }
}
