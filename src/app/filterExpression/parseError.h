//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef CSVFILTER_PARSE_ERROR_H
#define CSVFILTER_PARSE_ERROR_H

#include <string>

/**
 * @brief A range of characters
 *
 * A range of characters in an expression. Note that this range includes the character at begin, but not the character at end.
 *
 */
struct Range {
    Range();
    Range(int begin, int end);
    const bool operator == (const Range& rhs) const;
    const bool operator != (const Range& rhs) const;

    /// @brief  The beginning of the range
    int begin;
    /// @brief  One past the end of the range
    int end;
};

/**
 * @brief An error
 *
 * This class represents an error message. It includes the message itself, and
 * up to two ranges indicating which parts of the filter expression triggered
 * the message.
 *
 */
class ParseError {
public:
    ParseError();
    ParseError(const std::string& message, const Range& primary);
    ParseError(const std::string& message,
               const Range& primary,
               const Range& secondary);
    ~ParseError();

    std::string getMessage() const;
    Range getPrimaryRange() const;
    Range getSecondaryRange() const;

    const bool operator == (const ParseError& rhs) const;
private:
    std::string message_;
    Range primary_;
    Range secondary_;
};


#endif // CSVFILTER_PARSE_ERROR_H
