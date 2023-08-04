#include "lexer.hpp"
#include "parser.hpp"

#include <vector>
#include <string>
#include <iostream>

int main()
{
    std::string input_buffer;

    std::cout << "Give an input string:" << std::endl;
    std::getline(std::cin, input_buffer);

    Lexer lex(input_buffer);
    std::vector<Token> tokens;

    try
    {
        Token tok;
        do
        {
            tok = lex.next_token();
            tokens.push_back(tok);
            std::cout << tok << '\t';
        } while (tok.tag_ != TokenTag::bad && tok.tag_ != TokenTag::eof);
    }
    catch (const char *err)
    {
        std::cout << "Exception: " << err << std::endl;
    }
    std::cout << std::endl;

    Parser parser(std::move(tokens));
    auto root = parser.parse();

    std::cout << *root << std::endl;

    return 0;
}