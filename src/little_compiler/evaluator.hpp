#pragma once
#include "parser.hpp"
#include "binder.hpp"

class Evaluator
{
public:
    double evaluate_expression(std::shared_ptr<BoundNode> root)
    {
        if (root->tag_ == BoundExpressionTag::integer)
        {
            auto r = std::static_pointer_cast<BoundIntegerExpression>(root);
            return std::stoi(r->value_);
        }
        else if (root->tag_ == BoundExpressionTag::floating)
        {
            auto r = std::static_pointer_cast<BoundFloatingExpression>(root);
            return std::stod(r->value_);
        }
        else if (root->tag_ == BoundExpressionTag::binary)
        {
            auto r = std::static_pointer_cast<BoundBinaryExpression>(root);
            auto left = evaluate_expression(r->left_);
            auto right = evaluate_expression(r->right_);

            if (r->tag_ == BoundBinaryOperatorTag::addition)
            {
                return left + right;
            }
            else if (r->tag_ == BoundBinaryOperatorTag::subtraction)
            {
                return left - right;
            }
            else if (r->tag_ == BoundBinaryOperatorTag::multiplication)
            {
                return left * right;
            }
            else if (r->tag_ == BoundBinaryOperatorTag::division)
            {
                return left / right; // YOLO
            }
            else
            {
                // unreachable
                std::stringstream ss;
                std::cout << "Evaluator error: invalid binary op tag" << std::endl;
                throw "Evaluator error: invalid binary op tag";
            }
        }
        else if (root->tag_ == BoundExpressionTag::unary)
        {
            auto r = std::static_pointer_cast<BoundUnaryExpression>(root);
            if (r->tag_ == BoundUnaryOperatorTag::negation)
            {
                return -evaluate_expression(r->expr_);
            }
            else if (r->tag_ == BoundUnaryOperatorTag::identity)
            {
                return +evaluate_expression(r->expr_);
            }
            else
            {
                // unreachable
                std::cout << "Evaluator error: invalid unary op tag " << (int)r->tag_ << std::endl;
                throw "Evaluator error: invalid unary op token tag";
            }
        }
        else
        {
            // unreachable
            std::cout << "Evaluator error: invalid syntax node tag " << std::endl;
            throw "Evaluator error: invalid syntax node tag";
        }
    }
};
