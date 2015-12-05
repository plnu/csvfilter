//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef UNITTEST_TEST_H
#define UNITTEST_TEST_H

#include <string>
#include <app/filterExpression/parseError.h>

namespace Test {

bool that(bool result, std::string description);

bool eq(bool testVal, bool expectedVal, std::string description);
bool eq(int testVal, int expectedVal, std::string description);
bool eq(double testVal, double expectedVal, std::string description);
bool eq(unsigned testVal, unsigned expectedVal, std::string description);
bool eq(size_t testVal, size_t expectedVal, std::string description);
bool eq(const std::string& testVal,
        const std::string& expectedVal,
        std::string description);
bool eq(const char* testVal,
        const char* expectedVal,
        std::string description);
bool eq(const ParseError& testVal,
        const ParseError expectedVal,
        std::string description);
void beginSuite(const std::string& name);
void endSuite();
void beginGroup(const std::string& name);
void endGroup();


void printSummary();    

int exitCode();
};

#endif //UNITTEST_TEST_H
