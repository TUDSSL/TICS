int global_int = 0;

int main(void) {
    int local_int = 0;

    global_int = 42;
    local_int = 21;

    global_int++;
    ++global_int;

    local_int++;
    ++local_int;

    global_int--;
    --global_int;

    local_int--;
    --local_int;

    return 0;
}
