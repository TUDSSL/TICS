#ifndef LIBMSP_CLOCK_H
#define LIBMSP_CLOCK_H

#if defined(__MSP430F5340__) || defined(__CC430F5137__)

// See MSP430F5340 datasheet p44
#if CONFIG_XT1_CAP >= 12
#define CONFIG_XT1_CAP_BITS (XCAP0 | XCAP1)
#elif CONFIG_XT1_CAP >= 8
#define CONFIG_XT1_CAP_BITS (XCAP1)
#elif CONFIG_XT1_CAP >= 5
#define CONFIG_XT1_CAP_BITS (XCAP0)
#else
#define CONFIG_XT1_CAP_BITS 0
#endif

#if defined(CONFIG_DCO_REF_SOURCE_REFO)
#define CONFIG_DCO_REF_CLOCK_FREQ CONFIG_REFO_FREQ
#elif defined(CONFIG_DCO_REF_SOURCE_XT1)
#define CONFIG_DCO_REF_CLOCK_FREQ CONFIG_XT1_FREQ
#elif defined(CONFIG_DCO_REF_SOURCE_XT2)
#define CONFIG_DCO_REF_CLOCK_FREQ CONFIG_XT2_FREQ
#else // CONFIG_DCO_FREQ_SOURCE_*
#error Invalid DCO clock reference: see DCO_REF_SOURCE_*
#endif // CONFIG_DCO_REF_SOURCE_*

#define CONFIG_DCO_REF_FREQ (CONFIG_DCO_REF_CLOCK_FREQ / CONFIG_DCO_REF_CLOCK_DIV)

// DCO config
//
// NOTE: MSP430 crashes if it runs too fast?
// This may be caused by the average frequency from DCO modulation.  If we try to use
// a faster clock, the FLL may adjust the DCO above 25MHz to produce a clock with that
// average frequency.  If this happens, even for an instant, the MSP430 can crash.

// DCOCLK freq = 2^D * (N + 1) * REF_CLOCK_FREQ/REF_CLOCK_DIV
// DCOCLKDIV = DCOCLK / 2^D
//
// R | frequency range (MHz) (datasheet MSP430F5340 page 47)
// 0:  0.20 -  0.70
// 1:  0.36 -  1.47
// 2:  0.75 -  3.17
// 3:  1.51 -  6.07
// 4:  3.20 - 12.30
// 5:  6.00 - 23.70
// 6: 10.70 - 39.00
// 7: 19.60 - 60.00

#if CONFIG_DCOCLKDIV_FREQ == 24576000 && CONFIG_DCO_REF_FREQ == 32768

// 32768 * (668 + 1)
#define CONFIG_DCO_FREQ_R 7
#define CONFIG_DCO_FREQ_N 749
#define CONFIG_DCO_FREQ_D 0

#elif CONFIG_DCOCLKDIV_FREQ == 21921792 && CONFIG_DCO_REF_FREQ == 32768

// 32768 * (668 + 1)
#define CONFIG_DCO_FREQ_R 7
#define CONFIG_DCO_FREQ_N 668
#define CONFIG_DCO_FREQ_D 0

#elif CONFIG_DCOCLKDIV_FREQ == 16384000 && CONFIG_DCO_REF_FREQ == 32768

// 32768 * (499 + 1)
#define CONFIG_DCO_FREQ_R 6
#define CONFIG_DCO_FREQ_N 499
#define CONFIG_DCO_FREQ_D 1

#elif CONFIG_DCOCLKDIV_FREQ == 12288000 && CONFIG_DCO_REF_FREQ == 32768

// 32768 * (374 + 1)
#define CONFIG_DCO_FREQ_R 5
#define CONFIG_DCO_FREQ_N 374
#define CONFIG_DCO_FREQ_D 1

#elif CONFIG_DCOCLKDIV_FREQ == 8192000 && CONFIG_DCO_REF_FREQ == 32768

// 32768 * (249 + 1)
#define CONFIG_DCO_FREQ_R 5
#define CONFIG_DCO_FREQ_N 249
#define CONFIG_DCO_FREQ_D 1

#elif CONFIG_DCOCLKDIV_FREQ == 24000000 && CONFIG_DCO_REF_FREQ == 3000000

// 3000000 * (7 + 1)
#define CONFIG_DCO_FREQ_R 6
#define CONFIG_DCO_FREQ_N 7
#define CONFIG_DCO_FREQ_D 0

#endif // CONFIG_DCOCLKDIV_FREQ && CONFIG_DCO_REF_FREQ

// Worst-case settling time for the DCO when the DCO range bits have been changed:
// See MSP430x5xx Family User Manual (p. 165). The last fraction term is
// converting from FLL ref clock cycles to core clock cycles.
#define DCO_SETTLING_TIME \
    (1ull * CONFIG_DCO_REF_CLOCK_DIV * 32ull * 32ull * \
     (CONFIG_DCOCLKDIV_FREQ / CONFIG_DCO_REF_CLOCK_FREQ + 1ull))

#if CONFIG_DCOCLKDIV_FREQ != ((CONFIG_DCO_FREQ_N + 1) * CONFIG_DCO_REF_FREQ)
#error Inconsistent DCO freq config
#endif

// TODO: this is not the case for all possible configs
#define CONFIG_ACLK_SRC_FREQ    CONFIG_ACLK_XT1_FREQ

// Clock source for MCLK, SMCLK
#if defined(CONFIG_CLOCK_SOURCE_DCO)
#define CONFIG_MCLK_SRC_FREQ CONFIG_DCOCLKDIV_FREQ
#define CONFIG_SMCLK_SRC_FREQ CONFIG_DCOCLKDIV_FREQ
#elif defined(CONFIG_CLOCK_SOURCE_XT2)
#define CONFIG_MCLK_SRC_FREQ CONFIG_XT2_FREQ // for now, SMCLK source is not configurable
#define CONFIG_SMCLK_SRC_FREQ CONFIG_XT2_FREQ // for now, SMCLK source is not configurable
#else // CONFIG_CLOCK_SOURCE_*
#error Invalid main clock source: see CONFIG_CLOCK_SOURCE_*
#endif // CONFIG_CLOCK_SOURCE_*

#define CONFIG_ACLK_FREQ (CONFIG_ACLK_SRC_FREQ / CONFIG_CLK_DIV_ACLK)
#define CONFIG_SMCLK_FREQ (CONFIG_SMCLK_SRC_FREQ / CONFIG_CLK_DIV_SMCLK)
#define CONFIG_MCLK_FREQ (CONFIG_MCLK_SRC_FREQ / CONFIG_CLK_DIV_MCLK)

/** @brief Core voltage regulator setting
 * @details For freq above 8MHz level needs to be higher than 0 (see datasheet
 * "Recommended Operating Conditions" (p. 37)).
 */
#if CONFIG_MCLK_FREQ <= 8192000
#define CONFIG_CORE_VOLTAGE_LEVEL   0
#elif CONFIG_MCLK_FREQ <= 12000000
#define CONFIG_CORE_VOLTAGE_LEVEL   1
#elif CONFIG_MCLK_FREQ <= 20000000
#define CONFIG_CORE_VOLTAGE_LEVEL   2
#else // CONFIG_MCLK_FREQ
#define CONFIG_CORE_VOLTAGE_LEVEL   3
#endif // CONFIG_MCLK_FREQ

#elif defined(__MSP430FR5969__) || defined(__MSP430FR6989__) || defined(__MSP430FR5949__)
#if CONFIG_DCOCLKDIV_FREQ == 1000000
#define CONFIG_DCOFSEL_BITS DCOFSEL_0

#elif CONFIG_DCOCLKDIV_FREQ == 4000000
#define CONFIG_DCOFSEL_BITS DCOFSEL_3

#elif CONFIG_DCOCLKDIV_FREQ == 8000000
#define CONFIG_DCOFSEL_BITS DCOFSEL_6

#endif // CONFIG_DCOCLKDIV_FREQ

#else // __MSP430*__
#error Device not supported by libmsp/clock.c
#endif // __MSP430*__

void msp_clock_setup();

#endif // LIBMSP_CLOCK_H
