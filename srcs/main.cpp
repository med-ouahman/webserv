#include <iostream>

static int usage( void ) {
	std::cerr << "Usage: ./webserv <config_file>\n";
	return 1; // ERR_USAGE
}

static void parseConfigFile( const char* configFileName ) {
	std::string f = configFileName;
	std::cout << f << "\n";
}

int main( int argc, char** argv ) {
	if (argc != 2) {
		return usage();
	}
	parseConfigFile(argv[1]);
	return 0;
}
