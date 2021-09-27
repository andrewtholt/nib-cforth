// 
// Load tst1.fth
// 
void f_tst1() {
    printf("Started");
    c_cr();
    push(10);
    push(0);
    c_ado();
    c_i();
    c_dot();
    c_cr();
    c_aloop();
}
void f_tst2() {
    printf("An another");
    c_cr();
}

void extend_from_file() {
    create_codeword("tst1",f_tst1,0);
    create_codeword("tst2",f_tst2,0);
}

