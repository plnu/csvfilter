//
// csvfilter, Copyright (c) 2015, plnu
//

#include "operand.h"

#include <assert.h>
#include <sstream>

/**
 * @brief Constructor.
 *
 * Constructor for string and number constants
 *
 * @param lexToken  The lexer token represented by this operand
 */
Operand::Operand(ConstLexTokenRef lexToken)
    :token_(lexToken),
     identifierIndex_(-1),
     nodeType_(NODE_TYPE_UNKNOWN),
     result_(Variant::error("Uninitialised")) {
    initialiseNodeType();
}

/**
 * @brief Constructor.
 *
 * Constructor for operands that reference fields in the csv file
 *
 * @param lexToken  The lexer token represented by this operand
 * @param idIndex   The index of the field in the csv file
 *
 */
Operand::Operand(ConstLexTokenRef lexToken, int idIndex)
    :token_(lexToken),
     identifierIndex_(idIndex),
     nodeType_(NODE_TYPE_UNKNOWN),
     result_(Variant::error("Uninitialised")) {
    initialiseNodeType();
}

/**
 * @brief Destructor.
 *
 * Destructor.
 *
 */
Operand::~Operand() {

}

/**
 * @copydoc ParseTree::validateTypes
 */
ParseTree::NodeType Operand::validateTypes(ParseError& err) {
    NodeType nodeType = NODE_TYPE_ERROR;

    switch(token_->type()) {
    case LexToken::TYPE_NUMBER:
        nodeType = NODE_TYPE_NUMBER;
        result_->resetToNumber(token_->numberValue());
        break;
    case LexToken::TYPE_STRING:
        nodeType = NODE_TYPE_STRING;
        result_->resetToString(token_->value());
        break;
    case LexToken::TYPE_IDENTIFIER:
        nodeType = NODE_TYPE_UNKNOWN;
        break;
    default:
        // Operands should not be associated with any other token types
        abort();
        break;
    }

    return nodeType;
}

/**
 * @copydoc ParseTree::setType
 */
bool Operand::setType(NodeType t, ParseError& err) {
    bool success = true;
    assert(t == NODE_TYPE_STRING || t == NODE_TYPE_NUMBER);

    if (nodeType_ == NODE_TYPE_UNKNOWN) {
        nodeType_ = t;
    } else if (nodeType_ != t) {
        success = false;
        std::stringstream msg;
        msg << "Operand is the wrong type - expected a " << t
            << ", but got a " << nodeType_;
        err = ParseError(msg.str(), token_->position());
    }
    return success;
}

/**
 * @copydoc ParseTree::eval
 */
VariantRef Operand::eval(const LineParser& line, NodeType typeHint) const {
    if (token_->type() == LexToken::TYPE_STRING ||
        token_->type() == LexToken::TYPE_NUMBER) {
        // result is pre-populated
    } else {
        assert(token_->type() == LexToken::TYPE_IDENTIFIER);
        double lineVal = 0.0;
        if (typeHint == NODE_TYPE_NUMBER &&
            line.field(identifierIndex_)->asNumber(lineVal)) {
            result_->resetToNumber(lineVal);
        } else {
            result_->resetToString(line.field(identifierIndex_)->asString());
        }
    }
    return result_;
}

/**
 * @copydoc ParseTree::canBeNumber
 */
bool Operand::canBeNumber(const LineParser& line) const {
    double dummy = 0.0;
    return
        ( nodeType_ == NODE_TYPE_NUMBER ) ||
        ( nodeType_ == NODE_TYPE_UNKNOWN &&
          line.field(identifierIndex_)->asNumber(dummy) );
}


Range Operand::position() const {
    return token_->position();
}

/**
 * @copydoc ParseTree::stream
 */
void Operand::stream(std::ostream& out) {
    out << token_->value();
    if (token_->type() == LexToken::TYPE_IDENTIFIER) {
        out << "~" << identifierIndex_;
    }
    out << ":" << nodeType_;
}


/**
 * @brief Initialise the nodeType_ member variable
 *
 * Intialise nodeType_ based on the type of token_
 *
 */
void Operand::initialiseNodeType() {
    if (token_->type() == LexToken::TYPE_STRING) {
        nodeType_ = NODE_TYPE_STRING;
    } else if (token_->type() == LexToken::TYPE_NUMBER) {
        nodeType_ = NODE_TYPE_NUMBER;
    } else {
        nodeType_ = NODE_TYPE_UNKNOWN;
    }
}
