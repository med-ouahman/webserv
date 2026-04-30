#include <iostream>
#include <string>
#include <readline/readline.h>
#include <stdlib.h>
#define MAX_CHUNK_SIZE 100000

size_t parse_chunk_size( const std::string& line_buff ) {
		std::string hexas = "0123456789abcdef";
		::size_t chunk_size = 0;
		
		
		for ( ::size_t i(0); i < line_buff.size(); ++i ) {

			char c = ::tolower(line_buff[i]);
			if (c == ';') {
				break;
			}
			if (hexas.find(c) == std::string::npos) {
				return std::string::npos;
			}

			chunk_size = chunk_size * hexas.size() + hexas.find(c);

			if (chunk_size > MAX_CHUNK_SIZE) {
				/* I will be generous and log errors later */
				return MAX_CHUNK_SIZE;
		}
	}

	return chunk_size;
}

int main() {

	while (true) {
		const char* line = readline("> ");
		rl_his
		std::string f(line);
		size_t a = parse_chunk_size(f);
		if (a >= MAX_CHUNK_SIZE) {
			std::cout << "Error\n";
			continue;
			free((void*)line);
		}
		std::cout << "Num: " << a << "\n";
	}
}
