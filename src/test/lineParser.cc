//
// csvfilter, Copyright (c) 2015, plnu
//

#include "test.h"

#include <app/lineParser.h>

#include <cstdarg>
#include <vector>
#include <sstream>
#include <string.h>
#include <stdlib.h>

void testLine(const char* line, ...) {
    va_list va;
    va_start(va, line);

    std::vector<const char*> expectedVals = std::vector<const char*>();
    const char* arg = va_arg(va, const char*);
    while (arg != nullptr) {
        expectedVals.push_back(arg);
        arg = va_arg(va, const char*);
    }
    va_end(va);

    LineParser p;

    char* lineCpy = strdup(line);
    std::stringstream msg;
    msg << "Line '" << line << "' parses";
    Test::that(p.parse(lineCpy), msg.str());

    Test::eq(p.fieldCount(),
             expectedVals.size(),
             "Read the correct number of fields");
    int minLen = p.fieldCount() < expectedVals.size() ?
                                  p.fieldCount() :
                                  expectedVals.size();

    for (int i = 0; i < minLen; i++) {
        std::stringstream valMsg;
        valMsg << "Field " << i << ", raw value is " << expectedVals[i];
        Test::eq(p.field(i)->raw(), expectedVals[i], valMsg.str());
    }

    free(lineCpy);
}

void testBadLine(const char* line, std::string expectedError) {
    LineParser p;
    char* lineCpy = strdup(line);

    std::stringstream msg;
    msg << "Line '" << line << "' does not parse";
    Test::eq(p.parse(lineCpy), false, msg.str());

    msg.str("");
    msg << "Error text is: " << expectedError;
    Test::eq(p.errText(), expectedError, msg.str());

    free(lineCpy);
}

void lineParserTests() {
    Test::beginSuite("CSV line parsing");

    testLine("abc", "abc", NULL);
    testLine("a,b,c", "a", "b", "c", NULL);
    testLine(",b", "", "b", NULL);
    testLine("a,,b", "a", "", "b", NULL);
    testLine("a,", "a", "", NULL);
    testLine("\"a,b\"", "\"a,b\"", NULL);
    testLine("\"a,b\",abc", "\"a,b\"", "abc", NULL);
    testLine("\"a\"\"b\"", "\"a\"\"b\"", NULL);

    testBadLine("\"abc", "Unterminated string field");
    testBadLine("\"abc\"a", "Unexpected characters after string field");
        
    Test::endSuite();
}
