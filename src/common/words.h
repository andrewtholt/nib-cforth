/*
		Portable Forth in C
		===================

 
(c) 1993,1996 Nigel Bromley: nib@dial.pipex.com or +44 7010 700NIB (700642).

No warranties of any sort! Free use for non-commercial purposes only.
Author's copyright notice should appear with your own on any derived
versions.

Words.h - contains function prototypes for all functions implementing
Forth words contained in the kernel dictionary.

nib 1992-03-31
update nib 1993-07-03, 1996-01-21
*/

void c_bye(void);
void c_cpmcall(void);
void c_xor(void);
void c_uslmod(void);
void c_ustar(void);
void c_toggle(void);
void c_swap(void);
void c_spat(void);
void c_spstor(void);
void c_stod(void);
void c_rpat(void);
void c_rpstor(void);
void c_rot(void);
void c_min_rot(void);
void c_rat(void);
void c_rfrom(void);
void c_portat(void);
void c_portstor(void);
void c_portwat(void);
void c_portwstor(void);
void c_over(void);
void c_or(void);
void c_not(void);
void c_negate(void);
void c_lit(void);
void c_leave(void);
void c_j(void);
void c_itick(void);
void c_i(void);
void c_exit(void);
void c_execute(void);
void c_evaluate(void);
/*
void c_servent(void);
*/

void c_enclose(void);
void c_dup(void);
void c_nip(void);
void c_drop(void);
void c_dnegate(void);
void c_digit(void);
void c_ddup(void);
void c_ddrop(void);
void c_dplus(void);
void c_dstor(void);
void c_cat(void);
void c_wat(void);
void c_cstor(void);
void c_wstor(void);
void c_zbranch(void);
void c_branch(void);
void c_and(void);
void c_at(void);
void c_tor(void);
void c_aloop(void);
void c_aploop(void);
void c_afind(void);
void c_afill(void);
void c_ado(void);
void c_acmove(void);
void c_alcmove(void);
void c_aslloop(void);
void c_less(void);
void c_2minus(void);
void c_2plus(void);
void c_2star(void);
void c_1minus(void);
void c_1plus(void);
void c_zless(void);
void c_minus(void);
void c_pstor(void);
void c_plus(void);
void c_stor(void);
void c_hdot(void);
void c_rbrak(void);
void c_lbrak(void);
void c_word(void);
void c_vocabulary(void);
void c_traverse(void);
void c_s0(void);
void c_pick(void);
void c_number(void);
void c_max(void);
void c_mslmod(void);
void c_mplus(void);
void c_lfa(void);
void c_latest(void);
void c_key(void);
void c_interpret(void);
void c_hold(void);
void c_hex(void);
void c_here(void);
void c_expect(void);
void c_accept(void);
void c_emit(void);
void c_definitions(void);
void c_decimal(void);
void c_dplusm(void);
void c_cr(void);
void c_count(void);
void c_convert(void);
void c_cmove(void);
void c_cfa(void);
void c_allot(void);
void c_abort(void);
void c_qterminal(void);
void c_qdup(void);
void c_greater(void);
void c_equal(void);
void c_lcmove(void);
void c_zequal(void);
void c_mtrailing(void);
void c_mfind(void);
void c_plusm(void);
void c_star(void);
void c_tstream(void);
void c_qcomp(void);
void c_qcsp(void);
void c_aemit(void);
void c_akey(void);
void c_qstream(void);
void c_qpairs(void);
void c_space(void);
void c_smudge(void);
void c_sign(void);
void c_query(void);
void c_pfa(void);
void c_pad(void);
void c_nfa(void);
void c_move(void);
void c_min(void);
void c_immediate(void);
void c_find(void);
void c_dless(void);
void c_cold(void);
void c_then(void);
void c_ccomma(void);
void c_begin(void);
void c_abs(void);
void c_zgreater(void);
void c_comma(void);
void c_lpara(void);
void c_hash(void);
void c_dabs(void);
void c_bcompile(void);
void c_uless(void);
void c_type(void);
void c_spaces(void);
void c_quit(void);
void c_fill(void);
void c_msl(void);
void c_mstarsl(void);
void c_mstar(void);
void c_compile(void);
void c_hashs(void);
void c_hashgreat(void);
void c_lesshash(void);
void c_where(void);
void c_until(void);
void c_roll(void);
void c_loop(void);
void c_literal(void);
void c_if(void);
void c_forget(void);
void c_endif(void);
void c_else(void);
void c_builds(void);
void c_does(void);
void c_do(void);
void c_create(void);
void c_again(void);
void c_qstack(void);
void c_adotq(void);
void c_semi(void);
void c_slmod(void);
void c_slloop(void);
void c_ploop(void);
void c_starslmod(void);
void c_blank(void);
void c_while(void);
void c_variable(void);
void c_repeat(void);
void c_mod(void);
void c_dliteral(void);
void c_constant(void);
void c_aabortq(void);
void c_colon(void);
void c_slash(void);
void c_dotq(void);
void c_sdotq(void);
void c_asdotq(void);

void c_tick(void);
void c_starsl(void);
void c_abortq(void);
void c_2slash(void);
void c_user(void);
void c_depth(void);
void c_ddotr(void);
void c_ddot(void);
void c_dotr(void);
void c_udot(void);
void c_dot(void);
void c_quest(void);
void c_endf(void);
void c_endall(void);
void c_afkey(void);
void c_afemit(void);
void c_debug(void);
void c_x(void);
void c_afemit(void);
void c_align_ptr(void);

#ifdef LINUX
void c_fopenr(void);
void c_fopenw(void);
void c_fopena(void);
void c_fclose(void);
void c_fread(void);
void c_fwrite(void);
void c_getf(void);
void c_string_getf(char *);
#endif

void c_save(void);
void c_load(void);
void c_jsr(void);
void c_iddot(void);
void c_vlist(void);
void c_dots(void);
void c_parse(void);
void c_quiet(void);
void c_verbose(void);

void c_cpu(void);
void c_os(void);
void c_banner(void);
void c_dlopen(void);
void c_dlclose(void);
 void c_setprompt(void);

