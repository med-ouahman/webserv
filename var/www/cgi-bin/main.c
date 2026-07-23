#include <string.h>
#include <stdio.h>
#include <unistd.h>

int main()

{
	char *x = "hello please\n";
	int xlen = strlen(x);
	write(2, x, xlen);
	printf("Content-Type: text/html\r\nContent-length: 3\r\n\r\nhlx");

}
