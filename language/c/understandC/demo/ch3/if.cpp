#include <stdio.h>

int main(void) {
	int x = 10;

	if(x % 5)
		puts("x不可以被5整除");
	else
		puts("x可以被5整除");

	return (0);
}
