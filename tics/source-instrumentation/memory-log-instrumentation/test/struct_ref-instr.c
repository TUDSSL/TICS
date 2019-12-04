struct struct_def {
    int a;
    int b[2];
};


int main(void) {
    struct struct_def local_struct;
    struct struct_def *local_struct_ptr;

    local_struct_ptr = &local_struct;

    local_struct.a = 42;
    local_struct.b[1] = 21;

    VMEM_WR(local_struct_ptr->a) = 42;
    VMEM_WR(local_struct_ptr->b[1]) = 21;

    VMEM_WR(local_struct_ptr->a) += 2;

    VMEM_WR_PI(local_struct_ptr->a++);
    VMEM_WR_PD(local_struct_ptr->a--);

    return 0;
}
