#pragma once

#include "parser.hpp"

#include <string>
#include <vector>
#include <cassert>
// #include <set>

enum class BoundExpressionTag
{
    identifier,
    boolean,

    integer,
    floating,
    unary,
    binary,
    parenthesized
};

enum class Type
{
    // bad, // Indicates error (ie unsupported operation)
    boolean,
    integer,
    floating,
};

// Print support for types
std::ostream &operator<<(std::ostream &os, Type type)
{
#define TOKEN_TAG_CASE(tag) \
    case Type::tag:         \
        return os << #tag;

    switch (type)
    {
        TOKEN_TAG_CASE(boolean)
        TOKEN_TAG_CASE(integer)
        TOKEN_TAG_CASE(floating)
    };
    return os;
#undef TOKEN_TAG_CASE
}

enum class BoundUnaryOperatorTag
{
    identity,
    negation,
};

enum class BoundBinaryOperatorTag
{
    addition,
    subtraction,
    multiplication,
    division,
    equal,
    not_equal,
        and,
    or
    ,
    greater_than,
    less_than
};

// class OperatorTypeSupport
// {
//     using unary_pair_type = std::pair<BoundUnaryOperatorTag, Type>;
//     using binary_tuple_type = std::tuple<Type, BoundBinaryOperatorTag, Type>;

// public:
//     void add_unary(Type type, BoundUnaryOperatorTag op)
//     {
//         unary_set_.insert({op, type});
//     }

//     bool add_binary(Type t1, Type t2, BoundBinaryOperatorTag op)
//     {
//         binary_set_.insert({t1, op, t2});
//     }

//     bool is_valid_unary(Type type, BoundUnaryOperatorTag op)
//     {
//         return unary_set_.find({op, type}) != unary_set_.end();
//     }

//     bool is_valid_binary(Type t1, Type t2, BoundBinaryOperatorTag op)
//     {
//         return (binary_set_.find({t1, op, t2}) != binary_set_.end() ||
//                 binary_set_.find({t2, op, t1}) != binary_set_.end());
//     }

// private:
//     std::set<unary_pair_type> unary_set_;
//     std::set<binary_tuple_type> binary_set_;
// };

struct BoundNode
{
    BoundNode(BoundExpressionTag tag, Type type) : tag_(tag), type_(type) {}
    BoundExpressionTag tag_;
    Type type_;
};

struct BoundIntegerExpression : public BoundNode
{
    BoundIntegerExpression(Type type, std::string value)
        : BoundNode(BoundExpressionTag::integer, type), value_(value)
    {
    }

    std::string value_;
};
struct BoundFloatingExpression : public BoundNode
{
    BoundFloatingExpression(Type type, std::string value)
        : BoundNode(BoundExpressionTag::floating, type), value_(value)
    {
    }

    std::string value_;
};
struct BoundBooleanExpression : public BoundNode
{
    BoundBooleanExpression(Type type, std::string value)
        : BoundNode(BoundExpressionTag::boolean, type), value_(value)
    {
    }

    std::string value_;
};
struct BoundUnaryExpression : public BoundNode
{
    BoundUnaryExpression(Type type, BoundUnaryOperatorTag operation, std::shared_ptr<BoundNode> expr)
        : BoundNode(BoundExpressionTag::unary, type), tag_(operation), expr_(expr)
    {
    }

    BoundUnaryOperatorTag tag_;
    std::shared_ptr<BoundNode> expr_;
};
struct BoundBinaryExpression : public BoundNode
{
    BoundBinaryExpression(Type type, std::shared_ptr<BoundNode> left, BoundBinaryOperatorTag operation, std::shared_ptr<BoundNode> right)
        : BoundNode(BoundExpressionTag::binary, type), left_(left), tag_(operation), right_(right)
    {
    }
    BoundBinaryOperatorTag tag_;
    std::shared_ptr<BoundNode> left_;
    std::shared_ptr<BoundNode> right_;
};

class Binder
{
public:
    // Binder()
    // {
    //     // Add supported types
    //     operator_type_support.add_unary(Type::integer, BoundUnaryOperatorTag::identity);
    //     operator_type_support.add_unary(Type::integer, BoundUnaryOperatorTag::negation);
    //     operator_type_support.add_binary(Type::integer, Type::integer, BoundBinaryOperatorTag::addition);
    //     operator_type_support.add_binary(Type::integer, Type::integer, BoundBinaryOperatorTag::subtraction);
    //     operator_type_support.add_binary(Type::integer, Type::integer, BoundBinaryOperatorTag::multiplication);
    //     operator_type_support.add_binary(Type::integer, Type::integer, BoundBinaryOperatorTag::division);

    //     operator_type_support.add_unary(Type::floating, BoundUnaryOperatorTag::identity);
    //     operator_type_support.add_unary(Type::floating, BoundUnaryOperatorTag::negation);
    //     operator_type_support.add_binary(Type::floating, Type::floating, BoundBinaryOperatorTag::addition);
    //     operator_type_support.add_binary(Type::floating, Type::floating, BoundBinaryOperatorTag::subtraction);
    //     operator_type_support.add_binary(Type::floating, Type::floating, BoundBinaryOperatorTag::multiplication);
    //     operator_type_support.add_binary(Type::floating, Type::floating, BoundBinaryOperatorTag::division);
    // }

    std::shared_ptr<BoundNode> bind(std::shared_ptr<SyntaxNode> root)
    {
        // Reset state
        diagnostics_.clear();
        return bind_expression(root);
    }

private:
    std::shared_ptr<BoundNode> bind_expression(std::shared_ptr<SyntaxNode> root)
    {
        SyntaxTag tag = root->tag_;

        switch (tag)
        {
        case SyntaxTag::integer_expression:
            return bind_integer(root);
        case SyntaxTag::floating_expression:
            return bind_floating(root);
        case SyntaxTag::boolean_expression:
            return bind_boolean(root);
        case SyntaxTag::unary_expression:
            return bind_unary(root);
        case SyntaxTag::binary_expression:
            return bind_binary(root);
        case SyntaxTag::parenthesized_expression:
            return bind_parenthesis(root);
        default:
            std::cout << "Unreachable" << std::endl;
            throw "Unreachable";
        }
    }

    std::shared_ptr<BoundNode> bind_integer(std::shared_ptr<SyntaxNode> node)
    {
        assert(node->tag_ == SyntaxTag::integer_expression);
        auto p = std::static_pointer_cast<IntegerExpression>(node);
        return std::make_shared<BoundIntegerExpression>(Type::integer, p->tok_.val_);
    }

    std::shared_ptr<BoundNode> bind_floating(std::shared_ptr<SyntaxNode> node)
    {
        assert(node->tag_ == SyntaxTag::floating_expression);
        auto p = std::static_pointer_cast<FloatingExpression>(node);
        return std::make_shared<BoundFloatingExpression>(Type::floating, p->tok_.val_);
    }

    std::shared_ptr<BoundNode> bind_boolean(std::shared_ptr<SyntaxNode> node)
    {
        assert(node->tag_ == SyntaxTag::boolean_expression);
        auto p = std::static_pointer_cast<BooleanExpression>(node);
        return std::make_shared<BoundBooleanExpression>(Type::boolean, p->tok_.val_);
    }

    std::shared_ptr<BoundNode> bind_unary(std::shared_ptr<SyntaxNode> node)
    {
        assert(node->tag_ == SyntaxTag::unary_expression);
        auto p = std::static_pointer_cast<UnaryExpression>(node);
        auto expr = bind_expression(p->expr_);
        BoundUnaryOperatorTag op;
        if (expr->type_ == Type::floating || expr->type_ == Type::integer)
        {
            if (p->tok_.tag_ == TokenTag::plus)
                op = BoundUnaryOperatorTag::identity;
            else if (p->tok_.tag_ == TokenTag::minus)
                op = BoundUnaryOperatorTag::negation;
        }
        else if (expr->type_ == Type::boolean)
        {
            if (p->tok_.tag_ == TokenTag::bang)
                op = BoundUnaryOperatorTag::negation;
        }
        else
        {
            std::stringstream err;
            err << "Error: Can't use unary operator " << p->tok_ << " on type '" << expr->type_ << "'";
            diagnostics_.push_back(err.str());
        }
        return std::make_shared<BoundUnaryExpression>(expr->type_, op, expr);
    }

    std::shared_ptr<BoundNode> bind_binary(std::shared_ptr<SyntaxNode> node)
    {
        assert(node->tag_ == SyntaxTag::binary_expression);
        auto p = std::static_pointer_cast<BinaryExpression>(node);

        auto left = bind_expression(p->left_);
        auto right = bind_expression(p->right_);

        BoundBinaryOperatorTag tag;
        bool err_flag = false;
        Type return_type = left->type_;
        if (left->type_ == right->type_)
        {
            if ((left->type_ == Type::integer) || (left->type_ == Type::floating))
            {
                if (p->tok_.tag_ == TokenTag::plus)
                {
                    tag = BoundBinaryOperatorTag::addition;
                }
                else if (p->tok_.tag_ == TokenTag::minus)
                {
                    tag = BoundBinaryOperatorTag::subtraction;
                }
                else if (p->tok_.tag_ == TokenTag::star)
                {
                    tag = BoundBinaryOperatorTag::multiplication;
                }
                else if (p->tok_.tag_ == TokenTag::slash)
                {
                    tag = BoundBinaryOperatorTag::division;
                }
                else if (p->tok_.tag_ == TokenTag::greater_than)
                {
                    tag = BoundBinaryOperatorTag::greater_than;
                    return_type = Type::boolean;
                }
                else if (p->tok_.tag_ == TokenTag::less_than)
                {
                    tag = BoundBinaryOperatorTag::less_than;
                    return_type = Type::boolean;
                }
                else if (p->tok_.tag_ == TokenTag::equal)
                {
                    tag = BoundBinaryOperatorTag::equal;
                    return_type = Type::boolean;
                }
                else if (p->tok_.tag_ == TokenTag::not_equal)
                {
                    tag = BoundBinaryOperatorTag::not_equal;
                    return_type = Type::boolean;
                }
                else
                {
                    err_flag = true;
                }
            }
            else if (left->type_ == Type::boolean)
            {
                if (p->tok_.tag_ == TokenTag::equal)
                    tag = BoundBinaryOperatorTag::equal;
                else if (p->tok_.tag_ == TokenTag::not_equal)
                    tag = BoundBinaryOperatorTag::not_equal;
                else if (p->tok_.tag_ == TokenTag::double_ampersand)
                    tag = BoundBinaryOperatorTag::and;
                else if (p->tok_.tag_ == TokenTag::double_vertical)
                    tag = BoundBinaryOperatorTag:: or ;
                else
                {
                    err_flag = true;
                }
            }
            else
            {
                err_flag = true;
            }
        }
        else
        {
            err_flag = true;
        }
        if (err_flag)
        {
            std::stringstream err;
            err << "Error: Can't use operator " << p->tok_ << " on types '" << left->type_ << "' and '" << right->type_ << "'";
            diagnostics_.push_back(err.str());
        }
        return std::make_shared<BoundBinaryExpression>(return_type, left, tag, right);
    }

    std::shared_ptr<BoundNode>
    bind_parenthesis(std::shared_ptr<SyntaxNode> node)
    {
        assert(node->tag_ == SyntaxTag::parenthesized_expression);
        auto p = std::static_pointer_cast<ParenthesizedExpression>(node);
        return bind_expression(p->expr_);
    }

public:
    std::vector<std::string> &get_diagnostics()
    {
        return diagnostics_;
    }

private:
    std::vector<std::string> diagnostics_;

    // OperatorTypeSupport operator_type_support;
};