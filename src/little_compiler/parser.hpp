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
        return tokens_[p_++];
    }

    // TOFIX: return reference
    Token match(TokenTag tag)
    {
        if (current().tag_ == tag)
        {
            return next();
        }
        std::stringstream err;
        err << "Error: unexpected token (" << current() << ") at (" << current().line_count_ << ", "
            << current().char_count_ << "), expected <" << tag << "> type";
        diagnostics_.emplace_back(err.str());
        // return bad token if no match
        return Token(TokenTag::bad, current().line_count_, current().char_count_);
    }

    std::unique_ptr<SyntaxNode> parse_primary_expression()
    {
        Token tok = match(TokenTag::val);
        return std::make_unique<Expression>(tok);
    }

public:
    std::unique_ptr<SyntaxNode> parse(std::vector<Token> &&tokens)
    {
        tokens_ = std::move(tokens);
        p_ = 0;

        auto left = parse_primary_expression();
        while (current().tag_ == TokenTag::binary_op)
        {

            Token &op = current();
            next();
            auto right = parse_primary_expression();

            left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
        }
        return left;
    }

    void print_diagnostics(std::ostream &out)
    {
        if (!diagnostics_.empty())
        {
            out << "Parser diagnostics:" << std::endl;
            for (auto &msg : diagnostics_)
            {
                out << msg << std::endl;
            }
            diagnostics_.clear();
        }
    }

private:
    std::vector<Token> tokens_;
    size_t p_;
    std::vector<std::string> diagnostics_;
};
