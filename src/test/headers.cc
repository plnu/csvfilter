//
// csvfilter, Copyright (c) 2015, plnu
//

#include <app/headers.h>
#include <app/lineParser.h>

#include "test.h"

#include <vector>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sstream>

static void testValidHeaderLine() {
    Test::beginGroup("Valid header");

    char* lineStr = strdup("a,b,c,d,e,f,g");
    LineParser line;
    std::vector<std::string> outCols;

    Test::that(line.parse(lineStr), "headers line parsed");
    
    Headers headers(line, outCols);

    Test::that(headers.ok(), "Headers are ok");
    Test::eq(headers.indexOf("a"), 0, "Index of a");
    Test::eq(headers.indexOf("b"), 1, "Index of b");
    Test::eq(headers.indexOf("c"), 2, "Index of c");
    Test::eq(headers.indexOf("d"), 3, "Index of d");
    Test::eq(headers.indexOf("e"), 4, "Index of e");
    Test::eq(headers.indexOf("f"), 5, "Index of f");
    Test::eq(headers.indexOf("g"), 6, "Index of g");
    Test::eq(headers.indexOf("h"), -1, "Index of h");
    free(lineStr);

    Test::endGroup();
}

static void testRepeatedHeaders() {
    Test::beginGroup("Header with repeating heading");

    char* lineStr = strdup("a,b,a,s p a c e");
    LineParser line;
    std::vector<std::string> outCols;

    Test::that(line.parse(lineStr), "header");

    Headers headers(line, outCols);
    Test::that(headers.ok(), "Unusual headers are ok");

    Test::eq(headers.indexOf("a"), 0, "Index of a");
    Test::eq(headers.indexOf("b"), 1, "Index of b");
    Test::eq(headers.indexOf("a1"), 2, "Index of a1");
    Test::eq(headers.indexOf("s p a c e"), 3, "Index of s p a c e");
    Test::eq(headers.indexOf("s_p_a_c_e"), 3, "Index of s_p_a_c_e");

    free(lineStr);

    Test::endGroup();
}

static void testDefaultOutputCols() {
    Test::beginGroup("Default cols");

    char* lineStr = strdup("a,b,c,d,e,f,g");
    LineParser line;
    std::vector<std::string> outCols;

    Test::that(line.parse(lineStr), "headers line parsed");
    
    Headers headers(line, outCols);

    Test::that(headers.ok(), "Headers are ok");
    if (Test::eq(headers.outColCount(), 7, "7 output cols")) {
        for (int i = 0; i < 7; i++) {
            std::stringstream msg;
            msg << "Output col " << i << " is " << i;
            Test::eq(headers.outColIdx(i), i, msg.str());
            
        }
    }
    free(lineStr);

    Test::endGroup();
}

static void testInvalidOutputCols() {
    Test::beginGroup("Default cols");

    char* lineStr = strdup("a,b,c,d,e,f,g");
    LineParser line;
    std::vector<std::string> outCols{"a", "wrong"};

    Test::that(line.parse(lineStr), "headers line parsed");
    
    Headers headers(line, outCols);

    Test::eq(headers.ok(), false, "Headers are not ok");

    Test::eq(headers.errText(),
             "No such column \"wrong\"", "Error is no such column");

    free(lineStr);

    Test::endGroup();
}

static void testValidOutputCols() {
    Test::beginGroup("Default cols");

    char* lineStr = strdup("a,b,c,d,e,f,g");
    LineParser line;
    std::vector<std::string> outCols{
        "d", "b", "a", "d", "g"
    };

    Test::that(line.parse(lineStr), "headers line parsed");
    
    Headers headers(line, outCols);

    Test::eq(headers.ok(), true, "Headers are ok");

    Test::eq(headers.outColCount(), 5, "5 output cols");
    Test::eq(headers.outColIdx(0), 3, "Index 0 is correct");
    Test::eq(headers.outColIdx(1), 1, "Index 1 is correct");
    Test::eq(headers.outColIdx(2), 0, "Index 2 is correct");
    Test::eq(headers.outColIdx(3), 3, "Index 3 is correct");
    Test::eq(headers.outColIdx(4), 6, "Index 4 is correct");

    free(lineStr);

    Test::endGroup();
}

void headersTests() {
    Test::beginSuite("Header parsing");

    testValidHeaderLine();
    testRepeatedHeaders();
    testDefaultOutputCols();
    testInvalidOutputCols();
    testValidOutputCols();
    Test::endSuite();
}

