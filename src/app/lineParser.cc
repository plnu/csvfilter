//
// csvfilter, Copyright (c) 2015, plnu
//

#include "lineParser.h"

/**
 * @brief Constructor
 *
 * This constructor does not do anything other than create the object. Call
 * LineParser::parse to parse a line.
 *
 */
LineParser::LineParser()
    :error_(""), fields_(), usedFields_(0) {

}

/**
 * @brief Parse a line into separate fields.
 *
 * Parse a line of csv. The buffer containing the line is modified in-place to
 * save copies, and the Field classes will refer directly to the buffer until
 * the next call to parse. However ownership is not retained - the calling code
 * (usually a FileReader) must delete (or reuse) the memory. The fields should
 * not be accessed after that happens.
 *
 * @param line  The line to parse. This line will be edited in-place.
 *
 * @return  true if the line is a valid line of csv, false otherwise (in which
 *          case see LineParser::errText)
 */
bool LineParser::parse(char* line) {
    bool ok = true;
    error_ = "";

    // Note that we manually keep track of how many fields we've parsed out of
    // the line and reuse the field entries in fields_. This is purely a
    // performance optimisation - clearing the array and deleting all the field
    // objects is actually quite expensive.
    usedFields_ = 0;

    char* pos = line;
    char* startOfField = pos;

    if (*line) {
        bool more = true;
        while (ok && more) {

            pos = endOfField(pos);

            if (pos == nullptr) {
                ok = false;
            } else if (*pos) {
                *pos = '\0';
                pos++;
            } else {
                more = false;
            }
        
            if (usedFields_ < fields_.size()) {
                fields_[usedFields_]->reset(startOfField);
            } else {
                fields_.push_back(FieldRef(new Field(startOfField)));
            }
            usedFields_++;
            startOfField = pos;
        }
    }

    return ok;
}

/**
 * @brief  Error description
 *
 * @return  A description of the last parse error, if the last call to
 *          LineParser::parse returned false, or a blank string if it returned
 *          true. 
 *
 */
const std::string& LineParser::errText() const {
    return error_;
}

/**
 * @brief  The number of fields in the line
 *
 * @return  The number of fields in the current line
 *
 */
size_t LineParser::fieldCount() const {
    return usedFields_;
}

/**
 * @brief  Access to fields from the line
 *
 * Gets a field from the last line that was parsed.
 *
 * @param idx  The index of the field you want
 *
 * @return  The field at that index
 *
 */
FieldRef LineParser::field(int idx) const {
    return fields_[idx];
}



char* LineParser::endOfField(char* pos) {
    char* ret = pos;

    if (*ret == '"') {
        // quoted field
        bool keepGoing = true;

        while (*ret && keepGoing) {
            ret++;
            if (*ret == '"') {
                if (*(ret + 1) == '"') {
                    // double quote is a escaped quote
                    ret++;
                } else {
                    keepGoing = false;
                }
            }
        }

        if (*ret == '\0') {
            error_ = "Unterminated string field";
            ret = nullptr;
        } else {
            // we're pointing to the closing quote
            ret++;
            if (*ret != ',' && *ret != '\0') {
                ret = nullptr;
                error_ = "Unexpected characters after string field";
            }
        }
    } else {
        // unquoted field
        while (*ret && *ret != ',') {
            ret++;
        }
    }
    return ret;
}
