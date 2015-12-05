//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef CSVFILTER_CMDOPTIONS_H
#define CSVFILTER_CMDOPTIONS_H

#include <string>
#include <vector>

/**
 * @brief Command line parser.
 *
 * This class parses and validates the command line arguments for the
 * application.
 *
 */
class CmdOptions {
public:
    CmdOptions(int arcg, const char** argv);

    bool ok() const;
    const std::string errMsg() const;


    bool help() const;
    bool version() const;
    bool showHeaders() const;

    const std::vector<std::string>& columns() const;
    const std::string& file() const;
    const std::string& filter() const;

    void printUsage() const;
private:
    // copy and assignment opterators
    CmdOptions(const CmdOptions& other);
    CmdOptions& operator=(const CmdOptions& other);

    bool readCols(const char* cols);

    int help_;
    int version_;
    // popt uses old-style c bools, which seem to be different sizes to bool
    // we'll use ints instead
    int ok_;
    int showHeaders_;
    std::string errMsg_;
    std::string exeName_;
    std::string file_;
    std::string filter_;

    std::vector<std::string> columns_;
};

#endif // CSVFILTER_CMDOPTIONS_H
