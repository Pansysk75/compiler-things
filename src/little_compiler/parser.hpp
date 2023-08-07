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

    Token match(const std::vector<TokenTag> &tags)
    {
        if (std::find(tags.begin(), tags.end(), current().tag_) != tags.end())
        {
            return next();
        }
        std::stringstream err;
        err << "Error: Unexpected token (" << current() << ") at (" << current().line_count_ << ", "
            << current().char_count_ << "), expected primary type";
        diagnostics_.push_back(err.str());
        // return bad token if no match
        return Token(TokenTag::bad, current().line_count_, current().char_count_);
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

        if (current().tag_ == TokenTag::parenthesis_open)
        {
            Token open = match(TokenTag::parenthesis_open);
            auto expr = parse_expression(0);
            Token close = match(TokenTag::parenthesis_close);
            return std::make_shared<ParenthesizedExpression>(open, expr, close);
        }

        const std::vector<TokenTag> primary_expr_token_tags = {TokenTag::val_double, TokenTag::val_int, TokenTag::id};
        Token tok = match(primary_expr_token_tags);

        if (tok.tag_ == TokenTag::val_double)
            return std::make_shared<FloatingExpression>(tok);
        else if (tok.tag_ == TokenTag::val_int)
            return std::make_shared<IntegerExpression>(tok);
        else if (tok.tag_ == TokenTag::id && (tok.val_ == "true" || tok.val_ == "false"))
        {
            return std::make_shared<BooleanExpression>(tok);
        }
        else if (tok.tag_ == TokenTag::bad)
        {
            return std::make_shared<IntegerExpression>(tok); // Unkown tokens are filled in as ints
        }
        std::cout << "Unreachable" << std::endl;
        throw "Unreachable";
    }

    std::shared_ptr<SyntaxNode> parse_expression(int order = 0)
    {
        std::shared_ptr<SyntaxNode> left;

        // Handle unary operators
        int precedence = current().get_unary_operator_precedence();
        if (precedence != 0 && precedence >= order)
        {
            // Current token is a unary operator
            Token op = next();
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
            Token op = current();
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

        auto parse_tree = parse_expression();

        match(TokenTag::eof);
        return parse_tree;
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
