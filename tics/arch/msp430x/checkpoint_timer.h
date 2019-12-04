#ifndef CHECKPOINT_TIMER_H__
#define CHECKPOINT_TIMER_H__

#define checkpoint_timer_start __enable_interrupt

void checkpoint_timer_enable(void);
void checkpoint_timer_disable(void);
void checkpoint_timer_init(void);
void checkpoint_timer_reset(void);

#endif /* CHECKPOINT_TIMER_H__ */
