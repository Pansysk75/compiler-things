#include "lexer.hpp"
#include "parser.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        std::cout << "No input file" << std::endl;
        return -1;
    }

    std::cout << argv[0] << std::endl;
    std::cout << "Input file: " << argv[1] << std::endl;
    std::ifstream file(argv[1]);

    std::string line;
    Lexer lexer;
    Parser parser;

    while (std::getline(file, line))
    {
        std::cout << line << '\n'
                  << std::endl;

        // Tokenize line
        std::vector<Token> tokens = lexer.tokenize_line(std::move(line));

        // Parse tokens
        auto root = parser.parse(std::move(tokens));

        // Print result
        std::cout << *root << std::endl;

        // Print diagnostics
        parser.print_diagnostics(std::cout);
    }

    return 0;
}
