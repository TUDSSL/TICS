/*
 * adc.h
 *
 *  Created on: Dec 18, 2018
 *      Author: abubakar
 */

#ifndef ADC_H_
#define ADC_H_

#define CALADC12_12V_30C  *((unsigned int *)0x1A1A)   // Temperature Sensor Calibration-30 C
                                                      //See device datasheet for TLV table memory mapping
#define CALADC12_12V_85C  *((unsigned int *)0x1A1C)   // Temperature Sensor Calibration-85 C


void capAdcConfig()
{
    // Configure ADC12
    ADC12CTL0 &= ~ADC12ENC;                                 // Disable ADC12
    ADC12CTL0 = ADC12SHT0_2 ;                               // Sampling time, S&H=16, ADC12 on
    ADC12CTL1 = ADC12SHP;                                   // Use sampling timer
    ADC12CTL2 |= ADC12RES_2;                                // 12-bit conversion results
    ADC12MCTL0 |= ADC12INCH_3 | ADC12VRSEL_0 | ADC12DIF_0;  // A3 ADC input select; Vref=AVCC
    //ADC12CTL0 |= ADC12ENC;                                // Enable conversions
}

unsigned int capSample()
{
    P1DIR &= ~BIT3;                         // ADC pins
    P1REN |= BIT3;

    ADC12CTL0 &= ~ADC12ENC;                 // Disable conversions
    ADC12CTL0 |= ADC12ON;                   // Turn on ADC
    ADC12CTL0 |= ADC12ENC;                  // Enable conversions

    ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
    while (!(ADC12IFGR0 & BIT0));
    unsigned int adcVal = ADC12MEM0;

    ADC12CTL0 &= ~ADC12ENC;                 // Disable conversions
    ADC12CTL0 &= ~ADC12ON;                  // Turn off ADC
    ADC12CTL0 &= ~ADC12ENC;                 // Disable conversions

    P1DIR |= BIT3;                          // ADC pins
    P1REN &= ~BIT3;

    ADC12CTL0 &= 0;
    ADC12CTL0 = 0;
    ADC12CTL1 = 0;
    ADC12CTL2 |= 0;
    ADC12MCTL0 |= 0;

    return adcVal;
}

void adcConfig()
{
    // Configure ADC12
    ADC12CTL0 &= ~ADC12ENC;                                 // Disable ADC12
    ADC12CTL0 = ADC12SHT0_2 ;                                // Sampling time, S&H=16, ADC12 on
    ADC12CTL1 = ADC12SHP;                                   // Use sampling timer
    ADC12CTL2 |= ADC12RES_2;                                // 12-bit conversion results
    ADC12MCTL0 |= ADC12INCH_2 | ADC12VRSEL_0 | ADC12DIF_0;  // A2 ADC input select; Vref=AVCC
    //ADC12CTL0 |= ADC12ENC;                                // Enable conversions
}

unsigned int adcSample()
{
    P6OUT &= ~BIT1;                         // To power-up sensor
    P6DIR |= BIT1;

    P6OUT |= BIT1;                          // Turn on sensor
    __delay_cycles(50);

    P1DIR &= ~BIT2;                         // ADC pins
    P1REN |= BIT2;

    ADC12CTL0 &= ~ADC12ENC;                 // Disable conversions
    ADC12CTL0 |= ADC12ON;                   // Turn on ADC
    ADC12CTL0 |= ADC12ENC;                  // Enable conversions

    ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
    while (!(ADC12IFGR0 & BIT0));
    unsigned int adcVal = ADC12MEM0;

    ADC12CTL0 &= ~ADC12ENC;                 // Disable conversions
    ADC12CTL0 &= ~ADC12ON;                  // Turn off ADC
    ADC12CTL0 &= ~ADC12ENC;                 // Disable conversions

    P1DIR |= BIT2;                          // ADC pins
    P1REN &= ~BIT2;

    P6OUT &= ~BIT1;                         // Turn off sensor

    ADC12CTL0 &= 0;
    ADC12CTL0 = 0;
    ADC12CTL1 = 0;
    ADC12CTL2 |= 0;
    ADC12MCTL0 |= 0;

    return adcVal;
}

void tempConfig()
{
    while(REFCTL0 & REFGENBUSY);            // If ref generator busy, WAIT
    REFCTL0 |= REFVSEL_0 + REFON;           // Enable internal 1.2V reference

    // Initialize ADC12_A
    ADC12CTL0 &= ~ADC12ENC;                 // Disable ADC12
    ADC12CTL0 = ADC12SHT0_8;                // Set sample time
    ADC12CTL1 = ADC12SHP;                   // Enable sample timer
    ADC12CTL3 = ADC12TCMAP;                 // Enable internal temperature sensor
    ADC12MCTL0 |= ADC12VRSEL_1 | ADC12INCH_30; // ADC input ch A30 => temp sense
}

float tempDegC()
{
    ADC12CTL0 &= ~ADC12ENC;                 // Disable conversions
    ADC12CTL0 |= ADC12ON;                   // Turn on ADC
    ADC12CTL0 |= ADC12ENC;                  // Enable conversions


    ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
    while (!(ADC12IFGR0 & BIT0));
    unsigned int adcVal = ADC12MEM0;
    float temperatureDegC = (float)(((float)adcVal - CALADC12_12V_30C) * (85 - 30)) / (CALADC12_12V_85C - CALADC12_12V_30C) + 30.0f;


    ADC12CTL0 &= ~ADC12ENC;                 // Disable conversions
    ADC12CTL0 &= ~ADC12ON;                  // Turn off ADC

    ADC12CTL0 = 0;                 // Disable ADC12
    ADC12CTL0 = 0;                // Set sample time
    ADC12CTL1 = 0;                   // Enable sample timer
    ADC12CTL3 = 0;                 // Enable internal temperature sensor
    ADC12MCTL0 = 0; // ADC input ch A30 => temp sense

    return temperatureDegC;
}
#endif /* ADC_H_ */
