//
// csvfilter, Copyright (c) 2015, plnu
//

#include "parseError.h"

#include <assert.h>

/**
 * @brief Constructor.
 * 
 * Construct an empty range
 */
Range::Range() : begin(-1), end(-1) {

}

/**
 * @brief Constructor.
 *
 * Construct a range by specifying the beginning and end point
 *
 * @param beginVal  The beginning of the range
 * @param endVal    The end of the range
 */
Range::Range(int beginVal, int endVal) :begin(beginVal), end(endVal) {
    assert(begin >= 0);
    assert(end >= 0);
}

/**
 * @brief Equality check.
 */
const bool Range::operator == (const Range& rhs) const {
    return (begin == rhs.begin && end == rhs.end);
}

/**
 * @brief Inequality check.
 */
const bool Range::operator != (const Range& rhs) const {
    return (begin != rhs.begin || end != rhs.end);
}

/**
 * @brief Constructor.
 *
 * Construct a blank error message.
 */
ParseError::ParseError()
    :message_(""), primary_(), secondary_(){

}

/**
 * @brief Constructor
 *
 * @param message  The error message
 * @param primary  The part of the expression that caused the error
 *
 */
ParseError::ParseError(const std::string& message, const Range& primary)
    :message_(message), primary_(primary), secondary_(){

}

/**
 * @brief Constructor
 *
 * If an error message has both a primary and secondary range associated then
 * the primary range is typically an operand and the secondary is an
 * operand. For example, if you try to apply the '/' operator to a string, then
 * the primary range would point at the '/' operator, and the secondary range
 * would point to the string.
 *
 * @param message    The error message
 * @param primary    The primary source of the error
 * @param secondary  The secondary source of the error
 *
 */
ParseError::ParseError(const std::string& message,
                       const Range& primary,
                       const Range& secondary)
    :message_(message), primary_(primary), secondary_(secondary) {

}
ParseError::~ParseError() {

}

/**
 * @brief The error message.
 * 
 * @return The error message
 *
 */
std::string ParseError::getMessage() const {
    return message_;
}

/**
 * @brief The primary range
 *
 * @return The primary part of the expression that caused the error
 *
 */
Range ParseError::getPrimaryRange() const {
    return primary_;
}

/**
 * @brief The secondary range
 *
 * @return The secondary part of the expression that caused the error
 *
 */
Range ParseError::getSecondaryRange() const {
    return secondary_;
}

/**
 * @brief Equality check
 *
 */
const bool ParseError::operator == (const ParseError& rhs) const {
    return message_ == rhs.message_ &&
        primary_ == rhs.primary_ &&
        secondary_ == rhs.secondary_;
}
