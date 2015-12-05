//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef CSVFILTER_HEADERS_H
#define CSVFILTER_HEADERS_H

#include "lineParser.h"

#include <string>
#include <map>
#include <vector>
#include <list>

/**
 * @brief Tracks input and output headers.
 *
 * This class keeps track of the headers of the columns in the input file, and
 * the order of the columns in the output file.
 *
 */
class Headers {
public:
    Headers(const LineParser& headerLine,
            const std::vector<std::string>& outputHeaders);

    bool ok() const;
    const std::string& errText() const;

    void printHeaders() const;
    int indexOf(const std::string& column) const;

    int outColCount() const;
    int outColIdx(int inIdx) const;
private:
    Headers(const Headers& other);
    Headers& operator=(const Headers& other);

    void generateExpressionTokens(
        const std::list<std::pair<const std::string, int>>& tokensToTransform);
    bool readOutputHeaders(
        const std::vector<std::string>& outputHeaders);
    void printHeaderLine(const std::string& header,
                         const std::string& alias,
                         int headerWidth) const;

    bool ok_;
    std::string errorText_;
    std::map<const std::string, int> headerIndexes_;
    std::map<int, std::string> expressionTokens_;
    std::vector<std::string> originalHeaders_;
    std::vector<int> outCols_;
};

#endif // CSVFILTER_HEADERS_H
