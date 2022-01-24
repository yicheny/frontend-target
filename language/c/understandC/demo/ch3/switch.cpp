#include <stdio.h>

int main(void) {
	int num = 10;

	//printf("%d\n",num % 3);
	switch(num % 3) {
		case 0:
			puts("可以被3整数");
			break;
		case 1:
			puts("被3整数余数是1");
			break;
		case 2:
			puts("被3整数余数是2");
			break;
		default:
			puts("结果异常！");
			break;
	}

	return (0);
}
