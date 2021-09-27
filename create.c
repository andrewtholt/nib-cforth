/*
		Portable Forth in C
		===================

 
(c) 1993,1996 Nigel Bromley: nib@dial.pipex.com or +44 7010 700NIB (700642).

No warranties of any sort! Free use for non-commercial purposes only.
Author's copyright notice should appear with your own on any derived
versions.

Create.c - contains one big function which creates the kernel dictionary.

nib 1992-03-31
update nib 1993-07-03, 1996-01-21
*/

#include <stdio.h>
#include "forth.h"
#include "words.h"
void c_setprompt();

void create_kernel(void)
{
	create_user("sp0", SP0, 0);
	create_user("r0", R0, 0);
	create_user("tib", TIB, 0);
	create_user("width", WIDTH, 0);
	create_user("warning", WARNING, 0);
	create_user("fence", FENCE, 0);
	create_user("dp", DP, 0);
	create_user("voc-link", VOC_LINK, 0);
	create_user("'emit", TEMIT, 0);
	create_user("'key", TKEY, 0);
	create_user(">in", PIN, 0);
	create_user("base", BASE, 0);
	create_user("context", CONTEXT, 0);
	create_user("csp", CSP, 0);
	create_user("current", CURRENT, 0);
	create_user("dpl", DPL, 0);
	create_user("hld", HLD, 0);
	create_user("out", OUT, 0);
	create_user("state", STATE, 0);

	create_constant("up", (CELL)up);
	create_constant("bl", BL);
	create_constant("2", 2);
	create_constant("1", 1);
	create_constant("0", 0);
	create_constant("bytes/cell", CELL_SIZE);
	create_constant("top", (CELL)k_dict+KERNEL_SIZE);
	create_constant("EOF", EOF);
	create_constant("NULL", (int)NULL);

	create_codeword("bye", c_bye, 0);
	create_codeword("xor", c_xor, 0);
	create_codeword("u/mod", c_uslmod, 0);
	create_codeword("u*", c_ustar, 0);
	create_codeword("toggle", c_toggle, 0);
	create_codeword("swap", c_swap, 0);
	create_codeword("sp@", c_spat, 0);
	create_codeword("sp!", c_spstor, 0);
	create_codeword("s->d", c_stod, 0);
	create_codeword("rp@", c_rpat, 0);
	create_codeword("rp!", c_rpstor, 0);
	create_codeword("rot", c_rot, 0);
	create_codeword("-rot", c_min_rot, 0);
	create_codeword("r@", c_rat, 0);
	create_codeword("r>", c_rfrom, 0);
#if INTEL16 || INTEL32 && DOS
	create_codeword("p@", c_portat, 0);
	create_codeword("p!", c_portstor, 0);
	create_codeword("pw@", c_portwat, 0);
	create_codeword("pw!", c_portwstor, 0);
#endif
	create_codeword("over", c_over, 0);
	create_codeword("or", c_or, 0);
	create_codeword("not", c_not, 0);
	create_codeword("negate", c_negate, 0);
	create_codeword("lit", c_lit, 0);
	create_codeword("leave", c_leave, 0);
	create_codeword("j", c_j, 0);
	create_codeword("i'", c_itick, 0);
	create_codeword("i", c_i, 0);
	create_codeword("exit", c_exit, 0);
	create_codeword("execute", c_execute, 0);
	create_codeword("evaluate", c_evaluate, 0);
    /*
	create_codeword("servent", c_servent, 0);
    */

	create_codeword("enclose", c_enclose, 0);
	create_codeword("dup", c_dup, 0);
	create_codeword("drop", c_drop, 0);
	create_codeword("dnegate", c_dnegate, 0);
	create_codeword("digit", c_digit, 0);
	create_codeword("ddup", c_ddup, 0);
	create_codeword("ddrop", c_ddrop, 0);
	create_codeword("d+", c_dplus, 0);
	create_codeword("d!", c_dstor, 0);
	create_codeword("c@", c_cat, 0);
	create_codeword("w@", c_wat, 0);
	create_codeword("c!", c_cstor, 0);
	create_codeword("w!", c_wstor, 0);
	create_codeword("0branch", c_zbranch, 0);
	create_codeword("branch", c_branch, 0);
	create_codeword("and", c_and, 0);
	create_codeword("@", c_at, 0);
	create_codeword(">r", c_tor, 0);
	create_codeword("<loop>", c_aloop, 0);
	create_codeword("<+loop>", c_aploop, 0);
	create_codeword("<find>", c_afind, 0);
	create_codeword("<fill>", c_afill, 0);
	create_codeword("<do>", c_ado, 0);
	create_codeword("<cmove>", c_acmove, 0);
	create_codeword("<<cmove>", c_alcmove, 0);
	create_codeword("</loop>", c_aslloop, 0);
	create_codeword("<", c_less, 0);
	create_codeword("2-", c_2minus, 0);
	create_codeword("2+", c_2plus, 0);
	create_codeword("2*", c_2star, 0);
	create_codeword("1-", c_1minus, 0);
	create_codeword("1+", c_1plus, 0);
	create_codeword("0<", c_zless, 0);
	create_codeword("-", c_minus, 0);
	create_codeword("+!", c_pstor, 0);
	create_codeword("+", c_plus, 0);
	create_codeword("!", c_stor, 0);
	create_codeword("h.", c_hdot, 0);
	create_codeword("]", c_rbrak, 0);
	create_codeword("[", c_lbrak, IMMED);
	create_codeword("word", c_word, 0);
	create_codeword("vocabulary", c_vocabulary, 0);
	create_codeword("traverse", c_traverse, 0);
	create_codeword("s0", c_s0, 0);
	create_codeword("pick", c_pick, 0);
	create_codeword("number", c_number, 0);
	create_codeword("max", c_max, 0);
	create_codeword("m/mod", c_mslmod, 0);
	create_codeword("m+", c_mplus, 0);
	create_codeword("lfa", c_lfa, 0);
	create_codeword("latest", c_latest, 0);
	create_codeword("key", c_key, 0);
	create_codeword("interpret", c_interpret, 0);
	create_codeword("hold", c_hold, 0);
	create_codeword("hex", c_hex, 0);
	create_codeword("here", c_here, 0);
	create_codeword("expect", c_expect, 0);
	create_codeword("accept", c_accept, 0);
	create_codeword("emit", c_emit, 0);
	create_codeword("definitions", c_definitions, 0);
	create_codeword("decimal", c_decimal, 0);
	create_codeword("d+-", c_dplusm, 0);
	create_codeword("cr", c_cr, 0);
	create_codeword("count", c_count, 0);
	create_codeword("convert", c_convert, 0);
	create_codeword("cmove", c_cmove, 0);
	create_codeword("cfa", c_cfa, 0);
	create_codeword("allot", c_allot, 0);
	create_codeword("abort", c_abort, 0);
	create_codeword("?terminal", c_qterminal, 0);
	create_codeword("?dup", c_qdup, 0);
	create_codeword(">", c_greater, 0);
	create_codeword("=", c_equal, 0);
	create_codeword("<cmove", c_lcmove, 0);
	create_codeword("0=", c_zequal, 0);
	create_codeword("-trailing", c_mtrailing, 0);
	create_codeword("-find", c_mfind, 0);
	create_codeword("+-", c_plusm, 0);
	create_codeword("*", c_star, 0);
	create_codeword("'stream", c_tstream, 0);
	create_codeword("?comp", c_qcomp, 0);
	create_codeword("?csp", c_qcsp, 0);
	create_codeword("<emit>", c_aemit, 0);
	create_codeword("<key>", c_akey, 0);
	create_codeword("?stream", c_qstream, 0);
	create_codeword("?pairs", c_qpairs, 0);
	create_codeword("space", c_space, 0);
	create_codeword("smudge", c_smudge, 0);
	create_codeword("sign", c_sign, 0);
	create_codeword("query", c_query, 0);
	create_codeword("pfa", c_pfa, 0);
	create_codeword("pad", c_pad, 0);
	create_codeword("nfa", c_nfa, 0);
	create_codeword("move", c_move, 0);
	create_codeword("min", c_min, 0);
	create_codeword("immediate", c_immediate, 0);
	create_codeword("find", c_find, 0);
	create_codeword("d<", c_dless, 0);
	create_codeword("cold", c_cold, 0);
	create_codeword("then", c_then, IMMED);
	create_codeword("c,", c_ccomma, 0);
	create_codeword("begin", c_begin, IMMED);
	create_codeword("abs", c_abs, 0);
	create_codeword("0>", c_zgreater, 0);
	create_codeword(",", c_comma, 0);
	create_codeword("(", c_lpara, IMMED);
	create_codeword("#", c_hash, 0);
	create_codeword("dabs", c_dabs, 0);
	create_codeword("[compile]", c_bcompile, IMMED);
	create_codeword("u<", c_uless, 0);
	create_codeword("type", c_type, 0);
	create_codeword("spaces", c_spaces, 0);
	create_codeword("quit", c_quit, 0);
	create_codeword("fill", c_fill, 0);
	create_codeword("m/", c_msl, 0);
	create_codeword("m*/", c_mstarsl, 0);
	create_codeword("m*", c_mstar, 0);
	create_codeword("compile", c_compile, 0);
	create_codeword("#s", c_hashs, 0);
	create_codeword("#>", c_hashgreat, 0);
	create_codeword("<#", c_lesshash, 0);
	create_codeword("where", c_where, 0);
	create_codeword("until", c_until, IMMED);
	create_codeword("roll", c_roll, 0);
	create_codeword("loop", c_loop, IMMED);
	create_codeword("literal", c_literal, IMMED);
	create_codeword("if", c_if, IMMED);
	create_codeword("forget", c_forget, 0);
	create_codeword("endif", c_endif, IMMED);
	create_codeword("else", c_else, IMMED);
	/*
	create_codeword("<builds", c_builds, 0);
	*/
	create_codeword("create", c_builds, 0);
	create_codeword("does>", c_does, 0);
	create_codeword("do", c_do, IMMED);
	/*
	create_codeword("create", c_create, 0);
	*/
	create_codeword("again", c_again, IMMED);
	create_codeword("?stack", c_qstack, 0);
	create_codeword("<.\">", c_adotq, 0);
	create_codeword(";", c_semi, IMMED);
	create_codeword("/mod", c_slmod, 0);
	create_codeword("/loop", c_slloop, IMMED);
	create_codeword("+loop", c_ploop, IMMED);
	create_codeword("*/mod", c_starslmod, 0);
	create_codeword("blank", c_blank, 0);
	create_codeword("while", c_while, IMMED);
	create_codeword("variable", c_variable, 0);
	create_codeword("repeat", c_repeat, IMMED);
	create_codeword("mod", c_mod, 0);
	create_codeword("dliteral", c_dliteral, IMMED);
	create_codeword("constant", c_constant, 0);
	create_codeword("<abort\">", c_aabortq, 0);
	create_codeword(":", c_colon, 0);
	create_codeword("/", c_slash, 0);
	create_codeword(".\"", c_dotq, IMMED);
	create_codeword("'", c_tick, IMMED);
	create_codeword("*/", c_starsl, 0);
	create_codeword("abort\"", c_abortq, IMMED);
	create_codeword("2/", c_2slash, 0);
	create_codeword("user", c_user, 0);
	create_codeword("depth", c_depth, 0);
	create_codeword("d.r", c_ddotr, 0);
	create_codeword("d.", c_ddot, 0);
	create_codeword(".r", c_dotr, 0);
	create_codeword("u.", c_udot, 0);
	create_codeword(".", c_dot, 0);
	create_codeword("?", c_quest, 0);
	create_codeword("endf", c_endf, 0);
	create_codeword("end-all", c_endall, 0);
	create_codeword("<fkey>", c_afkey, 0);
	create_codeword("<femit>", c_afemit, 0);
	create_codeword("debug", c_debug, 0);
	create_codeword("align-ptr", c_align_ptr, 0);
	create_codeword("", c_x, IMMED);	/* null word */

	create_codeword("parse", c_parse, 0);
#ifdef LINUX
#ifdef FILESYSTEM
	create_codeword("fopenr", c_fopenr, 0);
	create_codeword("fopenw", c_fopenw, 0);
	create_codeword("fopena", c_fopena, 0);
	create_codeword("fclose", c_fclose, 0);
	create_codeword("fread", c_fread, 0);
	create_codeword("fwrite", c_fwrite, 0);

	create_codeword("getf", c_getf, 0);

	create_codeword("include", c_getf, 0);
#endif
#endif
	create_codeword("save", c_save, 0);
	create_codeword("load", c_load, 0);
	create_codeword("jsr", c_jsr, 0);
	create_codeword("id.", c_iddot, 0);
	create_codeword("words", c_vlist, 0);
	create_codeword(".s", c_dots, 0);
	create_codeword("quiet",c_quiet,0);
	create_codeword("verbose", c_verbose,0);
	create_codeword("c\"",countedString,0);

	create_codeword("cpu",c_cpu,0);
	create_codeword("os",c_os,0);
	create_codeword("banner",c_banner,0);
#ifdef DLIB
	create_codeword("dlopen",c_dlopen,0);
	create_codeword("dlclose",c_dlclose,0);
#endif

/*	create_codeword("s\"",string,IMMED); */
	create_codeword("s\"",c_sdotq,IMMED);
	create_codeword("dump",c_dump,0);
#ifdef DB
	create_codeword("db-bucket-stats",dbBucketStats,0);
	create_codeword("db-create", dbCreate,0);
	create_codeword("db-grow", dbGrow,0);
	create_codeword("db-occupancy",dbOccupancy,0);

	create_codeword("db-set-alarm", dbSetAlarmLevel,0);
	create_codeword("db-get-alarm", dbGetAlarmLevel,0);
	create_codeword("db-set-flags", dbSetFlags,0);
	create_codeword("db-set-max-records", dbSetMaxRecords,0);
	create_codeword("db-set-name-size", dbSetNameSize,0);
	create_codeword("db-set-def-size",dbSetDefSize,0);
	create_codeword("db-get-name",dbGetName,0);
	create_codeword("db-get-def",dbGetDef,0);
	create_codeword("db-set-name",dbSetName,0);
	create_codeword("db-set-def",dbSetDef,0);
	create_codeword("db-update",dbUpdate,0);
	create_codeword("db-status",dbStatus,0);
	create_codeword("$db-lookup",dbLookup,0);
	create_codeword("$db-dump",dbDump,0);
	create_codeword("$db-load",dbLoad,0);
	create_codeword("db-update",dbUpdate,0);
	create_codeword("db-install",dbInstall,0);
	create_codeword("db-display-record",dbDisplayRecord,0);
	create_codeword("db-debug-dump",dbDebugDump,0);
	create_codeword("db-find-first-def",dbFindFirstDef,0);
	create_codeword("db-find-next-def",dbFindNextDef,0);
	create_codeword("db-find-first",dbFindFirst,0);
	create_codeword("db-find-next",dbFindNext,0);
	create_codeword("db-delete",dbDelete,0);
#endif

    create_codeword("set-prompt",c_setprompt,0);
	create_vocab("forth");
    create_vocab("display");
}
