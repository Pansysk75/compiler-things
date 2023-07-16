/*
Author: Panos Syskakis, July 2023

This is a simple lexer, which extracts tokens from an input string.
Example input: "my_var1 = 51;"
Tokens can be identifiers(ie "my_var1"), immediate values (ie "51"),
or some other symbol (ie `=`).
Also handles whitespaces/tabs/newlines.
 */

#include <string>
#include <iostream>

namespace token
{
    // Types of tokens
    enum class tag
    {
        id,
        symbol,
        val_bool,
        val_int,
        val_float
    };

    // Print support for token::tag
    std::ostream &operator<<(std::ostream &os, token::tag tag)
    {
        switch (tag)
        {
        case tag::id:
            return os << "id";
        case tag::symbol:
            return os << "symbol";
        case tag::val_bool:
            return os << "val_bool";
        case tag::val_int:
            return os << "val_int";
        case tag::val_float:
            return os << "val_float";
        };
        return os;
    }

    // Base class for all tokens
    struct Token
    {
        Token(token::tag tag) : tag_(tag) {}

        virtual void print(std::ostream &out) const
        {
            out << " <" << tag_ << "> ";
        }

        token::tag tag_;
    };

    // Print support for tokens
    std::ostream &operator<<(std::ostream &out, const Token &tok)
    {
        tok.print(out);
        return out;
    }

    struct Int : Token
    {
        Int(int value) : Token(tag::val_int), value_(value)
        {
        }

        virtual void print(std::ostream &out) const
        {
            out << " <" << tag_ << ", " << value_ << "> ";
        }

        int value_;
    };

    struct Float : Token
    {
        Float(double value) : Token(tag::val_float), value_(value)
        {
        }

        virtual void print(std::ostream &out) const
        {
            out << " <" << tag_ << ", " << value_ << "> ";
        }

        double value_;
    };

    struct Word : Token
    {
        Word(std::string lexeme) : Token(tag::id), lexeme_(lexeme)
        {
        }

        virtual void print(std::ostream &out) const
        {
            out << " <" << tag_ << ", " << lexeme_ << "> ";
        }

        std::string lexeme_;
    };

    struct Symbol : Token
    {
        Symbol(const char &symbol) : Token(tag::symbol), symbol_(symbol)
        {
        }

        virtual void print(std::ostream &out) const
        {
            out << " <" << tag_ << ", " << symbol_ << "> ";
        }

        char symbol_;
    };

};

// Takes raw text as input and extracts token one at a time, from left to right.
class Lexer
{
public:
    Lexer(std::string &input_str)
        : input_str_(input_str), peek_(input_str[0]), p_(0), line_(0)
    {
    }

private:
    const char &next_input_char()
    {
        if (peek_ == '\0')
        {
            throw "invalid syntax: unexpected end of text";
        }
        return input_str_[++p_];
    }
    const char &peek_ahead()
    {
        return input_str_[p_ + 1];
    }

public:
    bool scan(std::unique_ptr<token::Token> &p)
    {

        // Step 1: Ignore spaces/tabs/newlines, exit if end of str
        for (;; peek_ = next_input_char())
        {
            if (peek_ == ' ' || peek_ == '\t')
            {
                continue;
            }
            // Increment line count
            else if (peek_ == '\n')
            {
                line_++;
            }

            else
            {
                break;
            }
        }

        // Step 2: Ignore comments
        if (peek_ == '/')
        {
            if (peek_ahead() == '/')
            {
                // ignore line
                while (peek_ != '\n' && peek_ != '\0')
                {
                    peek_ = next_input_char();
                }
            }
            else if (peek_ahead() == '*')
            {
                // ignore until "*/"
                peek_ = next_input_char();
                do
                {
                    peek_ = next_input_char();
                } while (peek_ != '*');
                peek_ = next_input_char();
                if (peek_ != '/')
                {
                    throw "invalid syntax: expected \"*/\" after \"/*\"";
                }
                peek_ = next_input_char();
            }
        }

        if (peek_ == '\0')
        {
            return false;
        }

        // Step 3: Actual parsing of a token

        // Floats
        if (peek_ == '.' && std::isdigit(peek_ahead()))
        {
            std::string buf;
            do
            {
                buf.push_back(peek_);
                peek_ = next_input_char();
            } while (std::isdigit(peek_));
            float value = std::stof(buf);
            p = std::make_unique<token::Float>(value);
            return true;
        }
        // Integers and floats
        if (std::isdigit(peek_))
        {
            bool is_float = false;
            std::string buf;
            do
            {
                buf.push_back(peek_);
                peek_ = next_input_char();
                if (peek_ == '.' && !is_float)
                {
                    is_float = true;
                    buf.push_back(peek_);
                    peek_ = next_input_char();
                }
            } while (std::isdigit(peek_));
            if (is_float)
            {
                float value = std::stof(buf);
                p = std::make_unique<token::Float>(value);
            }
            else
            {
                int value = std::stoi(buf);
                p = std::make_unique<token::Int>(value);
            }
            return true;
        }
        // Identifiers
        if (std::isalpha(peek_))
        {
            std::string buf;
            do
            {
                buf.push_back(peek_);
                peek_ = next_input_char();
            } while (std::isalnum(peek_));

            p = std::make_unique<token::Word>(buf);
            return true;
        }
        // Treat any unknown character as a "Symbol"
        else
        {
            p = std::make_unique<token::Symbol>(peek_);
            peek_ = next_input_char();
            return true;
        }
    }

private:
    char &peek_;
    std::string input_str_;
    unsigned int p_; // Pointer to current element in input_str_
    unsigned int line_;
};

int main()
{
    std::string input_buffer;

    std::cout << "Give an input string:" << std::endl;
    std::getline(std::cin, input_buffer);

    Lexer lex(input_buffer);

    std::unique_ptr<token::Token> tok;

    try
    {
        while (lex.scan(tok))
        {
            std::cout << (*tok) << '\t';
        }
    }
    catch (const char *err)
    {
        std::cout << "Exception: " << err << std::endl;
    }

    return 0;
}