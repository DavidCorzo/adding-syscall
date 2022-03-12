#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

int main() {
	long value = 0;
	char expr[] = "(1+2)*[6*(7+9)]";
	long is_correct = syscall(546, &value, expr);
	printf("value=%ld, is_correct=%s\n", value, (is_correct?"true":"false"));	
    return 0;  
}
