#include <unistd.h>
#include <stdio.h>

int main()
{
	char b[101];
	size_t size = 100;

	printf("CGI C\n");
	ssize_t s = read(0, b, size);
	if (s == -1) return 1;
	b[s] = 0;
	printf("%s", b);
	return  0;
}
