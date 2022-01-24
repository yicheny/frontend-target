#include <stdio.h>

int main(void) {
	int l[5] = {3,4,5,6,7}; 
	
	int size = sizeof(l) / sizeof(l[0]);
	
	for (int i=0; i<size; i++)
		printf("%d\n",l[i]);
		
	return (0);
}
