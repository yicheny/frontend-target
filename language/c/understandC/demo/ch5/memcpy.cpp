#include <stdio.h>
#include <cstring>

int main(void) {
	int a[5] = {3,4,5,6,7};
	int size = sizeof(a) / sizeof(a[0]);
	int b[size];
	
	memcpy(b,a,sizeof(a));
	
	for (int i=0; i<size; i++)
		printf("%d\n",b[i]);
		
	return (0);
}
