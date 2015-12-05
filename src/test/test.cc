//
// csvfilter, Copyright (c) 2015, plnu
//

#include "test.h"

#include <iostream>
#include <sstream>
#include <string.h>

namespace Test {

class Count {
public:
    int testCount;
    int testFailure;

    Count() : testCount(0), testFailure(0) {

    }
    
    void log(bool passed) {
        testCount++;
        if (!passed) {
            testFailure++;
        }
    }

    void reset() {
        testCount = 0;
        testFailure = 0;
    }

    void printSummary() {
        std::cout << testFailure << " of " << testCount << " failed: "
                  << ( (testFailure == 0) ? "PASSED" : "FAILED" )
                  << std::endl;
    }
};


static Count sectionCount = Count();
static Count globalCount = Count();
static std::string group = "";

bool that(bool result, std::string description) {
    std::string resultText = result ? "PASS" : "FAIL";
    std::cout << "    [" << resultText << "] ";
    if (!group.empty()) {
        std::cout << group << ": ";
    }
    std::cout << description << std::endl;

    sectionCount.log(result);
    globalCount.log(result);

    return result;
}

bool eq(bool testVal, bool expectedVal, std::string description) {
    bool result = that(testVal == expectedVal, description);
    if (!result) {
        std::cout << "           "
                  << "expected: " << expectedVal << " got: " << testVal
                  << std::endl;
    }
    return result;
}

bool eq(int testVal, int expectedVal, std::string description) {
    bool result = that(testVal == expectedVal, description);
    if (!result) {
        std::cout << "           "
                  << "expected: " << expectedVal << " got: " << testVal
                  << std::endl;
    }
    return result;
}

bool eq(double testVal, double expectedVal, std::string description) {
    bool result = that(testVal == expectedVal, description);
    if (!result) {
        std::cout << "           "
                  << "expected: " << expectedVal << " got: " << testVal
                  << std::endl;
    }
    return result;
}

bool eq(unsigned testVal, unsigned expectedVal, std::string description) {
    bool result = that(testVal == expectedVal, description);
    if (!result) {
        std::cout << "           "
                  << "expected: " << expectedVal << " got: " << testVal
                  << std::endl;
    }
    return result;
}

bool eq(size_t testVal, size_t expectedVal, std::string description) {
    bool result = that(testVal == expectedVal, description);
    if (!result) {
        std::cout << "           "
                  << "expected: " << expectedVal << " got: " << testVal
                  << std::endl;
    }
    return result;
}

bool eq(const char* testVal, const char* expectedVal, std::string description) {
    bool result = that(strcmp(testVal, expectedVal) == 0, description);
    if (!result) {
        std::cout << "           "
                  << "expected: " << expectedVal << " got: " << testVal
                  << std::endl;
    }
    return result;
}

bool eq(const std::string& testVal,
        const std::string& expectedVal,
        std::string description) {
    bool result = that(testVal == expectedVal, description);
    if (!result) {
        std::cout << "           "
                  << "expected: " << expectedVal << " got: " << testVal
                  << std::endl;
    }
    return result;
}

bool eq(const ParseError& testVal,
        const ParseError expectedVal,
        std::string description) {
    bool result = that(testVal == expectedVal, description);
    if (!result) {
        if (testVal.getMessage() != expectedVal.getMessage()) {
            std::cout
                << "           "
                << "expected message: " << expectedVal.getMessage() << "\n"
                << "           "
                << "got:              " << testVal.getMessage() 
                << std::endl;
        }
        if (testVal.getPrimaryRange() != expectedVal.getPrimaryRange()) {
            std::cout
                << "           "
                << "expected primary range: "
                << expectedVal.getPrimaryRange().begin
                << " - " << expectedVal.getPrimaryRange().end << "\n"
                << "           "
                << "got:                    "
                << testVal.getPrimaryRange().begin
                << " - " << testVal.getPrimaryRange().end
                << std::endl;
        }
        if (testVal.getSecondaryRange() != expectedVal.getSecondaryRange()) {
            std::cout
                << "           "
                << "expected secondary range: "
                << expectedVal.getSecondaryRange().begin
                << " - " << expectedVal.getSecondaryRange().end << "\n"
                << "           "
                << "got:                    "
                << testVal.getSecondaryRange().begin
                << " - " << testVal.getSecondaryRange().end
                << std::endl;
        }
    }
    return result;
}

void beginSuite(const std::string& name) {
    sectionCount.reset();

    std::cout << "SUITE: " << name << std::endl;
}

void endSuite() {
    sectionCount.printSummary();
    std::cout << std::endl;
}

void beginGroup(const std::string& name) {
    if (!group.empty()) {
        std::stringstream msg;
        msg << "Group \"" << name << "\" begun before group \""
            << group << "\" was ended";

        that(false, msg.str());
    }
    group = name;
}

void endGroup() {
    group = "";
}

void printSummary() {
    std::cout << "\nOVERALL RESULTS" << std::endl;
    globalCount.printSummary();
}

int exitCode() {
    return (globalCount.testFailure == 0) ? 0 : 1;
}
};
