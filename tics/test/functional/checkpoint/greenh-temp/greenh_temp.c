#include <stdio.h>
#include <msp430.h>
#include "adc.h"

#define SAMPLE_SIZE 5

char senseCount1 = 0;
char senseCount2 = 0;
char computeCount = 0;
char sendCount = 0;

unsigned int moisture[SAMPLE_SIZE] = { 0 };
float temperature[SAMPLE_SIZE] = { 0.0 };

unsigned int moist;
float temp;
struct Tuple moistTempAvg;

struct Tuple {
    int m;
    float t;
};

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
    WDTCTL = WDTPW | WDTHOLD;                   // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;

    P1OUT &= ~BIT1;                             // Clear LED to start
    P1DIR |= BIT1;                              // Set P1.1/LED to output

    P1OUT &= ~BIT0;                             // Clear LED to start
    P1DIR |= BIT0;                              // Set P1.0/LED to output
}

void portInterruptConfig()
{
    WDTCTL = WDTPW | WDTHOLD;                   // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;

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

void platformInit()
{
    watchdogInit();
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
        moisture[i] = moisture[i-1];
        temperature[i] = temperature[i-1];
    }
    moisture[0] = m;
    temperature[0] = t;
}

void compute(struct Tuple avg)
{
    ledConfig();
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
    __delay_cycles(30000);
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
    portInterruptConfig();
    ledConfig();
    while(1)
    {
        platformInit();
        //if(P8IN & 0x02)
        if (1)
        {
            // checkpoint
            adcConfig();
            moist = adcSample();
            senseCount1++;

            // checkpoint

            tempConfig();
            temp = tempDegC();
            senseCount2++;

            // checkpoint

            storeData(moist, temp);

            // checkpoint

            moistTempAvg = calcAvg();
            printf("Mosture: %u \t Moist avg.:%u \t Temp: %u \t MoistTempAvg: %u\n\r",
                    moist, moistTempAvg.m, (unsigned int)temp, (unsigned int)moistTempAvg.t);

            compute(moistTempAvg);
            computeCount++;

            // checkpoint

            sendData(moistTempAvg);
            sendCount++;

            // checkpoint

            delay(5);
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
