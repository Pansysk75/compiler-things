#pragma once
#include "lexer.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

// Describes the type of node in the AST
enum class SyntaxTag
{
    expression,
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
    virtual std::vector<std::shared_ptr<SyntaxNode>> get_children() const = 0;

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

class Expression : public SyntaxNode
{
public:
    Expression(Token tok) : SyntaxNode(tok, SyntaxTag::expression)
    {
    }

    std::vector<std::shared_ptr<SyntaxNode>> get_children() const
    {
        return {};
    };
};

class BinaryExpression : public SyntaxNode
{
public:
    // Must use some kind of ptr, because of inheritance
    // (children may be any class that inherits from SyntaxNode)
    using ptr_type = std::shared_ptr<SyntaxNode>;

    BinaryExpression(ptr_type left, Token &op, ptr_type right)
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

class ParenthesizedExpression : public SyntaxNode
{
public:
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

class Parser
{

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
        Token &curr = tokens_[p_];
        if (p_ < tokens_.size() - 1)
        {
            p_++;
        }
        return curr;
    }

    // TOFIX: return reference
    Token match(TokenTag tag)
    {
        if (current().tag_ == tag)
        {
            return next();
        }
        std::stringstream err;
        err << "Error: Unexpected token (" << current() << ") at (" << current().line_count_ << ", "
            << current().char_count_ << "), expected <" << tag << "> type";
        diagnostics_.push_back(err.str());
        // return bad token if no match
        return Token(TokenTag::bad, current().line_count_, current().char_count_);
    }

    std::shared_ptr<SyntaxNode> parse_primary_expression(int order = 0)
    {
        TokenTag curr_tag = current().tag_;
        if (curr_tag != TokenTag::val_double && curr_tag != TokenTag::val_int)
        {
            std::stringstream err;
            err << "Error: Unexpected token (" << current() << ") at (" << current().line_count_ << ", "
                << current().char_count_ << "), expected <" << TokenTag::val_double << "> or <" << TokenTag::val_int << "> type";
            diagnostics_.push_back(err.str());
        }
        Token tok = match(curr_tag);
        return std::make_shared<Expression>(tok);
    }

    std::shared_ptr<SyntaxNode> parse_expression(int order = 0)
    {
        auto left = parse_primary_expression();
        while (true)
        {
            int precedence = get_binary_operator_precedence(current());
            if (precedence == 0 || precedence <= order)
                break;
            // Current token is a binary operator
            Token &op = current();
            next();
            auto right = parse_expression(precedence);
            left = std::make_unique<BinaryExpression>(left, op, right);
        }
        return left;
    }

public:
    std::shared_ptr<SyntaxNode> parse(std::vector<Token> &&tokens)
    {
        // Reset state
        tokens_ = std::move(tokens);
        p_ = 0;
        diagnostics_.clear();

        auto ast = parse_expression();

        match(TokenTag::eof);
        return ast;
    }

    std::vector<std::string> &get_diagnostics()
    {
        return diagnostics_;
    }

private:
    std::vector<Token> tokens_;
    size_t p_;
    std::vector<std::string> diagnostics_;
};
