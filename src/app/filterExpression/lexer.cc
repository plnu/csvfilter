//
// csvfilter, Copyright (c) 2015, plnu
//

#include "lexer.h"

#include <sstream>
#include <assert.h>
#include <iostream>

/**
 * @brief Constructor
 *
 * Create a new lexer. The entire input string is lexed immediately, and
 * Lexer::ok should be checked to see if it was successful. If it was,
 * Lexer::pop can be used to get the tokens one at a time, and if it wasn't then
 * Lexer::errText can be used to get an error message.
 *
 * @param input  The string to lex.
 *
 */
Lexer::Lexer(const std::string& input)
    :ok_(true), err_(), queue_() {
    size_t pos = 0;
    consumeWhitespace(input, pos);
    while (ok_ && pos < input.length()) {
        ok_ = consumeToken(input, pos);
        consumeWhitespace(input, pos);
    }

    if (ok_) {
        push(LexToken::TYPE_EOF, Range(pos, pos + 1), "");
    } else {
        queue_ = std::queue<LexTokenRef>();
    }
}

/**
 * @brief Destructor
 *
 */
Lexer::~Lexer() {

}

/**
 * @brief Could the input be lexed?
 *
 * @return true if the input was lexed successfully, false otherwise.
 * @see  Lexer::errText
 */
bool Lexer::ok() {
    return ok_;
}

/**
 * @brief  An error message
 *
 * If the input string could not be lexed (i.e. Lexer::ok returns false), then
 * this function will return a descriptive error message.
 *
 * @return  an error message, or blank string if Lexer::ok is true.
 */
const ParseError Lexer::err() {
    return err_;
}

/**
 * @brief  Pop the next token
 *
 * Pop the next token off the queue. The token is removed, so repeated calls to
 * pop will work through the tokens from the input stream until the end is
 * reached. Once that happens, future calls to pop return a token of type
 * LexToken::TYPE_EOF.
 *
 * @return  The next token
 *
 */
LexTokenRef Lexer::pop() {
    LexTokenRef ret = queue_.front();
    if (queue_.size() > 1) {
        queue_.pop();
    } else {
        // There's only the EOF token left, leave it in place
    }
    return ret;
}

/**
 *
 * @brief Can the argument be used as an identifier?
 *
 * Check whether the supplied string can be used as an identifier in filter
 * expressions. This is used to check whether headers can be used as-is, or they
 * need to be assigned aliases.
 *
 * @param token  The string to check
 *
 * @return  true if it can be used as an identifier, or false otherwise.
 *
 */
bool Lexer::isIdentifier(const std::string& token) {
    bool isToken = token.length() > 0 && isIdentifierStartChar(token.at(0));

    for (int i = 1; isToken && i < token.length(); i++) {
        isToken = isIdentifierContinuedChar(token.at(i));
    }

    return isToken;
}

/**
 *
 * @brief Convert a string into an identifier.
 *
 * Convert the supplied string into one that can be used as an identifier. This
 * is done by replacing all unsuitable characters with underscores.
 *
 * @param token  The string to convert
 *
 * @return  The modified string
 *
 */
std::string Lexer::makeValidIdentifier(const std::string& token) {
    std::stringstream identifier;

    if (token.length() == 0) {
        identifier << "_";
    } else if (!isIdentifierStartChar(token.at(0))) {
        identifier << "_";
        if (isIdentifierContinuedChar(token.at(0))) {
            identifier << token.at(0);
        }
    } else {
        identifier << token.at(0);
    }

    for (int i = 1; i < token.length(); i++) {
        if (isIdentifierContinuedChar(token.at(i))) {
            identifier << token.at(i);
        } else {
            identifier << "_";
        }
    }

    return identifier.str();
}

void Lexer::consumeWhitespace(const std::string& input, size_t& pos) {
    while (pos < input.length() &&
           (input.at(pos) == ' ' ||
            input.at(pos) == '\t' ||
            input.at(pos) == '\n')) {
        pos++;
    }
}

bool Lexer::consumeToken(const std::string& input, size_t& pos) {
    bool consumed = true;
    if (isIdentifierStartChar(input.at(pos))) {
        consumeIdentifier(input, pos);
    } else if (isDigit(input.at(pos))) {
        ok_ = consumeNumber(input, pos);
    } else {
        switch (input.at(pos)) {
        case '+':
            push(LexToken::TYPE_PLUS, Range(pos, pos + 1), "+");
            pos++;
            break;
        case '-':
            push(LexToken::TYPE_MINUS, Range(pos, pos + 1), "-");
            pos++;
            break;
        case '*':
            push(LexToken::TYPE_TIMES, Range(pos, pos + 1), "*");
            pos++;
            break;
        case '/':
            push(LexToken::TYPE_DIVIDE, Range(pos, pos + 1), "/");
            pos++;
            break;
        case '(':
            push(LexToken::TYPE_OPEN_BRACKET, Range(pos, pos + 1), "(");
            pos++;
            break;
        case ')':
            push(LexToken::TYPE_CLOSE_BRACKET, Range(pos, pos + 1), ")");
            pos++;
            break;
        case '<':
            consumeLtToken(input, pos);
            break;
        case '>':
            consumeGtToken(input, pos);
            break;
        case '=':
            consumed = consumeToken(input, pos, "==", LexToken::TYPE_EQ);
            break;
        case '!':
            consumed = consumeToken(input, pos, "!=", LexToken::TYPE_NEQ);
            break;
        case '&':
            consumed = consumeToken(input, pos, "&&", LexToken::TYPE_AND);
            break;
        case '|':
            consumed = consumeToken(input, pos, "||", LexToken::TYPE_OR);
            break;
        case '"':
            consumed = consumeString(input, pos);
            break;
        default:
            consumed = false;
            err_ = ParseError("Unexpected token", Range(pos, pos + 1));
            break;
        }
    }
    return consumed;
}

void Lexer::consumeLtToken(const std::string& input, size_t& pos) {
    assert(input.at(pos) == '<');
    if (pos + 1 < input.length() && input.at(pos + 1) == '=') {
        push(LexToken::TYPE_LTE, Range(pos, pos + 2), "<=");
        pos += 2;
    } else {
        push(LexToken::TYPE_LT, Range(pos, pos + 1), "<");
        pos++;
    }
}

void Lexer::consumeGtToken(const std::string& input, size_t& pos) {
    assert(input.at(pos) == '>');
    if (pos + 1 < input.length() && input.at(pos + 1) == '=') {
        push(LexToken::TYPE_GTE, Range(pos, pos + 2), ">=");
        pos += 2;
    } else {
        push(LexToken::TYPE_GT, Range(pos, pos + 1), ">");
        pos++;
    }
}

bool Lexer::consumeToken(const std::string& input,
                         size_t& pos,
                         const std::string& expectedToken,
                         LexToken::Type type) {
    bool valid = true;
    for (int i = 0; valid && i < expectedToken.length(); i++) {
        if (pos + i >= input.length() ||
            input.at(pos + i) != expectedToken.at(i)) {
            valid = false;
            std::stringstream msg;
            msg << "Unrecognised token. Did you mean  '"
                << expectedToken << "'?";
            err_ = ParseError(msg.str(), Range(pos, pos + 1));
        }
    }
    if (valid) {
        push(type, Range(pos, pos + expectedToken.length()), expectedToken);
        pos += expectedToken.length();
    }
    return valid;
}

void Lexer::consumeIdentifier(const std::string& input, size_t& pos) {
    int start = pos;
    while (pos < input.length() &&
           isIdentifierContinuedChar(input.at(pos))) {
        pos++;
    }
    push(LexToken::TYPE_IDENTIFIER,
         Range(start, pos),
         input.substr(start, pos - start));
}

bool Lexer::consumeString(const std::string& input, size_t& pos) {
    bool success = false;
    int start = pos;
    std::stringstream val;

    assert(input.at(pos) == '"');
    pos++;

    while (pos < input.length() && input.at(pos) != '"') {
        if (input.at(pos) == '\\' &&
            pos + 1 < input.length() &&
            input.at(pos + 1) == '"') {
            pos++;
        }
        val << input.at(pos);
        pos++;
    }

    if (pos >= input.length()) {
        err_ = ParseError("Unterminated string constant",
                          Range(start, input.length()));
    } else {
        // consume closing quote
        pos++;
        success = true;
        push(LexToken::TYPE_STRING, Range(start, pos), val.str());
    }
    return success;
}

bool Lexer::consumeNumber(const std::string& input, size_t& pos) {
    assert(isDigit(input.at(pos)));
    bool consumed = true;
    size_t start = pos;
    skipDigits(input, pos);

    if (pos < input.length() && input.at(pos) == '.') {
        pos++;
        if (!skipDigits(input, pos)) {
            consumed = false;
            err_ = ParseError("Expected digits after decimal place",
                              Range(start, pos));
        }
    }

    if (consumed) {
        push(LexToken::TYPE_NUMBER,
             Range(start, pos),
             input.substr(start, pos - start));
    }

    return consumed;
}

bool Lexer::skipDigits(const std::string& input, size_t& pos) {
    bool foundDigits = false;
    while (pos < input.length() && isDigit(input.at(pos))) {
        pos++;
        foundDigits = true;
    }
    return foundDigits;
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isIdentifierStartChar(char c) {
    return ( (c >= 'A' && c <= 'Z') ||
             (c >= 'a' && c <= 'z') ||
             c == '_');
}

bool Lexer::isIdentifierContinuedChar(char c) {
    return isIdentifierStartChar(c) || (c >= '0' && c <= '9');
}

void Lexer::push(LexToken::Type type, Range pos, const std::string& token) {
    queue_.push(LexTokenRef(new LexToken(type, pos, token)));
}
