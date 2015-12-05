//
// csvfilter, Copyright (c) 2015, plnu
//

#include "application.h"
#include "filterExpression/variant.h"

#include "configure.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <assert.h>

/**
 * @brief Constructor.
 *
 * Constructor.
 *
 */
Application::Application()
    :expectedFieldCount_(-1), exitCode_(1) {

}

/**
 * @brief  Run the application
 *
 * Run the application.
 *
 * @param argc  argc, as passed into main
 * @param argv  argv, as passed into main
 *
 * @return  An exit code for the application.
 *
 */
int Application::run(int argc, char* argv[]) {
    exitCode_ = 0;
    expectedFieldCount_ = -1;
    if (parseCmdLine(argc, argv)) {
        if (cmdOptions_->help()) {
            cmdOptions_->printUsage();
        } else if (cmdOptions_->version()) {
            printVersion(argv[0]);
        } else if (openFile() && readHeader()) {
            if (cmdOptions_->showHeaders()) {
                headers_->printHeaders();
            } else if (parseExpression()) {
                // print headers
                printLine();
                // process rest of file
                processFile();
            }
        }
    }

    return exitCode_;
}

/**
 *
 * @brief Print an error message
 * Print the supplied error message and set the exit code to indicate there was
 * an error.
 *
 * @param errMsg  The error message.
 *
 */
void Application::error(const std::string& errMsg) {
    std::cerr << errMsg << std::endl;
    exitCode_ = 1;
}

/**
 *
 * @brief Print a parse error
 *
 * Print a parse error. This involves printing the error message itself and a
 * an copy of the expression with the position of the error indicated.
 *
 * @param err  The error.
 *
 */
void Application::error(const ParseError& err) {
    std::cerr << "Failed to parse filter expression: "
              << err.getMessage() << std::endl;
    std::cerr << cmdOptions_->filter() << std::endl;
    for (int i = 0;
         i < std::max(err.getPrimaryRange().end, err.getSecondaryRange().end);
         i++) {
        if (i >= err.getPrimaryRange().begin &&
            i < err.getPrimaryRange().end) {
            std::cerr << "^";
        } else if (i >= err.getSecondaryRange().begin &&
            i < err.getSecondaryRange().end) {
            std::cerr << "~";
        } else {
            std::cerr << " ";
        }
    }
    std::cerr << std::endl;
    exitCode_ = 1;
}

/**
 * @brief  Print version information
 *
 * Print version information for the application.
 *
 * @param appName  The binary name, as read from argv
 *
 */
void Application::printVersion(const char* appName) {
    std::cout << appName
              << " v" << CSVFILTER_VERSION_MAJOR
              << "." << CSVFILTER_VERSION_MINOR
              << std::endl;
}

bool Application::parseCmdLine(int argc, char* argv[]) {
    bool ok = false;
    cmdOptions_.reset(new CmdOptions(argc, (const char**) argv));
    if (!cmdOptions_->ok()) {
        error(cmdOptions_->errMsg());
    } else {
        ok = true;
    }
    return ok;
}
    
bool Application::openFile() {
    bool ok = false;
    fileReader_.reset(new FileReader(cmdOptions_->file()));
    if (!fileReader_->ok()) {
        error(fileReader_->errText());
    } else {
        ok = true;
    }
    return ok;
}

bool Application::readHeader() {
    bool ok = false;
    char* line;
    if ((line = fileReader_->getLine()) == nullptr) {
        if (!fileReader_->ok()) {
            error(fileReader_->errText());
        }
    } else if (!lineParser_.parse(line)) {
        error(lineParser_.errText());
    } else {
        expectedFieldCount_ = lineParser_.fieldCount();
        headers_.reset(new Headers(lineParser_, cmdOptions_->columns()));
        
        if (!headers_->ok()) {
            error(headers_->errText());
        } else {
            ok = true;
        }
    }
    
    return ok;
}

bool Application::parseExpression() {
    bool ok = false;

    if (cmdOptions_->filter().empty()) {
        ok = true;
    } else {
        filter_.reset(new Expression(cmdOptions_->filter(), *headers_));
        if (!filter_->ok()) {
            error(filter_->error());
        } else {
            ok = true;
        }
    }
    return ok;
}


void Application::processFile() {
    int lineCount = 1;
    char* line = nullptr;
    std::stringstream err;

    while (exitCode_ == 0 &&
           (line = fileReader_->getLine()) != nullptr) {
        if (!lineParser_.parse(line)) {
            error(lineParser_.errText());
        } else if (lineParser_.fieldCount() !=
                   expectedFieldCount_) {
            err << "Line " << lineCount
                << ": Incorrect number of entries. Expected "
                << expectedFieldCount_ << ", got "
                << lineParser_.fieldCount() << std::endl;
            error(err.str());
        } else {
            if (!filter_) {
                // no filter expression
                printLine();
            } else {
                VariantRef result = filter_->eval(lineParser_);
                if (result->type() ==Variant::ERROR) {
                    err << "Line " << lineCount
                        << ":  Failed to evaluate filter expression ("
                        << result->charVal() << ")"
                        << std::endl;
                    error(err.str());
                } else {
                    assert(result->type() == Variant::BOOLEAN);
                    if (result->booleanVal()) {
                        printLine();
                    }
                }
            }
        }
        lineCount++;
    }

    if (exitCode_ == 0 && !fileReader_->ok()) {
        error(fileReader_->errText());
    }

}


void Application::printLine() {
    for (int i = 0; i < headers_->outColCount(); i++) {
        int colIdx = headers_->outColIdx(i);
        if (i != 0) {
            std::cout << ",";
        }
        std::cout << lineParser_.field(colIdx)->raw();
    }
    std::cout << std::endl;
}
