//
// csvfilter, Copyright (c) 2015, plnu
//

#include <app/field.h>
#include "test.h"

#include <sstream>

void testStringVal(const char* raw, const char* result) {
    Field f(raw);

    std::stringstream msg;

    msg << "Raw value " << raw << " becomes " << result << " as a string";

    Test::eq(f.asString(), result, msg.str());
}


static void testNumberVal(const char* raw, double result) {
    Field f(raw);
    double val = 0.0;

    std::stringstream msg;
    msg << "Field value " << raw;
    Test::beginGroup(msg.str());

    if (Test::that(f.asNumber(val), "field is number")) {
        Test::eq(val, result, "Returned number is correct");
    }

    Test::endGroup();
}

static void testNotNumber(const char* raw) {
    Field f(raw);

    std::stringstream msg;
    msg << "Field value " << raw;
    Test::beginGroup(msg.str());
    double val = 0.0;
    Test::that(!f.asNumber(val), "field is not number");

    Test::endGroup();
}


void fieldTests() {
    Test::beginSuite("Fields");

    testStringVal("abc", "abc");
    testStringVal("\"abc\"", "abc");
    testStringVal("\"ab\"\"c\"", "ab\"c");

    testNumberVal("321", 321);
    testNumberVal("321.321", 321.321);
    testNumberVal("-5.4", -5.4);
    testNumberVal(" 32", 32);
    testNumberVal("013", 13);
    testNumberVal("0x5", 5);

    testNotNumber("\"123\"");
    testNotNumber("123a");
    testNotNumber(" 123 ");
    testNotNumber("0xabz");
    testNotNumber("123.21.1");
    
    Test::endSuite();
}
