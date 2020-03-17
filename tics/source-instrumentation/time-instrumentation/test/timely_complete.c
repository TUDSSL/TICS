#include <stdio.h>

#define WINDOW_SIZE 10

@expires_after=1s /* Data expires in 1 second */
int temperature[WINDOW_SIZE];

int read_sensor(void) {
    return 42;
}

int main(void) {
    int max = 100;
    temperature[0] @= read_sensor(); // Assign with timestamp

    /* Catch expiration */
    @expires(temperature[0]) {
        if (temperature[0] > max) {
            max = temperature[0];
        }
    }
    @catch {
        printf("Temperature expired\r\n");
        temperature[0] = 0;
    }

    while (1) {
        /* Branch in time */
        @timely(100ms)
        {
            if (temperature[0] > max) {
                max = temperature[0];
            }
        }
        break;
    }
}
