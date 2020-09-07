// 
// Load tst1.fth
// 
void f_tst() {

    const static char str001[] = "Started";
    push(str001);
    push(7);

    push(10);
    push(0);
    c_do();
    c_i();
    c_dot();
    c_cr();
    c_loop();

    const static char str002[] = "stopped";
    push(str002);
    push(7);

}
