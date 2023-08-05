#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

// Types of tokens
enum class TokenTag
{
    id,
    plus,
    minus,
    star,
    slash,
    parenthesis_open,
    parenthesis_close,
    val_int,
    val_double,
    eof,
    bad
};

// Print support for TokenTag
std::ostream &operator<<(std::ostream &os, TokenTag tag)
{
#define TOKEN_TAG_CASE(tag) \
    case TokenTag::tag:     \
        return os << #tag;

    switch (tag)
    {
        TOKEN_TAG_CASE(id)
        TOKEN_TAG_CASE(plus)
        TOKEN_TAG_CASE(minus)
        TOKEN_TAG_CASE(star)
        TOKEN_TAG_CASE(slash)
        TOKEN_TAG_CASE(parenthesis_open)
        TOKEN_TAG_CASE(parenthesis_close)
        TOKEN_TAG_CASE(val_int)
        TOKEN_TAG_CASE(val_double)
        TOKEN_TAG_CASE(eof)
        TOKEN_TAG_CASE(bad)
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
        if (tag_ == TokenTag::eof)
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
            std::stringstream err;
            err << "Error: invalid syntax: expected end of text at (" << line_ << ", " << p_ << ")";
            diagnostics_.push_back(err.str());
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
        for (peek_ = input_str_[p_];; peek_ = next_input_char())
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
                    std::stringstream err;
                    err << "Error: invalid syntax: expected \"/*\" to close with \"*/\") at (" << line_ << ", " << p_ << ")";
                    diagnostics_.push_back(err.str());
                    std::cout << err.str() << std::endl;
                }
                else
                {
                    peek_ = next_input_char();
                }
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
            return Token(TokenTag::val_double, buf, line_, p_);
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
            TokenTag tag = is_float ? TokenTag::val_double : TokenTag::val_int;
            return Token(tag, buf, line_, p_);
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
        // Binary operators
        if (peek_ == '+')
        {
            return Token(TokenTag::plus, peek_, line_, p_++);
        }
        if (peek_ == '-')
        {
            return Token(TokenTag::minus, peek_, line_, p_++);
        }
        if (peek_ == '*')
        {
            return Token(TokenTag::star, peek_, line_, p_++);
        }
        if (peek_ == '/')
        {
            return Token(TokenTag::slash, peek_, line_, p_++);
        }
        if (peek_ == '(')
        {
            return Token(TokenTag::parenthesis_open, peek_, line_, p_++);
        }
        if (peek_ == ')')
        {
            return Token(TokenTag::parenthesis_close, peek_, line_, p_++);
        }
        // Treat any unknown character as a bad token
        else
        {
            std::string val(1, peek_);

            std::stringstream err;
            err << "Error: Invalid token (" << val << ") at (" << line_ << ", " << p_ << ")";
            diagnostics_.push_back(err.str());

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

        Token tok;
        do
        {
            tok = this->next_token();
            tokens.push_back(tok);
            std::cout << tok << ' ';
        } while (tok.tag_ != TokenTag::bad && tok.tag_ != TokenTag::eof);

        std::cout << std::endl;

        line_++;

        return std::move(tokens);
    }

    void print_diagnostics(std::ostream &out)
    {
        if (!diagnostics_.empty())
        {
            out << "Lexer diagnostics:" << std::endl;
            for (auto &msg : diagnostics_)
            {
                out << msg << std::endl;
            }
            diagnostics_.clear();
        }
    }

private:
    char peek_;
    std::string input_str_;
    unsigned int p_; // Pointer to current element in input_str_
    unsigned int line_;
    std::vector<std::string> diagnostics_;
};
