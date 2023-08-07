#pragma once

#include "token.hpp"

#include <sstream>
#include <string>
#include <vector>

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

public:
    std::vector<Token> tokenize_line(std::string &&next_line)
    {
        // Reset state
        input_str_ = std::move(next_line);
        peek_ = input_str_[0];
        p_ = 0;
        diagnostics_.clear();

        std::vector<Token> tokens;

        Token tok;
        do
        {
            tok = this->next_token();
            tokens.push_back(tok);
        } while (tok.tag_ != TokenTag::bad && tok.tag_ != TokenTag::eof);

        line_++;

        return std::move(tokens);
    }

    std::vector<std::string> &get_diagnostics()
    {
        return diagnostics_;
    }

private:
    char peek_;
    std::string input_str_;
    unsigned int p_; // Pointer to current element in input_str_
    unsigned int line_;
    std::vector<std::string> diagnostics_;
};
