//
// csvfilter, Copyright (c) 2015, plnu
//

#include "unaryOperator.h"

#include <assert.h>
#include <sstream>

/**
 * @brief Constructor.
 *
 * Constructor.
 *
 * @param op        The lexer token represented by this operand
 * @param operand   The parse tree this operator acts on
 * 
 */
UnaryMinus::UnaryMinus(ConstLexTokenRef op, ParseTreeRef operand)
    :op_(op), operand_(operand), result_(Variant::error("Uninitialised"))
{
    assert(op->type() == LexToken::TYPE_MINUS);
}

/**
 * @brief Destructor.
 *
 * Destructor
 *
 */
UnaryMinus::~UnaryMinus() {

}

/**
 * @copydoc ParseTree::validateTypes
 */
UnaryMinus::NodeType UnaryMinus::validateTypes(ParseError& err) {
    NodeType result = operand_->validateTypes(err);
    if (result != NODE_TYPE_ERROR) {
        if (result == NODE_TYPE_UNKNOWN) {
            if (!operand_->setType(NODE_TYPE_NUMBER, err)) {
                result = NODE_TYPE_ERROR;
            }
        } else if (result != NODE_TYPE_NUMBER) {
            result = NODE_TYPE_ERROR;
            std::stringstream msg;
            msg << "Unary minus expects a number, not a " << result;
            err = ParseError(msg.str(), op_->position(), operand_->position());
        }
    }
    return result;
}

/**
 * @copydoc ParseTree::setType
 */
bool UnaryMinus::setType(NodeType t, ParseError& err) {
    bool ret = true;
    if (t != NODE_TYPE_NUMBER) {
        ret = false;
        std::stringstream msg;
        msg << "Expression cannot be coerced into a " << t;
        err = ParseError(msg.str(), position());
    }
    return ret;
}

/**
 * @copydoc ParseTree::eval
 */
VariantRef UnaryMinus::eval(const LineParser& line, NodeType typeHint) const {
    VariantRef val = operand_->eval(line, NODE_TYPE_NUMBER);
    VariantRef ret = result_;
    if (val->type() == Variant::ERROR) {
        ret = val;
    } else if (val->type() != Variant::NUMBER) {
            std::stringstream msg;
            msg << "The unary operator at " << op_->position().begin
                << "expects arguments of type number (got "
                << val->type() << ")";
            ret = Variant::error(msg.str());
    } else {
        result_->resetToNumber(-val->numberVal());
    }
    return ret;
}

/**
 * @copydoc ParseTree::stream
 */
void UnaryMinus::stream(std::ostream& out) {
    out << "(" << op_->value() << " ";
    operand_->stream(out);
    out << "):number";
}

/**
 * @copydoc ParseTree::canBeNumber
 */
bool UnaryMinus::canBeNumber(const LineParser& line) const {
    return true;
}

Range UnaryMinus::position() const {
    return Range(op_->position().begin, operand_->position().end);
}
