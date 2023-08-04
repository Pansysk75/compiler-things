#pragma once
#include "lexer.hpp"

#include <string>
#include <iostream>
#include <vector>

// Describes the type of node in the AST
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