//
// csvfilter, Copyright (c) 2015, plnu
//

#include "binaryOperator.h"

#include <sstream>
#include <string.h>
#include <assert.h>

/**
 *
 * @brief Constructor.
 *
 * Create a new NumericBinaryOperator node for the parse tree.
 *
 * @param op   The token for the operator. Must be a '-', '*' or '/'
 * @param lhs  The expression that was on the left hand of the operator
 * @param rhs  The expression that was on the right hand of the operator
 *
 */
NumericBinaryOperator::NumericBinaryOperator(ConstLexTokenRef op,
                                             ParseTreeRef lhs,
                                             ParseTreeRef rhs)
    :op_(op), lhs_(lhs), rhs_(rhs), result_(Variant::error("Undefined")) {
    assert(op_->type() == LexToken::TYPE_MINUS ||
           op_->type() == LexToken::TYPE_TIMES ||
           op_->type() == LexToken::TYPE_DIVIDE);
}

/**
 * @brief Destructor.
 *
 *
 */
NumericBinaryOperator::~NumericBinaryOperator() {

}

ParseTree::NodeType NumericBinaryOperator::validateTypes(ParseError& err) {
    NodeType ret = validateOperandType(lhs_, err);
    if (ret != NODE_TYPE_ERROR) {
        ret = validateOperandType(rhs_, err);
    }
    return ret;
}

bool NumericBinaryOperator::setType(NodeType t, ParseError& err) {
    bool success = true;
    if (t != NODE_TYPE_NUMBER) {
        success = false;
        std::stringstream msg;
        msg << "Cannot coerce expression into a " << t;
        err = ParseError(msg.str(),
                         Range(lhs_->position().begin,
                               rhs_->position().end));
    }
    return success;
}

VariantRef NumericBinaryOperator::eval(const LineParser& line,
                                       NodeType typeHint) const {
    VariantRef l = lhs_->eval(line, NODE_TYPE_NUMBER);
    VariantRef r = rhs_->eval(line, NODE_TYPE_NUMBER);
    VariantRef ret = result_;

    if (l->type() == Variant::ERROR) {
        ret = l;
    } else if (r->type() == Variant::ERROR) {
        ret = r;
    } else if (l->type() != Variant::NUMBER) {
        std::stringstream msg;
        msg << "Left hand side of operator at " << op_->position().begin
            << ": expected number, got " << l->type();
        result_->resetToError(msg.str());
    } else if (r->type() != Variant::NUMBER) {
        std::stringstream msg;
        msg << "Right hand side of operator at " << op_->position().begin
            << ": expected number, got " << r->type();
        result_->resetToError(msg.str());
    } else {
        switch (op_->type()) {
        case LexToken::TYPE_MINUS:
            result_->resetToNumber(l->numberVal() - r->numberVal());
            break;
        case LexToken::TYPE_TIMES:
            result_->resetToNumber(l->numberVal() * r->numberVal());
            break;
        case LexToken::TYPE_DIVIDE:
            result_->resetToNumber(l->numberVal() / r->numberVal());
            break;
        default:
            // should never happen - this function only handles numeric
            // operators
            abort();
            break;
        }
    }
    return ret;
}
    
void NumericBinaryOperator::stream(std::ostream& out) {
    out << "(" << op_->value() << " ";
    lhs_->stream(out);
    out << " ";
    rhs_->stream(out);
    out << "):" << NODE_TYPE_NUMBER;
}

bool NumericBinaryOperator::canBeNumber(const LineParser& line) const {
    return true;
}

Range NumericBinaryOperator::position() const {
    return Range(lhs_->position().begin, rhs_->position().end);
}

ParseTree::NodeType NumericBinaryOperator::validateOperandType(
    ParseTreeRef op,
    ParseError& err) {

    NodeType t = op->validateTypes(err);
    if (t == NODE_TYPE_UNKNOWN) {
        if (!op->setType(NODE_TYPE_NUMBER, err)) {
            t = NODE_TYPE_ERROR;
        }
    } else if (t != NODE_TYPE_NUMBER &&
               t != NODE_TYPE_ERROR) {
        op->setType(NODE_TYPE_NUMBER, err); // this will set the errText
        t = NODE_TYPE_ERROR;
    }
    return t;
}


/**
 *
 * @brief Constructor.
 *
 * Create a new LogicalBinaryOperator node for the parse tree.
 *
 * @param op   The token for the operator. Must be a '&&' or '||'
 * @param lhs  The expression that was on the left hand of the operator
 * @param rhs  The expression that was on the right hand of the operator
 *
 */
LogicalBinaryOperator::LogicalBinaryOperator(ConstLexTokenRef op,
                                             ParseTreeRef lhs,
                                             ParseTreeRef rhs)
    :op_(op), lhs_(lhs), rhs_(rhs) {
    assert(op_->type() == LexToken::TYPE_AND ||
           op_->type() == LexToken::TYPE_OR);
}

LogicalBinaryOperator::~LogicalBinaryOperator() {

}

ParseTree::NodeType LogicalBinaryOperator::validateTypes(ParseError& err) {
    NodeType ret = validateOperandType(lhs_, err);
    if (ret != NODE_TYPE_ERROR) {
        ret = validateOperandType(rhs_, err);
    }
    return ret;
}

bool LogicalBinaryOperator::setType(NodeType t, ParseError& err) {
    bool success = true;
    if (t != NODE_TYPE_BOOL) {
        success = false;
        std::stringstream msg;
        msg << "Cannot coerce expression into a " << t;
        err = ParseError(msg.str(), position());
    }
    return success;
}

VariantRef LogicalBinaryOperator::eval(const LineParser& line,
                                       NodeType typeHint) const {
    VariantRef ret;
    if (op_->type() == LexToken::TYPE_AND) {
        VariantRef l = lhs_->eval(line, NODE_TYPE_BOOL);
        if (l->type() == Variant::ERROR) {
            ret = l;
        } else if (l->type() != Variant::BOOLEAN) {
            std::stringstream msg;
            msg << "Left hand side of operator at " << op_->position().begin
                << ": expected boolean, got " << l->type();
            ret = Variant::error(msg.str());
        } else if (l->booleanVal() == false) {
            ret = l;
        } else {
            VariantRef r = rhs_->eval(line, NODE_TYPE_BOOL);
            if (r->type() == Variant::ERROR) {
                ret = r;
            } else if (r->type() != Variant::BOOLEAN) {
                std::stringstream msg;
                msg << "Right hand side of operator at "
                    << op_->position().begin
                    << ": expected boolean, got " << r->type();
                ret = Variant::error(msg.str());
            } else {
                ret = r;
            }
        }
    } else if (op_->type() == LexToken::TYPE_OR) {
        VariantRef l = lhs_->eval(line, NODE_TYPE_BOOL);
        if (l->type() == Variant::ERROR) {
            ret = l;
        } else if (l->type() != Variant::BOOLEAN) {
            std::stringstream msg;
            msg << "Left hand side of operator at " << op_->position().begin
                << ": expected boolean, got " << l->type();
            ret = Variant::error(msg.str());
        } else if (l->booleanVal() == true) {
            ret = l;
        } else {
            VariantRef r = rhs_->eval(line, NODE_TYPE_BOOL);
            if (r->type() == Variant::ERROR) {
                ret = r;
            } else if (r->type() != Variant::BOOLEAN) {
                std::stringstream msg;
                msg << "Right hand side of operator at "
                    << op_->position().begin
                    << ": expected boolean, got " << r->type();
                ret = Variant::error(msg.str());
            } else {
                ret = r;
            }
        }
    }

    return ret;
}
    
void LogicalBinaryOperator::stream(std::ostream& out) {
    out << "(" << op_->value() << " ";
    lhs_->stream(out);
    out << " ";
    rhs_->stream(out);
    out << "):" << NODE_TYPE_BOOL;
}

bool LogicalBinaryOperator::canBeNumber(const LineParser& line) const {
    return false;
}

Range LogicalBinaryOperator::position() const {
    return Range(lhs_->position().begin, rhs_->position().end);
}

ParseTree::NodeType LogicalBinaryOperator::validateOperandType(
    ParseTreeRef op,
    ParseError& err) {

    NodeType t = op->validateTypes(err);
    if (t != NODE_TYPE_BOOL && t != NODE_TYPE_ERROR) {
        std::stringstream msg;
        msg << "The arguments to '" << op_->value() << "' must be boolean, not "
            << t;
        err = ParseError(msg.str(), op_->position(), op->position());
        t = NODE_TYPE_ERROR;
    }
    return t;
}

/**
 *
 * @brief Constructor.
 *
 * Create a new ComparisonBinaryOperator node for the parse tree.
 *
 * @param op   The token for the operator. Must be a '<', '<=', '==', '!=',
 *             '>=' or '>'
 * @param lhs  The expression that was on the left hand of the operator
 * @param rhs  The expression that was on the right hand of the operator
 *
 */
ComparisonBinaryOperator::ComparisonBinaryOperator(ConstLexTokenRef op,
                                                   ParseTreeRef lhs,
                                                   ParseTreeRef rhs)
    :op_(op), lhs_(lhs), rhs_(rhs), result_(Variant::error("uninitialised")) {
    assert(op_->type() == LexToken::TYPE_LT  ||
           op_->type() == LexToken::TYPE_LTE ||
           op_->type() == LexToken::TYPE_EQ  ||
           op_->type() == LexToken::TYPE_NEQ ||
           op_->type() == LexToken::TYPE_GT  ||
           op_->type() == LexToken::TYPE_GTE);

}

ComparisonBinaryOperator::~ComparisonBinaryOperator() {

}

ParseTree::NodeType ComparisonBinaryOperator::validateTypes(
    ParseError& err) {
    NodeType l;
    NodeType r;
    NodeType ret = NODE_TYPE_BOOL;
    if ( (l = lhs_->validateTypes(err)) == NODE_TYPE_ERROR ||
         (r = rhs_->validateTypes(err))  == NODE_TYPE_ERROR ) {
        ret = NODE_TYPE_ERROR;
    } else if (l != NODE_TYPE_UNKNOWN &&
               l != NODE_TYPE_STRING &&
               l != NODE_TYPE_NUMBER) {
        std::stringstream msg;
        msg << "The arguments to '" << op_->value()
            << "' must be strings or numbers, not " << l;
        err = ParseError(msg.str(), op_->position(), lhs_->position());
        ret = NODE_TYPE_ERROR;
    } else if (r != NODE_TYPE_UNKNOWN &&
               r != NODE_TYPE_STRING &&
               r != NODE_TYPE_NUMBER) {
        std::stringstream msg;
        msg << "The arguments to '" << op_->value()
            << "' must be strings or numbers, not " << l;
        err = ParseError(msg.str(), op_->position(), rhs_->position());
        ret = NODE_TYPE_ERROR;
    } else if (l == NODE_TYPE_UNKNOWN && r == NODE_TYPE_UNKNOWN) {
        // This is ok, we just can't predict the types of the operators at parse
        // time
    } else if (l == NODE_TYPE_UNKNOWN) {
        if (!lhs_->setType(r, err)) {
            ret = NODE_TYPE_ERROR;
        }
    } else if (r == NODE_TYPE_UNKNOWN) {
        if (!rhs_->setType(l, err)) {
            ret = NODE_TYPE_ERROR;
        }
    } else if (l != r) {
        std::stringstream msg;
        msg << "'" << op_->value()
            << "' expects arguments of the same type (got a " << l
            << " and a " << r << ")";
        err = ParseError(msg.str(), op_->position(), position());
        ret = NODE_TYPE_ERROR;
    } else {
        // we're ok
    }

    return ret;
}

bool ComparisonBinaryOperator::setType(NodeType t, ParseError& err) {
    bool success = true;
    if (t != NODE_TYPE_BOOL) {
        success = false;
        std::stringstream msg;
        msg << "Cannot coerce expression into a " << t;
        err = ParseError(msg.str(), position());
    }
    return success;
}

VariantRef ComparisonBinaryOperator::eval(const LineParser& line,
                                          NodeType typeHint) const {
    VariantRef ret;
    if (lhs_->canBeNumber(line) && rhs_->canBeNumber(line)) {
        ret = evalComparisonNumber(line, typeHint);
    } else {
        ret = evalComparisonString(line, typeHint);
    }
    return ret;
}

void ComparisonBinaryOperator::stream(std::ostream& out) {
    out << "(" << op_->value() << " ";
    lhs_->stream(out);
    out << " ";
    rhs_->stream(out);
    out << "):" << NODE_TYPE_BOOL;
}

bool ComparisonBinaryOperator::canBeNumber(const LineParser& line) const {
    return false;
}

Range ComparisonBinaryOperator::position() const {
    return Range(lhs_->position().begin, rhs_->position().end);
}

/**
 * @brief Evaluate a comparison operator using strings
 *
 * Apply a comparison operator, assuming the operands are strings.
 *
 * @see BinaryOperator::evalComparison
 *
 * @param line      The current line from the csv file
 * @param typeHint  Ignored
 *
 * @return  The result of the operation, or an error.
 *
 */
VariantRef ComparisonBinaryOperator::evalComparisonString(
    const LineParser& line,
    NodeType typeHint) const {

    VariantRef l = lhs_->eval(line, NODE_TYPE_STRING);
    VariantRef r = rhs_->eval(line, NODE_TYPE_STRING);
    VariantRef ret = result_;
    
    if (l->type() == Variant::ERROR) {
        ret = l;
    } else if (r->type() == Variant::ERROR) {
        ret = r;
    } else if (l->type() != Variant::STRING) {
        std::stringstream msg;
        msg << "Left hand side of operator at " << op_->position().begin
            << ": expected string, got " << l->type();
        result_->resetToError(msg.str());
    } else if (r->type() != Variant::STRING) {
        std::stringstream msg;
        msg << "Right hand side of operator at " << op_->position().begin
            << ": expected string, got " << l->type();
        result_->resetToError(msg.str());
    } else {
        switch (op_->type()) {
        case LexToken::TYPE_LT:
            result_->resetToBoolean(strcmp(l->charVal(), r->charVal()) < 0);
            break;
        case LexToken::TYPE_LTE:
            result_->resetToBoolean(strcmp(l->charVal(), r->charVal()) <= 0);
            break;
        case LexToken::TYPE_EQ:
            result_->resetToBoolean(strcmp(l->charVal(), r->charVal()) == 0);
            break;
        case LexToken::TYPE_NEQ:
            result_->resetToBoolean(strcmp(l->charVal(), r->charVal()) != 0);
            break;
        case LexToken::TYPE_GT:
            result_->resetToBoolean(strcmp(l->charVal(), r->charVal()) > 0);
            break;
        case LexToken::TYPE_GTE:
            result_->resetToBoolean(strcmp(l->charVal(), r->charVal()) >= 0);
            break;
        default:
            // it's not a comparison operator
            abort();
            break;
        }
    }
    return ret;
}

/**
 * @brief Evaluate a comparison operator using numbers
 *
 * Apply a comparison operator, assuming the operands are numbers.
 *
 * @see BinaryOperator::evalComparison
 *
 * @param line      The current line from the csv file
 * @param typeHint  Ignored
 *
 * @return  The result of the operation, or an error.
 *
 */
VariantRef ComparisonBinaryOperator::evalComparisonNumber(
    const LineParser& line,
    NodeType typeHint) const {
    VariantRef l = lhs_->eval(line, NODE_TYPE_NUMBER);
    VariantRef r = rhs_->eval(line, NODE_TYPE_NUMBER);
    VariantRef ret = result_;
    if (l->type() == Variant::ERROR) {
        ret = l;
    } else if (r->type() == Variant::ERROR) {
        ret = r;
    } else if (l->type() != Variant::NUMBER) {
        std::stringstream msg;
        msg << "Left hand side of operator at " << op_->position().begin
            << ": expected number, got " << l->type();
        result_->resetToError(msg.str());
    } else if (r->type() != Variant::NUMBER) {
        std::stringstream msg;
        msg << "Right hand side of operator at " << op_->position().begin
            << ": expected number, got " << l->type();
        result_->resetToError(msg.str());
    } else {
        switch (op_->type()) {
        case LexToken::TYPE_LT:
            result_->resetToBoolean(l->numberVal() < r->numberVal());
            break;
        case LexToken::TYPE_LTE:
            result_->resetToBoolean(l->numberVal() <= r->numberVal());
            break;
        case LexToken::TYPE_EQ:
            result_->resetToBoolean(l->numberVal() == r->numberVal());
            break;
        case LexToken::TYPE_NEQ:
            result_->resetToBoolean(l->numberVal() != r->numberVal());
            break;
        case LexToken::TYPE_GT:
            result_->resetToBoolean(l->numberVal() > r->numberVal());
            break;
        case LexToken::TYPE_GTE:
            result_->resetToBoolean(l->numberVal() >= r->numberVal());
            break;
        default:
            // it's not a comparison operator
            abort();
            break;
        }
    }
    return ret;
}

/**
 *
 * @brief Constructor.
 *
 * Create a new PlusBinaryOperator node for the parse tree.
 *
 * @param op   The token for the operator. Must be a '+'
 * @param lhs  The expression that was on the left hand of the operator
 * @param rhs  The expression that was on the right hand of the operator
 *
 */
PlusBinaryOperator::PlusBinaryOperator(ConstLexTokenRef op,
                                       ParseTreeRef lhs,
                                       ParseTreeRef rhs)
    :op_(op),
     lhs_(lhs),
     rhs_(rhs),
     calculatedType_(NODE_TYPE_UNKNOWN),
     result_(Variant::error("Uninitialised")),
     addBuf_(nullptr),
     addBufLen_(0) {
    assert(op_->type() == LexToken::TYPE_PLUS);
}

PlusBinaryOperator::~PlusBinaryOperator() {
    delete[] addBuf_;
}

ParseTree::NodeType PlusBinaryOperator::validateTypes(
    ParseError& err) {
    NodeType l;
    NodeType r;

    NodeType ret = NODE_TYPE_UNKNOWN;
    if ( (l = lhs_->validateTypes(err)) == NODE_TYPE_ERROR ||
         (r = rhs_->validateTypes(err))  == NODE_TYPE_ERROR ) {
        ret = NODE_TYPE_ERROR;
    } else if (l != NODE_TYPE_UNKNOWN &&
               l != NODE_TYPE_STRING &&
               l != NODE_TYPE_NUMBER) {
        std::stringstream msg;
        msg << "Arguments to + should be strings or numbers, not " << l << ")";
        err = ParseError(msg.str(), op_->position(), lhs_->position());
        ret = NODE_TYPE_ERROR;
    } else if (r != NODE_TYPE_UNKNOWN &&
               r != NODE_TYPE_STRING &&
               r != NODE_TYPE_NUMBER) {
        std::stringstream msg;
        msg << "Arguments to + should be strings or numbers, not " << r << ")";
        err = ParseError(msg.str(), op_->position(), rhs_->position());
        ret = NODE_TYPE_ERROR;
    } else if (l == NODE_TYPE_UNKNOWN && r == NODE_TYPE_UNKNOWN) {
        // This is ok, we just can't predict the types of the operators at parse
        // time
    } else if (l == NODE_TYPE_UNKNOWN) {
        if (!lhs_->setType(r, err)) {
            ret = NODE_TYPE_ERROR;
        } else {
            ret = r;
        }
    } else if (r == NODE_TYPE_UNKNOWN) {
        if (!rhs_->setType(l, err)) {
            ret = NODE_TYPE_ERROR;
        } else {
            ret = l;
        }
    } else if (l != r) {
        std::stringstream msg;
        msg << "The + operator expects its arguments to be the same type, "
            << "got a " << l << " and a " << r;
        err = ParseError(msg.str(), op_->position(), position());
        ret = NODE_TYPE_ERROR;
    } else {
        ret = l;
    }

    if (ret != NODE_TYPE_ERROR) {
        calculatedType_ = ret;
    }

    return ret;
}

bool PlusBinaryOperator::setType(NodeType t, ParseError& err) {
    bool success = true;
    if (calculatedType_ == NODE_TYPE_UNKNOWN) {
        if (lhs_->setType(t, err) &&
            rhs_->setType(t, err)) {
            calculatedType_ = t;
        } else {
            success = false;
        }
    } else if (t != calculatedType_) {
        success = false;
        std::stringstream msg;
        msg << "Cannot coerce value into a " << t;
        err = ParseError(msg.str(), position());
    }
    return success;
}

VariantRef PlusBinaryOperator::eval(const LineParser& line,
                                          NodeType typeHint) const {
    VariantRef ret;
    if (calculatedType_ == NODE_TYPE_NUMBER) {
        ret = evalPlusNumber(line, typeHint);
    } else if (calculatedType_ == NODE_TYPE_STRING) {
        ret = evalPlusString(line, typeHint);
    } else if (calculatedType_ == NODE_TYPE_UNKNOWN) {
        if (typeHint == NODE_TYPE_NUMBER) {
            ret = evalPlusNumber(line, typeHint);
        } else if (calculatedType_ == NODE_TYPE_STRING) {
            ret = evalPlusString(line, typeHint);
        } else {
            if (lhs_->canBeNumber(line) && rhs_->canBeNumber(line)) {
                ret = evalPlusNumber(line, typeHint);
            } else {
                ret = evalPlusString(line, typeHint);
            }
        }
    }

    return ret;
}

void PlusBinaryOperator::stream(std::ostream& out) {
    out << "(" << op_->value() << " ";
    lhs_->stream(out);
    out << " ";
    rhs_->stream(out);
    out << "):" << calculatedType_;
}


bool PlusBinaryOperator::canBeNumber(const LineParser& line) const {
    return (calculatedType_ == NODE_TYPE_NUMBER) ||
        ( calculatedType_ == NODE_TYPE_UNKNOWN &&
          lhs_->canBeNumber(line) &&
          rhs_->canBeNumber(line) );
}

Range PlusBinaryOperator::position() const {
    return Range(lhs_->position().begin, rhs_->position().end);
}

/**
 * @brief Apply a plus for strings.
 *
 * Apply a plus operator as string concatenation.
 *
 * @see BinaryOperator::evalPlus
 *
 * @param line      The current line from the csv file
 * @param typeHint  A hint as to the expected return type
 *
 * @return  The result of the operation, or an error.
 *
 */
VariantRef PlusBinaryOperator::evalPlusString(const LineParser& line,
                                          NodeType typeHint) const {
    VariantRef l = lhs_->eval(line, NODE_TYPE_STRING);
    VariantRef r = rhs_->eval(line, NODE_TYPE_STRING);
    VariantRef ret = result_;

    if (l->type() == Variant::ERROR) {
        ret = l;
    } else if (r->type() == Variant::ERROR) {
        ret = r;
    } if (l->type() != Variant::STRING) {
        std::stringstream msg;
        msg << "Left hand side of operator at " << op_->position().begin
            << ": expected string, got " << l->type();
        result_->resetToError(msg.str());
    } else if (r->type() != Variant::STRING) {
        std::stringstream msg;
        msg << "Right hand side of operator at " << op_->position().begin
            << ": expected string, got " << r->type();
        result_->resetToError(msg.str());
    } else {
        int lLen = strlen(l->charVal());
        int rLen = strlen(r->charVal());
        int required = lLen + rLen + 1;
        if (addBufLen_ < required) {
            delete[] addBuf_;
            addBuf_ = new char[required];
        }
        strcpy(addBuf_, l->charVal());
        strcpy(addBuf_ + lLen, r->charVal());
        result_->resetToString(addBuf_);
    }

    return ret;
}

/**
 * @brief Apply a plus for numbers.
 *
 * Apply a plus operator as numeric addition.
 *
 * @see BinaryOperator::evalPlus
 *
 * @param line      The current line from the csv file
 * @param typeHint  A hint as to the expected return type
 *
 * @return  The result of the operation, or an error.
 *
 */
VariantRef PlusBinaryOperator::evalPlusNumber(const LineParser& line,
                                           NodeType typeHint) const {
    VariantRef l = lhs_->eval(line, NODE_TYPE_NUMBER);
    VariantRef r = rhs_->eval(line, NODE_TYPE_NUMBER);
    VariantRef ret = result_;

    if (l->type() == Variant::ERROR) {
        ret = l;
    } else if (r->type() == Variant::ERROR) {
        ret = r;
    } else if (l->type() != Variant::NUMBER) {
        std::stringstream msg;
        msg << "Left hand side of operator at " << op_->position().begin
            << ": expected number, got " << l->type();
        result_->resetToError(msg.str());
    } else if (r->type() != Variant::NUMBER) {
        std::stringstream msg;
        msg << "Right hand side of operator at " << op_->position().begin
            << ": expected number, got " << r->type();
        result_->resetToError(msg.str());
    } else {
        result_->resetToNumber(l->numberVal() + r->numberVal());
    }

    return ret;
}

