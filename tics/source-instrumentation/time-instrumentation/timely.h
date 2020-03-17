#ifndef TIMELY_H_
#define TIMELY_H_

#include <stdint.h>

/*
 * To use this Timely API, fill in these macros  with your RTC code
 * This API is for the general use and does not enforce a specific RTC
 */
#define TIMELY_GET_FUTURE_TIME(offset_) dummy_get_future_time(offset_)
#define TIMELY_MS_REMAINING(rtc_time_)  dummy_time_ms_remaining(rtc_time_)

#define TIMELY_CHECKPOINT()             dummy_checkpoint()
#define TIMELY_RESTORE()                dummy_restore()
#define TIMELY_DISABLE_CHECKPOINTS()    dummy_disable_checkpoints()
#define TIMELY_ENABLE_CHECKPOINTS()     dummy_enable_checkpoints()

#define TIMELY_START_EXPIRE_TIMER_MS(time_ms_)  dummy_start_expire_timer_ms(time_ms_)
#define TIMELY_STOP_EXPIRE_TIMER_MS()           dummy_stop_expire_timer()


typedef uint32_t expire_time_t;
typedef uint32_t expire_time_rtc_t;


// Dummy implementations
static inline expire_time_rtc_t dummy_rtc_get_time() {
    static expire_time_rtc_t fake_time = 0;

    fake_time += 1;
    return fake_time;
}

static inline expire_time_rtc_t dummy_get_future_time(expire_time_t offset) {
    expire_time_rtc_t fake_offset_time;

    fake_offset_time = dummy_rtc_get_time();
    fake_offset_time += offset;
}

static inline expire_time_t dummy_rtc_time_to_ms(expire_time_rtc_t rtc_time) {
    return rtc_time;
}

static inline expire_time_t dummy_time_ms_remaining(expire_time_rtc_t rtc_time) {
    expire_time_t time_now = dummy_rtc_get_time();
    if (rtc_time > time_now) {
        return (expire_time_t)(rtc_time - time_now);
    }
    return 0;
}

static inline void dummy_checkpoint(void) {
    return;
}

static inline void dummy_disable_checkpoints(void) {
    return;
}

static inline void dummy_enable_checkpoints(void) {
    return;
}

static inline void dummy_start_expire_timer_ms(expire_time_t time_expire_ms) {
    // Set expiration callback to:
    // TIMELY_RESTORE
    (void)time_expire_ms;
    return;
}

static inline void dummy_stop_expire_timer(void) {
    return;
}

typedef struct expires_meta {
    const expire_time_t expires_after_ms;
    expire_time_rtc_t expires; // RTC time when the variable expires
    expire_time_t last_ms_remaining;
} expires_meta_t;


#endif /* TIMELY_H_ */
