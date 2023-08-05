#pragma once
// #include "lexer.hpp"
#include "syntax_elements.hpp"

#include <string>
#include <vector>
#include <sstream>

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
        std::shared_ptr<SyntaxNode> left;

        // Handle unary operators
        int precedence = current().get_unary_operator_precedence();
        if (precedence != 0 && precedence >= order)
        {
            // Current token is a unary operator
            Token &op = next();
            auto expr = parse_expression(precedence);
            left = std::make_shared<UnaryExpression>(op, expr);
        }
        else
        {
            left = parse_primary_expression();
        }

        while (true)
        {
            // Handle binary operators
            int precedence = current().get_binary_operator_precedence();
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
