#pragma once

#include <iostream>
#include <string>

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

class Token
{
public:
    Token() : tag_(TokenTag::bad) {}

    Token(TokenTag tag, unsigned int line_count, unsigned int char_count)
        : tag_(tag), line_count_(line_count), char_count_(char_count) {}

    Token(TokenTag tag, char &val, unsigned int line_count, unsigned int char_count)
        : tag_(tag), val_(1, val), line_count_(line_count), char_count_(char_count) {}

    Token(TokenTag tag, std::string val, unsigned int line_count, unsigned int char_count)
        : tag_(tag), val_(val), line_count_(line_count), char_count_(char_count) {}

    // if tok represents binary operation, returns its precedence
    // else, returns 0
    int get_binary_operator_precedence()
    {
        switch (tag_)
        {
        case TokenTag::star:
        case TokenTag::slash:
            return 2;
        case TokenTag::plus:
        case TokenTag::minus:
            return 1;
        default:
            return 0;
        }
    }

    // if tok represents binary operation, returns its precedence
    // else, returns 0
    int get_unary_operator_precedence()
    {
        switch (tag_)
        {
        case TokenTag::plus:
        case TokenTag::minus:
            return 3;
        default:
            return 0;
        }
    }

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
