// Config ws=300

//#define CHECKPOINT_WORKINGSTACK
//#define ENABLE_MANUAL_CHECKPOINT
//#define PRINT_CP_STATS
//#define USE_TIME

#define SAMPLE_SIZE 5

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

#undef VMEM_WR
#define VMEM_WR

/* Time Var HACK */
//#include <math.h>
NVM float time;

#if 0
float getTime()
{
    capAdcConfig();
    unsigned int adcVal = capSample();
    //printf("%u \n\r", adcVal);
    float voltage = ((float)adcVal / 4096.0) * 2.67 - 0.5 * (2.67 / 4096.0);
    float t = (log(2.33 / voltage) * 3925.0 * 1.06);/* 0.988 3.925 */ /* 0.942, 1.10 1.24  */


//    printf("%u \t %u V \t %l mSec \n\r",adcVal, (long)voltage, (long)t);

    time = time + t;

    P8OUT |= BIT0;
    __delay_cycles(500);
    P8OUT &= ~BIT0;

    return time/1000.0;
}
#else
float getTime()
{
    //capAdcConfig();
    //unsigned int adcVal = capSample();
    // printf("%u \n\r", adcVal);
    // float voltage = ((float)adcVal / 4096.0)  2.67 - 0.5  (2.67 / 4096.0);
    // float t = (log(2.33 / voltage)  3925.0  1.06);/ 0.988 3.925 / / 0.942, 1.10 1.24  /


//    printf("%u \t %u V \t %l mSec \n\r",adcVal, (long)voltage, (long)t);

    unsigned int adcVal = 42;
    time = time + adcVal;

    P8OUT |= BIT0;
    __delay_cycles(500);
    P8OUT &= ~BIT0;

    return time;
}
#endif


//NVM uint32_t timev_seed=42;
//#define expired(v_) (rand() & 1)

#define expired(v_) ((getTime() > v_))
//#define expired(v_) ((false))
#define set_expire(v_, t_) (v_ = getTime() + t_)

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

// #if defined(__TI_COMPILER_VERSION__)

// #pragma PERSISTENT(senseCount1, senseCount2, computeCount, sendCount)
// char senseCount1 = 0;
// char senseCount2 = 0;
// char computeCount = 0;
// char sendCount = 0;


// #elif defined(__GNUC__)
// char __attribute__((persistent)) senseCount1 = 0;
// char __attribute__((persistent)) senseCount2 = 0;
// char __attribute__((persistent)) computeCount = 0;
// char __attribute__((persistent)) sendCount = 0;
// #else
// #error Compiler not supported!
// #endif

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

NVM unsigned int moist = 0;
NVM float tv_moist = 0.0f;
#define MOIST_ETIME 5.0f

NVM float temp = 0.0f;
NVM float tv_temp = 0.0f;
#define TEMP_ETIME 10.0f

NVM struct Tuple moistTempAvg = {0, 0.0f};
NVM float tv_moistTempAvg = 0.0f;
#define MOISTTEMPAVG_ETIME 4.0f

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

void ledConfig(void)
{
    P1OUT &= ~BIT1;                             // Clear LED to start
    P1DIR |= BIT1;                              // Set P1.1/LED to output

    P1OUT &= ~BIT0;                             // Clear LED to start
    P1DIR |= BIT0;                              // Set P1.0/LED to output
}

void watchdogInit(void)
{
    WDTCTL = WDTPW | WDTHOLD;                   // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;
}

void portInterruptConfig(void)
{
    P1OUT = BIT5;                               // Pull-up resistor on P1.1
    P1REN = BIT5;                               // Select pull-up mode for P1.1
    P1DIR = 0xFF ^ BIT5;                        // Set all but P1.1 to output direction
    P1IES = BIT5;                               // P1.1 Hi/Lo edge
    P1IFG = 0;                                  // Clear all P1 interrupt flags
    P1IE = BIT5;                                // P1.1 interrupt enabled
    __bis_SR_register(GIE);                     // Enable general interrupt
}

void clockInit(void)
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

    if (reboot == 0) {
        checkpoint_full();
        reboot = 1;
    } else {
        checkpoint_restore();
    }
}

struct Tuple calcAvg(void)
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
int main(void)
{
    platformInit();
    while(1)
    {
        if(P8IN & 0x02)
        {
            // checkpoint
            MANUAL_CHECKPOINT();

            adcConfig();
            VMEM_WR(moist) = adcSample();
            set_expire(tv_moist, MOIST_ETIME);
            VMEM_WR(senseCount1) = senseCount1 + 1;

            // checkpoint
            MANUAL_CHECKPOINT();

            tempConfig();
            VMEM_WR(temp) = tempDegC();
            set_expire(tv_temp, TEMP_ETIME);
            VMEM_WR(senseCount2) = senseCount2 + 1;

            // checkpoint
            MANUAL_CHECKPOINT();

            /* TIMEVAR */

            storeData(moist, temp);

            // checkpoint
            MANUAL_CHECKPOINT();

            VMEM_WR(moistTempAvg) = calcAvg();
            set_expire(tv_moistTempAvg, MOISTTEMPAVG_ETIME);
//            printf("%u \t %u \t %u \t %u\n\r", moist, moistTempAvg.m, (unsigned int)temperature, (unsigned int)moistTempAvg.t);

            /* TIMEVAR */

            compute(moistTempAvg);
            VMEM_WR(computeCount) = computeCount + 1;

            if (!expired(tv_moistTempAvg)) {
                // checkpoint
                MANUAL_CHECKPOINT();

                sendData(moistTempAvg);
                VMEM_WR(sendCount) = sendCount + 1;
            }
            // checkpoint
            MANUAL_CHECKPOINT();
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
            spiSend(sendCount);
            spiSend(4);
            spiSend(computeCount);

            delay(5);
        }
    }
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

