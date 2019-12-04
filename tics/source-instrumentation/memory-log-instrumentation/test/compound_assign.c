int global_int = 0;

int main(void) {
    global_int = 42;

    global_int += 10;
    global_int -= 10;

    global_int *= 10;
    global_int /= 10;
    global_int %= 10;

    global_int <<= 2;
    global_int >>= 2;

    global_int &= 10;
    global_int ^= 10;
    global_int |= 10;
}
