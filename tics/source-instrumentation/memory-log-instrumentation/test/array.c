int global_array[10];
int global_2d_array[5][5];

int main(void) {
    int local_array[5];
    int *ptr_local_array;

    ptr_local_array = (int *)&local_array;

    local_array[1] = 21;
    global_array[2] = 42;

    global_2d_array[1][2] = 60;

    ptr_local_array[2] = 7;

    static int filter[20];
    int i = 2;
    filter[i] = 0;

    return 0;
}
