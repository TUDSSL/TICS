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

    local_struct_ptr->a = 42;
    local_struct_ptr->b[1] = 21;

    local_struct_ptr->a += 2;

    local_struct_ptr->a++;
    local_struct_ptr->a--;

    return 0;
}
