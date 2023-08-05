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
            if (op.val_ == "+")
            {
                return left + right;
            }
            if (op.val_ == "-")
            {
                return left - right;
            }
            if (op.val_ == "*")
            {
                return left * right;
            }
            if (op.val_ == "/")
            {
                return left / right; // YOLO
            }
        }
    }
};
