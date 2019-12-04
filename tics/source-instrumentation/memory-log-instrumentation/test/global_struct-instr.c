struct global_struct_def {
    int a;
    int b[2];
};

struct global_struct_def global_struct;

int main(void) {
    VMEM_WR(global_struct.a) = 42;
    VMEM_WR(global_struct.b[1]) = 21;
    VMEM_WR(global_struct.a) += 42;

    VMEM_WR_PI(global_struct.a++);
    VMEM_WR_PD(global_struct.a--);

    return 0;
}
