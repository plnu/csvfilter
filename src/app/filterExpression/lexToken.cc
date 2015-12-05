//
// csvfilter, Copyright (c) 2015, plnu
//

#include "lexToken.h"

#include <assert.h>
#include <stdlib.h>
#include <iostream>

/**
 * @brief Constructor
 *
 * Create a new LexToken.
 *
 * @param type      The type of the token
 * @param position  The position of the token in the expression
 * @param value     The string value of the token
 *
 */
LexToken::LexToken(Type type, Range position, const std::string& value)
    :type_(type), position_(position), value_(value), unary_(false) {

}

/**
 * @brief Destructor
 *
 * Destructor
 *
 */
LexToken::~LexToken() {

}

/**
 * @brief The type of the LexToken
 *
 * @return  The type of this token
 *
 */
LexToken::Type LexToken::type() const {
    return type_;
}

/**
 * @brief The position of this token.
 *
 * @return The position of this token in the expression
 *
 */
Range LexToken::position() const {
    return position_;
}

/**
 * @brief The string value of this token
 *
 * @return  The string value of this token
 *
 */
const std::string& LexToken::value() const {
    return value_;
}

/**
 * @brief The numeric value of this token.
 *
 * Return the numeric value of this token. This can only be called if the type
 * of this token is TYPE_NUMBER.
 *
 * @return  The numeric value of this token.
 */
double LexToken::numberValue() const {
    assert(type_ == TYPE_NUMBER);
    return atof(value_.c_str());
}

/**
 * @brief Is this token an operator?
 *
 * Checks whether this operator is a token. For these purposes an operator is
 * anything that isn't an identifier, string or number, so brackets count as
 * operators, although they aren't strictly treated as operators by the
 * expression parser.
 *
 * @return  true if the token is an operator, false otherwise.
 *
 */
bool LexToken::isOperator() const {
    return (type_ != TYPE_IDENTIFIER &&
            type_ != TYPE_STRING &&
            type_ != TYPE_NUMBER);
}

/**
 * @brief The precedence of the operator
 *
 * Return the precedence of this operator. The higher the return value the
 * higher precedence this operator has when parsing an expression.
 *
 * This function can only be called on tokens that are operators.
 *
 * This precedence order is the same as that implemented by C.
 *
 * @return  The operator precedence.
 *
 */
int LexToken::operatorPrecedence() const {
    assert(isOperator());

    int precedence = 0;
    switch (type_) {
    case TYPE_TIMES:
    case TYPE_DIVIDE:
        precedence = 7;
        break;
    case TYPE_MINUS:
        if (unary_) {
            precedence = 8;
        } else {
            precedence = 6;
        }
        break;
    case TYPE_PLUS:
        precedence = 6;
        break;
    case TYPE_LT:
    case TYPE_LTE:
    case TYPE_GT:
    case TYPE_GTE:
        precedence = 5;
        break;
    case TYPE_EQ:
    case TYPE_NEQ:
        precedence = 4;
        break;
    case TYPE_AND:
        precedence = 3;
        break;
    case TYPE_OR:
        precedence = 2;
        break;
    case TYPE_EOF:
        precedence = 1;
        break;
    case TYPE_OPEN_BRACKET:
        precedence = 0;
        break;
    default:
        std::cerr << "No precedence configured for operator" << std::endl;
        abort();
        break;
    }

    return precedence;
}

/**
 * @brief  Is this a unary operator?
 *
 * Is this operator a unary operator? Currently the only unary operator is '-',
 * which can be either binary (as in "1 - 2") or unary (as in "-5").
 *
 * @see LexToken::setUnary
 *
 * @return  true if this token is a unary operator, or false otherwise
 *
 */
bool LexToken::isUnary() const {
    return unary_;
}


/**
 * @brief  Set whether this token is a unary operator?
 *
 * Set whether this token is a unary operator. This can only be done on a '-'.
 *
 * @see LexToken::isUnary
 *
 * @param newUnary  true if this token is a unary operator, or false otherwise
 *
 */
void LexToken::setUnary(bool newUnary) {
    assert(type_ == TYPE_MINUS);
    unary_ = newUnary;
}


/**
 *
 * @brief Streaming operator for LexToken::Type
 *
 * Streams a LexToken::Type.
 *
 * @param out  The stream to write to
 * @param t    The token to write
 *
 * @return  The stream that was written to
 *
 */
std::ostream& operator<< (std::ostream &out, LexToken::Type t)
{
    const char* label = "UNKNOWN";
    switch (t) {
    case LexToken::TYPE_PLUS:
        label = "TYPE_PLUS";
        break;
    case LexToken::TYPE_MINUS:
        label = "TYPE_MINUS";
        break;
    case LexToken::TYPE_TIMES:
        label = "TYPE_TIMES";
        break;
    case LexToken::TYPE_DIVIDE:
        label = "TYPE_DIVIDE";
        break;
    case LexToken::TYPE_LT:
        label = "TYPE_LT";
        break;
    case LexToken::TYPE_LTE:
        label = "TYPE_LTE";
        break;
    case LexToken::TYPE_EQ:
        label = "TYPE_EQ";
        break;
    case LexToken::TYPE_NEQ:
        label = "TYPE_NEQ";
        break;
    case LexToken::TYPE_GT:
        label = "TYPE_GTE";
        break;
    case LexToken::TYPE_GTE:
        label = "TYPE_GTE";
        break;
    case LexToken::TYPE_AND:
        label = "TYPE_AND";
        break;
    case LexToken::TYPE_OR:
        label = "TYPE_OR";
        break;
    case LexToken::TYPE_EOF:
        label = "TYPE_EOF";
        break;
    case LexToken::TYPE_OPEN_BRACKET:
        label = "TYPE_OPEN_BRACKET";
        break;
    case LexToken::TYPE_CLOSE_BRACKET:
        label = "TYPE_CLOSE_BRACKET";
        break;
    case LexToken::TYPE_IDENTIFIER:
        label = "TYPE_IDENTIFIER";
        break;
    case LexToken::TYPE_STRING:
        label = "TYPE_STRING";
        break;
    case LexToken::TYPE_NUMBER:
        label = "TYPE_NUMBER";
        break;
    }
    out << label;

    return out;
}


