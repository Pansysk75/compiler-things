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

struct ParenthesizedExpression : SyntaxNode
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
        err << "Error: Unexpected token (" << current() << ") at (" << current().line_count_ << ", "
            << current().char_count_ << "), expected <" << tag << "> type";
        diagnostics_.push_back(err.str());
        // return bad token if no match
        return Token(TokenTag::bad, current().line_count_, current().char_count_);
    }

    std::shared_ptr<SyntaxNode> do_parse(int order = 0)
    {
        if (order == 0)
        {
            // Parse weaker binary operators (+, -)
            auto left = do_parse(order + 1);
            while (current().tag_ == TokenTag::plus || current().tag_ == TokenTag::minus)
            {
                Token &op = current();
                next();
                auto right = do_parse(order + 1);
                left = std::make_unique<BinaryExpression>(left, op, right);
            }
            return left;
        }
        else if (order == 1)
        {
            // Parse strong binary operators (*, /)
            auto left = do_parse(order + 1);
            while (current().tag_ == TokenTag::star || current().tag_ == TokenTag::slash)
            {
                Token &op = current();
                next();
                auto right = do_parse(order + 1);
                ;

                left = std::make_unique<BinaryExpression>(left, op, right);
            }
            return left;
        }

        else if (order == 2)
        {
            // Parse parenthesis
            if (current().tag_ == TokenTag::parenthesis_open)
            {
                Token open = match(TokenTag::parenthesis_open);
                auto expr = do_parse(0);
                Token close = match(TokenTag::parenthesis_close);
                return std::make_shared<ParenthesizedExpression>(open, expr, close);
            }
            // Parse primary expressions (values)
            TokenTag curr_tag = current().tag_;
            if (curr_tag != TokenTag::val_double && curr_tag != TokenTag::val_int)
            {
                std::cout << "Parser: Unexpected token" << std::endl;
                throw "Parser: Unexpected token";
            }
            Token tok = match(curr_tag);
            return std::make_shared<Expression>(tok);
        }
        else
        {
            std::cout << "Parser error: invalid operator order value" << std::endl;
            throw "Parser error: invalid operator order value";
        }
    }

public:
    std::shared_ptr<SyntaxNode> parse(std::vector<Token> &&tokens)
    {
        tokens_ = std::move(tokens);
        p_ = 0;

        auto ast = do_parse();
        match(TokenTag::eof);
        return ast;
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
