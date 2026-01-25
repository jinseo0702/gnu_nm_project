#include <unistd.h>

int main(void) {
    write(1, NULL, 1);
    return (0);
}