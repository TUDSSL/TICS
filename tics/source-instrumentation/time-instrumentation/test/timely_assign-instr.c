#include "timely.h"
#include <stdio.h>

#define WINDOW_SIZE 10

expires_meta_t _temperature_meta = {.expires_after_ms=1000}; /* Data expires in 1 second */
int temperature[WINDOW_SIZE];

int read_sensor(void) {
    return 42;
}

int main(void) {
    TIMELY_DISABLE_CHECKPOINTS();
    _temperature_meta.expires = TIMELY_GET_FUTURE_TIME(_temperature_meta.expires_after_ms);
    temperature[0] = read_sensor(); // Assign with timestamp
    TIMELY_ENABLE_CHECKPOINTS();
    printf("Temperature[0] = %d\r\n", temperature[0]);
}
