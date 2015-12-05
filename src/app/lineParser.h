//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef CSVFILTER_LINEPARSER_H
#define CSVFILTER_LINEPARSER_H

#include "field.h"

#include <vector>
#include <string>

/**
 * @brief Parse a line of csv.
 *
 * This class provides methods to parse a line of csv into individual fields,
 * and has full support for quoted fields.
 *
 * Once the line is parsed (using LineParser::parse) you can access the
 * individual fields by using LineParser::fieldCount and LineParser::field.
 *
 */
class LineParser {
public:
    LineParser();
    bool parse(char*);

    size_t fieldCount() const;
    FieldRef field(int idx) const;

    const std::string& errText() const;
private:
    LineParser(const LineParser& other);
    LineParser& operator=(const LineParser& other);

    char* endOfField(char* pos);

    std::string error_;
    std::vector<FieldRef> fields_;
    int usedFields_;
};


#endif // CSVFILTER_LINEPARSER_H
