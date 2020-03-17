#include <stdio.h>

#define WINDOW_SIZE 10

@expires_after=1s /* Data expires in 1 second */
int temperature[WINDOW_SIZE];

int read_sensor(void) {
    return 42;
}

int main(void) {
    temperature[0] @= read_sensor(); // Assign with timestamp
    printf("Temperature[0] = %d\r\n", temperature[0]);
}
