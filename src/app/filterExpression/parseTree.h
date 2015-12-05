//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef CSVFILTER_PARSE_TREE_H
#define CSVFILTER_PARSE_TREE_H

#include "lexToken.h"
#include "variant.h"
#include "../lineParser.h"

#include <ostream>
#include <memory>

class ParseTree;

/**
 * @brief A reference to a parse tree.
 *
 * A reference to a parse tree
 *
 */
typedef std::shared_ptr<ParseTree> ParseTreeRef;

/**
 * @brief  Base class for parsed filter expressions. 
 *
 * This class is the base class for nodes in the parse tree for filter
 * expressions. It is abstract, the different types of nodes are actually
 * implemented by subclasses.
 *
 */
class ParseTree {
public:
    /**
     * @brief The return type of a node.
     *
     * This is used to represent the return type of a node in the parse tree
     * when it is evaluated.
     *
     */
    typedef enum {
        NODE_TYPE_UNKNOWN, /**< The type cannot be calculated (it depends on the
                            * content of the csv file). The only types we cannot
                            * differentiate between are NODE_TYPE_STRING and
                            * NODE_TYPE number, so the actual type will be one
                            * of those. */
        NODE_TYPE_STRING,  /**< String */
        NODE_TYPE_NUMBER,  /**< Number */
        NODE_TYPE_BOOL,    /**< Boolean */
        NODE_TYPE_ERROR    /**< Error - the parse tree is not valid */
    } NodeType;


    ParseTree();
    virtual ~ParseTree();

    /**
     * @brief  Traverse the parse tree, validating that types match.
     *
     * This function must be called on the top of the tree after it has been
     * constructed and before any evaluation takes place. It recursively
     * traverses the tree (calling ParseTree::validateTypes on all the child
     * nodes), making sure that all types are valid - for example, we confirm
     * that we're not comparing strings to integers (e.g. 3 == "a"). As part of
     * this pass we attempt to deduce the types of as many csv fields as
     * possible. In expressions like field == 3 we can deduce that field is of
     * type number, but that's not always possible - consider the expression
     * field1 == field2.
     *
     * @param err  If this function returns NODE_TYPE_ERROR, then err
     *             will be updated to contain details of the error.
     *
     * @return  The calculated type that this parse tree will return when
     *          evaluated. If the return type cannot be determined then
     *          NODE_TYPE_UNKNOWN will be returned, and if the types in the
     *          parse tree are invalid then NODE_TYPE_ERROR will be returned.
     *
     */
    virtual NodeType validateTypes(ParseError& err) = 0;

    /**
     * @brief Provide a type hint to a node.
     * 
     * Provide a type hint about what type this node should return. This is used
     * internally as part of the validateTypes pass, when the type of a subtree
     * can be deduced from the other argument to an operator.
     *
     * @param t        The type that this tree should attempt to return. This
     *                 must be one of NODE_TYPE_STRING or NODE_TYPE_NUMBER, as
     *                 those are the only types that are ambiguous.
     * @param err      If this function returns false, then this parameter
     *                 will be updated to contain details of the error.
     *
     * @return  true if the tree can be coerced to the type t, false otherwise,
     *          in which case errText will contain a descriptive error message.
     */
    virtual bool setType(NodeType t, ParseError& err) = 0;

    /**
     * @brief  Evaluate the parse tree
     *
     * Evaluate the parse tree in the context of a row from the csv file. This
     * is done by recursively calling eval on each node of the tree.
     *
     * @param line      The current line of the csv file
     * @param typeHint  A hint as to the type that should be returned. Used when
     *                  called recursively, set this to NODE_TYPE_UNKNOWN when
     *                  calling externally.
     *
     * @return  The result of the expression. Note that this Variant may contain
     *          an error if the expression could not be evaluated
     */
    virtual VariantRef eval(const LineParser& line,
                            NodeType typeHint) const = 0;

    /**
     * @brief  Can this node be a number?
     *
     * Check whether this node can evaluate to a number in the context of the
     * current line.
     *
     * @param line  The current line
     *
     * @return  true if the node can return a number, false otherwise.
     *
     */
    virtual bool canBeNumber(const LineParser& line) const = 0;

    /**
     *
     * Stream a string representation of this parse tree, for debugging
     * purposes.
     *
     * @see ParseTree::toString.
     *
     * @param out  The stream to write to
     *
     */
    virtual void stream(std::ostream& out) = 0;

    /**
     *
     * @brief The location of this node in the expression.
     *
     * The position in the original expression this part of the tree
     * represents. This includes the sub-trees, so is generally longer then
     * LexToken::position. It is used for reporting errors - it allows us to
     * underline parts of the expression that were not valid.
     *
     * @return  A range that covers the location of this node.
     *
     */
    virtual Range position() const = 0;

    std::string toString();


    static ParseTreeRef makeOperand(ConstLexTokenRef op);
    static ParseTreeRef makeOperand(ConstLexTokenRef op, int pos);

    static ParseTreeRef makeUnaryOperator(ConstLexTokenRef op,
                                          ParseTreeRef operand);

    static ParseTreeRef makeBinaryOperator(ConstLexTokenRef op,
                                           ParseTreeRef lhs,
                                           ParseTreeRef rhs);
protected:
    
private:
    ParseTree(const ParseTree& other);
    ParseTree& operator=(const ParseTree& other);
};


std::ostream& operator<< (std::ostream &out, ParseTree::NodeType t);

#endif // CSVFILTER_PARSE_TREE_H
