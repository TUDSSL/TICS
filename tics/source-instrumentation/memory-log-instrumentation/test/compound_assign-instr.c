int global_int = 0;

int main(void) {
    VMEM_WR(global_int) = 42;

    VMEM_WR(global_int) += 10;
    VMEM_WR(global_int) -= 10;

    VMEM_WR(global_int) *= 10;
    VMEM_WR(global_int) /= 10;
    VMEM_WR(global_int) %= 10;

    VMEM_WR(global_int) <<= 2;
    VMEM_WR(global_int) >>= 2;

    VMEM_WR(global_int) &= 10;
    VMEM_WR(global_int) ^= 10;
    VMEM_WR(global_int) |= 10;
}
