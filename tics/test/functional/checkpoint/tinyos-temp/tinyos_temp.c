#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// VISP

//#define CHECKPOINT_TIMER
//#define CHECKPOINT_WORKINGSTACK

#include "nvm.h"
#include "arch.h"
#include "checkpoint.h"
#include "stackpool.h"
#include "virtualaddr.h"
#include "virtualmem.h"

// VISP variables
extern uint16_t ws_cnt;
extern uint16_t ws_restore_cnt;

// VISP TIMER
#include "checkpoint_timer.h"

volatile static NVM uint16_t isr_cnt = 0;
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) TIMER0_A0_ISR (void)
{
    ++isr_cnt;
    //__disable_interrupt();
    //checkpoint_and_reset_safe();
    checkpoint_full_safe();
}

void visp_print_log(void)
{
    printf("ISR cnt: %d\n", isr_cnt);
    printf("WS cnt: %d\n", ws_cnt);
    printf("WS Restore cnt: %d\n", ws_restore_cnt);
}


// END VISP

#define CALADC12_12V_30C  *((unsigned int *)0x1A1A)   // Temperature Sensor Calibration-30 C
                                                      //See device datasheet for TLV table memory mapping
#define CALADC12_12V_85C  *((unsigned int *)0x1A1C)   // Temperature Sensor Calibration-85 C

#define SAMPLE_SIZE 10

#define nx_struct struct
#define nx_union union
#define dbg(mode, format, ...) ((void)0)
#define dbg_clear(mode, format, ...) ((void)0)
#define dbg_active(mode) 0

#ifdef TINYOS_TYPES
//# 150 "/opt/local/lib/gcc47/gcc/x86_64-apple-darwin16/4.7.4/include/stddef.h" 3
typedef long int ptrdiff_t;
//#line 213
typedef long unsigned int size_t;
//#line 325
typedef int wchar_t;
//# 8 "/usr/local/lib/ncc/deputy_nodeputy.h"
struct __nesc_attr_nonnull {
//#line 8
  int dummy;
}  ;
//#line 9
struct __nesc_attr_bnd {
//#line 9
  void *lo, *hi;
}  ;
//#line 10
struct __nesc_attr_bnd_nok {
//#line 10
  void *lo, *hi;
}  ;
//#line 11
struct __nesc_attr_count {
//#line 11
  int n;
}  ;
//#line 12
struct __nesc_attr_count_nok {
//#line 12
  int n;
}  ;
//#line 13
struct __nesc_attr_one {
//#line 13
  int dummy;
}  ;
//#line 14
struct __nesc_attr_one_nok {
//#line 14
  int dummy;
}  ;
//#line 15
struct __nesc_attr_dmemset {
//#line 15
  int a1, a2, a3;
}  ;
//#line 16
struct __nesc_attr_dmemcpy {
//#line 16
  int a1, a2, a3;
}  ;
//#line 17
struct __nesc_attr_nts {
//#line 17
  int dummy;
}  ;
//# 37 "/usr/include/i386/_types.h" 3
typedef signed char __int8_t;



typedef unsigned char __uint8_t;
typedef short __int16_t;
typedef unsigned short __uint16_t;
typedef int __int32_t;
typedef unsigned int __uint32_t;
typedef long long __int64_t;
typedef unsigned long long __uint64_t;

typedef long __darwin_intptr_t;
typedef unsigned int __darwin_natural_t;
//#line 70
typedef int __darwin_ct_rune_t;








//#line 76
typedef union __nesc_unnamed4242 {
  char __mbstate8[128];
  long long _mbstateL;
} __mbstate_t;

typedef __mbstate_t __darwin_mbstate_t;


typedef long int __darwin_ptrdiff_t;







typedef long unsigned int __darwin_size_t;





typedef __builtin_va_list __darwin_va_list;





typedef int __darwin_wchar_t;




typedef __darwin_wchar_t __darwin_rune_t;


typedef int __darwin_wint_t;




typedef unsigned long __darwin_clock_t;
typedef __uint32_t __darwin_socklen_t;
typedef long __darwin_ssize_t;
typedef long __darwin_time_t;
//# 55 "/usr/include/sys/_types.h" 3
typedef __int64_t __darwin_blkcnt_t;
typedef __int32_t __darwin_blksize_t;
typedef __int32_t __darwin_dev_t;
typedef unsigned int __darwin_fsblkcnt_t;
typedef unsigned int __darwin_fsfilcnt_t;
typedef __uint32_t __darwin_gid_t;
typedef __uint32_t __darwin_id_t;
typedef __uint64_t __darwin_ino64_t;

typedef __darwin_ino64_t __darwin_ino_t;



typedef __darwin_natural_t __darwin_mach_port_name_t;
typedef __darwin_mach_port_name_t __darwin_mach_port_t;
typedef __uint16_t __darwin_mode_t;
typedef __int64_t __darwin_off_t;
typedef __int32_t __darwin_pid_t;
typedef __uint32_t __darwin_sigset_t;
typedef __int32_t __darwin_suseconds_t;
typedef __uint32_t __darwin_uid_t;
typedef __uint32_t __darwin_useconds_t;
typedef unsigned char __darwin_uuid_t[16];
typedef char __darwin_uuid_string_t[37];
//# 57 "/usr/include/sys/_pthread/_pthread_types.h" 3
struct __darwin_pthread_handler_rec {
  void (*__routine)(void *arg_0x10a86ea00);
  void *__arg;
  struct __darwin_pthread_handler_rec *__next;
};

struct _opaque_pthread_attr_t {
  long __sig;
  char __opaque[56];
};

struct _opaque_pthread_cond_t {
  long __sig;
  char __opaque[40];
};

struct _opaque_pthread_condattr_t {
  long __sig;
  char __opaque[8];
};

struct _opaque_pthread_mutex_t {
  long __sig;
  char __opaque[56];
};

struct _opaque_pthread_mutexattr_t {
  long __sig;
  char __opaque[8];
};

struct _opaque_pthread_once_t {
  long __sig;
  char __opaque[8];
};

struct _opaque_pthread_rwlock_t {
  long __sig;
  char __opaque[192];
};

struct _opaque_pthread_rwlockattr_t {
  long __sig;
  char __opaque[16];
};

struct _opaque_pthread_t {
  long __sig;
  struct __darwin_pthread_handler_rec *__cleanup_stack;
  char __opaque[8176];
};

typedef struct _opaque_pthread_attr_t __darwin_pthread_attr_t;
typedef struct _opaque_pthread_cond_t __darwin_pthread_cond_t;
typedef struct _opaque_pthread_condattr_t __darwin_pthread_condattr_t;
typedef unsigned long __darwin_pthread_key_t;
typedef struct _opaque_pthread_mutex_t __darwin_pthread_mutex_t;
typedef struct _opaque_pthread_mutexattr_t __darwin_pthread_mutexattr_t;
typedef struct _opaque_pthread_once_t __darwin_pthread_once_t;
typedef struct _opaque_pthread_rwlock_t __darwin_pthread_rwlock_t;
typedef struct _opaque_pthread_rwlockattr_t __darwin_pthread_rwlockattr_t;
typedef struct _opaque_pthread_t *__darwin_pthread_t;
//# 40 "/usr/include/_types.h" 3
typedef int __darwin_nl_item;
typedef int __darwin_wctrans_t;

typedef __uint32_t __darwin_wctype_t;
//# 30 "/usr/include/sys/_types/_int8_t.h" 3
typedef signed char int8_t;
//# 30 "/usr/include/sys/_types/_int16_t.h" 3
typedef short int16_t;
//# 30 "/usr/include/sys/_types/_int32_t.h" 3
typedef int int32_t;
//# 30 "/usr/include/sys/_types/_int64_t.h" 3
typedef long long int64_t;
//# 31 "/usr/include/_types/_uint8_t.h" 3
typedef unsigned char uint8_t;
//# 31 "/usr/include/_types/_uint16_t.h" 3
typedef unsigned short uint16_t;
//# 31 "/usr/include/_types/_uint32_t.h" 3
typedef unsigned int uint32_t;
//# 31 "/usr/include/_types/_uint64_t.h" 3
typedef unsigned long long uint64_t;
//# 38 "/opt/local/lib/gcc47/gcc/x86_64-apple-darwin16/4.7.4/include-fixed/stdint.h" 3
typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;
typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;



typedef int8_t int_fast8_t;
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef int64_t int_fast64_t;
typedef uint8_t uint_fast8_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;
//# 30 "/usr/include/sys/_types/_u_int8_t.h" 3
typedef unsigned char u_int8_t;
//# 30 "/usr/include/sys/_types/_u_int16_t.h" 3
typedef unsigned short u_int16_t;
//# 30 "/usr/include/sys/_types/_u_int32_t.h" 3
typedef unsigned int u_int32_t;
//# 30 "/usr/include/sys/_types/_u_int64_t.h" 3
typedef unsigned long long u_int64_t;
//# 87 "/usr/include/i386/types.h" 3
typedef int64_t register_t;
//# 30 "/usr/include/sys/_types/_uintptr_t.h" 3
typedef unsigned long uintptr_t;
//# 97 "/usr/include/i386/types.h" 3
typedef u_int64_t user_addr_t;
typedef u_int64_t user_size_t;
typedef int64_t user_ssize_t;
typedef int64_t user_long_t;
typedef u_int64_t user_ulong_t;
typedef int64_t user_time_t;
typedef int64_t user_off_t;







typedef u_int64_t syscall_arg_t;
//# 32 "/usr/include/sys/_types/_intptr_t.h" 3
typedef __darwin_intptr_t intptr_t;
//# 32 "/usr/include/_types/_intmax_t.h" 3
typedef long int intmax_t;
//# 32 "/usr/include/_types/_uintmax_t.h" 3
typedef long unsigned int uintmax_t;
//# 242 "/usr/include/inttypes.h" 3
//#line 239
typedef struct __nesc_unnamed4243 {
  intmax_t quot;
  intmax_t rem;
} imaxdiv_t;
//# 431 "/usr/local/lib/ncc/nesc_nx.h"
typedef struct { unsigned char nxdata[1]; } __attribute__((packed)) nx_int8_t;typedef int8_t __nesc_nxbase_nx_int8_t  ;
typedef struct { unsigned char nxdata[2]; } __attribute__((packed)) nx_int16_t;typedef int16_t __nesc_nxbase_nx_int16_t  ;
typedef struct { unsigned char nxdata[4]; } __attribute__((packed)) nx_int32_t;typedef int32_t __nesc_nxbase_nx_int32_t  ;
typedef struct { unsigned char nxdata[8]; } __attribute__((packed)) nx_int64_t;typedef int64_t __nesc_nxbase_nx_int64_t  ;
typedef struct { unsigned char nxdata[1]; } __attribute__((packed)) nx_uint8_t;typedef uint8_t __nesc_nxbase_nx_uint8_t  ;
typedef struct { unsigned char nxdata[2]; } __attribute__((packed)) nx_uint16_t;typedef uint16_t __nesc_nxbase_nx_uint16_t  ;
typedef struct { unsigned char nxdata[4]; } __attribute__((packed)) nx_uint32_t;typedef uint32_t __nesc_nxbase_nx_uint32_t  ;
typedef struct { unsigned char nxdata[8]; } __attribute__((packed)) nx_uint64_t;typedef uint64_t __nesc_nxbase_nx_uint64_t  ;


typedef struct { unsigned char nxdata[1]; } __attribute__((packed)) nxle_int8_t;typedef int8_t __nesc_nxbase_nxle_int8_t  ;
typedef struct { unsigned char nxdata[2]; } __attribute__((packed)) nxle_int16_t;typedef int16_t __nesc_nxbase_nxle_int16_t  ;
typedef struct { unsigned char nxdata[4]; } __attribute__((packed)) nxle_int32_t;typedef int32_t __nesc_nxbase_nxle_int32_t  ;
typedef struct { unsigned char nxdata[8]; } __attribute__((packed)) nxle_int64_t;typedef int64_t __nesc_nxbase_nxle_int64_t  ;
typedef struct { unsigned char nxdata[1]; } __attribute__((packed)) nxle_uint8_t;typedef uint8_t __nesc_nxbase_nxle_uint8_t  ;
typedef struct { unsigned char nxdata[2]; } __attribute__((packed)) nxle_uint16_t;typedef uint16_t __nesc_nxbase_nxle_uint16_t  ;
typedef struct { unsigned char nxdata[4]; } __attribute__((packed)) nxle_uint32_t;typedef uint32_t __nesc_nxbase_nxle_uint32_t  ;
typedef struct { unsigned char nxdata[8]; } __attribute__((packed)) nxle_uint64_t;typedef uint64_t __nesc_nxbase_nxle_uint64_t  ;
//# 74 "/usr/include/string.h" 3
void *memset(void *__b, int __c, size_t __len);
//# 31 "/usr/include/sys/_types/_rsize_t.h" 3
typedef __darwin_size_t rsize_t;
//# 30 "/usr/include/sys/_types/_errno_t.h" 3
typedef int errno_t;
//# 31 "/usr/include/sys/_types/_ssize_t.h" 3
typedef __darwin_ssize_t ssize_t;
//# 83 "/usr/include/sys/wait.h" 3
//#line 79
typedef enum __nesc_unnamed4244 {
  P_ALL,
  P_PID,
  P_PGID
} idtype_t;
//# 31 "/usr/include/sys/_types/_pid_t.h" 3
typedef __darwin_pid_t pid_t;
//# 31 "/usr/include/sys/_types/_id_t.h" 3
typedef __darwin_id_t id_t;
//# 39 "/usr/include/i386/signal.h" 3
typedef int sig_atomic_t;
//# 46 "/usr/include/mach/i386/_structs.h" 3
struct __darwin_i386_thread_state {

  unsigned int __eax;
  unsigned int __ebx;
  unsigned int __ecx;
  unsigned int __edx;
  unsigned int __edi;
  unsigned int __esi;
  unsigned int __ebp;
  unsigned int __esp;
  unsigned int __ss;
  unsigned int __eflags;
  unsigned int __eip;
  unsigned int __cs;
  unsigned int __ds;
  unsigned int __es;
  unsigned int __fs;
  unsigned int __gs;
};
//#line 92
struct __darwin_fp_control {

  unsigned short __invalid : 1,
  __denorm : 1,
  __zdiv : 1,
  __ovrfl : 1,
  __undfl : 1,
  __precis : 1,  :
  2,
  __pc : 2,





  __rc : 2,  :






  1,  :
  3;
};
typedef struct __darwin_fp_control __darwin_fp_control_t;
//#line 150
struct __darwin_fp_status {

  unsigned short __invalid : 1,
  __denorm : 1,
  __zdiv : 1,
  __ovrfl : 1,
  __undfl : 1,
  __precis : 1,
  __stkflt : 1,
  __errsumm : 1,
  __c0 : 1,
  __c1 : 1,
  __c2 : 1,
  __tos : 3,
  __c3 : 1,
  __busy : 1;
};
typedef struct __darwin_fp_status __darwin_fp_status_t;
//#line 194
struct __darwin_mmst_reg {

  char __mmst_reg[10];
  char __mmst_rsrv[6];
};
//#line 213
struct __darwin_xmm_reg {

  char __xmm_reg[16];
};
//#line 229
struct __darwin_ymm_reg {

  char __ymm_reg[32];
};
//#line 245
struct __darwin_zmm_reg {

  char __zmm_reg[64];
};










struct __darwin_opmask_reg {

  char __opmask_reg[8];
};
//#line 281
struct __darwin_i386_float_state {

  int __fpu_reserved[2];
  struct __darwin_fp_control __fpu_fcw;
  struct __darwin_fp_status __fpu_fsw;
  __uint8_t __fpu_ftw;
  __uint8_t __fpu_rsrv1;
  __uint16_t __fpu_fop;
  __uint32_t __fpu_ip;
  __uint16_t __fpu_cs;
  __uint16_t __fpu_rsrv2;
  __uint32_t __fpu_dp;
  __uint16_t __fpu_ds;
  __uint16_t __fpu_rsrv3;
  __uint32_t __fpu_mxcsr;
  __uint32_t __fpu_mxcsrmask;
  struct __darwin_mmst_reg __fpu_stmm0;
  struct __darwin_mmst_reg __fpu_stmm1;
  struct __darwin_mmst_reg __fpu_stmm2;
  struct __darwin_mmst_reg __fpu_stmm3;
  struct __darwin_mmst_reg __fpu_stmm4;
  struct __darwin_mmst_reg __fpu_stmm5;
  struct __darwin_mmst_reg __fpu_stmm6;
  struct __darwin_mmst_reg __fpu_stmm7;
  struct __darwin_xmm_reg __fpu_xmm0;
  struct __darwin_xmm_reg __fpu_xmm1;
  struct __darwin_xmm_reg __fpu_xmm2;
  struct __darwin_xmm_reg __fpu_xmm3;
  struct __darwin_xmm_reg __fpu_xmm4;
  struct __darwin_xmm_reg __fpu_xmm5;
  struct __darwin_xmm_reg __fpu_xmm6;
  struct __darwin_xmm_reg __fpu_xmm7;
  char __fpu_rsrv4[14 * 16];
  int __fpu_reserved1;
};


struct __darwin_i386_avx_state {

  int __fpu_reserved[2];
  struct __darwin_fp_control __fpu_fcw;
  struct __darwin_fp_status __fpu_fsw;
  __uint8_t __fpu_ftw;
  __uint8_t __fpu_rsrv1;
  __uint16_t __fpu_fop;
  __uint32_t __fpu_ip;
  __uint16_t __fpu_cs;
  __uint16_t __fpu_rsrv2;
  __uint32_t __fpu_dp;
  __uint16_t __fpu_ds;
  __uint16_t __fpu_rsrv3;
  __uint32_t __fpu_mxcsr;
  __uint32_t __fpu_mxcsrmask;
  struct __darwin_mmst_reg __fpu_stmm0;
  struct __darwin_mmst_reg __fpu_stmm1;
  struct __darwin_mmst_reg __fpu_stmm2;
  struct __darwin_mmst_reg __fpu_stmm3;
  struct __darwin_mmst_reg __fpu_stmm4;
  struct __darwin_mmst_reg __fpu_stmm5;
  struct __darwin_mmst_reg __fpu_stmm6;
  struct __darwin_mmst_reg __fpu_stmm7;
  struct __darwin_xmm_reg __fpu_xmm0;
  struct __darwin_xmm_reg __fpu_xmm1;
  struct __darwin_xmm_reg __fpu_xmm2;
  struct __darwin_xmm_reg __fpu_xmm3;
  struct __darwin_xmm_reg __fpu_xmm4;
  struct __darwin_xmm_reg __fpu_xmm5;
  struct __darwin_xmm_reg __fpu_xmm6;
  struct __darwin_xmm_reg __fpu_xmm7;
  char __fpu_rsrv4[14 * 16];
  int __fpu_reserved1;
  char __avx_reserved1[64];
  struct __darwin_xmm_reg __fpu_ymmh0;
  struct __darwin_xmm_reg __fpu_ymmh1;
  struct __darwin_xmm_reg __fpu_ymmh2;
  struct __darwin_xmm_reg __fpu_ymmh3;
  struct __darwin_xmm_reg __fpu_ymmh4;
  struct __darwin_xmm_reg __fpu_ymmh5;
  struct __darwin_xmm_reg __fpu_ymmh6;
  struct __darwin_xmm_reg __fpu_ymmh7;
};


struct __darwin_i386_avx512_state {

  int __fpu_reserved[2];
  struct __darwin_fp_control __fpu_fcw;
  struct __darwin_fp_status __fpu_fsw;
  __uint8_t __fpu_ftw;
  __uint8_t __fpu_rsrv1;
  __uint16_t __fpu_fop;
  __uint32_t __fpu_ip;
  __uint16_t __fpu_cs;
  __uint16_t __fpu_rsrv2;
  __uint32_t __fpu_dp;
  __uint16_t __fpu_ds;
  __uint16_t __fpu_rsrv3;
  __uint32_t __fpu_mxcsr;
  __uint32_t __fpu_mxcsrmask;
  struct __darwin_mmst_reg __fpu_stmm0;
  struct __darwin_mmst_reg __fpu_stmm1;
  struct __darwin_mmst_reg __fpu_stmm2;
  struct __darwin_mmst_reg __fpu_stmm3;
  struct __darwin_mmst_reg __fpu_stmm4;
  struct __darwin_mmst_reg __fpu_stmm5;
  struct __darwin_mmst_reg __fpu_stmm6;
  struct __darwin_mmst_reg __fpu_stmm7;
  struct __darwin_xmm_reg __fpu_xmm0;
  struct __darwin_xmm_reg __fpu_xmm1;
  struct __darwin_xmm_reg __fpu_xmm2;
  struct __darwin_xmm_reg __fpu_xmm3;
  struct __darwin_xmm_reg __fpu_xmm4;
  struct __darwin_xmm_reg __fpu_xmm5;
  struct __darwin_xmm_reg __fpu_xmm6;
  struct __darwin_xmm_reg __fpu_xmm7;
  char __fpu_rsrv4[14 * 16];
  int __fpu_reserved1;
  char __avx_reserved1[64];
  struct __darwin_xmm_reg __fpu_ymmh0;
  struct __darwin_xmm_reg __fpu_ymmh1;
  struct __darwin_xmm_reg __fpu_ymmh2;
  struct __darwin_xmm_reg __fpu_ymmh3;
  struct __darwin_xmm_reg __fpu_ymmh4;
  struct __darwin_xmm_reg __fpu_ymmh5;
  struct __darwin_xmm_reg __fpu_ymmh6;
  struct __darwin_xmm_reg __fpu_ymmh7;
  struct __darwin_opmask_reg __fpu_k0;
  struct __darwin_opmask_reg __fpu_k1;
  struct __darwin_opmask_reg __fpu_k2;
  struct __darwin_opmask_reg __fpu_k3;
  struct __darwin_opmask_reg __fpu_k4;
  struct __darwin_opmask_reg __fpu_k5;
  struct __darwin_opmask_reg __fpu_k6;
  struct __darwin_opmask_reg __fpu_k7;
  struct __darwin_ymm_reg __fpu_zmmh0;
  struct __darwin_ymm_reg __fpu_zmmh1;
  struct __darwin_ymm_reg __fpu_zmmh2;
  struct __darwin_ymm_reg __fpu_zmmh3;
  struct __darwin_ymm_reg __fpu_zmmh4;
  struct __darwin_ymm_reg __fpu_zmmh5;
  struct __darwin_ymm_reg __fpu_zmmh6;
  struct __darwin_ymm_reg __fpu_zmmh7;
};
//#line 575
struct __darwin_i386_exception_state {

  __uint16_t __trapno;
  __uint16_t __cpu;
  __uint32_t __err;
  __uint32_t __faultvaddr;
};
//#line 595
struct __darwin_x86_debug_state32 {

  unsigned int __dr0;
  unsigned int __dr1;
  unsigned int __dr2;
  unsigned int __dr3;
  unsigned int __dr4;
  unsigned int __dr5;
  unsigned int __dr6;
  unsigned int __dr7;
};
//#line 627
struct __darwin_x86_thread_state64 {

  __uint64_t __rax;
  __uint64_t __rbx;
  __uint64_t __rcx;
  __uint64_t __rdx;
  __uint64_t __rdi;
  __uint64_t __rsi;
  __uint64_t __rbp;
  __uint64_t __rsp;
  __uint64_t __r8;
  __uint64_t __r9;
  __uint64_t __r10;
  __uint64_t __r11;
  __uint64_t __r12;
  __uint64_t __r13;
  __uint64_t __r14;
  __uint64_t __r15;
  __uint64_t __rip;
  __uint64_t __rflags;
  __uint64_t __cs;
  __uint64_t __fs;
  __uint64_t __gs;
};
//#line 682
struct __darwin_x86_float_state64 {

  int __fpu_reserved[2];
  struct __darwin_fp_control __fpu_fcw;
  struct __darwin_fp_status __fpu_fsw;
  __uint8_t __fpu_ftw;
  __uint8_t __fpu_rsrv1;
  __uint16_t __fpu_fop;


  __uint32_t __fpu_ip;
  __uint16_t __fpu_cs;

  __uint16_t __fpu_rsrv2;


  __uint32_t __fpu_dp;
  __uint16_t __fpu_ds;

  __uint16_t __fpu_rsrv3;
  __uint32_t __fpu_mxcsr;
  __uint32_t __fpu_mxcsrmask;
  struct __darwin_mmst_reg __fpu_stmm0;
  struct __darwin_mmst_reg __fpu_stmm1;
  struct __darwin_mmst_reg __fpu_stmm2;
  struct __darwin_mmst_reg __fpu_stmm3;
  struct __darwin_mmst_reg __fpu_stmm4;
  struct __darwin_mmst_reg __fpu_stmm5;
  struct __darwin_mmst_reg __fpu_stmm6;
  struct __darwin_mmst_reg __fpu_stmm7;
  struct __darwin_xmm_reg __fpu_xmm0;
  struct __darwin_xmm_reg __fpu_xmm1;
  struct __darwin_xmm_reg __fpu_xmm2;
  struct __darwin_xmm_reg __fpu_xmm3;
  struct __darwin_xmm_reg __fpu_xmm4;
  struct __darwin_xmm_reg __fpu_xmm5;
  struct __darwin_xmm_reg __fpu_xmm6;
  struct __darwin_xmm_reg __fpu_xmm7;
  struct __darwin_xmm_reg __fpu_xmm8;
  struct __darwin_xmm_reg __fpu_xmm9;
  struct __darwin_xmm_reg __fpu_xmm10;
  struct __darwin_xmm_reg __fpu_xmm11;
  struct __darwin_xmm_reg __fpu_xmm12;
  struct __darwin_xmm_reg __fpu_xmm13;
  struct __darwin_xmm_reg __fpu_xmm14;
  struct __darwin_xmm_reg __fpu_xmm15;
  char __fpu_rsrv4[6 * 16];
  int __fpu_reserved1;
};


struct __darwin_x86_avx_state64 {

  int __fpu_reserved[2];
  struct __darwin_fp_control __fpu_fcw;
  struct __darwin_fp_status __fpu_fsw;
  __uint8_t __fpu_ftw;
  __uint8_t __fpu_rsrv1;
  __uint16_t __fpu_fop;


  __uint32_t __fpu_ip;
  __uint16_t __fpu_cs;

  __uint16_t __fpu_rsrv2;


  __uint32_t __fpu_dp;
  __uint16_t __fpu_ds;

  __uint16_t __fpu_rsrv3;
  __uint32_t __fpu_mxcsr;
  __uint32_t __fpu_mxcsrmask;
  struct __darwin_mmst_reg __fpu_stmm0;
  struct __darwin_mmst_reg __fpu_stmm1;
  struct __darwin_mmst_reg __fpu_stmm2;
  struct __darwin_mmst_reg __fpu_stmm3;
  struct __darwin_mmst_reg __fpu_stmm4;
  struct __darwin_mmst_reg __fpu_stmm5;
  struct __darwin_mmst_reg __fpu_stmm6;
  struct __darwin_mmst_reg __fpu_stmm7;
  struct __darwin_xmm_reg __fpu_xmm0;
  struct __darwin_xmm_reg __fpu_xmm1;
  struct __darwin_xmm_reg __fpu_xmm2;
  struct __darwin_xmm_reg __fpu_xmm3;
  struct __darwin_xmm_reg __fpu_xmm4;
  struct __darwin_xmm_reg __fpu_xmm5;
  struct __darwin_xmm_reg __fpu_xmm6;
  struct __darwin_xmm_reg __fpu_xmm7;
  struct __darwin_xmm_reg __fpu_xmm8;
  struct __darwin_xmm_reg __fpu_xmm9;
  struct __darwin_xmm_reg __fpu_xmm10;
  struct __darwin_xmm_reg __fpu_xmm11;
  struct __darwin_xmm_reg __fpu_xmm12;
  struct __darwin_xmm_reg __fpu_xmm13;
  struct __darwin_xmm_reg __fpu_xmm14;
  struct __darwin_xmm_reg __fpu_xmm15;
  char __fpu_rsrv4[6 * 16];
  int __fpu_reserved1;
  char __avx_reserved1[64];
  struct __darwin_xmm_reg __fpu_ymmh0;
  struct __darwin_xmm_reg __fpu_ymmh1;
  struct __darwin_xmm_reg __fpu_ymmh2;
  struct __darwin_xmm_reg __fpu_ymmh3;
  struct __darwin_xmm_reg __fpu_ymmh4;
  struct __darwin_xmm_reg __fpu_ymmh5;
  struct __darwin_xmm_reg __fpu_ymmh6;
  struct __darwin_xmm_reg __fpu_ymmh7;
  struct __darwin_xmm_reg __fpu_ymmh8;
  struct __darwin_xmm_reg __fpu_ymmh9;
  struct __darwin_xmm_reg __fpu_ymmh10;
  struct __darwin_xmm_reg __fpu_ymmh11;
  struct __darwin_xmm_reg __fpu_ymmh12;
  struct __darwin_xmm_reg __fpu_ymmh13;
  struct __darwin_xmm_reg __fpu_ymmh14;
  struct __darwin_xmm_reg __fpu_ymmh15;
};


struct __darwin_x86_avx512_state64 {

  int __fpu_reserved[2];
  struct __darwin_fp_control __fpu_fcw;
  struct __darwin_fp_status __fpu_fsw;
  __uint8_t __fpu_ftw;
  __uint8_t __fpu_rsrv1;
  __uint16_t __fpu_fop;


  __uint32_t __fpu_ip;
  __uint16_t __fpu_cs;

  __uint16_t __fpu_rsrv2;


  __uint32_t __fpu_dp;
  __uint16_t __fpu_ds;

  __uint16_t __fpu_rsrv3;
  __uint32_t __fpu_mxcsr;
  __uint32_t __fpu_mxcsrmask;
  struct __darwin_mmst_reg __fpu_stmm0;
  struct __darwin_mmst_reg __fpu_stmm1;
  struct __darwin_mmst_reg __fpu_stmm2;
  struct __darwin_mmst_reg __fpu_stmm3;
  struct __darwin_mmst_reg __fpu_stmm4;
  struct __darwin_mmst_reg __fpu_stmm5;
  struct __darwin_mmst_reg __fpu_stmm6;
  struct __darwin_mmst_reg __fpu_stmm7;
  struct __darwin_xmm_reg __fpu_xmm0;
  struct __darwin_xmm_reg __fpu_xmm1;
  struct __darwin_xmm_reg __fpu_xmm2;
  struct __darwin_xmm_reg __fpu_xmm3;
  struct __darwin_xmm_reg __fpu_xmm4;
  struct __darwin_xmm_reg __fpu_xmm5;
  struct __darwin_xmm_reg __fpu_xmm6;
  struct __darwin_xmm_reg __fpu_xmm7;
  struct __darwin_xmm_reg __fpu_xmm8;
  struct __darwin_xmm_reg __fpu_xmm9;
  struct __darwin_xmm_reg __fpu_xmm10;
  struct __darwin_xmm_reg __fpu_xmm11;
  struct __darwin_xmm_reg __fpu_xmm12;
  struct __darwin_xmm_reg __fpu_xmm13;
  struct __darwin_xmm_reg __fpu_xmm14;
  struct __darwin_xmm_reg __fpu_xmm15;
  char __fpu_rsrv4[6 * 16];
  int __fpu_reserved1;
  char __avx_reserved1[64];
  struct __darwin_xmm_reg __fpu_ymmh0;
  struct __darwin_xmm_reg __fpu_ymmh1;
  struct __darwin_xmm_reg __fpu_ymmh2;
  struct __darwin_xmm_reg __fpu_ymmh3;
  struct __darwin_xmm_reg __fpu_ymmh4;
  struct __darwin_xmm_reg __fpu_ymmh5;
  struct __darwin_xmm_reg __fpu_ymmh6;
  struct __darwin_xmm_reg __fpu_ymmh7;
  struct __darwin_xmm_reg __fpu_ymmh8;
  struct __darwin_xmm_reg __fpu_ymmh9;
  struct __darwin_xmm_reg __fpu_ymmh10;
  struct __darwin_xmm_reg __fpu_ymmh11;
  struct __darwin_xmm_reg __fpu_ymmh12;
  struct __darwin_xmm_reg __fpu_ymmh13;
  struct __darwin_xmm_reg __fpu_ymmh14;
  struct __darwin_xmm_reg __fpu_ymmh15;
  struct __darwin_opmask_reg __fpu_k0;
  struct __darwin_opmask_reg __fpu_k1;
  struct __darwin_opmask_reg __fpu_k2;
  struct __darwin_opmask_reg __fpu_k3;
  struct __darwin_opmask_reg __fpu_k4;
  struct __darwin_opmask_reg __fpu_k5;
  struct __darwin_opmask_reg __fpu_k6;
  struct __darwin_opmask_reg __fpu_k7;
  struct __darwin_ymm_reg __fpu_zmmh0;
  struct __darwin_ymm_reg __fpu_zmmh1;
  struct __darwin_ymm_reg __fpu_zmmh2;
  struct __darwin_ymm_reg __fpu_zmmh3;
  struct __darwin_ymm_reg __fpu_zmmh4;
  struct __darwin_ymm_reg __fpu_zmmh5;
  struct __darwin_ymm_reg __fpu_zmmh6;
  struct __darwin_ymm_reg __fpu_zmmh7;
  struct __darwin_ymm_reg __fpu_zmmh8;
  struct __darwin_ymm_reg __fpu_zmmh9;
  struct __darwin_ymm_reg __fpu_zmmh10;
  struct __darwin_ymm_reg __fpu_zmmh11;
  struct __darwin_ymm_reg __fpu_zmmh12;
  struct __darwin_ymm_reg __fpu_zmmh13;
  struct __darwin_ymm_reg __fpu_zmmh14;
  struct __darwin_ymm_reg __fpu_zmmh15;
  struct __darwin_zmm_reg __fpu_zmm16;
  struct __darwin_zmm_reg __fpu_zmm17;
  struct __darwin_zmm_reg __fpu_zmm18;
  struct __darwin_zmm_reg __fpu_zmm19;
  struct __darwin_zmm_reg __fpu_zmm20;
  struct __darwin_zmm_reg __fpu_zmm21;
  struct __darwin_zmm_reg __fpu_zmm22;
  struct __darwin_zmm_reg __fpu_zmm23;
  struct __darwin_zmm_reg __fpu_zmm24;
  struct __darwin_zmm_reg __fpu_zmm25;
  struct __darwin_zmm_reg __fpu_zmm26;
  struct __darwin_zmm_reg __fpu_zmm27;
  struct __darwin_zmm_reg __fpu_zmm28;
  struct __darwin_zmm_reg __fpu_zmm29;
  struct __darwin_zmm_reg __fpu_zmm30;
  struct __darwin_zmm_reg __fpu_zmm31;
};
//#line 1140
struct __darwin_x86_exception_state64 {

  __uint16_t __trapno;
  __uint16_t __cpu;
  __uint32_t __err;
  __uint64_t __faultvaddr;
};
//#line 1160
struct __darwin_x86_debug_state64 {

  __uint64_t __dr0;
  __uint64_t __dr1;
  __uint64_t __dr2;
  __uint64_t __dr3;
  __uint64_t __dr4;
  __uint64_t __dr5;
  __uint64_t __dr6;
  __uint64_t __dr7;
};
//#line 1188
struct __darwin_x86_cpmu_state64 {

  __uint64_t __ctrs[16];
};
//# 39 "/usr/include/i386/_mcontext.h" 3
struct __darwin_mcontext32 {

  struct __darwin_i386_exception_state __es;
  struct __darwin_i386_thread_state __ss;
  struct __darwin_i386_float_state __fs;
};


struct __darwin_mcontext_avx32 {

  struct __darwin_i386_exception_state __es;
  struct __darwin_i386_thread_state __ss;
  struct __darwin_i386_avx_state __fs;
};



struct __darwin_mcontext_avx512_32 {

  struct __darwin_i386_exception_state __es;
  struct __darwin_i386_thread_state __ss;
  struct __darwin_i386_avx512_state __fs;
};
//#line 97
struct __darwin_mcontext64 {

  struct __darwin_x86_exception_state64 __es;
  struct __darwin_x86_thread_state64 __ss;
  struct __darwin_x86_float_state64 __fs;
};


struct __darwin_mcontext_avx64 {

  struct __darwin_x86_exception_state64 __es;
  struct __darwin_x86_thread_state64 __ss;
  struct __darwin_x86_avx_state64 __fs;
};



struct __darwin_mcontext_avx512_64 {

  struct __darwin_x86_exception_state64 __es;
  struct __darwin_x86_thread_state64 __ss;
  struct __darwin_x86_avx512_state64 __fs;
};
//#line 156
typedef struct __darwin_mcontext64 *mcontext_t;
//# 31 "/usr/include/sys/_pthread/_pthread_attr_t.h" 3
typedef __darwin_pthread_attr_t pthread_attr_t;
//# 42 "/usr/include/sys/_types/_sigaltstack.h" 3
struct __darwin_sigaltstack {

  void *ss_sp;
  __darwin_size_t ss_size;
  int ss_flags;
};
typedef struct __darwin_sigaltstack stack_t;
//# 42 "/usr/include/sys/_types/_ucontext.h" 3
struct __darwin_ucontext {

  int uc_onstack;
  __darwin_sigset_t uc_sigmask;
  struct __darwin_sigaltstack uc_stack;
  struct __darwin_ucontext *uc_link;
  __darwin_size_t uc_mcsize;
  struct __darwin_mcontext64 *uc_mcontext;
};





typedef struct __darwin_ucontext ucontext_t;
//# 31 "/usr/include/sys/_types/_sigset_t.h" 3
typedef __darwin_sigset_t sigset_t;
//# 31 "/usr/include/sys/_types/_uid_t.h" 3
typedef __darwin_uid_t uid_t;
//# 158 "/usr/include/sys/signal.h" 3
union sigval {

  int sival_int;
  void *sival_ptr;
};





struct sigevent {
  int sigev_notify;
  int sigev_signo;
  union sigval sigev_value;
  void (*sigev_notify_function)(union sigval arg_0x10aaf7230);
  pthread_attr_t *sigev_notify_attributes;
};
//#line 188
//#line 177
typedef struct __siginfo {
  int si_signo;
  int si_errno;
  int si_code;
  pid_t si_pid;
  uid_t si_uid;
  int si_status;
  void *si_addr;
  union sigval si_value;
  long si_band;
  unsigned long __pad[7];
} siginfo_t;
//#line 269
union __sigaction_u {
  void (*__sa_handler)(int arg_0x10aafe590);
  void (*__sa_sigaction)(int arg_0x10aafec70, struct __siginfo *arg_0x10aafd020,
  void *arg_0x10aafd300);
};


struct __sigaction {
  union __sigaction_u __sigaction_u;
  void (*sa_tramp)(void *arg_0x10aafb2b0, int arg_0x10aafb530, int arg_0x10aafb7b0, siginfo_t *arg_0x10aafbad0, void *arg_0x10aafbd90);
  sigset_t sa_mask;
  int sa_flags;
};




struct sigaction {
  union __sigaction_u __sigaction_u;
  sigset_t sa_mask;
  int sa_flags;
};
//#line 331
typedef void (*sig_t)(int arg_0x10aaf9990);
//#line 348
struct sigvec {
  void (*sv_handler)(int arg_0x10ab04580);
  int sv_mask;
  int sv_flags;
};
//#line 367
struct sigstack {
  char *ss_sp;
  int ss_onstack;
};
//# 34 "/usr/include/sys/_types/_timeval.h" 3
struct timeval {

  __darwin_time_t tv_sec;
  __darwin_suseconds_t tv_usec;
};
//# 89 "/usr/include/sys/resource.h" 3
typedef __uint64_t rlim_t;
//#line 152
struct rusage {
  struct timeval ru_utime;
  struct timeval ru_stime;








  long ru_maxrss;

  long ru_ixrss;
  long ru_idrss;
  long ru_isrss;
  long ru_minflt;
  long ru_majflt;
  long ru_nswap;
  long ru_inblock;
  long ru_oublock;
  long ru_msgsnd;
  long ru_msgrcv;
  long ru_nsignals;
  long ru_nvcsw;
  long ru_nivcsw;
};
//#line 193
typedef void *rusage_info_t;

struct rusage_info_v0 {
  uint8_t ri_uuid[16];
  uint64_t ri_user_time;
  uint64_t ri_system_time;
  uint64_t ri_pkg_idle_wkups;
  uint64_t ri_interrupt_wkups;
  uint64_t ri_pageins;
  uint64_t ri_wired_size;
  uint64_t ri_resident_size;
  uint64_t ri_phys_footprint;
  uint64_t ri_proc_start_abstime;
  uint64_t ri_proc_exit_abstime;
};

struct rusage_info_v1 {
  uint8_t ri_uuid[16];
  uint64_t ri_user_time;
  uint64_t ri_system_time;
  uint64_t ri_pkg_idle_wkups;
  uint64_t ri_interrupt_wkups;
  uint64_t ri_pageins;
  uint64_t ri_wired_size;
  uint64_t ri_resident_size;
  uint64_t ri_phys_footprint;
  uint64_t ri_proc_start_abstime;
  uint64_t ri_proc_exit_abstime;
  uint64_t ri_child_user_time;
  uint64_t ri_child_system_time;
  uint64_t ri_child_pkg_idle_wkups;
  uint64_t ri_child_interrupt_wkups;
  uint64_t ri_child_pageins;
  uint64_t ri_child_elapsed_abstime;
};

struct rusage_info_v2 {
  uint8_t ri_uuid[16];
  uint64_t ri_user_time;
  uint64_t ri_system_time;
  uint64_t ri_pkg_idle_wkups;
  uint64_t ri_interrupt_wkups;
  uint64_t ri_pageins;
  uint64_t ri_wired_size;
  uint64_t ri_resident_size;
  uint64_t ri_phys_footprint;
  uint64_t ri_proc_start_abstime;
  uint64_t ri_proc_exit_abstime;
  uint64_t ri_child_user_time;
  uint64_t ri_child_system_time;
  uint64_t ri_child_pkg_idle_wkups;
  uint64_t ri_child_interrupt_wkups;
  uint64_t ri_child_pageins;
  uint64_t ri_child_elapsed_abstime;
  uint64_t ri_diskio_bytesread;
  uint64_t ri_diskio_byteswritten;
};

struct rusage_info_v3 {
  uint8_t ri_uuid[16];
  uint64_t ri_user_time;
  uint64_t ri_system_time;
  uint64_t ri_pkg_idle_wkups;
  uint64_t ri_interrupt_wkups;
  uint64_t ri_pageins;
  uint64_t ri_wired_size;
  uint64_t ri_resident_size;
  uint64_t ri_phys_footprint;
  uint64_t ri_proc_start_abstime;
  uint64_t ri_proc_exit_abstime;
  uint64_t ri_child_user_time;
  uint64_t ri_child_system_time;
  uint64_t ri_child_pkg_idle_wkups;
  uint64_t ri_child_interrupt_wkups;
  uint64_t ri_child_pageins;
  uint64_t ri_child_elapsed_abstime;
  uint64_t ri_diskio_bytesread;
  uint64_t ri_diskio_byteswritten;
  uint64_t ri_cpu_time_qos_default;
  uint64_t ri_cpu_time_qos_maintenance;
  uint64_t ri_cpu_time_qos_background;
  uint64_t ri_cpu_time_qos_utility;
  uint64_t ri_cpu_time_qos_legacy;
  uint64_t ri_cpu_time_qos_user_initiated;
  uint64_t ri_cpu_time_qos_user_interactive;
  uint64_t ri_billed_system_time;
  uint64_t ri_serviced_system_time;
};

struct rusage_info_v4 {
  uint8_t ri_uuid[16];
  uint64_t ri_user_time;
  uint64_t ri_system_time;
  uint64_t ri_pkg_idle_wkups;
  uint64_t ri_interrupt_wkups;
  uint64_t ri_pageins;
  uint64_t ri_wired_size;
  uint64_t ri_resident_size;
  uint64_t ri_phys_footprint;
  uint64_t ri_proc_start_abstime;
  uint64_t ri_proc_exit_abstime;
  uint64_t ri_child_user_time;
  uint64_t ri_child_system_time;
  uint64_t ri_child_pkg_idle_wkups;
  uint64_t ri_child_interrupt_wkups;
  uint64_t ri_child_pageins;
  uint64_t ri_child_elapsed_abstime;
  uint64_t ri_diskio_bytesread;
  uint64_t ri_diskio_byteswritten;
  uint64_t ri_cpu_time_qos_default;
  uint64_t ri_cpu_time_qos_maintenance;
  uint64_t ri_cpu_time_qos_background;
  uint64_t ri_cpu_time_qos_utility;
  uint64_t ri_cpu_time_qos_legacy;
  uint64_t ri_cpu_time_qos_user_initiated;
  uint64_t ri_cpu_time_qos_user_interactive;
  uint64_t ri_billed_system_time;
  uint64_t ri_serviced_system_time;
  uint64_t ri_logical_writes;
  uint64_t ri_lifetime_max_phys_footprint;
  uint64_t ri_instructions;
  uint64_t ri_cycles;
  uint64_t ri_billed_energy;
  uint64_t ri_serviced_energy;
  uint64_t ri_interval_max_phys_footprint;

  uint64_t ri_unused[1];
};

typedef struct rusage_info_v4 rusage_info_current;
//#line 366
struct rlimit {
  rlim_t rlim_cur;
  rlim_t rlim_max;
};
//#line 401
struct proc_rlimit_control_wakeupmon {
  uint32_t wm_flags;
  int32_t wm_rate;
};
//# 194 "/usr/include/sys/wait.h" 3
union wait {
  int w_status;



  struct __nesc_unnamed4245 {

    unsigned int w_Termsig : 7,
    w_Coredump : 1,
    w_Retcode : 8,
    w_Filler : 16;
  }






  w_T;





  struct __nesc_unnamed4246 {

    unsigned int w_Stopval : 8,
    w_Stopsig : 8,
    w_Filler : 16;
  }





  w_S;
};
//# 32 "/usr/include/sys/_types/_ct_rune_t.h" 3
typedef __darwin_ct_rune_t ct_rune_t;
//# 31 "/usr/include/sys/_types/_rune_t.h" 3
typedef __darwin_rune_t rune_t;
//# 86 "/usr/include/stdlib.h" 3
//#line 83
typedef struct __nesc_unnamed4247 {
  int quot;
  int rem;
} div_t;




//#line 88
typedef struct __nesc_unnamed4248 {
  long quot;
  long rem;
} ldiv_t;





//#line 94
typedef struct __nesc_unnamed4249 {
  long long quot;
  long long rem;
} lldiv_t;
//# 31 "/usr/include/sys/_types/_dev_t.h" 3
typedef __darwin_dev_t dev_t;
//# 31 "/usr/include/sys/_types/_mode_t.h" 3
typedef __darwin_mode_t mode_t;
//# 59 "/opt/local/lib/gcc47/gcc/x86_64-apple-darwin16/4.7.4/include-fixed/math.h" 3
typedef float float_t;
typedef double double_t;
//#line 668
struct __float2 {
//#line 668
  float __sinval;
//#line 668
  float __cosval;
};
//#line 669
struct __double2 {
//#line 669
  double __sinval;
//#line 669
  double __cosval;
};
struct __float2;
struct __double2;
struct __float2;
struct __double2;
//#line 779
struct exception {
  int type;
  char *name;
  double arg1;
  double arg2;
  double retval;
};
//# 32 "/usr/include/sys/_types/_wint_t.h" 3
typedef __darwin_wint_t wint_t;
//# 65 "/usr/include/runetype.h" 3
//#line 60
typedef struct __nesc_unnamed4250 {
  __darwin_rune_t __min;
  __darwin_rune_t __max;
  __darwin_rune_t __map;
  __uint32_t *__types;
} _RuneEntry;




//#line 67
typedef struct __nesc_unnamed4251 {
  int __nranges;
  _RuneEntry *__ranges;
} _RuneRange;




//#line 72
typedef struct __nesc_unnamed4252 {
  char __name[14];
  __uint32_t __mask;
} _RuneCharClass;
//#line 106
//#line 77
typedef struct __nesc_unnamed4253 {
  char __magic[8];
  char __encoding[32];

  __darwin_rune_t (*__sgetrune)(const char *arg_0x10acbe060, __darwin_size_t arg_0x10acbe380, char const **arg_0x10acbe6c0);
  int (*__sputrune)(__darwin_rune_t arg_0x10acbee20, char *arg_0x10acbd110, __darwin_size_t arg_0x10acbd430, char **arg_0x10acbd730);
  __darwin_rune_t __invalid_rune;

  __uint32_t __runetype[1 << 8];
  __darwin_rune_t __maplower[1 << 8];
  __darwin_rune_t __mapupper[1 << 8];






  _RuneRange __runetype_ext;
  _RuneRange __maplower_ext;
  _RuneRange __mapupper_ext;

  void *__variable;
  int __variable_len;




  int __ncharclasses;
  _RuneCharClass *__charclasses;
} _RuneLocale;
//# 25 "../../tos/system/tos.h"
typedef uint8_t bool;
enum __nesc_unnamed4254 {
//#line 26
  FALSE = 0, TRUE = 1
};
typedef nx_int8_t nx_bool;

#endif /*TINYOS_TYPES*/
#define TRUE true
#define FALSE false






struct __nesc_attr_atmostonce {
};
//#line 37
struct __nesc_attr_atleastonce {
};
//#line 38
struct __nesc_attr_exactlyonce {
};
//# 51 "../../tos/types/TinyError.h"
enum __nesc_unnamed4255 {
  SUCCESS = 0,
  FAIL = 1,
  ESIZE = 2,
  ECANCEL = 3,
  EOFF = 4,
  EBUSY = 5,
  EINVAL = 6,
  ERETRY = 7,
  ERESERVE = 8,
  EALREADY = 9,
  ENOMEM = 10,
  ENOACK = 11,
  ETIMEOUT = 12,
  ELAST = 12
};

typedef uint8_t error_t  ;
//# 4 "../../tos/platforms/null/hardware.h"
static __inline void __nesc_enable_interrupt();
static __inline void __nesc_disable_interrupt();

typedef uint8_t __nesc_atomic_t;
typedef uint8_t mcu_power_t;

__inline __nesc_atomic_t __nesc_atomic_start(void )  ;



__inline void __nesc_atomic_end(__nesc_atomic_t x)  ;



typedef struct { unsigned char nxdata[4]; } __attribute__((packed)) nx_float;typedef float __nesc_nxbase_nx_float  ;
//#line 34
enum __nesc_unnamed4256 {
  TOS_SLEEP_NONE = 0
};
//# 41 "../../tos/lib/timer/Timer.h"
typedef struct __nesc_unnamed4257 {
//#line 41
  int notUsed;
}
//#line 41
TSecond;
typedef struct __nesc_unnamed4258 {
//#line 42
  int notUsed;
}
//#line 42
TMilli;
typedef struct __nesc_unnamed4259 {
//#line 43
  int notUsed;
}
//#line 43
T32khz;
typedef struct __nesc_unnamed4260 {
//#line 44
  int notUsed;
}
//#line 44
TMicro;

//# 43 "../../tos/types/Leds.h"
enum __nesc_unnamed4261 {
  LEDS_LED0 = 1 << 0,
  LEDS_LED1 = 1 << 1,
  LEDS_LED2 = 1 << 2,
  LEDS_LED3 = 1 << 3,
  LEDS_LED4 = 1 << 4,
  LEDS_LED5 = 1 << 5,
  LEDS_LED6 = 1 << 6,
  LEDS_LED7 = 1 << 7
};
typedef uint16_t SenseC__Read__val_t;
typedef TMilli SenseC__Timer__precision_tag;
typedef TMilli HilTimerMilliC__TimerMilli__precision_tag;
typedef TMilli HilTimerMilliC__LocalTime__precision_tag;
typedef uint16_t /*SenseAppC.Sensor*/DemoSensorC__0__Read__val_t;
typedef uint16_t /*SenseAppC.Sensor*/DemoSensorC__0__ReadStream__val_t;
//# 60 "../../tos/interfaces/Boot.nc"
static void SenseC__Boot__booted(void );
//# 63 "../../tos/interfaces/Read.nc"
static void SenseC__Read__readDone(error_t result, SenseC__Read__val_t val);
//# 75 "../../tos/interfaces/TaskBasic.nc"
static void SenseC__senseTask__runTask(void );
//# 83 "../../tos/lib/timer/Timer.nc"
static void SenseC__Timer__fired(void );
//# 62 "../../tos/interfaces/Init.nc"
static error_t PlatformC__Init__init(void );
//# 67 "../../tos/interfaces/TaskBasic.nc"
static error_t SchedulerBasicP__TaskBasic__postTask(
//# 56 "../../tos/system/SchedulerBasicP.nc"
uint8_t arg_0x10ad1e6a0);
//# 75 "../../tos/interfaces/TaskBasic.nc"
static void SchedulerBasicP__TaskBasic__default__runTask(
//# 56 "../../tos/system/SchedulerBasicP.nc"
uint8_t arg_0x10ad1e6a0);
//# 57 "../../tos/interfaces/Scheduler.nc"
static void SchedulerBasicP__Scheduler__init(void );
//#line 72
static void SchedulerBasicP__Scheduler__taskLoop(void );
//#line 65
static bool SchedulerBasicP__Scheduler__runNextTask(void );
//# 79 "../../tos/interfaces/McuSleep.nc"
static void McuSleepC__McuSleep__sleep(void );
//# 61 "../../tos/interfaces/Leds.nc"
static void LedsP__Leds__led0Off(void );










static void LedsP__Leds__led1On(void );




static void LedsP__Leds__led1Off(void );
//#line 94
static void LedsP__Leds__led2Off(void );
//#line 56
static void LedsP__Leds__led0On(void );
//#line 89
static void LedsP__Leds__led2On(void );
//# 40 "../../tos/interfaces/GeneralIO.nc"
static void PlatformLedsC__Led0__set(void );
static void PlatformLedsC__Led0__clr(void );
//#line 40
static void PlatformLedsC__Led1__set(void );
static void PlatformLedsC__Led1__clr(void );
//#line 40
static void PlatformLedsC__Led2__set(void );
static void PlatformLedsC__Led2__clr(void );
//# 64 "../../tos/lib/timer/Timer.nc"
static void HilTimerMilliC__TimerMilli__startPeriodic(
//# 18 "../../tos/platforms/null/HilTimerMilliC.nc"
uint8_t arg_0x10adea4c0,
//# 64 "../../tos/lib/timer/Timer.nc"
uint32_t dt);
//# 62 "../../tos/interfaces/Init.nc"
static error_t HilTimerMilliC__Init__init(void );
//# 55 "../../tos/interfaces/Read.nc"
static error_t /*SenseAppC.Sensor*/DemoSensorC__0__Read__read(void );
//#line 55
static error_t SenseC__Read__read(void );
//# 61 "../../tos/interfaces/Leds.nc"
static void SenseC__Leds__led0Off(void );










static void SenseC__Leds__led1On(void );




static void SenseC__Leds__led1Off(void );
//#line 94
static void SenseC__Leds__led2Off(void );
//#line 56
static void SenseC__Leds__led0On(void );
//#line 89
static void SenseC__Leds__led2On(void );
//# 67 "../../tos/interfaces/TaskBasic.nc"
static error_t SenseC__senseTask__postTask(void );
//# 64 "../../tos/lib/timer/Timer.nc"
static void SenseC__Timer__startPeriodic(uint32_t dt);
//# 66 "SenseC.nc"
enum SenseC____nesc_unnamed4262 {
//#line 66
  SenseC__senseTask = 0U
};
//#line 66
typedef int SenseC____nesc_sillytask_senseTask[SenseC__senseTask];
//#line 61
float SenseC__temperature[SAMPLE_SIZE] = { 0 };

static inline void SenseC__Boot__booted(void );




static void SenseC__senseTask__runTask(void );







static inline void SenseC__Timer__fired(void );




static inline void SenseC__Read__readDone(error_t result, uint16_t data);
//# 19 "../../tos/platforms/null/PlatformC.nc"
static inline error_t PlatformC__Init__init(void );
//# 62 "../../tos/interfaces/Init.nc"
static error_t RealMainP__SoftwareInit__init(void );
//# 60 "../../tos/interfaces/Boot.nc"
static void RealMainP__Boot__booted(void );
//# 62 "../../tos/interfaces/Init.nc"
static error_t RealMainP__PlatformInit__init(void );
//# 57 "../../tos/interfaces/Scheduler.nc"
static void RealMainP__Scheduler__init(void );
//#line 72
static void RealMainP__Scheduler__taskLoop(void );
//#line 65
static bool RealMainP__Scheduler__runNextTask(void );
//# 63 "../../tos/system/RealMainP.nc"
int main(void )   ;
//# 75 "../../tos/interfaces/TaskBasic.nc"
static void SchedulerBasicP__TaskBasic__runTask(
//# 56 "../../tos/system/SchedulerBasicP.nc"
uint8_t arg_0x10ad1e6a0);
//# 79 "../../tos/interfaces/McuSleep.nc"
static void SchedulerBasicP__McuSleep__sleep(void );
//# 61 "../../tos/system/SchedulerBasicP.nc"
enum SchedulerBasicP____nesc_unnamed4263 {

  SchedulerBasicP__NUM_TASKS = 1U,
  SchedulerBasicP__NO_TASK = 255
};

uint8_t SchedulerBasicP__m_head;
uint8_t SchedulerBasicP__m_tail;
uint8_t SchedulerBasicP__m_next[SchedulerBasicP__NUM_TASKS];








static __inline uint8_t SchedulerBasicP__popTask(void );
//#line 97
static inline bool SchedulerBasicP__isWaiting(uint8_t id);




static inline bool SchedulerBasicP__pushTask(uint8_t id);
//#line 124
static inline void SchedulerBasicP__Scheduler__init(void );









static bool SchedulerBasicP__Scheduler__runNextTask(void );
//#line 149
static inline void SchedulerBasicP__Scheduler__taskLoop(void );
//#line 170
static inline error_t SchedulerBasicP__TaskBasic__postTask(uint8_t id);




static inline void SchedulerBasicP__TaskBasic__default__runTask(uint8_t id);
//# 22 "../../tos/platforms/null/McuSleepC.nc"
static inline void McuSleepC__McuSleep__sleep(void );
//# 40 "../../tos/interfaces/GeneralIO.nc"
static void LedsP__Led0__set(void );
static void LedsP__Led0__clr(void );
//#line 40
static void LedsP__Led1__set(void );
static void LedsP__Led1__clr(void );
//#line 40
static void LedsP__Led2__set(void );
static void LedsP__Led2__clr(void );
//# 74 "../../tos/system/LedsP.nc"
static inline void LedsP__Leds__led0On(void );




static inline void LedsP__Leds__led0Off(void );









static inline void LedsP__Leds__led1On(void );




static inline void LedsP__Leds__led1Off(void );









static inline void LedsP__Leds__led2On(void );




static inline void LedsP__Leds__led2Off(void );
//# 25 "../../tos/platforms/null/PlatformLedsC.nc"
static inline void PlatformLedsC__Led0__set(void );


static inline void PlatformLedsC__Led0__clr(void );
//#line 45
static inline void PlatformLedsC__Led1__set(void );


static inline void PlatformLedsC__Led1__clr(void );
//#line 65
static inline void PlatformLedsC__Led2__set(void );


static inline void PlatformLedsC__Led2__clr(void );
//# 24 "../../tos/platforms/null/HilTimerMilliC.nc"
static inline error_t HilTimerMilliC__Init__init(void );



static inline void HilTimerMilliC__TimerMilli__startPeriodic(uint8_t num, uint32_t dt);
//# 22 "../../tos/platforms/null/DemoSensorC.nc"
static inline error_t /*SenseAppC.Sensor*/DemoSensorC__0__Read__read(void );
//# 10 "../../tos/platforms/null/hardware.h"
__inline  __nesc_atomic_t __nesc_atomic_start(void )
//#line 10
{
    __nesc_atomic_t result = (__get_SR_register() & 0x0008) != 0;

    //#line 414
      __nesc_disable_interrupt();
       __asm volatile ("" :  :  : "memory");
      return result;
}

__inline  void __nesc_atomic_end(__nesc_atomic_t reenable_interrupts)
//#line 14
{
    __asm volatile ("" :  :  : "memory");
     if (reenable_interrupts) {
       __nesc_enable_interrupt();
       }
}

//# 124 "../../tos/system/SchedulerBasicP.nc"
static inline void SchedulerBasicP__Scheduler__init(void )
{
  /* atomic removed: atomic calls only */
  {
    memset((void *)SchedulerBasicP__m_next, SchedulerBasicP__NO_TASK, sizeof SchedulerBasicP__m_next);
    SchedulerBasicP__m_head = SchedulerBasicP__NO_TASK;
    SchedulerBasicP__m_tail = SchedulerBasicP__NO_TASK;
  }
}

//# 57 "../../tos/interfaces/Scheduler.nc"
inline static void RealMainP__Scheduler__init(void ){
//#line 57
  SchedulerBasicP__Scheduler__init();
//#line 57
}
//#line 57
//# 19 "../../tos/platforms/null/PlatformC.nc"
static inline error_t PlatformC__Init__init(void )
//#line 19
{
    WDTCTL = WDTPW | WDTHOLD;                   // Stop WDT
           PM5CTL0 &= ~LOCKLPM5;

           P1OUT &= ~BIT1;                             // Clear LED to start
           P1DIR |= BIT1;                              // Set P1.1/LED to output

           P1OUT &= ~BIT0;                             // Clear LED to start
           P1DIR |= BIT0;                              // Set P1.0/LED to output

           CSCTL0_H = CSKEY_H;                         // Unlock CS registers
           CSCTL1 = DCOFSEL_0 | DCORSEL;               // Set DCO to 1MHz
           CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
           CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;       // Set all dividers
           CSCTL0_H = 0;                               // Lock CS registers
  return SUCCESS;
}

//# 62 "../../tos/interfaces/Init.nc"
inline static error_t RealMainP__PlatformInit__init(void ){
//#line 62
  unsigned char __nesc_result;
//#line 62

//#line 62
  __nesc_result = PlatformC__Init__init();
//#line 62

//#line 62
  return __nesc_result;
//#line 62
}
//#line 62
//# 65 "../../tos/interfaces/Scheduler.nc"
inline static bool RealMainP__Scheduler__runNextTask(void ){
//#line 65
  unsigned char __nesc_result;
//#line 65

//#line 65
  __nesc_result = SchedulerBasicP__Scheduler__runNextTask();
//#line 65

//#line 65
  return __nesc_result;
//#line 65
}
//#line 65
//# 22 "../../tos/platforms/null/DemoSensorC.nc"
static inline error_t /*SenseAppC.Sensor*/DemoSensorC__0__Read__read(void )
//#line 22
{
    while(REFCTL0 & REFGENBUSY);            // If ref generator busy, WAIT
        REFCTL0 |= REFVSEL_0 + REFON;           // Enable internal 1.2V reference

        // Initialize ADC12_A
        ADC12CTL0 &= ~ADC12ENC;                 // Disable ADC12
        ADC12CTL0 = ADC12SHT0_8 | ADC12ON;      // Set sample time
        ADC12CTL1 = ADC12SHP;                   // Enable sample timer
        ADC12CTL3 = ADC12TCMAP;                 // Enable internal temperature sensor
        ADC12MCTL0 = ADC12VRSEL_1 + ADC12INCH_30; // ADC input ch A30 => temp sense

        while(!(REFCTL0 & REFGENRDY));          // Wait for reference generator


        ADC12CTL0 &= ~ADC12ENC;                 // Disable conversions
        ADC12CTL0 |= ADC12ON;                   // Turn on ADC
        ADC12CTL0 |= ADC12ENC;                  // Enable conversions


        ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
        while (!(ADC12IFGR0 & BIT0));
        unsigned int adcVal = ADC12MEM0;
        float temperatureDegC = (float)(((float)adcVal - CALADC12_12V_30C) * (85 - 30)) / (CALADC12_12V_85C - CALADC12_12V_30C) + 30.0f;


        ADC12CTL0 &= ~ADC12ENC;                 // Disable conversions
        ADC12CTL0 &= ~ADC12ON;                  // Turn off ADC
        ADC12CTL0 &= ~ADC12ENC;                  // Disable conversions


        SenseC__Read__readDone(SUCCESS, temperatureDegC);
  return SUCCESS;
}

//# 55 "../../tos/interfaces/Read.nc"
inline static error_t SenseC__Read__read(void ){
//#line 55
  unsigned char __nesc_result;
//#line 55

//#line 55
  __nesc_result = /*SenseAppC.Sensor*/DemoSensorC__0__Read__read();
//#line 55

//#line 55
  return __nesc_result;
//#line 55
}
//#line 55
//# 25 "../../tos/platforms/null/PlatformLedsC.nc"
static inline void PlatformLedsC__Led0__set(void )
//#line 25
{
    P1OUT &= ~BIT0;
}

//# 40 "../../tos/interfaces/GeneralIO.nc"
inline static void LedsP__Led0__set(void ){
//#line 40
  PlatformLedsC__Led0__set();
//#line 40
}
//#line 40
//# 79 "../../tos/system/LedsP.nc"
static inline void LedsP__Leds__led0Off(void )
//#line 79
{
  LedsP__Led0__set();
  ;
//#line 81
  ;
}

//# 61 "../../tos/interfaces/Leds.nc"
inline static void SenseC__Leds__led0Off(void ){
//#line 61
  LedsP__Leds__led0Off();
//#line 61
}
//#line 61
//# 28 "../../tos/platforms/null/PlatformLedsC.nc"
static inline void PlatformLedsC__Led0__clr(void )
//#line 28
{
    P1OUT |= BIT0;
}

//# 41 "../../tos/interfaces/GeneralIO.nc"
inline static void LedsP__Led0__clr(void ){
//#line 41
  PlatformLedsC__Led0__clr();
//#line 41
}
//#line 41
//# 74 "../../tos/system/LedsP.nc"
static inline void LedsP__Leds__led0On(void )
//#line 74
{
  LedsP__Led0__clr();
  ;
//#line 76
  ;
}

//# 56 "../../tos/interfaces/Leds.nc"
inline static void SenseC__Leds__led0On(void ){
//#line 56
  LedsP__Leds__led0On();
//#line 56
}
//#line 56
//# 45 "../../tos/platforms/null/PlatformLedsC.nc"
static inline void PlatformLedsC__Led1__set(void )
//#line 45
{
    P1OUT &= ~BIT1;
}

//# 40 "../../tos/interfaces/GeneralIO.nc"
inline static void LedsP__Led1__set(void ){
//#line 40
  PlatformLedsC__Led1__set();
//#line 40
}
//#line 40
//# 94 "../../tos/system/LedsP.nc"
static inline void LedsP__Leds__led1Off(void )
//#line 94
{
  LedsP__Led1__set();
  ;
//#line 96
  ;
}

//# 77 "../../tos/interfaces/Leds.nc"
inline static void SenseC__Leds__led1Off(void ){
//#line 77
  LedsP__Leds__led1Off();
//#line 77
}
//#line 77
//# 48 "../../tos/platforms/null/PlatformLedsC.nc"
static inline void PlatformLedsC__Led1__clr(void )
//#line 48
{
    P1OUT |= BIT1;
}

//# 41 "../../tos/interfaces/GeneralIO.nc"
inline static void LedsP__Led1__clr(void ){
//#line 41
  PlatformLedsC__Led1__clr();
//#line 41
}
//#line 41
//# 89 "../../tos/system/LedsP.nc"
static inline void LedsP__Leds__led1On(void )
//#line 89
{
  LedsP__Led1__clr();
  ;
//#line 91
  ;
}

//# 72 "../../tos/interfaces/Leds.nc"
inline static void SenseC__Leds__led1On(void ){
//#line 72
  LedsP__Leds__led1On();
//#line 72
}
//#line 72
//# 65 "../../tos/platforms/null/PlatformLedsC.nc"
static inline void PlatformLedsC__Led2__set(void )
//#line 65
{
}

//# 40 "../../tos/interfaces/GeneralIO.nc"
inline static void LedsP__Led2__set(void ){
//#line 40
  PlatformLedsC__Led2__set();
//#line 40
}
//#line 40
//# 109 "../../tos/system/LedsP.nc"
static inline void LedsP__Leds__led2Off(void )
//#line 109
{
  LedsP__Led2__set();
  ;
//#line 111
  ;
}

//# 94 "../../tos/interfaces/Leds.nc"
inline static void SenseC__Leds__led2Off(void ){
//#line 94
  LedsP__Leds__led2Off();
//#line 94
}
//#line 94
//# 68 "../../tos/platforms/null/PlatformLedsC.nc"
static inline void PlatformLedsC__Led2__clr(void )
//#line 68
{
}

//# 41 "../../tos/interfaces/GeneralIO.nc"
inline static void LedsP__Led2__clr(void ){
//#line 41
  PlatformLedsC__Led2__clr();
//#line 41
}
//#line 41
//# 104 "../../tos/system/LedsP.nc"
static inline void LedsP__Leds__led2On(void )
//#line 104
{
  LedsP__Led2__clr();
  ;
//#line 106
  ;
}

//# 89 "../../tos/interfaces/Leds.nc"
inline static void SenseC__Leds__led2On(void ){
//#line 89
  LedsP__Leds__led2On();
//#line 89
}
//#line 89
//# 79 "SenseC.nc"
static inline void SenseC__Read__readDone(error_t result, uint16_t data)
{
    if (result == SUCCESS) {
          unsigned int i = 0;
          float avg = 0;
          for(i = SAMPLE_SIZE - 1; i > 0; i--)
          {
              SenseC__temperature[i] = SenseC__temperature[i-1];
              avg += SenseC__temperature[i];
          }
          SenseC__temperature[0] = data;
          avg += SenseC__temperature[0];
          avg = avg/(float)SAMPLE_SIZE;

          if (avg > 10 && avg < 28) {
              SenseC__Leds__led0Off(); // red LED
              SenseC__Leds__led1On();  // green LED
          }
          else if (avg >= 28) {
              SenseC__Leds__led0On();  // red LED
              SenseC__Leds__led1On();  // green LED
          }
          else {
    //#line 89
              SenseC__Leds__led0Off();  // red LED
              SenseC__Leds__led1Off();  // green LED
            }
    //#line 90
        }
}

//# 24 "../../tos/platforms/null/HilTimerMilliC.nc"
static inline error_t HilTimerMilliC__Init__init(void )
//#line 24
{
  return SUCCESS;
}

//# 62 "../../tos/interfaces/Init.nc"
inline static error_t RealMainP__SoftwareInit__init(void ){
//#line 62
  unsigned char __nesc_result;
//#line 62

//#line 62
  __nesc_result = HilTimerMilliC__Init__init();
//#line 62

//#line 62
  return __nesc_result;
//#line 62
}
//#line 62
//# 4 "../../tos/platforms/null/hardware.h"
static __inline void __nesc_enable_interrupt()
//#line 4
{
//    __bis_SR_register(GIE);                 // Enable general interrupt
    __enable_interrupt();
}

//# 28 "../../tos/platforms/null/HilTimerMilliC.nc"
static inline void HilTimerMilliC__TimerMilli__startPeriodic(uint8_t num, uint32_t dt)
//#line 28
{
    TA1CCR0 = dt;
    TA1CTL = TASSEL__SMCLK | MC__UP | ID_3;      // SMCLK, UP mode
    TA1CCTL0 = CCIE;                             // TACCR0 interrupt enabled
}

//# 64 "../../tos/lib/timer/Timer.nc"
inline static void SenseC__Timer__startPeriodic(uint32_t dt){
//#line 64
  HilTimerMilliC__TimerMilli__startPeriodic(0U, dt);
//#line 64
}
//#line 64
//# 97 "../../tos/system/SchedulerBasicP.nc"
static inline bool SchedulerBasicP__isWaiting(uint8_t id)
{
  return SchedulerBasicP__m_next[id] != SchedulerBasicP__NO_TASK || SchedulerBasicP__m_tail == id;
}

static inline bool SchedulerBasicP__pushTask(uint8_t id)
{
  if (!SchedulerBasicP__isWaiting(id))
    {
      if (SchedulerBasicP__m_head == SchedulerBasicP__NO_TASK)
        {
          SchedulerBasicP__m_head = id;
          SchedulerBasicP__m_tail = id;
        }
      else
        {
          SchedulerBasicP__m_next[SchedulerBasicP__m_tail] = id;
          SchedulerBasicP__m_tail = id;
        }
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

//#line 170
static inline error_t SchedulerBasicP__TaskBasic__postTask(uint8_t id)
{
  { __nesc_atomic_t __nesc_atomic = __nesc_atomic_start();
//#line 172
    {
//#line 172
      {
        unsigned char __nesc_temp =
//#line 172
        SchedulerBasicP__pushTask(id) ? SUCCESS : EBUSY;

        {
//#line 172
          __nesc_atomic_end(__nesc_atomic);
//#line 172
          return __nesc_temp;
        }
      }
    }
//#line 175
    __nesc_atomic_end(__nesc_atomic); }
}

//# 67 "../../tos/interfaces/TaskBasic.nc"
inline static error_t SenseC__senseTask__postTask(void ){
//#line 67
  unsigned char __nesc_result;
//#line 67

//#line 67
  __nesc_result = SchedulerBasicP__TaskBasic__postTask(SenseC__senseTask);
//#line 67

//#line 67
  return __nesc_result;
//#line 67
}
//#line 67
//# 74 "SenseC.nc"
static inline void SenseC__Timer__fired(void )
{
  SenseC__senseTask__postTask();
}

//#line 61
static inline void SenseC__Boot__booted(void )
//#line 61
{
//  SenseC__Timer__fired();
  SenseC__Timer__startPeriodic(50000);
}

//# 60 "../../tos/interfaces/Boot.nc"
inline static void RealMainP__Boot__booted(void ){
//#line 60
  SenseC__Boot__booted();
//#line 60
}
//#line 60
//# 175 "../../tos/system/SchedulerBasicP.nc"
static inline void SchedulerBasicP__TaskBasic__default__runTask(uint8_t id)
{
}

//# 75 "../../tos/interfaces/TaskBasic.nc"
inline static void SchedulerBasicP__TaskBasic__runTask(uint8_t arg_0x10ad1e6a0){
//#line 75
  switch (arg_0x10ad1e6a0) {
//#line 75
    case SenseC__senseTask:
//#line 75
      SenseC__senseTask__runTask();
//#line 75
      break;
//#line 75
    default:
//#line 75
      SchedulerBasicP__TaskBasic__default__runTask(arg_0x10ad1e6a0);
//#line 75
      break;
//#line 75
    }
//#line 75
}
//#line 75
//# 22 "../../tos/platforms/null/McuSleepC.nc"
static inline void McuSleepC__McuSleep__sleep(void )
//#line 22
{
    __bis_SR_register(LPM3_bits + GIE);     // LPM3, COMPE_ISR will force exit

//    __low_power_mode_3();
    __nesc_disable_interrupt();
}

//# 79 "../../tos/interfaces/McuSleep.nc"
inline static void SchedulerBasicP__McuSleep__sleep(void ){
//#line 79
  McuSleepC__McuSleep__sleep();
//#line 79
}
//#line 79
//# 78 "../../tos/system/SchedulerBasicP.nc"
static __inline uint8_t SchedulerBasicP__popTask(void )
{
  if (SchedulerBasicP__m_head != SchedulerBasicP__NO_TASK)
    {
      uint8_t id = SchedulerBasicP__m_head;

//#line 83
      SchedulerBasicP__m_head = SchedulerBasicP__m_next[SchedulerBasicP__m_head];
      if (SchedulerBasicP__m_head == SchedulerBasicP__NO_TASK)
        {
          SchedulerBasicP__m_tail = SchedulerBasicP__NO_TASK;
        }
      SchedulerBasicP__m_next[id] = SchedulerBasicP__NO_TASK;
      return id;
    }
  else
    {
      return SchedulerBasicP__NO_TASK;
    }
}

//#line 149
static inline void SchedulerBasicP__Scheduler__taskLoop(void )
{
  for (; ; )
    {
      uint8_t nextTask;

      { __nesc_atomic_t __nesc_atomic = __nesc_atomic_start();
        {
          while ((nextTask = SchedulerBasicP__popTask()) == SchedulerBasicP__NO_TASK)
            {
              SchedulerBasicP__McuSleep__sleep();
            }
        }
//#line 161
        __nesc_atomic_end(__nesc_atomic); }
      SchedulerBasicP__TaskBasic__runTask(nextTask);
    }
}

//# 72 "../../tos/interfaces/Scheduler.nc"
inline static void RealMainP__Scheduler__taskLoop(void ){
//#line 72
  SchedulerBasicP__Scheduler__taskLoop();
//#line 72
}
//#line 72
//# 5 "../../tos/platforms/null/hardware.h"
static __inline void __nesc_disable_interrupt()
//#line 5
{
    __disable_interrupt();
//    __bic_SR_register(GIE);
    __nop();
}

NVM static int reboot = 0; /* VISP */
//# 63 "../../tos/system/RealMainP.nc"
  int main(void )
//#line 63
{
    // VISP
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    RealMainP__PlatformInit__init();

#ifdef CHECKPOINT_TIMER
    checkpoint_timer_init();
#endif
    if (reboot == 0) {
        checkpoint_full();
        reboot = 1;
    } else {
        checkpoint_restore();
    }

    // VISP END

  { __nesc_atomic_t __nesc_atomic = __nesc_atomic_start();
    {





      {
      }
//#line 71
      ;

      RealMainP__Scheduler__init();





      RealMainP__PlatformInit__init();
      while (RealMainP__Scheduler__runNextTask()) ;





      RealMainP__SoftwareInit__init();
      while (RealMainP__Scheduler__runNextTask()) ;
    }
//#line 88
    __nesc_atomic_end(__nesc_atomic); }


  __nesc_enable_interrupt();

  RealMainP__Boot__booted();


  RealMainP__Scheduler__taskLoop();




  return -1;
}

//# 134 "../../tos/system/SchedulerBasicP.nc"
static bool SchedulerBasicP__Scheduler__runNextTask(void )
{
  uint8_t nextTask;

  /* atomic removed: atomic calls only */
//#line 138
  {
    nextTask = SchedulerBasicP__popTask();
    if (nextTask == SchedulerBasicP__NO_TASK)
      {
        {
          unsigned char __nesc_temp =
//#line 142
          FALSE;

//#line 142
          return __nesc_temp;
        }
      }
  }
//#line 145
  SchedulerBasicP__TaskBasic__runTask(nextTask);
  return TRUE;
}

//# 66 "SenseC.nc"
static void SenseC__senseTask__runTask(void )
//#line 66
{
  uint16_t data = 0;

  data = SenseC__Read__read();
  //SenseC__Read__readDone(SUCCESS, data);
}

// Timer1_A interrupt service routine  ---------- For on time recording
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) Timer1_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
//    P1OUT ^= BIT0;
    SenseC__Timer__fired();
    __low_power_mode_off_on_exit();
//    __bic_SR_register_on_exit(LPM3_bits); // Exit active CPU
//    SenseC__Read__read();

}
