//#define CHECKPOINT_WORKINGSTACK
#define ENABLE_MANUAL_CHECKPOINT

#include <string.h>
#include <msp430.h>
#include "adc.h"

#include "driverlib.h"
#include "nvm.h"
#include "arch.h"
#include "checkpoint.h"
#include "stackpool.h"
#include "virtualaddr.h"
#include "virtualmem.h"
#include "checkpoint_timer.h"

//#undef VMEM_WR
//#define VMEM_WR

/* VISP */
#ifdef CHECKPOINT_WORKINGSTACK
#define CPWS __attribute__((segstack))
#else
#define CPWS
#endif

#ifdef PRINTF_PRINT
#define PRINTF printf
#else
#define PRINTF(...)
#endif /* PRINTF_PRINT */

extern uint16_t ws_cnt;
extern uint16_t ws_restore_cnt;
NVM uint32_t _manual_cp_cnt = 0;
NVM static int reboot = 0;

#ifdef ENABLE_MANUAL_CHECKPOINT
#define MANUAL_CHECKPOINT() checkpoint_full(); ++_manual_cp_cnt;
#else /* !ENABLE_MANUAL_CHECKPOINT */
#define MANUAL_CHECKPOINT()
#endif /* ENABLE_MANUAL_CHECKPOINT */

#define SAMPLE_SIZE 5

//#if defined(__TI_COMPILER_VERSION__)
//
//#pragma PERSISTENT(senseCount1, senseCount2, computeCount, sendCount)
//char senseCount1 = 0;
//char senseCount2 = 0;
//char computeCount = 0;
//char sendCount = 0;
//
//
//#elif defined(__GNUC__)
//char __attribute__((persistent)) senseCount1 = 0;
//char __attribute__((persistent)) senseCount2 = 0;
//char __attribute__((persistent)) computeCount = 0;
//char __attribute__((persistent)) sendCount = 0;
//#else
//#error Compiler not supported!
//#endif

struct Tuple {
    int m;
    float t;
};

volatile NVM char senseCount1 = 0;
volatile NVM char senseCount2 = 0;
volatile NVM char computeCount = 0;
volatile NVM char sendCount = 0;


NVM unsigned int moisture[SAMPLE_SIZE] = { 0 };
NVM float temperature[SAMPLE_SIZE] = { 0.0 };

NVM unsigned int moist;
NVM float temp;
NVM struct Tuple moistTempAvg;

CPWS void storeData(unsigned int m, float t);
CPWS void compute(struct Tuple avg);
CPWS void sendData(struct Tuple data);
CPWS void delay(unsigned int n);

/* Special */
struct Tuple calcAvg(void); //TODO: Structs directly might not work due too the MSP ABI

void portInterruptConfig(void); // in init
void ledConfig(void); // in init
void clockInit(void); // in init

void spiConfig(void)
{
    P5SEL1 &= ~(BIT0 | BIT1 | BIT2);            // USCI_B1 SCLK, MOSI, and MISO pin
    P5SEL0 |= (BIT0 | BIT1 | BIT2);

    UCB1CTLW0 = UCSWRST;                            // **Put state machine in reset**
    UCB1CTLW0 |= UCMST | UCSYNC | UCCKPL | UCMSB;   // 3-pin, 8-bit SPI master
                                                    // Clock polarity high, MSB
    UCB1CTLW0 |= UCSSEL__ACLK;                      // ACLK
    UCB1BRW = 0x02;                                 // /2

    UCB1CTLW0 &= ~UCSWRST;                          // **Initialize USCI state machine**
}

void spiSend(unsigned char _data)
{
    UCB1TXBUF = _data;                              // Transmit characters

    /* Wait for a rx character? */
    while (!(UCB1IFG & UCRXIFG));

    /* Reading clears RXIFG flag. */
    return UCB1RXBUF;
}

void ledConfig()
{
    P1OUT &= ~BIT1;                             // Clear LED to start
    P1DIR |= BIT1;                              // Set P1.1/LED to output

    P1OUT &= ~BIT0;                             // Clear LED to start
    P1DIR |= BIT0;                              // Set P1.0/LED to output
}

void watchdogInit()
{
    WDTCTL = WDTPW | WDTHOLD;                   // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;
}

void portInterruptConfig()
{
    P1OUT = BIT5;                               // Pull-up resistor on P1.1
    P1REN = BIT5;                               // Select pull-up mode for P1.1
    P1DIR = 0xFF ^ BIT5;                        // Set all but P1.1 to output direction
    P1IES = BIT5;                               // P1.1 Hi/Lo edge
    P1IFG = 0;                                  // Clear all P1 interrupt flags
    P1IE = BIT5;                                // P1.1 interrupt enabled
    __bis_SR_register(GIE);                     // Enable general interrupt
}

void clockInit()
{
    CSCTL0_H = CSKEY_H;                         // Unlock CS registers
    CSCTL1 = DCOFSEL_0 | DCORSEL;               // Set DCO to 1MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;       // Set all dividers
    CSCTL0_H = 0;                               // Lock CS register
}

static inline void platformInit(void)
{
    watchdogInit();
    ledConfig();
    portInterruptConfig();
//    clockInit();
    P8DIR &= ~BIT1;                             // For spiSend

}

struct Tuple calcAvg()
{
    unsigned int i = 0;
    struct Tuple avg = {0, 0.0};
    for(i = 0; i < SAMPLE_SIZE; i++)
    {
        avg.m += moisture[i];
        avg.t += temperature[i];
    }
    avg.m = avg.m/SAMPLE_SIZE;
    avg.t = avg.t/SAMPLE_SIZE;
    return avg;
}

void storeData(unsigned int m, float t)
{
    unsigned int i = 0;
    for(i = SAMPLE_SIZE - 1; i > 0; i--)
    {
        VMEM_WR(moisture[i]) = moisture[i-1];
        VMEM_WR(temperature[i]) = temperature[i-1];
    }
    VMEM_WR(moisture[0]) = m;
    VMEM_WR(temperature[0]) = t;
}

void compute(struct Tuple avg)
{
    if (avg.t > 10 && avg.t < 22)
    {
        P1OUT &= ~BIT0;
        P1OUT |= BIT1;
    }
    else if (avg.t >= 22)
    {
            P1OUT |= BIT0;
            P1OUT |= BIT1;
    }
    else
    {
        P1OUT &= ~BIT0;
        P1OUT &= ~BIT1;
    }
//    if (avg.m > 2500)
//    {
//        P1OUT &= ~BIT0;
//        P1OUT |= BIT1;
//    }
//    else if (avg.m < 2500)
//    {
//        P1OUT |= BIT0;
//        P1OUT |= BIT1;
//    }
//    else
//    {
//        P1OUT &= ~BIT0;
//        P1OUT &= ~BIT1;
//    }
}

void sendData(struct Tuple data)
{
    delay(2);
}

void delay(unsigned int n)
{
    unsigned int i = 0;
    for(i = 0; i < n; i++)
    {
        __delay_cycles(100000);
    }
}

#define nx_struct struct
#define nx_union union
#define dbg(mode, format, ...) ((void)0)
#define dbg_clear(mode, format, ...) ((void)0)
#define dbg_active(mode) 0

#ifdef TINYOS_TYPES
# 147 "/usr/lib/gcc/x86_64-linux-gnu/4.8/include/stddef.h" 3
typedef long int ptrdiff_t;
#line 212
typedef long unsigned int size_t;
#line 324
typedef int wchar_t;
# 8 "/usr/lib/ncc/deputy_nodeputy.h"
struct __nesc_attr_nonnull {
#line 8
  int dummy;
}  ;
#line 9
struct __nesc_attr_bnd {
#line 9
  void *lo, *hi;
}  ;
#line 10
struct __nesc_attr_bnd_nok {
#line 10
  void *lo, *hi;
}  ;
#line 11
struct __nesc_attr_count {
#line 11
  int n;
}  ;
#line 12
struct __nesc_attr_count_nok {
#line 12
  int n;
}  ;
#line 13
struct __nesc_attr_one {
#line 13
  int dummy;
}  ;
#line 14
struct __nesc_attr_one_nok {
#line 14
  int dummy;
}  ;
#line 15
struct __nesc_attr_dmemset {
#line 15
  int a1, a2, a3;
}  ;
#line 16
struct __nesc_attr_dmemcpy {
#line 16
  int a1, a2, a3;
}  ;
#line 17
struct __nesc_attr_nts {
#line 17
  int dummy;
}  ;
# 36 "/usr/include/stdint.h" 3
typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;

typedef long int int64_t;







typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;

typedef unsigned int uint32_t;



typedef unsigned long int uint64_t;









typedef signed char int_least8_t;
typedef short int int_least16_t;
typedef int int_least32_t;

typedef long int int_least64_t;






typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;

typedef unsigned long int uint_least64_t;









typedef signed char int_fast8_t;

typedef long int int_fast16_t;
typedef long int int_fast32_t;
typedef long int int_fast64_t;








typedef unsigned char uint_fast8_t;

typedef unsigned long int uint_fast16_t;
typedef unsigned long int uint_fast32_t;
typedef unsigned long int uint_fast64_t;
#line 119
typedef long int intptr_t;


typedef unsigned long int uintptr_t;
#line 134
typedef long int intmax_t;
typedef unsigned long int uintmax_t;
# 34 "/usr/include/inttypes.h" 3
typedef int __gwchar_t;
#line 275
#line 271
typedef struct __nesc_unnamed4242 {

  long int quot;
  long int rem;
} imaxdiv_t;
# 431 "/usr/lib/ncc/nesc_nx.h"
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
# 66 "/usr/include/string.h" 3
extern void *memset(void *__s, int __c, size_t __n) __attribute((__leaf__)) __attribute((__nothrow__)) __attribute((__nonnull__(1))) ;
# 39 "/usr/include/xlocale.h" 3
#line 27
typedef struct __locale_struct {


  struct __locale_data *__locales[13];


  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;


  const char *__names[13];
} *__locale_t;


typedef __locale_t locale_t;
# 55 "/usr/include/x86_64-linux-gnu/bits/waitflags.h" 3
#line 50
typedef enum __nesc_unnamed4243 {

  P_ALL,
  P_PID,
  P_PGID
} idtype_t;
# 30 "/usr/include/x86_64-linux-gnu/bits/types.h" 3
typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;


typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;

typedef signed long int __int64_t;
typedef unsigned long int __uint64_t;







typedef long int __quad_t;
typedef unsigned long int __u_quad_t;
#line 124
typedef unsigned long int __dev_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef unsigned long int __ino_t;
typedef unsigned long int __ino64_t;
typedef unsigned int __mode_t;
typedef unsigned long int __nlink_t;
typedef long int __off_t;
typedef long int __off64_t;
typedef int __pid_t;
typedef struct __nesc_unnamed4244 {
#line 134
  int __val[2];
}
#line 134
__fsid_t;
typedef long int __clock_t;
typedef unsigned long int __rlim_t;
typedef unsigned long int __rlim64_t;
typedef unsigned int __id_t;
typedef long int __time_t;
typedef unsigned int __useconds_t;
typedef long int __suseconds_t;

typedef int __daddr_t;
typedef int __key_t;


typedef int __clockid_t;


typedef void *__timer_t;


typedef long int __blksize_t;




typedef long int __blkcnt_t;
typedef long int __blkcnt64_t;


typedef unsigned long int __fsblkcnt_t;
typedef unsigned long int __fsblkcnt64_t;


typedef unsigned long int __fsfilcnt_t;
typedef unsigned long int __fsfilcnt64_t;


typedef long int __fsword_t;

typedef long int __ssize_t;


typedef long int __syscall_slong_t;

typedef unsigned long int __syscall_ulong_t;



typedef __off64_t __loff_t;
typedef __quad_t *__qaddr_t;
typedef char *__caddr_t;


typedef long int __intptr_t;


typedef unsigned int __socklen_t;
# 66 "/usr/include/x86_64-linux-gnu/bits/waitstatus.h" 3
union wait {

  int w_status;
  struct __nesc_unnamed4245 {


    unsigned int __w_termsig : 7;
    unsigned int __w_coredump : 1;
    unsigned int __w_retcode : 8;
    unsigned int  : 16;
  }






  __wait_terminated;
  struct __nesc_unnamed4246 {


    unsigned int __w_stopval : 8;
    unsigned int __w_stopsig : 8;
    unsigned int  : 16;
  }





  __wait_stopped;
};
# 71 "/usr/include/stdlib.h" 3
#line 67
typedef union __nesc_unnamed4247 {

  union wait *__uptr;
  int *__iptr;
} __WAIT_STATUS __attribute((__transparent_union__)) ;
#line 101
#line 97
typedef struct __nesc_unnamed4248 {

  int quot;
  int rem;
} div_t;







#line 105
typedef struct __nesc_unnamed4249 {

  long int quot;
  long int rem;
} ldiv_t;







__extension__



#line 117
typedef struct __nesc_unnamed4250 {

  long long int quot;
  long long int rem;
} lldiv_t;
#line 157
__extension__
#line 194
__extension__




__extension__








__extension__




__extension__
# 33 "/usr/include/x86_64-linux-gnu/sys/types.h" 3
typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;




typedef __loff_t loff_t;



typedef __ino_t ino_t;
#line 60
typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;





typedef __off_t off_t;
#line 98
typedef __pid_t pid_t;





typedef __id_t id_t;




typedef __ssize_t ssize_t;





typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;





typedef __key_t key_t;
# 59 "/usr/include/time.h" 3
typedef __clock_t clock_t;
#line 75
typedef __time_t time_t;
#line 91
typedef __clockid_t clockid_t;
#line 103
typedef __timer_t timer_t;
# 150 "/usr/include/x86_64-linux-gnu/sys/types.h" 3
typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
#line 200
typedef unsigned int u_int8_t __attribute((__mode__(__QI__))) ;
typedef unsigned int u_int16_t __attribute((__mode__(__HI__))) ;
typedef unsigned int u_int32_t __attribute((__mode__(__SI__))) ;
typedef unsigned int u_int64_t __attribute((__mode__(__DI__))) ;

typedef int register_t __attribute((__mode__(__word__))) ;
# 22 "/usr/include/x86_64-linux-gnu/bits/sigset.h" 3
typedef int __sig_atomic_t;







#line 27
typedef struct __nesc_unnamed4251 {

  unsigned long int __val[1024 / (8 * sizeof(unsigned long int ))];
} __sigset_t;
# 37 "/usr/include/x86_64-linux-gnu/sys/select.h" 3
typedef __sigset_t sigset_t;
# 120 "/usr/include/time.h" 3
struct timespec {

  __time_t tv_sec;
  __syscall_slong_t tv_nsec;
};
# 30 "/usr/include/x86_64-linux-gnu/bits/time.h" 3
struct timeval {

  __time_t tv_sec;
  __suseconds_t tv_usec;
};
# 48 "/usr/include/x86_64-linux-gnu/sys/select.h" 3
typedef __suseconds_t suseconds_t;





typedef long int __fd_mask;
#line 75
#line 64
typedef struct __nesc_unnamed4252 {







  __fd_mask __fds_bits[1024 / (8 * (int )sizeof(__fd_mask ))];
}

fd_set;






typedef __fd_mask fd_mask;
# 26 "/usr/include/x86_64-linux-gnu/sys/sysmacros.h" 3
__extension__


__extension__


__extension__
# 228 "/usr/include/x86_64-linux-gnu/sys/types.h" 3
typedef __blksize_t blksize_t;






typedef __blkcnt_t blkcnt_t;



typedef __fsblkcnt_t fsblkcnt_t;



typedef __fsfilcnt_t fsfilcnt_t;
# 60 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 3
typedef unsigned long int pthread_t;


union pthread_attr_t {

  char __size[56];
  long int __align;
};

typedef union pthread_attr_t pthread_attr_t;









#line 75
typedef struct __pthread_internal_list {

  struct __pthread_internal_list *__prev;
  struct __pthread_internal_list *__next;
} __pthread_list_t;
#line 127
#line 90
typedef union __nesc_unnamed4253 {

  struct __pthread_mutex_s {

    int __lock;
    unsigned int __count;
    int __owner;

    unsigned int __nusers;



    int __kind;

    short __spins;
    short __elision;
    __pthread_list_t __list;
  }
#line 124
  __data;
  char __size[40];
  long int __align;
} pthread_mutex_t;





#line 129
typedef union __nesc_unnamed4254 {

  char __size[4];
  int __align;
} pthread_mutexattr_t;
#line 153
#line 138
typedef union __nesc_unnamed4255 {

  struct __nesc_unnamed4256 {

    int __lock;
    unsigned int __futex;
    __extension__ unsigned long long int __total_seq;
    __extension__ unsigned long long int __wakeup_seq;
    __extension__ unsigned long long int __woken_seq;
    void *__mutex;
    unsigned int __nwaiters;
    unsigned int __broadcast_seq;
  } __data;
  char __size[48];
  __extension__ long long int __align;
} pthread_cond_t;





#line 155
typedef union __nesc_unnamed4257 {

  char __size[4];
  int __align;
} pthread_condattr_t;



typedef unsigned int pthread_key_t;



typedef int pthread_once_t;
#line 213
#line 173
typedef union __nesc_unnamed4258 {


  struct __nesc_unnamed4259 {

    int __lock;
    unsigned int __nr_readers;
    unsigned int __readers_wakeup;
    unsigned int __writer_wakeup;
    unsigned int __nr_readers_queued;
    unsigned int __nr_writers_queued;
    int __writer;
    int __shared;
    unsigned long int __pad1;
    unsigned long int __pad2;


    unsigned int __flags;
  }
  __data;
#line 211
  char __size[56];
  long int __align;
} pthread_rwlock_t;





#line 215
typedef union __nesc_unnamed4260 {

  char __size[8];
  long int __align;
} pthread_rwlockattr_t;





typedef volatile int pthread_spinlock_t;








#line 230
typedef union __nesc_unnamed4261 {

  char __size[32];
  long int __align;
} pthread_barrier_t;





#line 236
typedef union __nesc_unnamed4262 {

  char __size[4];
  int __align;
} pthread_barrierattr_t;
# 343 "/usr/include/stdlib.h" 3
struct random_data {

  int32_t *fptr;
  int32_t *rptr;
  int32_t *state;
  int rand_type;
  int rand_deg;
  int rand_sep;
  int32_t *end_ptr;
};
#line 412
struct drand48_data {

  unsigned short int __x[3];
  unsigned short int __old_x[3];
  unsigned short int __c;
  unsigned short int __init;
  __extension__ unsigned long long int __a;
};
#line 742
typedef int (*__compar_fn_t)(const void *arg_0x2b4e20a13cf8, const void *arg_0x2b4e20a12020);
#line 780
__extension__
#line 797
__extension__
# 28 "/usr/include/x86_64-linux-gnu/bits/mathdef.h" 3
typedef float float_t;
typedef double double_t;
# 326 "/usr/include/x86_64-linux-gnu/bits/mathcalls.h" 3
__extension__





__extension__
#line 326
__extension__





__extension__
#line 326
__extension__





__extension__
# 189 "/usr/include/math.h" 3
enum __nesc_unnamed4263 {

  FP_NAN =

  0,
  FP_INFINITE =

  1,
  FP_ZERO =

  2,
  FP_SUBNORMAL =

  3,
  FP_NORMAL =

  4
};
#line 308
#line 301
typedef enum __nesc_unnamed4264 {

  _IEEE_ = -1,
  _SVID_,
  _XOPEN_,
  _POSIX_,
  _ISOC_
} _LIB_VERSION_TYPE;
#line 326
struct exception {


  int type;
  char *name;
  double arg1;
  double arg2;
  double retval;
};
# 46 "/usr/include/ctype.h" 3
enum __nesc_unnamed4265 {

  _ISupper = 0 < 8 ? (1 << 0) << 8 : (1 << 0) >> 8,
  _ISlower = 1 < 8 ? (1 << 1) << 8 : (1 << 1) >> 8,
  _ISalpha = 2 < 8 ? (1 << 2) << 8 : (1 << 2) >> 8,
  _ISdigit = 3 < 8 ? (1 << 3) << 8 : (1 << 3) >> 8,
  _ISxdigit = 4 < 8 ? (1 << 4) << 8 : (1 << 4) >> 8,
  _ISspace = 5 < 8 ? (1 << 5) << 8 : (1 << 5) >> 8,
  _ISprint = 6 < 8 ? (1 << 6) << 8 : (1 << 6) >> 8,
  _ISgraph = 7 < 8 ? (1 << 7) << 8 : (1 << 7) >> 8,
  _ISblank = 8 < 8 ? (1 << 8) << 8 : (1 << 8) >> 8,
  _IScntrl = 9 < 8 ? (1 << 9) << 8 : (1 << 9) >> 8,
  _ISpunct = 10 < 8 ? (1 << 10) << 8 : (1 << 10) >> 8,
  _ISalnum = 11 < 8 ? (1 << 11) << 8 : (1 << 11) >> 8
};
# 25 "/opt/tinyos-2.1.2/tos/system/tos.h"
typedef uint8_t bool;
enum __nesc_unnamed4266 {
#line 26
  FALSE = 0, TRUE = 1
};
typedef nx_int8_t nx_bool;

#endif /*TINYOS_TYPES*/
#define TRUE true
#define FALSE false






struct __nesc_attr_atmostonce {
};
#line 37
struct __nesc_attr_atleastonce {
};
#line 38
struct __nesc_attr_exactlyonce {
};
# 51 "/opt/tinyos-2.1.2/tos/types/TinyError.h"
enum __nesc_unnamed4267 {
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
  ELAST = 11
};

typedef uint8_t error_t  ;
# 4 "/opt/tinyos-2.1.2/tos/platforms/null/hardware.h"
static __inline void __nesc_enable_interrupt();
static __inline void __nesc_disable_interrupt();

typedef uint8_t __nesc_atomic_t;
typedef uint8_t mcu_power_t;

__inline __nesc_atomic_t __nesc_atomic_start(void )  ;



__inline void __nesc_atomic_end(__nesc_atomic_t x)  ;



typedef struct { unsigned char nxdata[4]; } __attribute__((packed)) nx_float;typedef float __nesc_nxbase_nx_float  ;
#line 34
enum __nesc_unnamed4268 {
  TOS_SLEEP_NONE = 0
};
# 41 "/opt/tinyos-2.1.2/tos/lib/timer/Timer.h"
typedef struct __nesc_unnamed4269 {
#line 41
  int notUsed;
}
#line 41
TSecond;
typedef struct __nesc_unnamed4270 {
#line 42
  int notUsed;
}
#line 42
TMilli;
typedef struct __nesc_unnamed4271 {
#line 43
  int notUsed;
}
#line 43
T32khz;
typedef struct __nesc_unnamed4272 {
#line 44
  int notUsed;
}
#line 44
TMicro;
# 43 "/opt/tinyos-2.1.2/tos/types/Leds.h"
enum __nesc_unnamed4273 {
  LEDS_LED0 = 1 << 0,
  LEDS_LED1 = 1 << 1,
  LEDS_LED2 = 1 << 2,
  LEDS_LED3 = 1 << 3,
  LEDS_LED4 = 1 << 4,
  LEDS_LED5 = 1 << 5,
  LEDS_LED6 = 1 << 6,
  LEDS_LED7 = 1 << 7
};
typedef uint16_t SenseC$SenseTemp$val_t;
typedef uint16_t SenseC$SenseMoist$val_t;
typedef uint16_t /*SenseAppC.SensorMoist*/DemoSensorC$0$Read$val_t;
typedef uint16_t /*SenseAppC.SensorMoist*/DemoSensorC$0$ReadStream$val_t;
typedef uint16_t /*SenseAppC.SensorTemp*/DemoSensorC$1$Read$val_t;
typedef uint16_t /*SenseAppC.SensorTemp*/DemoSensorC$1$ReadStream$val_t;
# 63 "/opt/tinyos-2.1.2/tos/interfaces/Read.nc"
CPWS static void SenseC$SenseTemp$readDone(error_t result, SenseC$SenseTemp$val_t val);
# 60 "/opt/tinyos-2.1.2/tos/interfaces/Boot.nc"
CPWS static void SenseC$Boot$booted(void );
# 63 "/opt/tinyos-2.1.2/tos/interfaces/Read.nc"
CPWS static void SenseC$SenseMoist$readDone(error_t result, SenseC$SenseMoist$val_t val);
# 3 "Send.nc"
CPWS static void SenseC$Send$sendDone(error_t result);
# 75 "/opt/tinyos-2.1.2/tos/interfaces/TaskBasic.nc"
CPWS static void SenseC$sendTask$runTask(void );
#line 75
CPWS static void SenseC$senseTempTask$runTask(void );
#line 75
CPWS static void SenseC$senseMoistTask$runTask(void );
# 62 "/opt/tinyos-2.1.2/tos/interfaces/Init.nc"
CPWS static error_t PlatformC$Init$init(void );
#line 62
CPWS static error_t RealMainP$SoftwareInit$default$init(void );
# 67 "/opt/tinyos-2.1.2/tos/interfaces/TaskBasic.nc"
CPWS static error_t SchedulerBasicP$TaskBasic$postTask(
# 56 "/opt/tinyos-2.1.2/tos/system/SchedulerBasicP.nc"
uint8_t arg_0x2b4e20c392f0);
# 75 "/opt/tinyos-2.1.2/tos/interfaces/TaskBasic.nc"
CPWS static void SchedulerBasicP$TaskBasic$default$runTask(
# 56 "/opt/tinyos-2.1.2/tos/system/SchedulerBasicP.nc"
uint8_t arg_0x2b4e20c392f0);
# 57 "/opt/tinyos-2.1.2/tos/interfaces/Scheduler.nc"
CPWS static void SchedulerBasicP$Scheduler$init(void );
#line 72
CPWS static void SchedulerBasicP$Scheduler$taskLoop(void );
#line 65
CPWS static bool SchedulerBasicP$Scheduler$runNextTask(void );
# 76 "/opt/tinyos-2.1.2/tos/interfaces/McuSleep.nc"
CPWS static void McuSleepC$McuSleep$sleep(void );
# 61 "/opt/tinyos-2.1.2/tos/interfaces/Leds.nc"
CPWS static void LedsP$Leds$led0Off(void );










CPWS static void LedsP$Leds$led1On(void );




CPWS static void LedsP$Leds$led1Off(void );
#line 94
CPWS static void LedsP$Leds$led2Off(void );
#line 56
CPWS static void LedsP$Leds$led0On(void );
#line 89
CPWS static void LedsP$Leds$led2On(void );
# 40 "/opt/tinyos-2.1.2/tos/interfaces/GeneralIO.nc"
CPWS static void PlatformLedsC$Led0$set(void );
CPWS static void PlatformLedsC$Led0$clr(void );
#line 40
CPWS static void PlatformLedsC$Led1$set(void );
CPWS static void PlatformLedsC$Led1$clr(void );
#line 40
CPWS static void PlatformLedsC$Led2$set(void );
CPWS static void PlatformLedsC$Led2$clr(void );
# 55 "/opt/tinyos-2.1.2/tos/interfaces/Read.nc"
CPWS static error_t /*SenseAppC.SensorMoist*/DemoSensorC$0$Read$read(void );
#line 55
CPWS static error_t /*SenseAppC.SensorTemp*/DemoSensorC$1$Read$read(void );
# 2 "Send.nc"
CPWS static error_t SendC$Send$send(void );
# 55 "/opt/tinyos-2.1.2/tos/interfaces/Read.nc"
CPWS static error_t SenseC$SenseTemp$read(void );
#line 55
CPWS static error_t SenseC$SenseMoist$read(void );
# 2 "Send.nc"
CPWS static error_t SenseC$Send$send(void );
# 67 "/opt/tinyos-2.1.2/tos/interfaces/TaskBasic.nc"
CPWS static error_t SenseC$sendTask$postTask(void );
#line 67
CPWS static error_t SenseC$senseTempTask$postTask(void );
# 61 "/opt/tinyos-2.1.2/tos/interfaces/Leds.nc"
CPWS static void SenseC$Leds$led0Off(void );










CPWS static void SenseC$Leds$led1On(void );




CPWS static void SenseC$Leds$led1Off(void );
#line 94
CPWS static void SenseC$Leds$led2Off(void );
#line 56
CPWS static void SenseC$Leds$led0On(void );
#line 89
CPWS static void SenseC$Leds$led2On(void );
# 67 "/opt/tinyos-2.1.2/tos/interfaces/TaskBasic.nc"
CPWS static error_t SenseC$senseMoistTask$postTask(void );
# 59 "SenseC.nc"
enum SenseC$__nesc_unnamed4274 {
#line 59
  SenseC$sendTask = 0U
};
#line 59
typedef int SenseC$__nesc_sillytask_sendTask[SenseC$sendTask];




enum SenseC$__nesc_unnamed4275 {
#line 64
  SenseC$senseTempTask = 1U
};
#line 64
typedef int SenseC$__nesc_sillytask_senseTempTask[SenseC$senseTempTask];




enum SenseC$__nesc_unnamed4276 {
#line 69
  SenseC$senseMoistTask = 2U
};
#line 69
typedef int SenseC$__nesc_sillytask_senseMoistTask[SenseC$senseMoistTask];
#line 59
CPWS static inline void SenseC$sendTask$runTask(void );




CPWS static inline void SenseC$senseTempTask$runTask(void );




CPWS static inline void SenseC$senseMoistTask$runTask(void );




CPWS static inline void SenseC$Boot$booted(void );


CPWS static void SenseC$SenseMoist$readDone(error_t result, uint16_t data);
#line 96
CPWS static void SenseC$SenseTemp$readDone(error_t result, uint16_t data);
#line 115
CPWS static inline void SenseC$Send$sendDone(error_t result);
# 19 "/opt/tinyos-2.1.2/tos/platforms/null/PlatformC.nc"
CPWS static inline error_t PlatformC$Init$init(void );
# 62 "/opt/tinyos-2.1.2/tos/interfaces/Init.nc"
CPWS static error_t RealMainP$SoftwareInit$init(void );
# 60 "/opt/tinyos-2.1.2/tos/interfaces/Boot.nc"
CPWS static void RealMainP$Boot$booted(void );
# 62 "/opt/tinyos-2.1.2/tos/interfaces/Init.nc"
CPWS static error_t RealMainP$PlatformInit$init(void );
# 57 "/opt/tinyos-2.1.2/tos/interfaces/Scheduler.nc"
CPWS static void RealMainP$Scheduler$init(void );
#line 72
CPWS static void RealMainP$Scheduler$taskLoop(void );
#line 65
CPWS static bool RealMainP$Scheduler$runNextTask(void );
# 63 "/opt/tinyos-2.1.2/tos/system/RealMainP.nc"
int main(void )   ;
#line 105
CPWS static inline error_t RealMainP$SoftwareInit$default$init(void );
# 75 "/opt/tinyos-2.1.2/tos/interfaces/TaskBasic.nc"
CPWS static void SchedulerBasicP$TaskBasic$runTask(
# 56 "/opt/tinyos-2.1.2/tos/system/SchedulerBasicP.nc"
uint8_t arg_0x2b4e20c392f0);
# 76 "/opt/tinyos-2.1.2/tos/interfaces/McuSleep.nc"
CPWS static void SchedulerBasicP$McuSleep$sleep(void );
# 61 "/opt/tinyos-2.1.2/tos/system/SchedulerBasicP.nc"
enum SchedulerBasicP$__nesc_unnamed4277 {

  SchedulerBasicP$NUM_TASKS = 3U,
  SchedulerBasicP$NO_TASK = 255
};

NVM uint8_t SchedulerBasicP$m_head = 0;
NVM uint8_t SchedulerBasicP$m_tail = 0;
NVM uint8_t SchedulerBasicP$m_next[SchedulerBasicP$NUM_TASKS];








static __inline uint8_t SchedulerBasicP$popTask(void );
#line 97
static inline bool SchedulerBasicP$isWaiting(uint8_t id);




static inline bool SchedulerBasicP$pushTask(uint8_t id);
#line 124
static inline void SchedulerBasicP$Scheduler$init(void );









static bool SchedulerBasicP$Scheduler$runNextTask(void );
#line 149
static inline void SchedulerBasicP$Scheduler$taskLoop(void );
#line 170
static error_t SchedulerBasicP$TaskBasic$postTask(uint8_t id);




static inline void SchedulerBasicP$TaskBasic$default$runTask(uint8_t id);
# 22 "/opt/tinyos-2.1.2/tos/platforms/null/McuSleepC.nc"
static inline void McuSleepC$McuSleep$sleep(void );
# 40 "/opt/tinyos-2.1.2/tos/interfaces/GeneralIO.nc"
static void LedsP$Led0$set(void );
static void LedsP$Led0$clr(void );
#line 40
static void LedsP$Led1$set(void );
static void LedsP$Led1$clr(void );
#line 40
static void LedsP$Led2$set(void );
static void LedsP$Led2$clr(void );
# 74 "/opt/tinyos-2.1.2/tos/system/LedsP.nc"
static inline void LedsP$Leds$led0On(void );




static inline void LedsP$Leds$led0Off(void );









static inline void LedsP$Leds$led1On(void );




static inline void LedsP$Leds$led1Off(void );









static inline void LedsP$Leds$led2On(void );




static inline void LedsP$Leds$led2Off(void );
# 25 "/opt/tinyos-2.1.2/tos/platforms/null/PlatformLedsC.nc"
static inline void PlatformLedsC$Led0$set(void );


static inline void PlatformLedsC$Led0$clr(void );
#line 45
static inline void PlatformLedsC$Led1$set(void );


static inline void PlatformLedsC$Led1$clr(void );
#line 65
static inline void PlatformLedsC$Led2$set(void );


static inline void PlatformLedsC$Led2$clr(void );
# 22 "/opt/tinyos-2.1.2/tos/platforms/null/DemoSensorC.nc"
static inline error_t /*SenseAppC.SensorMoist*/DemoSensorC$0$Read$read(void );
#line 22
static inline error_t /*SenseAppC.SensorTemp*/DemoSensorC$1$Read$read(void );
# 8 "SendC.nc"
static inline error_t SendC$Send$send(void );
# 10 "/opt/tinyos-2.1.2/tos/platforms/null/hardware.h"
__inline  __nesc_atomic_t __nesc_atomic_start(void )
#line 10
{
  return 0;
}

__inline  void __nesc_atomic_end(__nesc_atomic_t x)
#line 14
{
}

# 124 "/opt/tinyos-2.1.2/tos/system/SchedulerBasicP.nc"
static inline void SchedulerBasicP$Scheduler$init(void )
{
  /* atomic removed: atomic calls only */
  {
    // VISP can stay the same as it was never read before
    memset((void *)SchedulerBasicP$m_next, SchedulerBasicP$NO_TASK, sizeof SchedulerBasicP$m_next);
    SchedulerBasicP$m_head = SchedulerBasicP$NO_TASK;
    SchedulerBasicP$m_tail = SchedulerBasicP$NO_TASK;
  }
}

# 57 "/opt/tinyos-2.1.2/tos/interfaces/Scheduler.nc"
inline static void RealMainP$Scheduler$init(void ){
#line 57
  SchedulerBasicP$Scheduler$init();
#line 57
}
#line 57
# 19 "/opt/tinyos-2.1.2/tos/platforms/null/PlatformC.nc"
static inline error_t PlatformC$Init$init(void )
#line 19
{
    platformInit();
  return SUCCESS;
}

# 62 "/opt/tinyos-2.1.2/tos/interfaces/Init.nc"
inline static error_t RealMainP$PlatformInit$init(void ){
#line 62
  unsigned char __nesc_result;
#line 62

#line 62
  __nesc_result = PlatformC$Init$init();
#line 62

#line 62
  return __nesc_result;
#line 62
}
#line 62
# 65 "/opt/tinyos-2.1.2/tos/interfaces/Scheduler.nc"
inline static bool RealMainP$Scheduler$runNextTask(void ){
#line 65
  unsigned char __nesc_result;
#line 65
    MANUAL_CHECKPOINT(); // VISP
#line 65
  __nesc_result = SchedulerBasicP$Scheduler$runNextTask();
#line 65

#line 65
  return __nesc_result;
#line 65
}
#line 65
# 68 "/opt/tinyos-2.1.2/tos/platforms/null/PlatformLedsC.nc"
static inline void PlatformLedsC$Led2$clr(void )
#line 68
{
}

# 41 "/opt/tinyos-2.1.2/tos/interfaces/GeneralIO.nc"
inline static void LedsP$Led2$clr(void ){
#line 41
  PlatformLedsC$Led2$clr();
#line 41
}
#line 41
# 104 "/opt/tinyos-2.1.2/tos/system/LedsP.nc"
static inline void LedsP$Leds$led2On(void )
#line 104
{
  LedsP$Led2$clr();
  ;
#line 106
  ;
}

# 89 "/opt/tinyos-2.1.2/tos/interfaces/Leds.nc"
inline static void SenseC$Leds$led2On(void ){
#line 89
  LedsP$Leds$led2On();
#line 89
}
#line 89
# 65 "/opt/tinyos-2.1.2/tos/platforms/null/PlatformLedsC.nc"
static inline void PlatformLedsC$Led2$set(void )
#line 65
{
}

# 40 "/opt/tinyos-2.1.2/tos/interfaces/GeneralIO.nc"
inline static void LedsP$Led2$set(void ){
#line 40
  PlatformLedsC$Led2$set();
#line 40
}
#line 40
# 109 "/opt/tinyos-2.1.2/tos/system/LedsP.nc"
static inline void LedsP$Leds$led2Off(void )
#line 109
{
  LedsP$Led2$set();
  ;
#line 111
  ;
}

# 94 "/opt/tinyos-2.1.2/tos/interfaces/Leds.nc"
inline static void SenseC$Leds$led2Off(void ){
#line 94
  LedsP$Leds$led2Off();
#line 94
}
#line 94
# 48 "/opt/tinyos-2.1.2/tos/platforms/null/PlatformLedsC.nc"
static inline void PlatformLedsC$Led1$clr(void )
#line 48
{
}

# 41 "/opt/tinyos-2.1.2/tos/interfaces/GeneralIO.nc"
inline static void LedsP$Led1$clr(void ){
#line 41
  PlatformLedsC$Led1$clr();
#line 41
}
#line 41
# 89 "/opt/tinyos-2.1.2/tos/system/LedsP.nc"
static inline void LedsP$Leds$led1On(void )
#line 89
{
  LedsP$Led1$clr();
  ;
#line 91
  ;
}

# 72 "/opt/tinyos-2.1.2/tos/interfaces/Leds.nc"
inline static void SenseC$Leds$led1On(void ){
#line 72
  LedsP$Leds$led1On();
#line 72
}
#line 72
# 45 "/opt/tinyos-2.1.2/tos/platforms/null/PlatformLedsC.nc"
static inline void PlatformLedsC$Led1$set(void )
#line 45
{
}

# 40 "/opt/tinyos-2.1.2/tos/interfaces/GeneralIO.nc"
inline static void LedsP$Led1$set(void ){
#line 40
  PlatformLedsC$Led1$set();
#line 40
}
#line 40
# 94 "/opt/tinyos-2.1.2/tos/system/LedsP.nc"
static inline void LedsP$Leds$led1Off(void )
#line 94
{
  LedsP$Led1$set();
  ;
#line 96
  ;
}

# 77 "/opt/tinyos-2.1.2/tos/interfaces/Leds.nc"
inline static void SenseC$Leds$led1Off(void ){
#line 77
  LedsP$Leds$led1Off();
#line 77
}
#line 77
# 28 "/opt/tinyos-2.1.2/tos/platforms/null/PlatformLedsC.nc"
static inline void PlatformLedsC$Led0$clr(void )
#line 28
{
}

# 41 "/opt/tinyos-2.1.2/tos/interfaces/GeneralIO.nc"
inline static void LedsP$Led0$clr(void ){
#line 41
  PlatformLedsC$Led0$clr();
#line 41
}
#line 41
# 74 "/opt/tinyos-2.1.2/tos/system/LedsP.nc"
static inline void LedsP$Leds$led0On(void )
#line 74
{
  LedsP$Led0$clr();
  ;
#line 76
  ;
}

# 56 "/opt/tinyos-2.1.2/tos/interfaces/Leds.nc"
inline static void SenseC$Leds$led0On(void ){
#line 56
  LedsP$Leds$led0On();
#line 56
}
#line 56
# 25 "/opt/tinyos-2.1.2/tos/platforms/null/PlatformLedsC.nc"
static inline void PlatformLedsC$Led0$set(void )
#line 25
{
}

# 40 "/opt/tinyos-2.1.2/tos/interfaces/GeneralIO.nc"
inline static void LedsP$Led0$set(void ){
#line 40
  PlatformLedsC$Led0$set();
#line 40
}
#line 40
# 79 "/opt/tinyos-2.1.2/tos/system/LedsP.nc"
static inline void LedsP$Leds$led0Off(void )
#line 79
{
  LedsP$Led0$set();
  ;
#line 81
  ;
}

# 61 "/opt/tinyos-2.1.2/tos/interfaces/Leds.nc"
inline static void SenseC$Leds$led0Off(void ){
#line 61
  LedsP$Leds$led0Off();
#line 61
}
#line 61
# 67 "/opt/tinyos-2.1.2/tos/interfaces/TaskBasic.nc"
inline static error_t SenseC$senseTempTask$postTask(void ){
#line 67
  unsigned char __nesc_result;
#line 67

#line 67
  __nesc_result = SchedulerBasicP$TaskBasic$postTask(SenseC$senseTempTask);
#line 67

#line 67
  return __nesc_result;
#line 67
}
#line 67
# 97 "/opt/tinyos-2.1.2/tos/system/SchedulerBasicP.nc"
static inline bool SchedulerBasicP$isWaiting(uint8_t id)
{
  return SchedulerBasicP$m_next[id] != SchedulerBasicP$NO_TASK || SchedulerBasicP$m_tail == id;
}

static inline bool SchedulerBasicP$pushTask(uint8_t id)
{
  if (!SchedulerBasicP$isWaiting(id))
    {
      if (SchedulerBasicP$m_head == SchedulerBasicP$NO_TASK)
        {
          VMEM_WR(SchedulerBasicP$m_head) = id;
          VMEM_WR(SchedulerBasicP$m_tail) = id;
        }
      else
        {
          VMEM_WR(SchedulerBasicP$m_next[SchedulerBasicP$m_tail]) = id;
          VMEM_WR(SchedulerBasicP$m_tail) = id;
        }
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

# 67 "/opt/tinyos-2.1.2/tos/interfaces/TaskBasic.nc"
inline static error_t SenseC$sendTask$postTask(void ){
#line 67
  unsigned char __nesc_result;
#line 67

#line 67
  __nesc_result = SchedulerBasicP$TaskBasic$postTask(SenseC$sendTask);
#line 67

#line 67
  return __nesc_result;
#line 67
}
#line 67
# 105 "/opt/tinyos-2.1.2/tos/system/RealMainP.nc"
static inline error_t RealMainP$SoftwareInit$default$init(void )
#line 105
{
#line 105
  return SUCCESS;
}

# 62 "/opt/tinyos-2.1.2/tos/interfaces/Init.nc"
inline static error_t RealMainP$SoftwareInit$init(void ){
#line 62
  unsigned char __nesc_result;
#line 62

#line 62
  __nesc_result = RealMainP$SoftwareInit$default$init();
#line 62

#line 62
  return __nesc_result;
#line 62
}
#line 62
# 4 "/opt/tinyos-2.1.2/tos/platforms/null/hardware.h"
static __inline void __nesc_enable_interrupt()
#line 4
{
}

# 67 "/opt/tinyos-2.1.2/tos/interfaces/TaskBasic.nc"
inline static error_t SenseC$senseMoistTask$postTask(void ){
#line 67
  unsigned char __nesc_result;
#line 67

#line 67
  __nesc_result = SchedulerBasicP$TaskBasic$postTask(SenseC$senseMoistTask);
#line 67

#line 67
  return __nesc_result;
#line 67
}
#line 67
# 74 "SenseC.nc"
static inline void SenseC$Boot$booted(void )
#line 74
{
  SenseC$senseMoistTask$postTask();
}

# 60 "/opt/tinyos-2.1.2/tos/interfaces/Boot.nc"
inline static void RealMainP$Boot$booted(void ){
#line 60
  SenseC$Boot$booted();
#line 60
}
#line 60
# 115 "SenseC.nc"
static inline void SenseC$Send$sendDone(error_t result)
#line 115
{
    VMEM_WR(sendCount) = sendCount + 1;
    SenseC$senseMoistTask$postTask();
}

# 8 "SendC.nc"
static inline error_t SendC$Send$send(void )
{
    sendData(moistTempAvg);
    SenseC$Send$sendDone(SUCCESS);
  return SUCCESS;
}

# 2 "Send.nc"
inline static error_t SenseC$Send$send(void ){
#line 2
  unsigned char __nesc_result;
#line 2

#line 2
  __nesc_result = SendC$Send$send();
#line 2

#line 2
  return __nesc_result;
#line 2
}
#line 2
# 59 "SenseC.nc"
static inline void SenseC$sendTask$runTask(void )
#line 59
{
  SenseC$Send$send();
}

# 22 "/opt/tinyos-2.1.2/tos/platforms/null/DemoSensorC.nc"
static inline error_t /*SenseAppC.SensorTemp*/DemoSensorC$1$Read$read(void )
#line 22
{
    tempConfig();
    VMEM_WR(temp) = tempDegC();
    SenseC$SenseTemp$readDone(SUCCESS, 0);
  return SUCCESS;
}

# 55 "/opt/tinyos-2.1.2/tos/interfaces/Read.nc"
inline static error_t SenseC$SenseTemp$read(void ){
#line 55
  unsigned char __nesc_result;
#line 55

#line 55
  __nesc_result = /*SenseAppC.SensorTemp*/DemoSensorC$1$Read$read();
#line 55

#line 55
  return __nesc_result;
#line 55
}
#line 55
# 64 "SenseC.nc"
static inline void SenseC$senseTempTask$runTask(void )
#line 64
{
  SenseC$SenseTemp$read();
}

# 22 "/opt/tinyos-2.1.2/tos/platforms/null/DemoSensorC.nc"
static inline error_t /*SenseAppC.SensorMoist*/DemoSensorC$0$Read$read(void )
#line 22
{
    adcConfig();
    VMEM_WR(moist) = adcSample();
    SenseC$SenseMoist$readDone(SUCCESS, moist);
  return SUCCESS;
}

# 55 "/opt/tinyos-2.1.2/tos/interfaces/Read.nc"
inline static error_t SenseC$SenseMoist$read(void ){
#line 55
  unsigned char __nesc_result;
#line 55

#line 55
  __nesc_result = /*SenseAppC.SensorMoist*/DemoSensorC$0$Read$read();
#line 55

#line 55
  return __nesc_result;
#line 55
}
#line 55
# 69 "SenseC.nc"
static inline void SenseC$senseMoistTask$runTask(void )
#line 69
{
    if(P8IN & 0x02)
    {
        SenseC$SenseMoist$read();
    }
    else
    {
        P1OUT ^= BIT0;
        spiConfig();
        spiSend(1);
        spiSend(senseCount1);
        spiSend(2);
        spiSend(senseCount2);
        spiSend(3);
        spiSend(computeCount);
        spiSend(4);
        spiSend(sendCount);

        SenseC$senseMoistTask$postTask();
        delay(5);
    }
}

# 175 "/opt/tinyos-2.1.2/tos/system/SchedulerBasicP.nc"
static inline void SchedulerBasicP$TaskBasic$default$runTask(uint8_t id)
{
}

# 75 "/opt/tinyos-2.1.2/tos/interfaces/TaskBasic.nc"
inline static void SchedulerBasicP$TaskBasic$runTask(uint8_t arg_0x2b4e20c392f0){
#line 75
  switch (arg_0x2b4e20c392f0) {
#line 75
    case SenseC$sendTask:
#line 75
      SenseC$sendTask$runTask();
#line 75
      break;
#line 75
    case SenseC$senseTempTask:
#line 75
      SenseC$senseTempTask$runTask();
#line 75
      break;
#line 75
    case SenseC$senseMoistTask:
#line 75
      SenseC$senseMoistTask$runTask();
#line 75
      break;
#line 75
    default:
#line 75
      SchedulerBasicP$TaskBasic$default$runTask(arg_0x2b4e20c392f0);
#line 75
      break;
#line 75
    }
#line 75
}
#line 75
# 22 "/opt/tinyos-2.1.2/tos/platforms/null/McuSleepC.nc"
static inline void McuSleepC$McuSleep$sleep(void )
#line 22
{
}

# 76 "/opt/tinyos-2.1.2/tos/interfaces/McuSleep.nc"
inline static void SchedulerBasicP$McuSleep$sleep(void ){
#line 76
  McuSleepC$McuSleep$sleep();
#line 76
}
#line 76
# 78 "/opt/tinyos-2.1.2/tos/system/SchedulerBasicP.nc"
static __inline uint8_t SchedulerBasicP$popTask(void )
{
  if (SchedulerBasicP$m_head != SchedulerBasicP$NO_TASK)
    {
      uint8_t id = SchedulerBasicP$m_head;

#line 83
      VMEM_WR(SchedulerBasicP$m_head) = SchedulerBasicP$m_next[SchedulerBasicP$m_head];
      if (SchedulerBasicP$m_head == SchedulerBasicP$NO_TASK)
        {
          VMEM_WR(SchedulerBasicP$m_tail) = SchedulerBasicP$NO_TASK;
        }
      VMEM_WR(SchedulerBasicP$m_next[id]) = SchedulerBasicP$NO_TASK;
      return id;
    }
  else
    {
      return SchedulerBasicP$NO_TASK;
    }
}

#line 149
static inline void SchedulerBasicP$Scheduler$taskLoop(void )
{
  for (; ; )
    {
      uint8_t nextTask;

      { __nesc_atomic_t __nesc_atomic = __nesc_atomic_start();
        {
          while ((nextTask = SchedulerBasicP$popTask()) == SchedulerBasicP$NO_TASK)
            {
              SchedulerBasicP$McuSleep$sleep();
            }
        }
#line 161
        __nesc_atomic_end(__nesc_atomic); }
      MANUAL_CHECKPOINT(); // VISP
      SchedulerBasicP$TaskBasic$runTask(nextTask);
    }
}

# 72 "/opt/tinyos-2.1.2/tos/interfaces/Scheduler.nc"
inline static void RealMainP$Scheduler$taskLoop(void ){
#line 72
  SchedulerBasicP$Scheduler$taskLoop();
#line 72
}
#line 72
# 5 "/opt/tinyos-2.1.2/tos/platforms/null/hardware.h"
static __inline void __nesc_disable_interrupt()
#line 5
{
}

# 63 "/opt/tinyos-2.1.2/tos/system/RealMainP.nc"
  int main(void )
#line 63
{
   platformInit(); // VISP

   if (reboot == 0) {
       checkpoint_full();
       reboot = 1;
   } else {
       checkpoint_restore();
   }

  { __nesc_atomic_t __nesc_atomic = __nesc_atomic_start();
    {





      {
      }
#line 71
      ;

      RealMainP$Scheduler$init();


      RealMainP$PlatformInit$init();
      while (RealMainP$Scheduler$runNextTask()) ;





      RealMainP$SoftwareInit$init();
      while (RealMainP$Scheduler$runNextTask()) ;
    }
#line 88
    __nesc_atomic_end(__nesc_atomic); }


  __nesc_enable_interrupt();

  RealMainP$Boot$booted();


  RealMainP$Scheduler$taskLoop();




  return -1;
}

# 134 "/opt/tinyos-2.1.2/tos/system/SchedulerBasicP.nc"
static bool SchedulerBasicP$Scheduler$runNextTask(void )
{
  uint8_t nextTask;

  /* atomic removed: atomic calls only */
#line 138
  {
    nextTask = SchedulerBasicP$popTask();
    if (nextTask == SchedulerBasicP$NO_TASK)
      {
        {
          unsigned char __nesc_temp =
#line 142
          FALSE;

#line 142
          return __nesc_temp;
        }
      }
  }
#line 145
  SchedulerBasicP$TaskBasic$runTask(nextTask);
  return TRUE;
}

# 77 "SenseC.nc"
static void SenseC$SenseMoist$readDone(error_t result, uint16_t data)
{
    VMEM_WR(senseCount1) = senseCount1 + 1;
  SenseC$senseTempTask$postTask();
}

# 170 "/opt/tinyos-2.1.2/tos/system/SchedulerBasicP.nc"
static error_t SchedulerBasicP$TaskBasic$postTask(uint8_t id)
{
  { __nesc_atomic_t __nesc_atomic = __nesc_atomic_start();
#line 172
    {
#line 172
      {
        unsigned char __nesc_temp =
#line 172
        SchedulerBasicP$pushTask(id) ? SUCCESS : EBUSY;

        {
#line 172
          __nesc_atomic_end(__nesc_atomic);
#line 172
          return __nesc_temp;
        }
      }
    }
#line 175
    __nesc_atomic_end(__nesc_atomic); }
}

# 96 "SenseC.nc"
static void SenseC$SenseTemp$readDone(error_t result, uint16_t data)
{
    VMEM_WR(senseCount2) = senseCount2 + 1;
    storeData(moist, temp);
    VMEM_WR(moistTempAvg) = calcAvg();
    compute(moistTempAvg);
    VMEM_WR(computeCount) = computeCount + 1;

  SenseC$sendTask$postTask();
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void port1_isr_handler(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) port1_isr_handler (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(P1IV, P1IV__P1IFG7))
    {
        case P1IV__P1IFG5:                  // Vector  8:  P1.3 interrupt flag
            P1OUT ^= BIT0;
            senseCount1 = 0;
            senseCount2 = 0;
            computeCount = 0;
            sendCount = 0;
            __delay_cycles(200);
            P1OUT ^= BIT0;
            break;
        default: break;
    }
}

