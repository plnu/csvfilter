//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef CSVFILTER_UNARY_OPERATOR_H
#define CSVFILTER_UNARY_OPERATOR_H

#include "parseTree.h"


/**
 * @brief A unary minus in the parse tree for filter expressions.
 *
 * This class represents a unary minus in the parse tree for an
 * expression. I.e. it's a minus sign with only one operand, like "-3". Binary
 * minus operators (e.g. "3-4") are represented by the BinaryOperator class.
 *
 */
class UnaryMinus : public ParseTree {
public:
    UnaryMinus(ConstLexTokenRef op,ParseTreeRef operand);
    virtual ~UnaryMinus();

    virtual NodeType validateTypes(ParseError& err);
    virtual bool setType(NodeType t, ParseError& err);

    virtual VariantRef eval(const LineParser& line, NodeType typeHint) const;
    
    virtual void stream(std::ostream& out);
    virtual bool canBeNumber(const LineParser& line) const;
    virtual Range position() const;

private:
    ConstLexTokenRef op_;
    ParseTreeRef operand_;
    VariantRef result_;
};

#endif //CSVFILTER_UNARY_OPERATOR_H
