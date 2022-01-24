#include <stdio.h>

int main(void) {
	int count = 0;

	do {
		printf("%d",count++);
	} while(count < 9);
	
	return (0);
}
