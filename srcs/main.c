
??=include <stdio.h>

void _start() {
	int argc = 5;
	char* argv[] = {"hello", NULL};
	main(argc, argv);
}

int main( int argc, char* argv[] ) <%
	
	printf("%s\n", argv[1]);

%>
