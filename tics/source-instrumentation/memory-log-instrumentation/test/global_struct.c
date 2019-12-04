struct global_struct_def {
    int a;
    int b[2];
};

struct global_struct_def global_struct;

int main(void) {
    global_struct.a = 42;
    global_struct.b[1] = 21;
    global_struct.a += 42;

    global_struct.a++;
    global_struct.a--;

    return 0;
}
