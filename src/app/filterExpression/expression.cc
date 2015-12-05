//
// csvfilter, Copyright (c) 2015, plnu
//

#include "expression.h"
#include "lexer.h"

#include <assert.h>
#include <iostream>

/**
 * @brief Constructor
 *
 * Constructor. The expression is parsed as part of the constructor. After
 * creating an Expression, use Expression::ok to check whether that
 * parsing was successful.
 *
 * @param expression  The filter expression to parse.
 * @param headers     The headers from the csv file. These are used to confirm
 *                    that any headers referenced in the expression actually
 *                    exist.
 *
 */
Expression::Expression(const std::string& expression,
                       const Headers& headers)
    :ok_(true),
     error_(),
     tree_(nullptr),
     resultType_(ParseTree::NODE_TYPE_UNKNOWN) {

    ParseState state(expression, headers);
    if (!state.lexer_.ok()) {
        ok_ = false;
        error_ = state.lexer_.err();
    } else {
        makeParseTree(state);
    }
}

/**
 * @brief Destructor.
 *
 * Destructor.
 *
 */
Expression::~Expression() {

}

/**
 * @brief  Did the expression parse?
 *
 * Did the expression parse correctly? if not, then check
 * Expression::errText for a descriptive error message.
 *
 * @see Expression::errText
 *
 * @return  true if the expression parsed correctly, false otherwise.
 *
 */
bool Expression::ok() const {
    return ok_;
}

/**
 * @brief An error description.
 *
 * If the expression did not parse (Expression::ok returns false) then
 * errText will return a description of the error.
 *
 * @return A description of the error.
 *
 */
const ParseError Expression::error() const {
    return error_;
}

/**
 * @brief  Evaluate the expression.
 *
 * Evaluate the expression, using the current row as context.
 *
 * @param l  The current line
 *
 * @return  Either the result, or a variant containing an error if the
 *          expression could not be parsed.
 *
 */
VariantRef Expression::eval(const LineParser& l) {
    assert(ok_);
    return tree_->eval(l, ParseTree::NODE_TYPE_UNKNOWN);
}

/**
 * @brief  A string representation of the parse tree
 *
 * A string representation of the parse tree. This is purely for debugging
 * purposes.
 *
 * @return A string representation of the parse tree.
 *
 */
const std::string Expression::treeString() const {
    std::string ret;
    if (tree_ != nullptr) {
        ret = tree_->toString();
    }
    return ret;
}

void Expression::makeParseTree(ParseState& state) {
    while (ok_ && !state.done_) {
        if (state.expectedToken_ == EXPECT_OPERATOR) {
            processOperator(state);
        } else {
            processOperand(state);
        }
    }

    if (state.done_ && ok_) {
        assert(state.operators_.empty());
        assert(state.operands_.size() == 1);
        tree_ = state.operands_.top();

        resultType_ = tree_->validateTypes(error_);
        if (resultType_ == ParseTree::NODE_TYPE_ERROR) {
            ok_ = false;
        }
    }
}


void Expression::processOperator(ParseState& state) {
    ConstLexTokenRef token = state.lexer_.pop();
    if (!token->isOperator()) {
        error_ = ParseError("Unexpected operand", token->position());
        ok_ = false;
    } else {
        if (token->type() == LexToken::TYPE_CLOSE_BRACKET) {
            endBracketedExpression(token, state);
        } else {
            while (!(state.operators_.empty()) &&
                   token->operatorPrecedence() <=
                   state.operators_.top()->operatorPrecedence()) {
                applyLastOperator(state);
            }
            if (token->type() == LexToken::TYPE_EOF) {
                if (!state.operators_.empty()) {
                    ConstLexTokenRef remainingToken = state.operators_.top();
                    assert(remainingToken->type() ==
                           LexToken::TYPE_OPEN_BRACKET);
                    ok_ = false;
                    error_ = ParseError("Unmatched bracket",
                                        remainingToken->position());
                }
                state.done_ = true;
            } else {
                state.operators_.push(token);
                state.expectedToken_ = EXPECT_OPERAND;
            }
        }
    }
}


void Expression::processOperand(ParseState& state) {
    LexTokenRef token = state.lexer_.pop();
    if (!token->isOperator()) {
        if (token->type() == LexToken::TYPE_IDENTIFIER) {
            int position = state.headers_.indexOf(token->value());
            if (position < 0) {
                std::stringstream msg;
                msg << "Identifier \"" << token->value()
                         << "\" not found in headers";
                error_ = ParseError(msg.str(), token->position());
                ok_ = false;
            } else {
                state.operands_.push(
                    ParseTree::makeOperand(token, position));
            }
        } else {
            state.operands_.push(ParseTree::makeOperand(token));
        }
        state.expectedToken_ = EXPECT_OPERATOR;
    } else if (token->type() == LexToken::TYPE_MINUS) {
        token->setUnary(true);
        state.operators_.push(token);
        // we expect another operand
    } else if (token->type() == LexToken::TYPE_OPEN_BRACKET) {
        state.operators_.push(token);
    }  else if (token->type() == LexToken::TYPE_EOF) {
        error_ = ParseError("Unexpected end of expression", token->position());
        ok_ = false;
    } else {
        error_ = ParseError("Unexpected operator", token->position());
        ok_ = false;
    }
}


void Expression::applyLastOperator(ParseState& state) {
    ConstLexTokenRef op = state.operators_.top();
    state.operators_.pop();

    if (op->isUnary()) {
        ParseTreeRef operand = state.operands_.top();
        state.operands_.pop();
        // our only unary operator is '-'
        ParseTreeRef result = ParseTree::makeUnaryOperator(op, operand);
        state.operands_.push(result);
    } else {
        ParseTreeRef rhs = state.operands_.top();
        state.operands_.pop();
        ParseTreeRef lhs = state.operands_.top();
        state.operands_.pop();
        ParseTreeRef result = ParseTree::makeBinaryOperator(op, lhs, rhs);
        state.operands_.push(result);
    }
}

void Expression::endBracketedExpression(ConstLexTokenRef closeBrace,
                                              ParseState& state) {
    bool done = false;
    while (ok_ && !done) {
        if (state.operators_.empty()) {
            error_ = ParseError("No matching open brace for close brace",
                                closeBrace->position());
            ok_ = false;
        } else {
            LexToken::Type type = state.operators_.top()->type();

            if (type == LexToken::TYPE_OPEN_BRACKET) {
                state.operators_.pop();
                done = true;
            } else {
                applyLastOperator(state);
            }
        }
    }
}

Expression::ParseState::ParseState(const std::string& input,
                                         const Headers& headers)
    :lexer_(input), headers_(headers), operators_(), operands_(),
     expectedToken_(EXPECT_OPERAND), done_(false) {

}
