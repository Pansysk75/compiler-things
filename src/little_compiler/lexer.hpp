#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

// Types of tokens
enum class TokenTag
{
    id,
    binary_op,
    val,
    eof,
    bad
};

// Print support for TokenTag
std::ostream &operator<<(std::ostream &os, TokenTag tag)
{
    switch (tag)
    {
    case TokenTag::id:
        return os << "id";
    case TokenTag::binary_op:
        return os << "binary_op";
    case TokenTag::val:
        return os << "val";
    case TokenTag::eof:
        return os << "eof";
    case TokenTag::bad:
        return os << "bad";
    };
    return os;
}

struct Token
{
    Token() : tag_(TokenTag::bad) {}

    Token(TokenTag tag, unsigned int line_count, unsigned int char_count)
        : tag_(tag), line_count_(line_count), char_count_(char_count) {}

    Token(TokenTag tag, char &val, unsigned int line_count, unsigned int char_count)
        : tag_(tag), val_(1, val), line_count_(line_count), char_count_(char_count) {}

    Token(TokenTag tag, std::string val, unsigned int line_count, unsigned int char_count)
        : tag_(tag), val_(val), line_count_(line_count), char_count_(char_count) {}

    void print(std::ostream &out) const
    {
        if (tag_ == TokenTag::bad || tag_ == TokenTag::eof)
        {
            out << "<" << tag_ << ">";
        }
        else
        {
            out << "<" << val_ << ">";
        }
    }

    TokenTag tag_;
    std::string val_;
    unsigned int line_count_;
    unsigned int char_count_;
};

// Print support for tokens
std::ostream &operator<<(std::ostream &out, const Token &tok)
{
    tok.print(out);
    return out;
}

// Takes raw text as input and extracts token one at a time, from left to right.
class Lexer
{

public:
    Lexer() : line_(0), p_(0) {}

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
    Token next_token()
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
            return Token(TokenTag::eof, line_, p_);
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
            return Token(TokenTag::val, buf, line_, p_);
        }
        // Integers or floats
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

            return Token(TokenTag::val, buf, line_, p_);
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

            return Token(TokenTag::id, buf, line_, p_);
        }
        if (peek_ == '+' || peek_ == '-' || peek_ == '*' || peek_ == '/' || peek_ == '=')
        {
            Token tok = Token(TokenTag::binary_op, peek_, line_, p_);
            peek_ = next_input_char();
            return tok;
        }
        // Treat any unknown character as a bad token
        else
        {
            std::string val(1, peek_);
            peek_ = next_input_char();
            return Token(TokenTag::bad, val, line_, p_);
        }
    }

    std::vector<Token> tokenize_line(std::string &&next_line)
    {
        input_str_ = std::move(next_line);
        peek_ = input_str_[0];
        p_ = 0;

        std::vector<Token> tokens;

        try
        {
            Token tok;
            do
            {
                tok = this->next_token();
                tokens.push_back(tok);
                std::cout << tok << ' ';
            } while (tok.tag_ != TokenTag::bad && tok.tag_ != TokenTag::eof);
        }
        catch (const char *err)
        {
            std::cout << "Exception: " << err << std::endl;
        }
        std::cout << std::endl;

        line_++;

        return std::move(tokens);
    }

private:
    char peek_;
    std::string input_str_;
    unsigned int p_; // Pointer to current element in input_str_
    unsigned int line_;
};
