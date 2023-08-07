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
        else if (root->tag_ == BoundExpressionTag::boolean)
        {
            auto r = std::static_pointer_cast<BoundBooleanExpression>(root);
            return r->value_ == "true";
        }
        else if (root->tag_ == BoundExpressionTag::binary)
        {
            auto r = std::static_pointer_cast<BoundBinaryExpression>(root);
            auto left = evaluate_expression(r->left_);
            auto right = evaluate_expression(r->right_);

            switch (r->tag_)
            {
            case BoundBinaryOperatorTag::addition:
                return left + right;
            case BoundBinaryOperatorTag::subtraction:
                return left - right;
            case BoundBinaryOperatorTag::multiplication:
                return left * right;
            case BoundBinaryOperatorTag::division:
                return left / right; // YOLO
            case BoundBinaryOperatorTag::equal:
                return left == right ? 1 : 0;
            case BoundBinaryOperatorTag::not_equal:
                return left != right ? 1 : 0;
            case BoundBinaryOperatorTag::greater_than:
                return left > right ? 1 : 0;
            case BoundBinaryOperatorTag::less_than:
                return left < right ? 1 : 0;
            case BoundBinaryOperatorTag::and:
                return ((bool)left && (bool)right) ? 1 : 0;
            case BoundBinaryOperatorTag:: or:
                return ((bool)left || (bool)right) ? 1 : 0;
            }

            // unreachable
            std::stringstream ss;
            std::cout << "Evaluator error: invalid binary op tag" << std::endl;
            throw "Evaluator error: invalid binary op tag";
        }
        else if (root->tag_ == BoundExpressionTag::unary)
        {
            auto r = std::static_pointer_cast<BoundUnaryExpression>(root);
            switch (r->tag_)
            {
            case BoundUnaryOperatorTag::negation:
                if (r->type_ == Type::boolean)
                    return evaluate_expression(r->expr_) > 0 ? 0 : 1;
                else
                    return -evaluate_expression(r->expr_);

            case BoundUnaryOperatorTag::identity:
                return evaluate_expression(r->expr_);
            }
            // unreachable
            std::cout << "Evaluator error: invalid unary op tag " << (int)r->tag_ << std::endl;
            throw "Evaluator error: invalid unary op token tag";
        }
        else
        {
            // unreachable
            std::cout << "Evaluator error: invalid syntax node tag " << std::endl;
            throw "Evaluator error: invalid syntax node tag";
        }
    }
};
