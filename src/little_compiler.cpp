/*
Author: Panos Syskakis, July 2023

This is a simple lexer, which extracts tokens from an input string.
Example input: "my_var1 = 51;"
Tokens can be identifiers(ie "my_var1"), immediate values (ie "51"),
or some other symbol (ie `=`).
Also handles whitespaces/tabs/newlines.
 */

#include <vector>
#include <string>
#include <iostream>

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////* Lexing *//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////* Parsing */////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

enum class SyntaxTag
{
    expression,
    binary_expression
};

struct SyntaxNode;
std::ostream &operator<<(std::ostream &out, const SyntaxNode &node);

// A syntax node represents a node in the AST , and is a base class of all expressions.
struct SyntaxNode
{
    SyntaxNode(Token tok, SyntaxTag tag) : tok_(tok), tag_(tag)
    {
    }

    virtual std::vector<std::shared_ptr<SyntaxNode>> get_children() const = 0;

    Token tok_;
    SyntaxTag tag_;

    // Print all children recursively
    void print(std::ostream &out) const
    {
        out << this->tok_;
        for (const auto &child : this->get_children())
        {
            out << *child;
        }
    }

    // std::vector<std::unique_ptr<SyntaxNode>> children_;
};

// Print support for SyntaxNode
std::ostream &operator<<(std::ostream &out, const SyntaxNode &node)
{
    node.print(out);
    return out;
}

struct Expression : SyntaxNode
{
    Expression(Token tok) : SyntaxNode(tok, SyntaxTag::expression)
    {
    }

    std::vector<std::shared_ptr<SyntaxNode>> get_children() const
    {
        return {};
    };
};

struct BinaryExpression : SyntaxNode
{
    // Must use some kind of ptr, because of inheritance
    // (children may be any class that inherits fron SyntaxNode)
    using ptr_type = std::shared_ptr<SyntaxNode>;

    // // Assume ownership
    // BinaryExpression(SyntaxNode *left, Token op, SyntaxNode *right)
    //     : SyntaxNode(SyntaxTag::binary_expression), left_(left), op_(op), right_(right)
    // {
    // }

    BinaryExpression(ptr_type &&left, Token op, ptr_type &&right)
        : SyntaxNode(op, SyntaxTag::binary_expression), left_(std::move(left)), right_(std::move(right))
    {
    }

    std::vector<std::shared_ptr<SyntaxNode>> get_children() const
    {
        return {left_, right_};
    };

    ptr_type left_;
    ptr_type right_;
};

class Parser
{
public:
    Parser(std::vector<Token> &&tokens) : tokens_(tokens), p_(0) {}

private:
    Token &peek(int offset)
    {
        size_t pos = std::min(p_ + offset, tokens_.size() - 1);
        return tokens_[pos];
    }

    Token &current()
    {
        return tokens_[p_];
    }

    Token &next()
    {
        return tokens_[p_++];
    }

    Token match(TokenTag tag)
    {
        if (current().tag_ == tag)
        {
            return next();
        }
        // return bad token if no match
        return Token(TokenTag::bad);
    }

    std::unique_ptr<SyntaxNode> parse_primary_expression()
    {
        Token tok = match(TokenTag::val_int);
        return std::make_unique<Expression>(tok);
    }

public:
    std::unique_ptr<SyntaxNode> parse()
    {
        auto expr = parse_primary_expression();
        while (current().val_ == "+")
        {
            std::cout << "PARSING binary" << std::endl;
            std::cout << "LEFT " << expr->tok_ << std::endl;

            auto op = current();
            next();
            auto right = parse_primary_expression();
            std::cout << "RIGHT " << right->tok_ << std::endl;

            return std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

private:
    std::vector<Token> tokens_;
    size_t p_;
};

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