int add(int a, int b) { return a + b; }

int global = 42;
static int sglobal = 7;

int main(void) {
    return add(global, sglobal);
}