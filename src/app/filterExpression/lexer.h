//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef CSVFILTER_LEXER_H
#define CSVFILTER_LEXER_H

#include "lexToken.h"
#include "parseError.h"

#include <string>
#include <queue>

/**
 * @brief Lex a string.
 *
 * Turns an input string into a sequence of lexicographical tokens.
 *
 * This class is used as part of parsing any row filter expression that has
 * been supplied.
 */
class Lexer {
public:
    Lexer(const std::string& input);
    ~Lexer();

    bool ok();
    const ParseError err();

    LexTokenRef pop();

    static bool isIdentifier(const std::string& token);
    static std::string makeValidIdentifier(const std::string& token);

private:
    Lexer(const Lexer& other);
    Lexer& operator=(const Lexer& other);

    void consumeWhitespace(const std::string& input, size_t& pos);
    bool consumeToken(const std::string& input, size_t& pos);

    void consumeLtToken(const std::string& input, size_t& pos);
    void consumeGtToken(const std::string& input, size_t& pos);
    bool consumeToken(const std::string& input,
                      size_t& pos,
                      const std::string& expectedToken,
                      LexToken::Type type);
    void consumeIdentifier(const std::string& input, size_t& pos);
    bool consumeString(const std::string& input, size_t& pos);
    bool consumeNumber(const std::string& input, size_t& pos);

    bool skipDigits(const std::string& input, size_t& pos);
    bool isDigit(char c);
    void push(LexToken::Type type, Range pos, const std::string& token);

    static bool isIdentifierStartChar(char c);
    static bool isIdentifierContinuedChar(char c);

    bool ok_;
    ParseError err_;
    std::queue<LexTokenRef> queue_;
};



#endif // CSVFILTER_LEXER_H

