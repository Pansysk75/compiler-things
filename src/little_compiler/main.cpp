#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"

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
    Evaluator evaluator;

    while (std::getline(file, line))
    {
        std::cout << "\nParsing next line: \n"
                  << line << std::endl;

        // Tokenize line
        std::vector<Token> tokens = lexer.tokenize_line(std::move(line));

        // Print diagnostics, if any
        if (!lexer.get_diagnostics().empty())
        {
            std::cout << "Lexer error:" << std::endl;
            for (auto &msg : lexer.get_diagnostics())
            {
                std::cout << msg << std::endl;
            }
            continue;
        }

        if (tokens.size() == 1 && tokens[0].tag_ == TokenTag::eof)
        {
            // If empty line, continue to next line
            continue;
        }

        // Parse tokens
        auto root = parser.parse(std::move(tokens));

        // Print result
        std::cout << *root << std::endl;

        // Print diagnostics, if any.
        if (!parser.get_diagnostics().empty())
        {
            std::cout << "Parser error:" << std::endl;
            for (auto &msg : parser.get_diagnostics())
            {
                std::cout << msg << std::endl;
            }
            continue;
        }

        // Evaluate
        auto result = evaluator.evaluate_expression(root);
        std::cout << "Evaluated: " << result << std::endl;
    }

    return 0;
}
