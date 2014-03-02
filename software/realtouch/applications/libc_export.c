/**
 * export for c library symbols or aeabi symbols
 */
#include <rtm.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/********** begin export for libc **********/
RTM_EXPORT(atof);
RTM_EXPORT(sprintf);
/* Standard C String & Character */
//RTM_EXPORT(atof);
RTM_EXPORT(atoi);
RTM_EXPORT(atol);
//RTM_EXPORT(isalnum);
RTM_EXPORT(isalpha);
//RTM_EXPORT(iscntrl);
RTM_EXPORT(isdigit);
//RTM_EXPORT(isgraph);
//RTM_EXPORT(islower);
RTM_EXPORT(isprint);
//RTM_EXPORT(ispunct);
RTM_EXPORT(isspace);
//RTM_EXPORT(isupper);
//RTM_EXPORT(isxdigit);
//RTM_EXPORT(memchr);
RTM_EXPORT(memcmp);
RTM_EXPORT(memcpy);
RTM_EXPORT(memmove);
RTM_EXPORT(memset);
RTM_EXPORT(strcat);
RTM_EXPORT(strchr);
RTM_EXPORT(strcmp);
//RTM_EXPORT(strcoll);
RTM_EXPORT(strcpy);
RTM_EXPORT(strcspn);
//RTM_EXPORT(strerror);
RTM_EXPORT(strlen);
RTM_EXPORT(strncat);
RTM_EXPORT(strncmp);
RTM_EXPORT(strncpy);
//RTM_EXPORT(strpbrk);
RTM_EXPORT(strrchr);
RTM_EXPORT(strspn);
RTM_EXPORT(strstr);
//RTM_EXPORT(strtod);
RTM_EXPORT(strtok);
//RTM_EXPORT(strtol);
//RTM_EXPORT(strtoul);
//RTM_EXPORT(strxfrm);
RTM_EXPORT(tolower);
RTM_EXPORT(toupper);

/* Standard C Math */
//RTM_EXPORT(abs);
//RTM_EXPORT(acos);
//RTM_EXPORT(asin);
//RTM_EXPORT(atan);
//RTM_EXPORT(atan2);
//RTM_EXPORT(ceil);
//RTM_EXPORT(cos);
//RTM_EXPORT(cosh);
//RTM_EXPORT(div);
//RTM_EXPORT(exp);
//RTM_EXPORT(fabs);
//RTM_EXPORT(floor);
//RTM_EXPORT(fmod);
//RTM_EXPORT(frexp);
//RTM_EXPORT(labs);
//RTM_EXPORT(ldexp);
//RTM_EXPORT(ldiv);
//RTM_EXPORT(log);
//RTM_EXPORT(log10);
//RTM_EXPORT(modf);
//RTM_EXPORT(pow);
//RTM_EXPORT(sin);
//RTM_EXPORT(sinh);
//RTM_EXPORT(sqrt);
//RTM_EXPORT(tan);
//RTM_EXPORT(tanh);

/* Standard C Time & Date */
//RTM_EXPORT(asctime);
//RTM_EXPORT(clock);
//RTM_EXPORT(ctime);
//RTM_EXPORT(difftime);
//RTM_EXPORT(gmtime);
//RTM_EXPORT(localtime);
//RTM_EXPORT(mktime);
//RTM_EXPORT(strftime);
//RTM_EXPORT(time);

/* Standard C Memory */
RTM_EXPORT(calloc);
RTM_EXPORT(free);
RTM_EXPORT(malloc);
RTM_EXPORT(realloc);

/* Other standard C functions */
//RTM_EXPORT(abort);
//RTM_EXPORT(assert);
//RTM_EXPORT(atexit);
//RTM_EXPORT(bsearch);
//RTM_EXPORT(exit);
//RTM_EXPORT(getenv);
//RTM_EXPORT(longjmp);
//RTM_EXPORT(qsort);
//RTM_EXPORT(raise);
RTM_EXPORT(rand);
//RTM_EXPORT(setjmp);
//RTM_EXPORT(signal);
RTM_EXPORT(srand);
//RTM_EXPORT(system);
//RTM_EXPORT(va_arg);

/********** end export for libc **********/

/* BPABI symbols */
extern int __aeabi_cdcmpeq;
extern int __aeabi_cdcmple;
extern int __aeabi_cdrcmple;
extern int __aeabi_cfcmpeq;
extern int __aeabi_cfcmple;
extern int __aeabi_cfrcmple;
extern int __aeabi_d2f;
extern int __aeabi_d2iz;
extern int __aeabi_d2lz;
extern int __aeabi_d2uiz;
extern int __aeabi_d2ulz;
extern int __aeabi_dadd;
extern int __aeabi_dcmpeq;
extern int __aeabi_dcmpge;
extern int __aeabi_dcmpgt;
extern int __aeabi_dcmple;
extern int __aeabi_dcmplt;
extern int __aeabi_dcmpun;
extern int __aeabi_ddiv;
extern int __aeabi_dmul;
extern int __aeabi_dneg;
extern int __aeabi_drsub;
extern int __aeabi_dsub;
extern int __aeabi_f2d;
extern int __aeabi_f2iz;
extern int __aeabi_f2lz;
extern int __aeabi_f2uiz;
extern int __aeabi_f2ulz;
extern int __aeabi_fadd;
extern int __aeabi_fcmpeq;
extern int __aeabi_fcmpge;
extern int __aeabi_fcmpgt;
extern int __aeabi_fcmple;
extern int __aeabi_fcmplt;
extern int __aeabi_fcmpun;
extern int __aeabi_fdiv;
extern int __aeabi_fmul;
extern int __aeabi_fneg;
extern int __aeabi_frsub;
extern int __aeabi_fsub;
extern int __aeabi_i2d;
extern int __aeabi_i2f;
extern int __aeabi_idiv;
extern int __aeabi_idiv0;
extern int __aeabi_idivmod;
extern int __aeabi_l2d;
extern int __aeabi_l2f;
extern int __aeabi_lasr;
extern int __aeabi_lcmp;
extern int __aeabi_ldiv0;
extern int __aeabi_ldivmod;
extern int __aeabi_llsl;
extern int __aeabi_llsr;
extern int __aeabi_lmul;
extern int __aeabi_ui2d;
extern int __aeabi_ui2f;
extern int __aeabi_uidiv;
extern int __aeabi_uidivmod;
extern int __aeabi_uldivmod;
extern int __aeabi_ulcmp;
extern int __aeabi_ul2d;
extern int __aeabi_ul2f;
extern int __aeabi_uread4;
extern int __aeabi_uread8;
extern int __aeabi_uwrite4;
extern int __aeabi_uwrite8;

RTM_EXPORT(__aeabi_cdcmpeq);
RTM_EXPORT(__aeabi_cdcmple);
RTM_EXPORT(__aeabi_cdrcmple);
RTM_EXPORT(__aeabi_cfcmpeq);
RTM_EXPORT(__aeabi_cfcmple);
RTM_EXPORT(__aeabi_cfrcmple);
RTM_EXPORT(__aeabi_d2f);
RTM_EXPORT(__aeabi_d2iz);
RTM_EXPORT(__aeabi_d2lz);
RTM_EXPORT(__aeabi_d2uiz);
RTM_EXPORT(__aeabi_d2ulz);
RTM_EXPORT(__aeabi_dadd);
RTM_EXPORT(__aeabi_dcmpeq);
RTM_EXPORT(__aeabi_dcmpge);
RTM_EXPORT(__aeabi_dcmpgt);
RTM_EXPORT(__aeabi_dcmple);
RTM_EXPORT(__aeabi_dcmplt);
RTM_EXPORT(__aeabi_dcmpun);
RTM_EXPORT(__aeabi_ddiv);
RTM_EXPORT(__aeabi_dmul);
RTM_EXPORT(__aeabi_dneg);
RTM_EXPORT(__aeabi_drsub);
RTM_EXPORT(__aeabi_dsub);
RTM_EXPORT(__aeabi_f2d);
RTM_EXPORT(__aeabi_f2iz);
RTM_EXPORT(__aeabi_f2lz);
RTM_EXPORT(__aeabi_f2uiz);
RTM_EXPORT(__aeabi_f2ulz);
RTM_EXPORT(__aeabi_fadd);
RTM_EXPORT(__aeabi_fcmpeq);
RTM_EXPORT(__aeabi_fcmpge);
RTM_EXPORT(__aeabi_fcmpgt);
RTM_EXPORT(__aeabi_fcmple);
RTM_EXPORT(__aeabi_fcmplt);
RTM_EXPORT(__aeabi_fcmpun);
RTM_EXPORT(__aeabi_fdiv);
RTM_EXPORT(__aeabi_fmul);
RTM_EXPORT(__aeabi_fneg);
RTM_EXPORT(__aeabi_frsub);
RTM_EXPORT(__aeabi_fsub);
RTM_EXPORT(__aeabi_i2d);
RTM_EXPORT(__aeabi_i2f);
RTM_EXPORT(__aeabi_idiv);
RTM_EXPORT(__aeabi_idiv0);
RTM_EXPORT(__aeabi_idivmod);
RTM_EXPORT(__aeabi_l2d);
RTM_EXPORT(__aeabi_l2f);
RTM_EXPORT(__aeabi_lasr);
RTM_EXPORT(__aeabi_lcmp);
RTM_EXPORT(__aeabi_ldiv0);
RTM_EXPORT(__aeabi_ldivmod);
RTM_EXPORT(__aeabi_llsl);
RTM_EXPORT(__aeabi_llsr);
RTM_EXPORT(__aeabi_lmul);
RTM_EXPORT(__aeabi_ui2d);
RTM_EXPORT(__aeabi_ui2f);
RTM_EXPORT(__aeabi_uidiv);
RTM_EXPORT(__aeabi_uidivmod);
RTM_EXPORT(__aeabi_uldivmod);
RTM_EXPORT(__aeabi_ulcmp);
RTM_EXPORT(__aeabi_ul2d);
RTM_EXPORT(__aeabi_ul2f);
RTM_EXPORT(__aeabi_uread4);
RTM_EXPORT(__aeabi_uread8);
RTM_EXPORT(__aeabi_uwrite4);
RTM_EXPORT(__aeabi_uwrite8);
