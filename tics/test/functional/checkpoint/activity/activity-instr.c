//#define LOG_PRINT
//#define PRINTF_PRINT

#define CHECKPOINT_WORKINGSTACK
//#define CHECKPOINT_ON_TASK_BOUNDARY
//#define CHECKPOINT_TIMER

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <msp430.h>
#include "driverlib.h"
#include "nvm.h"
#include "arch.h"
#include "checkpoint.h"
#include "stackarray.h"
#include "virtualmem.h"
#include "checkpoint_timer.h"

#include "ftest_util.h"
void mspconsole_init();

//#undef VMEM_WR
//#define VMEM_WR(a_) a_

/* Sqrt.c */
/* Square root by Newton's method */
#ifndef __clang__
uint16_t sqrt16(uint32_t x)
{
    uint16_t hi = 0xffff;
    uint16_t lo = 0;
    uint16_t mid = ((uint32_t)hi + (uint32_t)lo) >> 1;
    uint32_t s = 0;

    while (s != x && hi - lo > 1) {
        mid = ((uint32_t)hi + (uint32_t)lo) >> 1;
        s = (uint32_t)mid* (uint32_t)mid;
        if (s < x)
            lo = mid;
        else
            hi = mid;
    }

    return mid;
}
#endif

#ifdef CHECKPOINT_WORKINGSTACK
#define CPWS __attribute__((segstack))
#else
#define CPWS
#endif

#ifdef LOG_PRINT
#define LOG(...) do {       \
    printf("Log: ");        \
    printf(__VA_ARGS__);    \
} while(0)
#else /* !LOG_PRINT */
#define LOG(...)
#endif /* LOG_PRINT */

#ifdef PRINTF_PRINT
#define PRINTF printf
#else
#define PRINTF(...)
#endif /* PRINTF_PRINT */

//#define USE_LEDS

// Number of samples to discard before recording training set
#define NUM_WARMUP_SAMPLES 3

#define ACCEL_WINDOW_SIZE 3
#define MODEL_SIZE 16
#define SAMPLE_NOISE_FLOOR 10 // TODO: made up value

// Number of classifications to complete in one experiment
#define SAMPLES_TO_COLLECT 128

volatile int nop_var = 2;

/* VISP */
extern volatile uint16_t __isr_checkpoint_cnt;

extern uint16_t __workingstack_grow_cnt;
extern uint16_t __workingstack_shrink_cnt;
extern uint16_t __workingstack_checkpoint_cnt;

NVM static uint16_t __task_checkpoint_cnt = 0;
#ifdef CHECKPOINT_ON_TASK_BOUNDARY
#define TASK_CHECKPOINT() checkpoint(); ++__task_checkpoint_cnt;
//#define TASK_CHECKPOINT() checkpoint_and_reset(); ++__task_checkpoint_cnt;
#else /* !CHECKPOINT_ON_TASK_BOUNDARY */
#define TASK_CHECKPOINT(...)
#endif /* CHECKPOINT_ON_TASK_BOUNDARY */

NVM static uint16_t __software_checkpoint_cnt = 0;
#define SOFTWARE_CHECKPOINT() checkpoint(); ++__software_checkpoint_cnt;
/* VISP END */

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t z;
    #ifdef __clang__
	uint8_t padding; // clang crashes with type size mismatch assert failure
#endif
} threeAxis_t_8;

typedef threeAxis_t_8 accelReading;
typedef accelReading accelWindow[ACCEL_WINDOW_SIZE];

typedef struct {
    unsigned meanmag;
    unsigned stddevmag;
} features_t;

typedef enum {
    CLASS_STATIONARY,
    CLASS_MOVING,
} class_t;

typedef struct {
    features_t stationary[MODEL_SIZE];
    features_t moving[MODEL_SIZE];
} model_t;

typedef enum {
    MODE_IDLE = 3,
    MODE_TRAIN_STATIONARY = 2,
    MODE_TRAIN_MOVING = 1,
    MODE_RECOGNIZE = 0, // default
} run_mode_t;

typedef struct {
    unsigned totalCount;
    unsigned movingCount;
    unsigned stationaryCount;
} stats_t;


/* Globals */
NVM unsigned int count = 1;
NVM model_t model;
NVM  unsigned int _v_seed = 1;

/* Prototypes */
CPWS void ACCEL_singleSample(threeAxis_t_8* result);
CPWS void acquire_window(accelWindow window);
CPWS void transform(accelWindow window);
CPWS void featurize(features_t *features, accelWindow aWin);
CPWS class_t classify(features_t *features, model_t *model);
CPWS void record_stats(stats_t *stats, class_t class);
CPWS void warmup_sensor(void);
CPWS void train(features_t *classModel);
CPWS void recognize(model_t *model);
CPWS run_mode_t select_mode(uint8_t *prev_pin_state);


void ACCEL_singleSample(threeAxis_t_8* result){

    unsigned int seed = _v_seed;

    threeAxis_t_8 *result_a = result;
    VMEM_WR(result_a->x) = (seed*17)%85;
    VMEM_WR(result_a->y) = (seed*17*17)%85;
    VMEM_WR(result_a->z) = (seed*17*17*17)%85;

    VMEM_WR(_v_seed) = ++seed;
}


#define accel_sample ACCEL_singleSample

void acquire_window(accelWindow window)
{
    accelReading sample;
    unsigned samplesInWindow = 0;

    TASK_CHECKPOINT();

    while (samplesInWindow < ACCEL_WINDOW_SIZE) {
        accel_sample(&sample);
        LOG("acquire: sample %u %u %u\r\n", sample.x, sample.y, sample.z);

        //window[samplesInWindow++] = sample;
        VMEM_WR(window[samplesInWindow++]) = sample;
    }
}

void transform(accelWindow window)
{
    unsigned i = 0;

    LOG("transform\r\n");

    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
        accelReading *sample = &window[i];

        if (sample->x < SAMPLE_NOISE_FLOOR ||
            sample->y < SAMPLE_NOISE_FLOOR ||
            sample->z < SAMPLE_NOISE_FLOOR) {

            LOG("transform: sample %u %u %u\r\n",
                sample->x, sample->y, sample->z);

            VMEM_WR(sample->x) = (sample->x > SAMPLE_NOISE_FLOOR) ? sample->x : 0;
            VMEM_WR(sample->y) = (sample->y > SAMPLE_NOISE_FLOOR) ? sample->y : 0;
            VMEM_WR(sample->z) = (sample->z > SAMPLE_NOISE_FLOOR) ? sample->z : 0;
        }
    }
}

void featurize(features_t *features, accelWindow aWin)
{
    TASK_CHECKPOINT();

    accelReading mean;
    accelReading stddev;

    mean.x = mean.y = mean.z = 0;
    stddev.x = stddev.y = stddev.z = 0;
    int i;
    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
        mean.x += aWin[i].x;  // x
        mean.y += aWin[i].y;  // y
        mean.z += aWin[i].z;  // z
    }
    /*
       mean.x = mean.x / ACCEL_WINDOW_SIZE;
       mean.y = mean.y / ACCEL_WINDOW_SIZE;
       mean.z = mean.z / ACCEL_WINDOW_SIZE;
       */
    mean.x >>= 2;
    mean.y >>= 2;
    mean.z >>= 2;

    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
        stddev.x += aWin[i].x > mean.x ? aWin[i].x - mean.x
            : mean.x - aWin[i].x;  // x
        stddev.y += aWin[i].y > mean.y ? aWin[i].y - mean.y
            : mean.y - aWin[i].y;  // y
        stddev.z += aWin[i].z > mean.z ? aWin[i].z - mean.z
            : mean.z - aWin[i].z;  // z
    }
    /*
       stddev.x = stddev.x / (ACCEL_WINDOW_SIZE - 1);
       stddev.y = stddev.y / (ACCEL_WINDOW_SIZE - 1);
       stddev.z = stddev.z / (ACCEL_WINDOW_SIZE - 1);
       */
    stddev.x >>= 2;
    stddev.y >>= 2;
    stddev.z >>= 2;

    unsigned meanmag = mean.x*mean.x + mean.y*mean.y + mean.z*mean.z;
    unsigned stddevmag = stddev.x*stddev.x + stddev.y*stddev.y + stddev.z*stddev.z;

    VMEM_WR(features->meanmag)   = sqrt16(meanmag);
    VMEM_WR(features->stddevmag) = sqrt16(stddevmag);

    LOG("featurize: mean %u sd %u\r\n", features->meanmag, features->stddevmag);
}

class_t classify(features_t *features, model_t *model)
{
    int move_less_error = 0;
    int stat_less_error = 0;
    features_t *model_features;
    int i;

    TASK_CHECKPOINT();

    for (i = 0; i < MODEL_SIZE; ++i) {
        model_features = &model->stationary[i];

        long int stat_mean_err = (model_features->meanmag > features->meanmag)
            ? (model_features->meanmag - features->meanmag)
            : (features->meanmag - model_features->meanmag);

        long int stat_sd_err = (model_features->stddevmag > features->stddevmag)
            ? (model_features->stddevmag - features->stddevmag)
            : (features->stddevmag - model_features->stddevmag);

        model_features = &model->moving[i];

        long int move_mean_err = (model_features->meanmag > features->meanmag)
            ? (model_features->meanmag - features->meanmag)
            : (features->meanmag - model_features->meanmag);

        long int move_sd_err = (model_features->stddevmag > features->stddevmag)
            ? (model_features->stddevmag - features->stddevmag)
            : (features->stddevmag - model_features->stddevmag);

        if (move_mean_err < stat_mean_err) {
            move_less_error++;
        } else {
            stat_less_error++;
        }

        if (move_sd_err < stat_sd_err) {
            move_less_error++;
        } else {
            stat_less_error++;
        }
    }

    class_t class = move_less_error > stat_less_error ?
                        CLASS_MOVING : CLASS_STATIONARY;
    LOG("classify: class %u\r\n", class);

    return class;
}

void record_stats(stats_t *stats, class_t class)
{
    TASK_CHECKPOINT();

    /* stats->totalCount, stats->movingCount, and stats->stationaryCount have an
     * nv-internal consistency requirement.  This code should be atomic. */

    VMEM_WR(stats->totalCount) = stats->totalCount + 1;

    switch (class) {
        case CLASS_MOVING:
            VMEM_WR(stats->movingCount) = stats->movingCount + 1;
            break;

        case CLASS_STATIONARY:
            VMEM_WR(stats->stationaryCount) = stats->stationaryCount + 1;
            break;
    }

    LOG("stats: s %u m %u t %u\r\n",
        stats->stationaryCount, stats->movingCount, stats->totalCount);
}

void print_stats(stats_t *stats)
{
    unsigned resultStationaryPct = stats->stationaryCount * 100 / stats->totalCount;
    unsigned resultMovingPct = stats->movingCount * 100 / stats->totalCount;

    unsigned sum = stats->stationaryCount + stats->movingCount;

    printf("stats: s %u (%u%%) m %u (%u%%) sum/tot %u/%u: %c\r\n",
           stats->stationaryCount, resultStationaryPct,
           stats->movingCount, resultMovingPct,
           stats->totalCount, sum,
           sum == stats->totalCount && sum == SAMPLES_TO_COLLECT ? 'V' : 'X');
}

void warmup_sensor(void)
{
    unsigned discardedSamplesCount = 0;
    accelReading sample;

    TASK_CHECKPOINT();

    LOG("warmup\r\n");

    while (discardedSamplesCount++ < NUM_WARMUP_SAMPLES) {
        accel_sample(&sample);
    }
}

void train(features_t *classModel)
{
    accelWindow sampleWindow;
    features_t features;
    unsigned i;

    warmup_sensor();

    for (i = 0; i < MODEL_SIZE; ++i) {
        acquire_window(sampleWindow);
        transform(sampleWindow);
        featurize(&features, sampleWindow);

        TASK_CHECKPOINT();

        VMEM_WR(classModel[i]) = features;
    }

    PRINTF("train: done: mn %u sd %u\r\n",
           features.meanmag, features.stddevmag);
}

void recognize(model_t *model)
{
#ifdef MEMENTOS_NONVOLATILE
    static __nv stats_t stats;
#else
    stats_t stats;
#endif

    accelWindow sampleWindow;
    features_t features;
    class_t class;
    unsigned i;

    stats.totalCount = 0;
    stats.stationaryCount = 0;
    stats.movingCount = 0;

    for (i = 0; i < SAMPLES_TO_COLLECT; ++i) {
        acquire_window(sampleWindow);
        transform(sampleWindow);
        featurize(&features, sampleWindow);
        class = classify(&features, model);
        record_stats(&stats, class);
    }

#ifdef PRINTF_PRINT
    print_stats(&stats);
#endif
}

void end_of_benchmark(void)
{
    checkpoint_timer_disable();
    nop_var = 21;
    printf("ISR count: %d\n", __isr_checkpoint_cnt);
    printf("Task Checkpoint count: %d\n", __task_checkpoint_cnt);
    printf("Workingstack checkpoint count: %d\n", __workingstack_checkpoint_cnt);
    printf("Workingstack count: %d\n", __workingstack_grow_cnt);
    printf("Workingstack restore count: %d\n", __workingstack_shrink_cnt);
    /* Copy line */
    // name, Working stack size, vmem size, checkpoints, task checkpoints, isr checkpoints, workingstack checkpoints, workingstack grow, workingstack shrink
    printf("AR, %d, %d, %d, %d, %d, %d, %d, %d\n",
            WORKINGSTACK_SIZE,
            VIRTUALMEM_STRATEGY_SIZE,
            __software_checkpoint_cnt,
            __task_checkpoint_cnt,
            __isr_checkpoint_cnt,
            __workingstack_checkpoint_cnt,
            __workingstack_grow_cnt,
            __workingstack_shrink_cnt);

    while (1);
}

void count_error(void)
{
    PRINTF("An error occured during count, count = %d\n", count);
}


run_mode_t select_mode(uint8_t *prev_pin_state)
{
    uint8_t pin_state;

    TASK_CHECKPOINT();

    count = count + 1;

    /* The InK order
     *  rounds:
     *      1,2 = MODE_TRAIN_MOVING
     *      3,4 = MODE_TRAIN_STATIONARY
     *      5,6 = MODE_RECOGNIZE
     *      7   = END OF BENCHMARK
     */
    switch(count) {
        case 1:
        case 2:
            pin_state = MODE_TRAIN_MOVING;
            break;
        case 3:
        case 4:
            pin_state = MODE_TRAIN_STATIONARY;
            break;
        case 5:
        case 6:
            pin_state = MODE_RECOGNIZE;
            break;
        case 7:
            end_of_benchmark();
            break;
        default:
            pin_state = MODE_IDLE;
            count_error();
    }

    //pin_state = GPIO(PORT_AUX, IN) & (BIT(PIN_AUX_1) | BIT(PIN_AUX_2));

    // Don't re-launch training after finishing training
    // Vito: could have done this while assigning pin_state. But keep is the same as the original
    if ((pin_state == MODE_TRAIN_STATIONARY ||
        pin_state == MODE_TRAIN_MOVING) &&
        pin_state == *prev_pin_state) {
        pin_state = MODE_IDLE;
    } else {
        VMEM_WR(*prev_pin_state) = pin_state;
    }

    LOG("selectMode: pins %04x\r\n", pin_state);

    return (run_mode_t)pin_state;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    // Disable FRAM wait cycles to allow clock operation over 8MHz
    FRCTL0 = 0xA500 | ((1) << 4); // FRCTLPW | NWAITS_1;
    //__delay_cycles(3);

    /* init FRAM */
    FRCTL0_H |= (FWPW) >> 8;

    PM5CTL0 &= ~LOCKLPM5;

    // UART
    mspconsole_init();
    __enable_interrupt();

    // init timer
#ifdef CHECKPOINT_TIMER
    checkpoint_timer_init();
#endif

    checkpoint_init();
}

#if 0
#include "checkpoint_stack.h"
void __cyg_profile_func_enter (void *this_fn, void *call_site)
{
    char *stack_ptr =  (char *)0x002BBEUL - checkpoint_get_sp();
    printf(DBG_PTR_PRINT", "DBG_PTR_PRINT", "DBG_PTR_PRINT"\n",
            DBG_PTR(this_fn), DBG_PTR(call_site), DBG_PTR(stack_ptr));
}

void __cyg_profile_func_exit (void *this_fn, void *call_site) {

}
#endif

void application_main(void)
{
    // "Globals" must be on the stack because Mementos doesn't handle real
    // globals correctly
    uint8_t prev_pin_state = MODE_IDLE;

    nop_var = 42;
    SOFTWARE_CHECKPOINT();

    VMEM_WR(count) = 1;

    while (1)
    {
        run_mode_t mode = select_mode(&prev_pin_state);
        switch (mode) {
            case MODE_TRAIN_STATIONARY:
                LOG("mode: stationary\r\n");
                train(model.stationary);
                break;
            case MODE_TRAIN_MOVING:
                LOG("mode: moving\r\n");
                train(model.moving);
                break;
            case MODE_RECOGNIZE:
                LOG("mode: recognize\r\n");
                recognize(&model);
                break;
            default:
                LOG("mode: idle\r\n");
                break;
        }
    }

}
