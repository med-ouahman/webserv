#include <iostream>

static int usage( void ) {
	int ERR_USAGE = 1; /* WILL BE TRANSFORMED TO A MACRO */
	std::cerr << "Usage: ./webserv [ <config_file> ]\n";
	return ERR_USAGE;
}


int main( int argc, char** argv ) {

	if (argc > 2) {
		return usage();
	}
	(void)argv; // to avoid unused variable warning for now
	std::cout << "WebServer is starting..." << std::endl;
	return 0;
}


/*

man, this shit rocks 
I will finish this and then go to bed
yep

*/