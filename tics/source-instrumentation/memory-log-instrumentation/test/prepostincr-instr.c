int global_int = 0;

int main(void) {
    int local_int = 0;

    VMEM_WR(global_int) = 42;
    local_int = 21;

    VMEM_WR_PI(global_int++);
    VMEM_WR(++global_int);

    local_int++;
    ++local_int;

    VMEM_WR_PD(global_int--);
    VMEM_WR(--global_int);

    local_int--;
    --local_int;

    return 0;
}
