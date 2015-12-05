//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef CSVFILTER_LEXTOKEN_H
#define CSVFILTER_LEXTOKEN_H

#include "parseError.h"

#include <string>
#include <ostream>
#include <memory>

/**
 * @brief  A lexicographical token.
 *
 * This class represents a single lexicographical token produced by the Lexer.
 *
 * @see Lexer
 *
 *
 */
class LexToken {
public:

    /**
     * @brief The token type.
     *
     * The type of the token.
     *
     */
    typedef enum {
        TYPE_PLUS, /**< '+' */
        TYPE_MINUS, /**< '-' */
        TYPE_TIMES, /**< '*' */
        TYPE_DIVIDE, /**< '/' */
        TYPE_LT, /**< '<' */
        TYPE_LTE, /**< '<=' */
        TYPE_EQ, /**< '==' */
        TYPE_NEQ, /**< '!=' */
        TYPE_GT, /**< '>' */
        TYPE_GTE, /**< '>=' */
        TYPE_AND, /**< '&&' */
        TYPE_OR, /**< '||' */
        TYPE_OPEN_BRACKET, /**< '(' */
        TYPE_CLOSE_BRACKET,  /**< ')' */
        TYPE_IDENTIFIER, /**< An identifier, for example a variable name */
        TYPE_STRING, /**< A string constant - this will have been surrounded
                      * with double quotes. */
        TYPE_NUMBER, /**< A numeric constant. We do not differentiate between
                      * ints and doubles, and it will be positive - negative
                      * numbers will appear as a minus token then a number
                      * token */
        TYPE_EOF /**< The end of the token stream */
    } Type;

    LexToken(Type type, Range position, const std::string& value);
    ~LexToken();

    Type type() const;
    Range position() const;
    const std::string& value() const;
    double numberValue() const;

    bool isOperator() const;
    int operatorPrecedence() const;

    bool isUnary() const;
    void setUnary(bool newUnary);
private:
    LexToken(const LexToken& other);
    LexToken& operator=(const LexToken& other);

    Type type_;
    Range position_;
    const std::string value_;
    bool unary_;
};


std::ostream& operator<< (std::ostream &out, LexToken::Type t);

typedef std::shared_ptr<LexToken> LexTokenRef;
typedef std::shared_ptr<const LexToken> ConstLexTokenRef;

#endif // CSVFILTER_LEXTOKEN_H

