#pragma once
#include "parser.hpp"

class Evaluator
{
public:
    double evaluate_expression(std::shared_ptr<SyntaxNode> root)
    {
        if (root->tag_ == SyntaxTag::expression)
        {
            auto r = std::static_pointer_cast<Expression>(root);
            return std::stod(r->tok_.val_);
        }
        else if (root->tag_ == SyntaxTag::binary_expression)
        {
            auto r = std::static_pointer_cast<BinaryExpression>(root);
            auto left = evaluate_expression(r->left_);
            auto right = evaluate_expression(r->right_);

            auto &op = r->tok_;
            if (op.tag_ == TokenTag::plus)
            {
                return left + right;
            }
            else if (op.tag_ == TokenTag::minus)
            {
                return left - right;
            }
            else if (op.tag_ == TokenTag::star)
            {
                return left * right;
            }
            else if (op.tag_ == TokenTag::slash)
            {
                return left / right; // YOLO
            }
            else
            {
                // unreachable
                std::stringstream ss;
                std::cout << "Evaluator error: invalid binary op token tag " << op.tag_ << std::endl;
                throw "Evaluator error: invalid binary op token tag node tag";
            }
        }
        else if (root->tag_ == SyntaxTag::parenthesized_expression)
        {
            auto r = std::static_pointer_cast<ParenthesizedExpression>(root);
            return evaluate_expression(r->expr_);
        }
        else if (root->tag_ == SyntaxTag::unary_expression)
        {
            auto r = std::static_pointer_cast<UnaryExpression>(root);
            auto &op = r->tok_;
            if (op.tag_ == TokenTag::minus)
            {
                return -evaluate_expression(r->expr_);
            }
            else if (op.tag_ == TokenTag::plus)
            {
                return +evaluate_expression(r->expr_);
            }
            else
            {
                // unreachable
                std::cout << "Evaluator error: invalid unary op token tag " << op.tag_ << std::endl;
                throw "Evaluator error: invalid unary op token tag node tag";
            }
        }
        else
        {
            // unreachable
            std::cout << "Evaluator error: invalid syntax node tag " << root->tok_ << std::endl;
            throw "Evaluator error: invalid syntax node tag";
        }
    }
};
