//
// csvfilter, Copyright (c) 2015, plnu
//

#include <iostream>

#include "test.h"

void cmdOptionsTests();
void lineParserTests();
void fieldTests();
void headersTests();
void lexerTests();
void expressionParserTests();

int main(int argc, char* argv[]) {
    cmdOptionsTests();
    lineParserTests();
    fieldTests();
    headersTests();
    lexerTests();
    expressionParserTests();
    
    Test::printSummary();
    return Test::exitCode();
}
