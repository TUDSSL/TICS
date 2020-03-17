#include <stdio.h>

#define WINDOW_SIZE 10

int temperature[WINDOW_SIZE];

int read_sensor(void) {
    return 42;
}

int main(void) {
    temperature[0] = read_sensor();

    @timely(200ms) {
        temperature[0] += 10;
    } else {
        printf("Temperature expired\r\n");
    }

    @timely(1s)
    {
        temperature[0] += 10;
    }
    printf("Temperature[0] = %d\r\n", temperature[0]);
}
