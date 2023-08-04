#pragma once
#include <string>
#include <iostream>

// Types of tokens
enum class TokenTag
{
    id,
    symbol,
    val_bool,
    val_int,
    val_float,
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
    case TokenTag::symbol:
        return os << "symbol";
    case TokenTag::val_bool:
        return os << "val_bool";
    case TokenTag::val_int:
        return os << "val_int";
    case TokenTag::val_float:
        return os << "val_float";
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
    Token(TokenTag tag) : tag_(tag) {}
    Token(TokenTag tag, char &val) : tag_(tag), val_(1, val) {}
    Token(TokenTag tag, std::string val) : tag_(tag), val_(val) {}

    void print(std::ostream &out) const
    {
        out << " <" << tag_ << ", " << val_ << "> ";
    }

    TokenTag tag_;
    std::string val_;
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
            return Token(TokenTag::eof);
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
            return Token(TokenTag::val_float, buf);
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
                return Token(TokenTag::val_float, buf);
            }
            else
            {
                return Token(TokenTag::val_int, buf);
            }
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

            return Token(TokenTag::id, buf);
        }
        if (peek_ == '+' || peek_ == '-' || peek_ == '*' || peek_ == '/' || peek_ == '=')
        {
            Token tok = Token(TokenTag::symbol, peek_);
            peek_ = next_input_char();
            return tok;
        }
        // Treat any unknown character as "bad"
        else
        {
            std::string val(1, peek_);
            peek_ = next_input_char();
            return Token(TokenTag::bad, val);
        }
    }

private:
    char &peek_;
    std::string input_str_;
    unsigned int p_; // Pointer to current element in input_str_
    unsigned int line_;
};
