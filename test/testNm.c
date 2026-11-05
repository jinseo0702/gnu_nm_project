#include <stdio.h>

int global_var = 42;            /* Data Section (D) */
int uninit_var;                 /* BSS Section (B) */
static int static_var = 10;     /* Data Section Local (d) */
const int read_only = 100;      /* Read-only Data (R/r) */

void test_func(void) {          /* Text Section (T) */
	printf("Hello nm!\\\\n");
}

int main(void) {
	test_func();
	return 0;
}