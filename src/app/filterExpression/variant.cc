//
// csvfilter, Copyright (c) 2015, plnu
//

#include "variant.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <ostream>

/**
 * @brief Constructor
 *
 * Constructor for string and error types.
 *
 * @param t    The type. Must be STRING or ERROR
 * @param val  The string value
 *
 */
Variant::Variant(Type t, const char* val)
    :type_(t) {
    val_.charVal_ = strdup(val);
}

/**
 * @brief Constructor
 *
 * Constructor for boolean types
 *
 * @param val  The boolean value to store
 */
Variant::Variant(bool val)
    :type_(BOOLEAN) {
    val_.boolVal_ = val;
}

/**
 * @brief Constructor
 *
 * Constructor for number types
 *
 * @param val  The double value to store
 */
Variant::Variant(double val)
    :type_(NUMBER) {
    val_.numberVal_ = val;
}

/**
 * @brief Destructor.
 *
 * Destructor.
 *
 */
Variant::~Variant() {
    clear();
}

/**
 * @brief Reset the variant to a number
 *
 * Change the value stored in this variant to a number. This is a performance
 * enhancement to avoid deleting and creating lots of variants.
 *
 * @param num  The new value
 *
 */
void Variant::resetToNumber(double num) {
    clear();
    type_ = NUMBER;
    val_.numberVal_ = num;
}

/**
 * @brief Reset the variant to a string
 *
 * Change the value stored in this variant to a string. This is a performance
 * enhancement to avoid deleting and creating lots of variants.
 *
 * @param val  The new value
 *
 */
void Variant::resetToString(const char* val) {
    clear();
    type_ = STRING;
    val_.charVal_ = strdup(val);
}

/**
 * @brief Reset the variant to a string
 *
 * Change the value stored in this variant to a string. This is a performance
 * enhancement to avoid deleting and creating lots of variants.
 *
 * @param val  The new value
 *
 */
void Variant::resetToString(const std::string& val) {
    clear();
    type_ = STRING;
    val_.charVal_ = strdup(val.c_str());
}

/**
 * @brief Reset the variant to an error
 *
 * Change the value stored in this variant to an error. This is a performance
 * enhancement to avoid deleting and creating lots of variants.
 *
 * @param err  The new value
 *
 */
void Variant::resetToError(const std::string& err) {
    clear();
    type_ = ERROR;
    val_.charVal_ = strdup(err.c_str());
}

/**
 * @brief Reset the variant to a boolean.
 *
 * Change the value stored in this variant to a boolean. This is a performance
 * enhancement to avoid deleting and creating lots of variants.
 *
 * @param b  The new value
 *
 */
void Variant::resetToBoolean(bool b) {
    clear();
    type_ = BOOLEAN;
    val_.boolVal_ = b;
}

/**
 * @brief Create a string
 *
 * Create a new variant containing a string
 *
 * @param val  The string to store.
 *
 * @return  A reference to the new variant
 *
 */
VariantRef Variant::string(const char* val) {
    return VariantRef(new Variant(STRING, val));
}

/**
 * @brief Create a string
 *
 * Create a new variant containing a string
 *
 * @param val  The string to store.
 *
 * @return  A reference to the new variant
 *
 */
VariantRef Variant::string(const std::string& val) {
    return VariantRef(new Variant(STRING, val.c_str()));
}

/**
 * @brief Create a error
 *
 * Create a new variant containing a error
 *
 * @param val  The error message to store.
 *
 * @return  A reference to the new variant
 *
 */
VariantRef Variant::error(const char* val) {
    return VariantRef(new Variant(ERROR, val));
}

/**
 * @brief Create a error
 *
 * Create a new variant containing a error
 *
 * @param val  The error message to store.
 *
 * @return  A reference to the new variant
 *
 */
VariantRef Variant::error(const std::string& val) {
    return VariantRef(new Variant(ERROR, val.c_str()));
}

/**
 * @brief Create a number
 *
 * Create a new variant containing a number
 *
 * @param val  The number to store.
 *
 * @return  A reference to the new variant
 *
 */ 
VariantRef Variant::number(double val) {
    return VariantRef(new Variant(val));
}

/**
 * @brief Create a boolean
 *
 * Create a new variant containing a boolean
 *
 * @param val  The boolean to store.
 *
 * @return  A reference to the new variant
 *
 */ 
VariantRef Variant::boolean(bool val) {
    return VariantRef(new Variant(val));
}


/**
 * @brief The type of the variant
 *
 * The type being stored by the variant
 *
 * @return  The type of the variant
 */
Variant::Type Variant::type() const {
    return type_;
}


/**
 * @brief The number value
 *
 * The number value being stored by the variant. This can only be called if the
 * variant type is NUMBER
 *
 * @return  The number being stored
 *
 */
double Variant::numberVal() const {
    assert(type_ == NUMBER);
    return val_.numberVal_;
}


/**
 * @brief The boolean value
 *
 * The boolean value being stored by the variant. This can only be called if the
 * variant type is BOOLEAN
 *
 * @return  The boolean being stored
 *
 */
bool Variant::booleanVal() const {
    assert(type_ == BOOLEAN);
    return val_.boolVal_;
}


/**
 * @brief The string value
 *
 * The string value being stored by the variant. This can only be called if the
 * variant type is STRING or ERROR
 *
 * @return  The string being stored
 *
 */
const char* Variant::charVal() const {
    assert(type_ == STRING || type_ == ERROR);
    return val_.charVal_;
}

void Variant::clear() {
    if (type_ == STRING || type_ == ERROR) {
        free(val_.charVal_);
    }
}

/**
 *
 * @brief Streaming operator for LexToken::Type
 *
 * Streams a LexToken::Type.
 *
 * @param out  The stream to write to
 * @param t    The token to write
 *
 * @return  The stream that was written to
 *
 */
std::ostream& operator<< (std::ostream &out, Variant::Type t)
{
    const char* label = "UNKNOWN";
    switch (t) {
    case Variant::NUMBER:
        label = "number";
        break;
    case Variant::BOOLEAN:
        label = "boolean";
        break;
    case Variant::STRING:
        label = "string";
        break;
    case Variant::ERROR:
        label = "error";
        break;
    }
    out << label;

    return out;
}
