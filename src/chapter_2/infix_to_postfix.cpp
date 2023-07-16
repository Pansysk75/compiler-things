/*
Author: Panos Syskakis, July 2023

Translate infix expressions (ie 5+8+1) into postfix form (ie 58+1-)
This is done in the simplest way possible, only handling single digits
and +,- operators.
 */

#include <string>
#include <iostream>

class Parser
{
public:
    Parser(const std::string &input_str)
        : input_str_(input_str), p_(0)
    {
        lookahead_ = input_str[0];
    }

    // Checks for match and advances the lookahead one character
    void match(const char &c)
    {
        if (input_str_[p_] == c)
        {
            lookahead_ = input_str_[++p_];
        }
        else
        {
            throw "syntax error";
        }
    }

    // Matches a single digit
    void term()
    {
        if (std::isdigit(lookahead_))
        {
            const char t = lookahead_;
            match(lookahead_);
            result_str_.push_back(t);
        }
        else
        {
            throw "syntax error";
        }
    }

    void expr()
    {
        result_str_.clear();

        // Match first term
        term();

        // Match (operator term) pairs, in succession if needed
        while (true)
        {
            if (lookahead_ == '+')
            {
                match('+');
                term();
                result_str_.push_back('+');
                continue;
            }
            else if (lookahead_ == '-')
            {
                match('-');
                term();
                result_str_.push_back('-');
                continue;
            }
            break;
        }
    }

    const std::string &get_result()
    {
        return result_str_;
    }

    const std::string &input_str_;
    std::string result_str_;
    unsigned int p_; // pointer to some element in input_str_
    char lookahead_;
};

int main()
{
    std::string input_buffer;

    std::cout << "Give an input string:" << std::endl;
    std::getline(std::cin, input_buffer);

    Parser parse(input_buffer);

    try
    {
        parse.expr();
        std::cout << "Postfix: " << parse.get_result() << std::endl;
    }
    catch (const char *err)
    {
        std::cout << "Caught exception: " << err << std::endl;
    }

    return 0;
}