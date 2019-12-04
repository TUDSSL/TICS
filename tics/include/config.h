#ifndef CONFIG_H__
#define CONFIG_H__

#define MCLK 1000000UL // 1Mhz system clock

#define CODE_MODEL_SMALL

#define CHECKPOINT_DMA

#define N_STACK_BLOCKS  8
#define STACK_BLOCK_SIZE 512

//TODO make get values directly from the linkerscript
#ifdef CODE_MODEL_SMALL
#define VIRTUALMEM_START 0x4400 //0x4000
#else /* CODE_MODEL_LARGE */
#define VIRTUALMEM_START 0x4000
#endif /* CODE_MODEL_SMALL */

#define VIRTUALMEM_SIZE (1024*4)
#define VIRTUALMEM_STRATEGY_SIZE 64
#define VIRTUALMEM_DMA

//TODO make get values directly from the linkerscript
#ifdef CODE_MODEL_SMALL
#define STACK_START         0x2200
#else /* CODE_MODEL_LARGE */
#define STACK_START         0x2BCE
#endif /* CODE_MODEL_SMALL */

#define STACK_END           0x1C00
#define WORKINGSTACK_SIZE   50
//#define WORKINGSTACK_LIMIT (0x1C00 + 0x0FCE) // test limit
#define WORKINGSTACK_LIMIT  (STACK_START - WORKINGSTACK_SIZE) // test limit

#define WORKINGSTACK_MULTIPLE

#define CHECKPOINT_TIMER_FREQ 100 // 100Hz

#endif /* CONFIG_H__ */
