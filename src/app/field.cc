//
// csvfilter, Copyright (c) 2015, plnu
//

#include "field.h"

#include <string.h>
#include <stdlib.h>

/**
 * @brief Constructor.
 *
 * Create a new field from the raw string value read from a csv file, including
 * any surrounding quotes and escaped characters (i.e. quote characters in a
 * quoted string will still be represented as "").
 *
 * Note that this class stores, but does not take ownership of, rawVal to
 * save a string copy. rawVal will typically be a piece of memory owned by
 * fileReader, and will only be valid until the next line is read.
 *
 * @param rawVal  The raw, potentially still quoted and unescaped, value of
 *                this field from the input file.
 */
Field::Field(const char* rawVal)
    :rawVal_(rawVal),
     stringVal_(nullptr),
     canBeNumber_(UNKNOWN),
     doubleVal_(0) {

}

/**
 * @brief Destructor.
 *
 */
Field::~Field() {
    delete[] stringVal_;
}


/**
 *
 * @brief  Change the stored value
 *
 * Change the value that is being stored in this Field object. This is a
 * performance enhancement to allow reuse of this class rather than destroying
 * it and creating a new one.
 *
 * @see  Field::Field
 *
 * @param rawVal  The raw, potentially still quoted and unescaped, value of
 *                this field from the input file.
 */
void Field::reset(const char* rawVal) {
    rawVal_ = rawVal;
    delete[] stringVal_;
    stringVal_ = nullptr;
    canBeNumber_ = UNKNOWN;
    doubleVal_ = 0;
}

/**
 * @brief  The raw field value.
 *
 * The raw value of the field, as read from the input file. This will contain
 * any quoting end escaping that was present in the input file, so is suitable
 * for writing directly to the output.
 *
 * Note that this is the original char* passed in to the class, so ownership
 * remains with the original FileReader, and it is only valid until the next
 * line is read.
 *
 * @return  The raw value read for this field
 */
const char* Field::raw() const {
    return rawVal_;
}

/**
 * @brief  The value of the field
 *
 * @return  The string this field contains, with surrounding quotes (if any)
 *          removed, and escaped characters (if any) unescaped. Note that this
 *          is calculated from Field::raw, so the same rules apply about
 *          lifetime. 
 *
 */
const char* Field::asString() {
    const char* ret = nullptr;
    if (*rawVal_ != '"') {
        ret = rawVal_;
    } else if (stringVal_ != nullptr) {
        ret = stringVal_;
    } else {
        int len = strlen(rawVal_);
        stringVal_ = new char[len + 1];
        ret = stringVal_;

        // copy across the string, dropping the surrounding quotes,
        // and unescaping any contained quotes.
        char* dest = stringVal_;
        const char* source = rawVal_ + 1; // ignore first quote
        while (*(source + 1) != '\0') { // ignore last quote
            *dest = *source;
            if (*source == '"') { // must be an escaped quote
                source++;
            }
            source++;
            dest++;
        }
        *dest = '\0';
    }

    return ret;
}

/**
 * @brief  Return the field as a number.
 *
 * Attempt to convert the filed to a number, and return it.
 *
 * @param val  This will be set to the number value of the field.
 *
 * @return  true if the field can be converted to a number (in which case val
 *          will be set), or false otherwise.
 *
 */
bool Field::asNumber(double& val) {
    if (canBeNumber_ == UNKNOWN) {
        if (*rawVal_ == '"') {
            canBeNumber_ = NO;
        } else {
            char* end = nullptr;
            doubleVal_ = strtod(rawVal_, &end);
            canBeNumber_ = (*end == '\0') ? YES : NO;
        }
    }
    if (canBeNumber_ == YES) {
        val = doubleVal_;
    }
    return (canBeNumber_ == YES);
}
