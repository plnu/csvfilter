//
// csvfilter, Copyright (c) 2015, plnu
//

#include <app/filterExpression/lexer.h>

#include "../test.h"

#include <vector>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sstream>

static void testToken(ConstLexTokenRef t,
                      LexToken::Type expectedType,
                      const std::string& expectedValue,
                      Range expectedPosition) {
  std::stringstream msg;
    msg << "Type is " << expectedType;
    Test::eq(t->type(), expectedType, msg.str());
    Test::eq(t->value(), expectedValue, "Expected value");
    Test::that(t->position() ==  expectedPosition,
               "Expected position is correct");
}

static void testEmptyString() {
    Test::beginGroup("Parse empty string");
    Lexer l("");
    Test::that(l.ok(), "Lexer is ok");
    testToken(l.pop(), LexToken::TYPE_EOF, "", Range(0, 1));
    Test::endGroup();
}

static void testSimpleToken(const std::string& input,
                            LexToken::Type expectedType) {
    std::stringstream group;
    group << "Simple token " << input;
    Test::beginGroup(group.str());
    Lexer l(input);
    if (Test::that(l.ok(), "Lexer is ok")) {
        testToken(l.pop(),
                  expectedType,
                  input,
                  Range(0, input.length()));
        testToken(l.pop(),
                  LexToken::TYPE_EOF,
                  "",
                  Range(input.length(),
                        input.length() + 1));
    }
    Test::endGroup();
}

static void testForError(const std::string& input,
                         const ParseError& expectedError) {
    std::stringstream group;
    group << "Invalid input " << input;
    Test::beginGroup(group.str());
    Lexer l(input);
    Test::eq(l.ok(), false, "Lexer is not ok");
    Test::eq(l.err(), expectedError, "Lexer error is correct");
    Test::endGroup();
}

static void testStringToken(const std::string& input,
                            const std::string& tokenValue) {
    std::stringstream group;
    group << "String token " << input;
    Test::beginGroup(group.str());
    Lexer l(input);
    if (Test::that(l.ok(), "Lexer is ok")) {
        testToken(l.pop(),
                  LexToken::TYPE_STRING,
                  tokenValue,
                  Range(0, input.length()));
        testToken(l.pop(),
                  LexToken::TYPE_EOF,
                  "",
                  Range(input.length(), input.length() + 1));
    }
    Test::endGroup();
}

static void testMultipleTokens() {
    Test::beginGroup("Multiple tokens");
    Lexer l("  1 + 2 - \"abc\"\t/\n7  ");

    if (Test::that(l.ok(), "Lexer is ok")) {
        testToken(l.pop(), LexToken::TYPE_NUMBER, "1", Range(2, 3));
        testToken(l.pop(), LexToken::TYPE_PLUS, "+", Range(4, 5));
        testToken(l.pop(), LexToken::TYPE_NUMBER, "2", Range(6, 7));
        testToken(l.pop(), LexToken::TYPE_MINUS, "-", Range(8, 9));
        testToken(l.pop(), LexToken::TYPE_STRING, "abc", Range(10, 15));
        testToken(l.pop(), LexToken::TYPE_DIVIDE, "/", Range(16, 17));
        testToken(l.pop(), LexToken::TYPE_NUMBER, "7", Range(18, 19));
        testToken(l.pop(), LexToken::TYPE_EOF, "", Range(21, 22));
    }

    Test::endGroup();
}

static void testIsIdentifier(const std::string& id) {
    std::stringstream msg;
    Test::that(Lexer::isIdentifier(id), msg.str());
}

static void testNotIdentifier(const std::string& id) {
    std::stringstream msg;
    msg << "'" << id << "' is not an identifier";
    Test::that(!Lexer::isIdentifier(id), msg.str());
}

static void testCreateIdentifier(const std::string& orig,
                                 const std::string& result) {
    std::stringstream msg;
    msg << "'" << orig << "' becomes " << result;
    Test::eq(Lexer::makeValidIdentifier(orig), result, msg.str());
}


static void testIdentifierFunctions() {
    Test::beginGroup("Identifier validation");

    testIsIdentifier("abc");
    testIsIdentifier("a1");
    testIsIdentifier("a_1");
    testIsIdentifier("vfdjvhjkdfhvdkjvhdfjkvhdfjkvhdfjvdkvjkfvhdfjhvdkv");
    testIsIdentifier("_1");
    testNotIdentifier("");
    testNotIdentifier("123.4");
    testNotIdentifier("1abc");
    testNotIdentifier(" fjiew ");
    testNotIdentifier(" ");
    testNotIdentifier("a ");
    testNotIdentifier("1");
    testNotIdentifier("-a");
    Test::endGroup();

    Test::beginGroup("Identifier creation");
    testCreateIdentifier("a", "a");
    testCreateIdentifier("", "_");
    testCreateIdentifier("1b", "_1b");
    testCreateIdentifier(" ", "_");
    testCreateIdentifier("a heading", "a_heading");
    Test::endGroup();

}


void lexerTests() {
    Test::beginSuite("Lexer");

    testEmptyString();
    testSimpleToken("+", LexToken::TYPE_PLUS);
    testSimpleToken("-", LexToken::TYPE_MINUS);
    testSimpleToken("*", LexToken::TYPE_TIMES);
    testSimpleToken("/", LexToken::TYPE_DIVIDE);
    testSimpleToken("<", LexToken::TYPE_LT);
    testSimpleToken("<=", LexToken::TYPE_LTE);
    testSimpleToken("==", LexToken::TYPE_EQ);
    testSimpleToken("!=", LexToken::TYPE_NEQ);
    testSimpleToken(">", LexToken::TYPE_GT);
    testSimpleToken(">=", LexToken::TYPE_GTE);
    testSimpleToken("&&", LexToken::TYPE_AND);
    testSimpleToken("||", LexToken::TYPE_OR);
    testForError("=",
                 ParseError("Unrecognised token. Did you mean  '=='?",
                            Range(0, 1)));
    testForError("!",
                 ParseError("Unrecognised token. Did you mean  '!='?",
                            Range(0, 1)));
    testForError("&",
                 ParseError("Unrecognised token. Did you mean  '&&'?",
                            Range(0, 1)));
    testForError("|",
                 ParseError("Unrecognised token. Did you mean  '||'?",
                            Range(0, 1)));
    testSimpleToken("abcdef", LexToken::TYPE_IDENTIFIER);
    testSimpleToken("ABCDE", LexToken::TYPE_IDENTIFIER);
    testSimpleToken("ZdsajA", LexToken::TYPE_IDENTIFIER);
    testSimpleToken("_FFEI", LexToken::TYPE_IDENTIFIER);
    testSimpleToken("A01234567789Z", LexToken::TYPE_IDENTIFIER);
    testStringToken("\"ABC\"", "ABC");
    testStringToken("\"A space\"", "A space");
    testStringToken("\"A \\\"\"", "A \""); // escaped quote
    testStringToken("\"A \\ \"", "A \\ "); // escape doesn't escape anything
                                           // else
    testForError("\"abc",
                 ParseError("Unterminated string constant", Range(0, 4)));
    testForError("\"ab\\",
                 ParseError("Unterminated string constant", Range(0, 4)));
    testForError("\"ab\\\"",
                 ParseError("Unterminated string constant", Range(0, 5)));
    testSimpleToken("1234", LexToken::TYPE_NUMBER);
    testSimpleToken("12.324", LexToken::TYPE_NUMBER);

    testMultipleTokens();

    testIdentifierFunctions();

    Test::endSuite();
}

