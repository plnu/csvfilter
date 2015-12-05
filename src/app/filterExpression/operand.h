#ifndef CSVFILTER_OPERAND_H
#define CSVFILTER_OPERAND_H

#include "parseTree.h"

/**
 * @brief An operand in the parse tree for filter expressions.
 *
 * This node in the filter expression parse tree is an operand. It may be a
 * constant (string or number), or a reference to a field in the current row of
 * the csv file.
 *
 */
class Operand : public ParseTree {
public:
    Operand(ConstLexTokenRef lexToken);
    Operand(ConstLexTokenRef lexToken, int idIndex);
    virtual ~Operand();

    virtual NodeType validateTypes(ParseError& err);
    virtual bool setType(NodeType t, ParseError& err);

    virtual VariantRef eval(const LineParser& line, NodeType typeHint) const;
    virtual bool canBeNumber(const LineParser& line) const;

    virtual Range position() const;

    virtual void stream(std::ostream& out);

private:
    Operand(const Operand& other);
    Operand& operator=(const Operand& other);

    void initialiseNodeType();
    ConstLexTokenRef token_;
    int identifierIndex_;
    NodeType nodeType_;
    VariantRef result_;
};

#endif // CSVFILTER_OPERAND_H
