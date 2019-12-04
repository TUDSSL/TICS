#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "driverlib.h"

#include "nvm.h"
#include "arch.h"
#include "checkpoint.h"

/*
 * Functional Test
 *  Test the working of the checkpoint and checkpoint_restore functions by
 *  adding a checkpoint and a restore after almost every statement in this
 *  CRC application from MiBench (link below).
 *
 *  A checkpoint is restored if the last checkpoint was NOT a restore, and
 *  checkpointed after that (always). This causes all the steps in the CRC
 *  to be executed twice, that way we can show that the memory stays consistent.
 *
 * Running the test
 *  The test can be run by starting the program, and when it enters the
 *  infinite loop and test_all_ok == 1.
 *
 * The compare variables are in NVM to not spoil the data to be checkpointed.
 *
 * NB The crcInit has a good number of iterations, each making a checkpoint
 * during this test. So the test takes about 4 minutes to complete.
 * Be patient ;)
 * Also, the printf statements will be executed multiple times.
 */

// Starts at -1 so it looks like a restore to force the first checkpoint
NVM volatile int _result = -1;
NVM volatile int _line = -1;

#define REDO do {                   \
    _line = __LINE__;               \
    if (_result == 0) {             \
        checkpoint_restore();       \
    }                               \
    _result = checkpoint();         \
} while (0)

NVM volatile int test_all_ok = -1;

////////////////////////////////////////////////////////////////////////////////
// CRC code
//
// https://github.com/impedimentToProgress/MiBench2/tree/master/crc
//
/**********************************************************************
 *
 * Filename:    crc.h
 *
 * Description: A header file describing the various CRC standards.
 *
 * Notes:
 *
 *
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/

#ifndef _crc_h
#define _crc_h


#define FALSE	0
#define TRUE	!FALSE

/*
 * Select the CRC standard from the list that follows.
 */
#define CRC_CCITT


#if defined(CRC_CCITT)

typedef unsigned short  crc;

#define CRC_NAME			"CRC-CCITT"
#define POLYNOMIAL			0x1021
#define INITIAL_REMAINDER	0xFFFF
#define FINAL_XOR_VALUE		0x0000
#define REFLECT_DATA		FALSE
#define REFLECT_REMAINDER	FALSE
#define CHECK_VALUE			0x29B1

#elif defined(CRC16)

typedef unsigned short  crc;

#define CRC_NAME			"CRC-16"
#define POLYNOMIAL			0x8005
#define INITIAL_REMAINDER	0x0000
#define FINAL_XOR_VALUE		0x0000
#define REFLECT_DATA		TRUE
#define REFLECT_REMAINDER	TRUE
#define CHECK_VALUE			0xBB3D

#elif defined(CRC32)

typedef unsigned long  crc;

#define CRC_NAME			"CRC-32"
#define POLYNOMIAL			0x04C11DB7
#define INITIAL_REMAINDER	0xFFFFFFFF
#define FINAL_XOR_VALUE		0xFFFFFFFF
#define REFLECT_DATA		TRUE
#define REFLECT_REMAINDER	TRUE
#define CHECK_VALUE			0xCBF43926

#else

#error "One of CRC_CCITT, CRC16, or CRC32 must be #define'd."

#endif


void  crcInit(void);
crc   crcSlow(unsigned char const message[], int nBytes);
crc   crcFast(unsigned char const message[], int nBytes);



/**********************************************************************
 *
 * Filename:    crc.c
 *
 * Description: Slow and fast implementations of the CRC standards.
 *
 * Notes:       The parameters for each supported CRC standard are
 *				defined in the header file crc.h.  The implementations
 *				here should stand up to further additions to that list.
 *
 *
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/

/*
 * Derive parameters from the standard-specific parameters in crc.h.
 */
#define WIDTH    (8 * sizeof(crc))
#define TOPBIT   (1 << (WIDTH - 1))

#if (REFLECT_DATA == TRUE)
#undef  REFLECT_DATA
#define REFLECT_DATA(X)			((unsigned char) reflect((X), 8))
#else
#undef  REFLECT_DATA
#define REFLECT_DATA(X)			(X)
#endif

#if (REFLECT_REMAINDER == TRUE)
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)	((crc) reflect((X), WIDTH))
#else
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)	(X)
#endif


/*********************************************************************
 *
 * Function:    reflect()
 *
 * Description: Reorder the bits of a binary sequence, by reflecting
 *				them about the middle position.
 *
 * Notes:		No checking is done that nBits <= 32.
 *
 * Returns:		The reflection of the original data.
 *
 *********************************************************************/
unsigned long
reflect(unsigned long data, unsigned char nBits)
{
	unsigned long  reflection = 0x00000000;
	unsigned char  bit;

	/*
	 * Reflect the data about the center bit.
	 */
	for (bit = 0; bit < nBits; ++bit)
	{
		/*
		 * If the LSB bit is set, set the reflection of it.
		 */
		if (data & 0x01)
		{
			reflection |= (1 << ((nBits - 1) - bit));
		}

		data = (data >> 1);
	}

	return (reflection);

}	/* reflect() */


/*********************************************************************
 *
 * Function:    crcSlow()
 *
 * Description: Compute the CRC of a given message.
 *
 * Notes:
 *
 * Returns:		The CRC of the message.
 *
 *********************************************************************/
crc
crcSlow(unsigned char const message[], int nBytes)
{
    crc            remainder = INITIAL_REMAINDER;
	int            byte;
	unsigned char  bit;


    /*
     * Perform modulo-2 division, a byte at a time.
     */
REDO;
    for (byte = 0; byte < nBytes; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
REDO;
        remainder ^= (REFLECT_DATA(message[byte]) << (WIDTH - 8));

        /*
         * Perform modulo-2 division, a bit at a time.
         */
REDO;
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
REDO;
            if (remainder & TOPBIT)
            {
REDO;
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
REDO;
                remainder = (remainder << 1);
            }
        }
    }

    /*
     * The final remainder is the CRC result.
     */
REDO;
    return (REFLECT_REMAINDER(remainder) ^ FINAL_XOR_VALUE);

}   /* crcSlow() */


crc  crcTable[256];


/*********************************************************************
 *
 * Function:    crcInit()
 *
 * Description: Populate the partial CRC lookup table.
 *
 * Notes:		This function must be rerun any time the CRC standard
 *				is changed.  If desired, it can be run "offline" and
 *				the table results stored in an embedded system's ROM.
 *
 * Returns:		None defined.
 *
 *********************************************************************/
void
crcInit(void)
{
    crc			   remainder;
	int			   dividend;
	unsigned char  bit;


    /*
     * Compute the remainder of each possible dividend.
     */
REDO;
    for (dividend = 0; dividend < 256; ++dividend)
    {
REDO;
        /*
         * Start with the dividend followed by zeros.
         */
        remainder = dividend << (WIDTH - 8);

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
REDO;
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT)
            {
REDO;
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
REDO;
                remainder = (remainder << 1);
            }
REDO;
        }

REDO;
        /*
         * Store the result into the table.
         */
        crcTable[dividend] = remainder;
REDO;
    }

}   /* crcInit() */


/*********************************************************************
 *
 * Function:    crcFast()
 *
 * Description: Compute the CRC of a given message.
 *
 * Notes:		crcInit() must be called first.
 *
 * Returns:		The CRC of the message.
 *
 *********************************************************************/
crc
crcFast(unsigned char const message[], int nBytes)
{
    crc	           remainder = INITIAL_REMAINDER;
    unsigned char  data;
	int            byte;


REDO;
    /*
     * Divide the message by the polynomial, a byte at a time.
     */
    for (byte = 0; byte < nBytes; ++byte)
    {
REDO;
        data = REFLECT_DATA(message[byte]) ^ (remainder >> (WIDTH - 8));
  		remainder = crcTable[data] ^ (remainder << 8);
    }
REDO;

    /*
     * The final remainder is the CRC.
     */
    return (REFLECT_REMAINDER(remainder) ^ FINAL_XOR_VALUE);

}   /* crcFast() */

#endif /* _crc_h */
//
// END CRC
////////////////////////////////////////////////////////////////////////////////

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    printf("CRC test (takes about 4 min.)\n");

    REDO;

    unsigned char  test[] = "123456789";
    int len = strlen((char *)test);
    crc crc_res;

    /*
     * Print the check value for the selected CRC algorithm.
     */
    printf("The check value for the %s standard is 0x%X\n", CRC_NAME, CHECK_VALUE);

    /*
     * Compute the CRC of the test message, slowly.
     */
    crc_res = crcSlow(test, len);
    printf("The crcSlow() of \"123456789\" is 0x%X\n", crc_res);

    if (crc_res == CHECK_VALUE) {
        test_all_ok = 1;
    }

    /*
     * Compute the CRC of the test message, more efficiently.
     */
    crcInit();
    crc_res = crcFast(test, len);
    printf("The crcFast() of \"123456789\" is 0x%X\n", crc_res);

    if (crc_res == CHECK_VALUE && test_all_ok == 1) {
        test_all_ok = 1;
    } else {
        test_all_ok = 0;
    }

    while (1) {
        test_all_ok = test_all_ok;
    }

}
