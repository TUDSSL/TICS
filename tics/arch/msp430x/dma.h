#ifndef DMA_H__
#define DMA_H__

// size should be in words
void dma_word_copy(char *dst, char *src, size_t size);

// Byte copy, rounds it up to words
static inline void dma_copy(char *dst, char *src, size_t size)
{
    size_t word_size;

    //TODO: Does DMA care about alignment?
    // If the number of bytes is odd (i.e. not a whole number of words)
    // we already move the last one (size-1) and copy the rest trough DMA
    if (size % 2) {
        --size;
        dst[size] = src[size];
    }

    word_size = size / 2;
    if (word_size != 0)
        dma_word_copy(dst, src, word_size);
}

#endif /* DMA_H__ */
