#include <stdlib.h>
#include <stdint.h>

#include "msp430.h"
#include "dma.h"

// size should be in words
void dma_word_copy(char *dst, char *src, size_t size)
{
    if ((dst == NULL) || (src == 0) || (size == 0)) {
        // TODO: Make the error known somewhere
        return;
    }

    // Configure DMA channel 0
    __data16_write_addr((intptr_t) &DMA0SA,(intptr_t) src);
                                              // Source block address
    __data16_write_addr((intptr_t) &DMA0DA,(intptr_t) dst);
                                              // Destination single address
    DMA0SZ = size;                          // Block size
    DMA0CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3; // Rpt, inc
    DMA0CTL |= DMAEN;                         // Enable DMA0

    DMA0CTL |= DMAREQ;                      // Trigger block transfer

    while (!(DMA0CTL & DMAIFG)) ;   // Wait for DMA to finish

    DMA0CTL &= ~DMAEN;
}
