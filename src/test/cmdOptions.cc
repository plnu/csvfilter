//
// csvfilter, Copyright (c) 2015, plnu
//

#include <sstream>

#include <app/cmdOptions.h>

#include "test.h"

class ExpectedVals {
public:
    ExpectedVals()
        :help(false),
         version(false),
         file(""),
         filter("")
    {}
    
    bool help;
    bool version;
    std::string file;
    std::string filter;
    std::vector<std::string> columns;

    void check(const CmdOptions& o) {
        Test::eq(o.help(), help, "Help option registered correctly");
        Test::eq(o.version(), version, "Version option registered correctly");
        Test::eq(o.file(), file, "File argument registered correctly");
        Test::eq(o.filter(), filter, "Filter argument registered correctly");

        std::stringstream msg;
        msg << columns.size() << " columns registered";
        if (Test::eq(o.columns().size(), columns.size(), msg.str())) {
            for (int i = 0; i < columns.size(); i++) {
                std::stringstream itmMsg;
                itmMsg << "Column " << i << " is " << columns[i];
                Test::eq(o.columns()[i], columns[i], itmMsg.str());
            }
        }
    }
};


void testValidCmdLine(const char** args, ExpectedVals& expected) {
    int count = 0;
    std::stringstream description;
    while (args[count] != nullptr) {
        if (count > 0) {
            description << " ";
        }
        description << "\"" << args[count] << "\"";
        count++;
    }
    description << " parses";
    CmdOptions opts(count, args);
    Test::that(opts.ok(), description.str());
    expected.check(opts);
}

void cmdOptionsTests() {
    Test::beginSuite("Command-line option parsing");

    const char* helpArgs[] = {"exe", "-h", nullptr};
    ExpectedVals helpExpected;
    helpExpected.help = true;
    testValidCmdLine(helpArgs, helpExpected);

    const char* versionArgs[] = {"exe", "-v", nullptr};
    ExpectedVals versionExpected;
    versionExpected.version = true;
    testValidCmdLine(versionArgs, versionExpected);

    const char* fileArgs[] = {"exe", "filename", nullptr};
    ExpectedVals fileExpected;
    fileExpected.file = "filename";
    testValidCmdLine(fileArgs, fileExpected);

    const char* columnArgs[] = {"exe", "-c", "a,b,c,\"def\"", nullptr};
    ExpectedVals colsExpected;
    colsExpected.columns.push_back("a");
    colsExpected.columns.push_back("b");
    colsExpected.columns.push_back("c");
    colsExpected.columns.push_back("def");
    testValidCmdLine(columnArgs, colsExpected);

    const char* filterArgs[] = {"exe", "-f", "2 + 3 / 4", nullptr};
    ExpectedVals filterExpected;    
    filterExpected.filter = "2 + 3 / 4";
    testValidCmdLine(filterArgs, filterExpected);

    Test::endSuite();
}
