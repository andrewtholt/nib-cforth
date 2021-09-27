#include <sys/utsname.h>
#include <strings.h>
#include "forth.h"
#include "cpu.h"
#include "os.h"

void c_cpu() {
    struct utsname buf;
    int res;
    int cpu = CPU_UNKNOWN;

    res = uname(&buf);

    if ((strcasecmp(buf.machine, "i686") == 0) || (strcasecmp(buf.machine, "i386") == 0) || (strcasecmp(buf.machine, "x86pc") == 0) ) {
        cpu = CPU_X86;
    } else if ( (strcasecmp(buf.machine, "Power Macintosh") == 0) || (strcasecmp(buf.machine, "ppc") == 0) ) {
        cpu = CPU_PPC;
    } else if ((strcasecmp(buf.machine, "armv5tel") == 0) || (strcasecmp(buf.machine, "armv6l") == 0) ) {
        cpu = CPU_ARM;
    } else if (strcasecmp(buf.machine, "m68knommu") == 0) {
        cpu = CPU_COLDFIRE;
    } else if (strcasecmp(buf.machine, "mips") == 0) {
        cpu = CPU_MIPS;
    }
    push(cpu);
}

void c_os() {
    struct utsname buf;
    int res;
    int os = OS_UNKNOWN;

    res = uname(&buf);

    if (strcasecmp(buf.sysname, "linux") == 0) {
        os = OS_LINUX;
    } else if (strcasecmp(buf.sysname, "darwin") == 0) {
        os = OS_DARWIN;
    } else if (strcasecmp(buf.sysname, "uClinux") == 0) {
        os = OS_UCLINUX;
    }  else if (strcasecmp(buf.sysname, "QNX") == 0) {
        os = OS_QNX;
    }

    push(os);
}

void extend_misc() {
    create_codeword("cpu",c_cpu,0);
    create_codeword("os",c_os,0);
}
