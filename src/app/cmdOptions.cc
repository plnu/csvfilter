//
// csvfilter, Copyright (c) 2015, plnu
//

#include <popt.h>
#include <sstream>
#include <iostream>
#include <string.h>

#include "cmdOptions.h"
#include "lineParser.h"

/**
 * @brief Constructor.
 *
 * This class takes argc and argv, as passed to main, and parses them. Check
 * CmdOptions::ok to see if the options are valid, and if they are not, then
 * CmdOptions::errMsg will contain a error message suitable for users.
 *
 * @param argc  As passed to main
 * @param argv  As passed to main
 */
CmdOptions::CmdOptions(int argc, const char** argv)
    :help_(0),
     version_(0),
     ok_(false),
     showHeaders_(false),
     errMsg_(""),
     exeName_(argv[0]),
     file_(""),
     filter_(""),
     columns_() {
    char* colArg = nullptr;
    char* filterArg = nullptr;

    struct poptOption po[] = {  
         {"help", 'h', POPT_ARG_NONE, &help_, 0, "help", NULL},
         {"version", 'v', POPT_ARG_NONE, &version_, 0, "version", NULL},
         {"columns", 'c', POPT_ARG_STRING, &colArg, 0,
                                             "columns to write", NULL},
         {"filter", 'f', POPT_ARG_STRING, &filterArg, 0,
                                             "filer expression to apply", NULL},
         {"show-headers", 's', POPT_ARG_NONE, &showHeaders_, 0, "version", NULL},
         {NULL}  
     };  
       
     // pc is the context for all popt-related functions  
     poptContext pc = poptGetContext(NULL, argc, argv, po, 0);  

     int rc = poptGetNextOpt(pc);
     if (rc != -1) {
         std::stringstream msg;
         msg << poptBadOption(pc, 0) << ": " << poptStrerror(rc);
         errMsg_ = msg.str();
     } else {
         const char* arg = poptGetArg(pc);

         if (arg != nullptr && poptPeekArg(pc) != nullptr) {
             std::stringstream msg;
             msg << "Unexpected argument: " << poptGetArg(pc);
             errMsg_ = msg.str();             
         } else {
             if (arg != nullptr) {
                 file_ = arg;
             }

             if (filterArg != nullptr) {
                 filter_ = filterArg;
             }
             // Valgrind suggests we should free this, but that causes problems
             // on macs, where it is reported as a free of unallocated memory
             // free((char*)arg);
             ok_ = true;
         }
     }

     poptFreeContext(pc);

     if (ok_ && colArg) {
         ok_ = readCols(colArg);
     }


}

/**
 * @brief  Were the command line options valid?
 *
 * @return  true if the command line arguments are valid, false otherwise
 *
 */
bool CmdOptions::ok() const {
    return ok_;
}

/**
 * @brief Was -h present?
 *
 * @return  true if -h / --help was found in the command line options
 *
 */
bool CmdOptions::help() const {
    return help_;
}

/**
 * @brief Was -v present?
 *
 * @return  true if -v / --version was found in the command line options
 *
 */
bool CmdOptions::version() const {
    return version_;
}

/**
 *
 * @brief Was -s present?
 *
 * @return true if -s / --show-headers was found in the command line options
 *
 */
bool CmdOptions::showHeaders() const {
    return showHeaders_;
}

/**
 * @brief The columns specified via -c.
 *
 * The -c / --columns option allows the user to specify a list of columns to be
 * output. The columns are specified in csv format (e.g. -c "col1,col2,col3"),
 * but are parsed into a vector.
 *
 * @return  The list of columns that was specified via the -c / --columns
 *          command line options. A zero-length vector is returned if the option
 *          was not present.
 *
 */
const std::vector<std::string>& CmdOptions::columns() const {
    return columns_;
}

/**
 * @brief The supplied filename.
 *
 * @return  The name of file to use as input.
 *
 */
const std::string& CmdOptions::file() const {
    return file_;
}

/**
 * @brief The filter expression.
 *
 * @return  The filter expression to apply to each line, or a blank string if
 *          there wasn't one to supplied.
 *
 */
const std::string& CmdOptions::filter() const {
    return filter_;
}

/**
 * @brief Description of any parse error.
 *
 * If the command line options were not valid (i.e. CmdOptions::ok returns
 * false) then the error will be described in this error message.
 *
 * @return  An error message suitable for displaying to the user
 *
 */
const std::string CmdOptions::errMsg() const {
    return errMsg_;
}

/**
 * @brief Print a usage message.
 *
 * Prints a usage message to cout.
 *
 */
void CmdOptions::printUsage() const {
    std::cout << "Usage: " << exeName_ << " -[hvs] "
                                          "[-c <columns>] [-f <filter>] "
              <<                          "[<file>]\n"
              << "\n"
              << " -h: Print help message\n"
              << " -v: Print version information\n"
              << " -s: Show the headers and header aliases from the csv file\n"
              << " -c: A (comma-separated) list of output columns\n"
              << " -f: A filter expression to apply to the rows"
              << std::endl;
}


bool CmdOptions::readCols(const char* constCols) {
    char * cols = strdup(constCols);
    LineParser parser;
    bool ok = parser.parse(cols);
    if (!ok) {
        std::stringstream msg;
        msg << "Failed to parse requested columns: " << parser.errText();
        errMsg_ = msg.str();
    } else {
        for (int i = 0; i < parser.fieldCount(); i++) {
            columns_.push_back(std::string(parser.field(i)->asString()));
        }
    }
    free(cols);
    return ok;
}
