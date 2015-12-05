//
// csvfilter, Copyright (c) 2015, plnu
//

#include "parseTree.h"
#include "operand.h"
#include "binaryOperator.h"
#include "unaryOperator.h"

#include <sstream>
#include <assert.h>
#include <iostream>

/**
 * @brief Constructor.
 *
 * Constructor
 *
 */
ParseTree::ParseTree() {

}

/**
 * @brief Destructor.
 *
 * Destructor
 *
 */
ParseTree::~ParseTree() {

}

/**
 * @brief  A string representation of the parse tree
 *
 * Returns a string representation of the parse tree, suitable for debugging
 * purposes.
 *
 * @return  A string representation of the parse tree.
 *
 */
std::string ParseTree::toString() {
    std::stringstream s;
    stream(s);
    return s.str();
}


/**
 *
 * Create an parse tree node representing a constant (either a string or numeric
 * constant)
 *
 * @param op  The token representing the constant
 *
 * @return  The new node
 *
 */
ParseTreeRef ParseTree::makeOperand(ConstLexTokenRef op) {
    return ParseTreeRef(new Operand(op));
}

/**
 *
 * Create an parse tree node representing a variable operand - i.e. a reference
 * to a column in the csv file
 *
 * @param op   The token from the expression
 * @param pos  The index of the field from the csv file
 *
 * @return  The new node
 *
 */
ParseTreeRef ParseTree::makeOperand(ConstLexTokenRef op, int pos) {
    return ParseTreeRef(new Operand(op, pos));
}

/**
 *
 * Create an parse tree node representing a unary minus
 *
 * @param op        The minus operator
 * @param operand  The expression to act on
 *
 * @return  The new node
 *
 */
ParseTreeRef ParseTree::makeUnaryOperator(ConstLexTokenRef op,
                                          ParseTreeRef operand) {
    return ParseTreeRef(new UnaryMinus(op, operand));
}

/**
 *
 * Create an parse tree node representing a binary operator
 *
 * @param op   The operator
 * @param lhs  The left hand expression
 * @param rhs  The right hand expression
 *
 * @return  The new node
 *
 */
ParseTreeRef ParseTree::makeBinaryOperator(ConstLexTokenRef op,
                                           ParseTreeRef lhs,
                                           ParseTreeRef rhs) {
    ParseTreeRef ret;
    switch (op->type()) {
    case LexToken::TYPE_MINUS:
    case LexToken::TYPE_TIMES:
    case LexToken::TYPE_DIVIDE:
        ret = ParseTreeRef(new NumericBinaryOperator(op, lhs, rhs));
        break;
    case LexToken::TYPE_AND:
    case LexToken::TYPE_OR:
        ret = ParseTreeRef(new LogicalBinaryOperator(op, lhs, rhs));
        break;
    case LexToken::TYPE_LT:
    case LexToken::TYPE_LTE:
    case LexToken::TYPE_EQ:
    case LexToken::TYPE_NEQ:
    case LexToken::TYPE_GT:
    case LexToken::TYPE_GTE:
        ret = ParseTreeRef(new ComparisonBinaryOperator(op, lhs, rhs));
        break;
    case LexToken::TYPE_PLUS:
        ret = ParseTreeRef(new PlusBinaryOperator(op, lhs, rhs));
        break;
    default:
        // unsupported token
        abort();
        break;
    }

    return ret;
}

std::ostream& operator<< (std::ostream &out, ParseTree::NodeType t)
{
    const char* label = "UNKNOWN";
    switch (t) {
    case ParseTree::NODE_TYPE_UNKNOWN:
        label = "unknown";
        break;
    case ParseTree::NODE_TYPE_STRING:
        label = "string";
        break;
    case ParseTree::NODE_TYPE_NUMBER:
        label = "number";
        break;
    case ParseTree::NODE_TYPE_BOOL:
        label = "boolean";
        break;
    case ParseTree::NODE_TYPE_ERROR:
        label = "error";
        break;
    }
    out << label;

    return out;
}

