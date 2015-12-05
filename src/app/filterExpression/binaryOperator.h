//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef CSVFILTER_BINARY_OPERATOR_H
#define CSVFILTER_BINARY_OPERATOR_H

#include "parseTree.h"

/**
 *
 * @brief handles the '-', '*' and '/' operators.
 *
 * The NumericBinaryOperator class handles the minus, times and divide
 * operators. Note that '+', which can act on strings too, is handled by
 * PlusBinaryOperator.
 *
 */
class NumericBinaryOperator : public ParseTree {
public:
    NumericBinaryOperator(ConstLexTokenRef op,
                          ParseTreeRef lhs,
                          ParseTreeRef rhs);
    virtual ~NumericBinaryOperator();

    virtual NodeType validateTypes(ParseError& err);
    virtual bool setType(NodeType t, ParseError& err);

    virtual VariantRef eval(const LineParser& line, NodeType typeHint) const;
    
    virtual void stream(std::ostream& out);
    virtual bool canBeNumber(const LineParser& line) const;
    virtual Range position() const;

private:
    NumericBinaryOperator(const NumericBinaryOperator& other);
    NumericBinaryOperator& operator=(const NumericBinaryOperator& other);

    NodeType validateOperandType(ParseTreeRef op, ParseError& err);

    ConstLexTokenRef op_;
    ParseTreeRef lhs_;
    ParseTreeRef rhs_;
    VariantRef result_;
};

/**
 * @brief handles the '&&' and '||' operators
 *
 * The LogicalBinaryOperator class handles the 'and' and 'or' operators. Unlike
 * C, we treat booleans as a separate type and expect the arguments to these
 * operators to be boolean - i.e. you cannot apply these operators to strings or
 * integers. To get a boolean use one of the comparison operators supplied by
 * ComparisonBinaryOperator.
 *
 */
class LogicalBinaryOperator : public ParseTree {
public:
    LogicalBinaryOperator(ConstLexTokenRef op,
                          ParseTreeRef lhs,
                          ParseTreeRef rhs);
    virtual ~LogicalBinaryOperator();

    virtual NodeType validateTypes(ParseError& err);
    virtual bool setType(NodeType t, ParseError& err);

    virtual VariantRef eval(const LineParser& line, NodeType typeHint) const;
    
    virtual void stream(std::ostream& out);
    virtual bool canBeNumber(const LineParser& line) const;
    virtual Range position() const;

private:
    LogicalBinaryOperator(const LogicalBinaryOperator& other);
    LogicalBinaryOperator& operator=(const LogicalBinaryOperator& other);

    NodeType validateOperandType(ParseTreeRef op,
                                 ParseError& err);

    ConstLexTokenRef op_;
    ParseTreeRef lhs_;
    ParseTreeRef rhs_;
};

/**
 * @brief handles the '<', '<=', '==', '!=', '>=' and '>' operators
 *
 * The ComparisonBinaryOperator class handles comparisons. The two operands must
 * be the same type, but may be either numbers or strings, and the returned
 * value will be a boolean.
 *
 */
class ComparisonBinaryOperator : public ParseTree {
public:
    ComparisonBinaryOperator(ConstLexTokenRef op,
                          ParseTreeRef lhs,
                          ParseTreeRef rhs);
    virtual ~ComparisonBinaryOperator();

    virtual NodeType validateTypes(ParseError& err);
    virtual bool setType(NodeType t, ParseError& err);

    virtual VariantRef eval(const LineParser& line, NodeType typeHint) const;
    
    virtual void stream(std::ostream& out);
    virtual bool canBeNumber(const LineParser& line) const;
    virtual Range position() const;

private:
    ComparisonBinaryOperator(const ComparisonBinaryOperator& other);
    ComparisonBinaryOperator& operator=(const ComparisonBinaryOperator& other);

    VariantRef evalComparisonNumber(const LineParser& line,
                                    NodeType typeHint) const;
    VariantRef evalComparisonString(const LineParser& line,
                                    NodeType typeHint) const;

    ConstLexTokenRef op_;
    ParseTreeRef lhs_;
    ParseTreeRef rhs_;
    VariantRef result_;
};


/**
 * @brief handles the '+' binary operator
 *
 * The PlusBinaryOperator class handles addition and string concatenation,
 * depending on the types of the operands.
 *
 */
class PlusBinaryOperator : public ParseTree {
public:
    PlusBinaryOperator(ConstLexTokenRef op,
                          ParseTreeRef lhs,
                          ParseTreeRef rhs);
    virtual ~PlusBinaryOperator();

    virtual NodeType validateTypes(ParseError& err);
    virtual bool setType(NodeType t, ParseError& err);

    virtual VariantRef eval(const LineParser& line, NodeType typeHint) const;
    
    virtual void stream(std::ostream& out);
    virtual bool canBeNumber(const LineParser& line) const;
    virtual Range position() const;

private:
    PlusBinaryOperator(const PlusBinaryOperator& other);
    PlusBinaryOperator& operator=(const PlusBinaryOperator& other);

    VariantRef evalPlusNumber(const LineParser& line,
                                    NodeType typeHint) const;
    VariantRef evalPlusString(const LineParser& line,
                                    NodeType typeHint) const;

    ConstLexTokenRef op_;
    ParseTreeRef lhs_;
    ParseTreeRef rhs_;
    NodeType calculatedType_;
    VariantRef result_;
    mutable char* addBuf_;
    mutable int addBufLen_;
};


#endif // CSVFILTER_BINARY_OPERATOR_H
