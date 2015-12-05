//
// csvfilter, Copyright (c) 2015, plnu
//

#include "../test.h"

#include <app/filterExpression/expression.h>
#include <app/headers.h>

#include <string.h>

static void testFailedParse(const std::string& expression,
                            const std::string& headerLine,
                          const ParseError expectedError) {
    std::stringstream group;
    group << "Parsing bad expression " << expression;
    Test::beginGroup(group.str());

    LineParser l;
    char* line = strdup(headerLine.c_str());
    l.parse(line);
    Headers h(l, std::vector<std::string>() );
    Expression exp(expression, h);
    Test::eq(exp.ok(), false, "Lexing fails");
    Test::eq(exp.error(), expectedError, "Error message is correct");
    Test::endGroup();
    free(line);
}

static void testParse(const std::string input,
                      const std::string headerLine,
                      const std::string expectedTree) {
    std::stringstream group;
    group << "Parsing " << input;
    Test::beginGroup(group.str());

    LineParser l;
    char* line = strdup(headerLine.c_str());
    l.parse(line);
    Headers h(l, std::vector<std::string>() );

    Expression e(input, h);
    Test::eq(e.ok(), true, "Expression parser is ok");
    Test::eq(e.treeString(), expectedTree, "tree is ok");
    Test::endGroup();
    free(line);
}

static void testEval(const std::string expr,
                     const std::string headerStr,
                     const std::string lineStr,
                     VariantRef result) {
    std::stringstream group;
    group << "Eval " << expr;
    Test::beginGroup(group.str());

    LineParser l;
    char* headers = strdup(headerStr.c_str());
    l.parse(headers);
    Headers h(l, std::vector<std::string>() );

    Expression e(expr, h);
    Test::eq(e.ok(), true, "Expression parser is ok");

    char* line = strdup(lineStr.c_str());
    l.parse(line);

    VariantRef v = e.eval(l);

    if (Test::eq(v->type(), result->type(), "Result is the correct type")) {
        switch (result->type()) {
        case Variant::NUMBER:
            Test::eq(v->numberVal(),
                     result->numberVal(),
                     "double value is correct");
            break;
        case Variant::BOOLEAN:
            Test::eq(v->booleanVal(),
                     result->booleanVal(),
                     "boolean value is correct");
            break;
        case Variant::STRING:
        case Variant::ERROR:
            Test::eq(v->charVal(),
                     result->charVal(),
                     "char value is correct");
            break;
        default:
            Test::that(false, "Unrecognised type");
            break;
        };
    }

    Test::endGroup();
    free(headers);
    free(line);
}

void expressionParserTests() {
    Test::beginSuite("Expression parsing");
    testParse("token", "token", "token~0:unknown"); // simple token
    testParse("t1 + t3", "t1,t2,t3", "(+ t1~0:unknown t3~2:unknown):unknown");
    testParse("1-2+3", "a", "(+ (- 1:number 2:number):number 3:number):number");
    testParse("1*3", "a", "(* 1:number 3:number):number");
    testParse("1-2*3", "a", "(- 1:number (* 2:number 3:number):number):number");
    testParse(
        "1/2+3*4",
        "a",
        "(+ (/ 1:number 2:number):number (* 3:number 4:number):number):number");
    testParse("1-2*3", "a", "(- 1:number (* 2:number 3:number):number):number");

    // this error is passed up from the lexer
    testFailedParse("\"an incomplete ", "a",
                    ParseError("Unterminated string constant", Range(0, 15)));
    // missing operand at end
    testFailedParse("1 +", "a",
                    ParseError("Unexpected end of expression", Range(3, 4)));
    // missing operand between operators
    testFailedParse("1 * * 2", "a",
                    ParseError("Unexpected operator", Range(4, 5)));
    // missing operator
    testFailedParse("1 2", "a",
                    ParseError("Unexpected operand", Range(2, 3)));
    // unrecognised identifier
    testFailedParse("1 + col", "nocol",
                    ParseError("Identifier \"col\" not found in headers",
                               Range(4, 7)));

    // Tests for type detection

    // * can only be applied to numbers
    testParse("a*b", "a,b", "(* a~0:number b~1:number):number");

    // + can be applied to strings and numbers
    testParse("a+b", "a,b", "(+ a~0:unknown b~1:unknown):unknown");

    // Comparisons don't tell you anything about the operands, but
    // return a bool
    testParse("a<b", "a,b", "(< a~0:unknown b~1:unknown):boolean");

    // You can deduce the type of one operand from the other
    testParse("a<1", "a,b", "(< a~0:number 1:number):boolean");
    testParse("a<\"b\"", "a,b", "(< a~0:string b:string):boolean");

    testParse("-1", "a,b", "(- 1:number):number");
    testParse("--1", "a,b", "(- (- 1:number):number):number");
    testParse("3 + -1", "a,b", "(+ 3:number (- 1:number):number):number");
    testParse("3 - -1", "a,b", "(- 3:number (- 1:number):number):number");
    testParse("3 - --1", "a,b",
              "(- 3:number (- (- 1:number):number):number):number");

    testFailedParse("-", "nocol",
                    ParseError("Unexpected end of expression", Range(1, 2)));
    testFailedParse("1+-", "nocol",
                    ParseError("Unexpected end of expression", Range(3, 4)));

    // logical operators act on bools
    testParse("1<2 || a<b",
              "a,b",
              "(|| (< 1:number 2:number):boolean (< a~0:unknown b~1:unknown)"
                  ":boolean):boolean");
    
    // mismatched types
    testFailedParse(
        "\"a\" + 1", "a",
        ParseError("The + operator expects its arguments to be the "
                   "same type, got a string and a number",
                   Range(4, 5),
                   Range(0, 7)));
    
    testFailedParse(
        "1 && 3", "a",
        ParseError("The arguments to '&&' must be boolean, not number",
                   Range(2, 4),
                   Range(0, 1)));

    testFailedParse(
        "\"a\" / 2", "a",
        ParseError("Operand is the wrong type - "
                   "expected a number, but got a string",
                   Range(0, 3)));

    testFailedParse(
        "\"a\" + a + 2", "a",
        ParseError("The + operator expects its arguments to be the same type, "
                   "got a string and a number",
                   Range(8, 9),
                   Range(0, 11)));


    // braces
    testParse("1 * (3 + 5)",
              "a,b",
              "(* 1:number (+ 3:number 5:number):number):number");
           
    testFailedParse(
        "(1 + 2", "a",
        ParseError("Unmatched bracket",
                   Range(0, 1)));

    testFailedParse(
        ")", "a",
        ParseError("Unexpected operator",
                   Range(0, 1)));

    testFailedParse(
        "1)", "a",
        ParseError("No matching open brace for close brace",
                   Range(1, 2)));

    testFailedParse(
        "1 (", "a",
        ParseError("Unexpected end of expression",
                   Range(3, 4)));

    testFailedParse(
        "()", "a",
        ParseError("Unexpected operator", Range(1, 2)));

    // constant string
    testEval(
        "\"a\"",
        "header",
        "value",
        Variant::string("a"));

    // constant number
    testEval(
        "123.0",
        "header",
        "value",
        Variant::number(123.0));

    testEval(
        "a",
        "a",
        "value",
        Variant::string("value"));

    testEval(
        "c",
        "a,b,c",
        "vala,valb,valc",
        Variant::string("valc"));

    // without any hints as to type lone operands will return a string
    testEval(
        "a",
        "a",
        "10",
        Variant::string("10"));

    testEval("a + 10", "a", "20", Variant::number(30));
    testEval("a + \"end\"", "a", "abcdef", Variant::string("abcdefend"));
    testEval("a + \"end\"", "a", "123", Variant::string("123end"));
    
    testEval(
        "a + 10",
        "a",
        "abc",
        Variant::error("Left hand side of operator at 2: expected number, got "
                       "string"));
    
    testEval(
        "a - 10",
        "a",
        "abc",
        Variant::error("Left hand side of operator at 2: expected number, got "
                       "string"));
    
    testEval("a - 10", "a", "5", Variant::number(-5));
    testEval("a * 10", "a", "7", Variant::number(70));
    testEval("a / 10", "a", "3", Variant::number(0.3));
    
    testEval("a < 10", "a", "3", Variant::boolean(true));
    testEval("a < 10", "a", "12", Variant::boolean(false));
    testEval("a < 10", "a", "10", Variant::boolean(false));
    testEval("a < 10", "a","blah",  Variant::error("Left hand side of operator "
                                                   "at 2: expected number, got "
                                                   "string"));
    testEval("10 < 10 - a",
             "a",
             "abc",
             Variant::error("Right hand side of operator at 8: expected number,"
                            " got string"));

    testEval("a <= 11", "a", "3", Variant::boolean(true));
    testEval("a <= 11", "a", "12", Variant::boolean(false));
    testEval("a <= 11", "a", "11", Variant::boolean(true));

    testEval("a == 11", "a", "3", Variant::boolean(false));
    testEval("a == 11", "a", "11", Variant::boolean(true));
    testEval("a == 11", "a", "11.1", Variant::boolean(false));

    testEval("a != 11", "a", "3", Variant::boolean(true));
    testEval("a != 11", "a", "11", Variant::boolean(false));
    testEval("a != 11", "a", "11.1", Variant::boolean(true));

    testEval("a > 10", "a", "3", Variant::boolean(false));
    testEval("a > 10", "a", "12", Variant::boolean(true));
    testEval("a > 10", "a", "10", Variant::boolean(false));
    
    testEval("a >= 10", "a", "3", Variant::boolean(false));
    testEval("a >= 10", "a", "12", Variant::boolean(true));
    testEval("a >= 10", "a", "10", Variant::boolean(true));
    testEval("a + 10 >= 10",
             "a",
             "abc",
             Variant::error("Left hand side of operator at 2: expected number, "
                            "got string"));


    testEval("a < \"f\"", "a", "abc", Variant::boolean(true));
    testEval("a < \"f\"", "a", "f", Variant::boolean(false));
    testEval("a < \"f\"", "a", "g", Variant::boolean(false));
    testEval("a < \"f\"", "a", "3", Variant::boolean(true)); // will be cast to
                                                             // string

    testEval("a <= \"f\"", "a", "abc", Variant::boolean(true));
    testEval("a <= \"f\"", "a", "f", Variant::boolean(true));
    testEval("a <= \"f\"", "a", "g", Variant::boolean(false));
    
    testEval("a == \"f\"", "a", "ab", Variant::boolean(false));
    testEval("a == \"f\"", "a", "f", Variant::boolean(true));
    testEval("a == \"f\"", "a", "fa", Variant::boolean(false));
    
    testEval("a != \"f\"", "a", "ab", Variant::boolean(true));
    testEval("a != \"f\"", "a", "f", Variant::boolean(false));
    testEval("a != \"f\"", "a", "fa", Variant::boolean(true));
    
    testEval("a >= \"f\"", "a", "abc", Variant::boolean(false));
    testEval("a >= \"f\"", "a", "f", Variant::boolean(true));
    testEval("a >= \"f\"", "a", "g", Variant::boolean(true));
    
    testEval("a > \"f\"", "a", "abc", Variant::boolean(false));
    testEval("a > \"f\"", "a", "f", Variant::boolean(false));
    testEval("a > \"f\"", "a", "g", Variant::boolean(true));

    // logical operators
    // both true
    testEval("1 < 2 && 3 < 4", "a", "notused", Variant::boolean(true));
    // lh true
    testEval("1 < 2 && 7 < 4", "a", "notused", Variant::boolean(false));
    // rh true
    testEval("8 < 2 && 1 < 4", "a", "notused", Variant::boolean(false));
    // all false
    testEval("8 < 2 && 5 < 4", "a", "notused", Variant::boolean(false));
    // lh false, rhs not evaluated
    testEval("8 < 2 && a < 4", "a", "notused", Variant::boolean(false));

    // both true
    testEval("1 < 2 || 3 < 4", "a", "notused", Variant::boolean(true));
    // lh true
    testEval("1 < 2 || 7 < 4", "a", "notused", Variant::boolean(true));
    // rh true
    testEval("8 < 2 || 1 < 4", "a", "notused", Variant::boolean(true));
    // all false
    testEval("8 < 2 || 5 < 4", "a", "notused", Variant::boolean(false));
    // lh true, rhs not evaluated
    testEval("8 < 12 || a < 4", "a", "notused", Variant::boolean(true));

    // unary minus
    testEval("-3", "a", "1", Variant::number(-3));
    testEval("--3", "a", "1", Variant::number(3));
    testEval("1 - -3", "a", "1", Variant::number(4));
    testEval("1 + -7", "a", "1", Variant::number(-6));
    testEval("-a", "a", "1", Variant::number(-1));

    Test::endSuite();
}
