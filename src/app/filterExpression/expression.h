//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef CSVFILTER_EXPRESSION_PARSER_H
#define CSVFILTER_EXPRESSION_PARSER_H

#include "parseTree.h"
#include "lexer.h"
#include "../headers.h"
#include "variant.h"

#include <sstream>
#include <string>
#include <stack>

/**
 * @brief  Parse and evaluate a filter expression
 *
 * This class parses and evaluates a filter expression. The expression is parsed
 * at construction time, and assuming it parses successfully it can be used to
 * evaluate it in the context of a line from the csv file.
 *
 */
class Expression {
public:
    Expression(const std::string& expression, const Headers& headers);
    ~Expression();

    bool ok() const;
    const ParseError error() const;

    VariantRef eval(const LineParser& l);

    const std::string treeString() const;

private:
    Expression(const Expression& other);
    Expression& operator=(const Expression& other);

    typedef enum {
        EXPECT_OPERATOR,
        EXPECT_OPERAND
    } ExpectedToken;


    /**
     * This struct contains all the internal state used during the parse
     * operation. As the parsing is split across a few functions it's easier to
     * pass around a single object reference, rater than a number of different
     * arguments.
     *
     * It is not needed once the parsing is complete.
     *
     */
    typedef struct ParseState {
    public:
        ParseState(const std::string& input, const Headers& headers);
        Lexer lexer_;
        const Headers& headers_;
        std::stack<ConstLexTokenRef> operators_;
        std::stack<ParseTreeRef> operands_;
        ExpectedToken expectedToken_;
        bool done_;
    } ParseState;

    void makeParseTree(ParseState& state);
    void processOperator(ParseState& state);
    void processOperand(ParseState& state);
    
    void applyLastOperator(ParseState& state);
    void endBracketedExpression(ConstLexTokenRef closeBrace, ParseState& state);
    
    bool ok_;
    ParseError error_;
    ParseTreeRef tree_;
    ParseTree::NodeType resultType_;
};

#endif // CSVFILTER_EXPRESSION_PARSER_H

