#pragma once

#include "token.hpp"

#include <iostream>
#include <vector>

// Describes the type of node in the AST
enum class SyntaxTag
{
    // identifier_expression,
    boolean_expression,

    integer_expression,
    floating_expression,

    unary_expression,
    binary_expression,
    parenthesized_expression
};

class SyntaxNode;
std::ostream &operator<<(std::ostream &out, const SyntaxNode &node);

// A syntax node represents a node in the AST , and is a base class of all expressions.
class SyntaxNode
{
public:
    SyntaxNode(Token tok, SyntaxTag tag) : tok_(tok), tag_(tag)
    {
    }

    // TOFIX: return something more efficient
    virtual std::vector<std::shared_ptr<SyntaxNode>> get_children() const
    {
        return {};
    };

    Token tok_;
    SyntaxTag tag_;

    // Print all children recursively
    void print(std::ostream &out, std::string indent = "", bool is_last = true) const
    {
        std::string marker = is_last ? "'---" : "|---";
        out << indent;
        out << marker;
        out << this->tok_ << "\n";

        indent += is_last ? "    " : "|   ";
        for (const auto &child : this->get_children())
        {
            child->print(out, indent, child == this->get_children().back());
        }
    }
};

// Print support for SyntaxNode
std::ostream &operator<<(std::ostream &out, const SyntaxNode &node)
{
    node.print(out);
    return out;
}

struct IntegerExpression : public SyntaxNode
{
    IntegerExpression(Token tok) : SyntaxNode(tok, SyntaxTag::integer_expression)
    {
    }
};

struct FloatingExpression : public SyntaxNode
{
    FloatingExpression(Token tok) : SyntaxNode(tok, SyntaxTag::floating_expression)
    {
    }
};

struct BooleanExpression : public SyntaxNode
{
    BooleanExpression(Token tok) : SyntaxNode(tok, SyntaxTag::boolean_expression)
    {
    }
};

struct BinaryExpression : public SyntaxNode
{
    // Must use some kind of ptr, because of inheritance
    // (children may be any class that inherits from SyntaxNode)
    using ptr_type = std::shared_ptr<SyntaxNode>;

    BinaryExpression(ptr_type left, Token op, ptr_type right)
        : SyntaxNode(op, SyntaxTag::binary_expression), left_(left), right_(right)
    {
    }

    std::vector<std::shared_ptr<SyntaxNode>> get_children() const
    {
        return {left_, right_};
    };

    ptr_type left_;
    ptr_type right_;
};

struct UnaryExpression : public SyntaxNode
{
    // Must use some kind of ptr, because of inheritance
    // (children may be any class that inherits from SyntaxNode)
    using ptr_type = std::shared_ptr<SyntaxNode>;

    UnaryExpression(Token op, ptr_type expr)
        : SyntaxNode(op, SyntaxTag::unary_expression), expr_(expr)
    {
    }

    std::vector<std::shared_ptr<SyntaxNode>> get_children() const
    {
        return {expr_};
    };

    ptr_type expr_;
};

struct ParenthesizedExpression : public SyntaxNode
{
    using ptr_type = std::shared_ptr<SyntaxNode>;

    ParenthesizedExpression(Token paren_open, ptr_type expr, Token paren_close)
        // TOFIX: A base class with a token member doesn't make sense here
        : SyntaxNode(paren_open, SyntaxTag::parenthesized_expression), paren_open_(paren_open),
          expr_(expr), paren_close_(paren_close)
    {
    }

    std::vector<std::shared_ptr<SyntaxNode>> get_children() const
    {
        return {expr_};
    };
    Token paren_open_;
    ptr_type expr_;
    Token paren_close_;
};