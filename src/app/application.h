//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef CSVFILTER_APPLICATION_H
#define CSVFILTER_APPLICATION_H

#include "cmdOptions.h"
#include "fileReader.h"
#include "lineParser.h"
#include "headers.h"
#include "filterExpression/expression.h"
#include "filterExpression/parseError.h"

#include <memory>

/**
 * @brief The main application class
 *
 * The main application class. This class is responsible for controlling the
 * overall flow of the application.
 *
 */
class Application {
public:
    Application();
    int run(int argc, char* argv[]);
private:
    Application(const Application& other);
    Application& operator=(const Application& other);

    void error(const std::string& errMsg);
    void error(const ParseError& err);

    void printVersion(const char* appName);
    bool parseCmdLine(int argc, char* argv[]);
    bool openFile();
    bool readHeader();
    bool parseExpression();

    void processFile();
    void printLine();

    std::unique_ptr<CmdOptions> cmdOptions_;
    std::unique_ptr<FileReader> fileReader_;
    std::unique_ptr<Expression> filter_;
    LineParser lineParser_;
    std::unique_ptr<Headers> headers_;
    int expectedFieldCount_;
    int exitCode_;
};


#endif // CSVFILTER_APPLICATION_H
