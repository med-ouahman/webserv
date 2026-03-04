#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::ifstream file("file.conf");

    if (!file.is_open())
    {
        std::cerr << "Error: could not open file\n";
        return 1;
    }

    std::vector<std::string> lines;
    std::string line;

    // while (std::getline(file, line))
    // {
        // parse_line(line);
    // }
    while (std::getline(file, line))
        lines.push_back(line);

    for (const std::string& l : lines)
        std::cout << l << '\n';

    return 0;
}