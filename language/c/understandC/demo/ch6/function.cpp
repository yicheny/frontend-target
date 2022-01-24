#include <stdio.h>

int maxof(int x,int y){
	return (x > y) ? x : y;
} 

int main(void) {	
	printf("%d",maxof(3,4));
		
	return (0);
}
