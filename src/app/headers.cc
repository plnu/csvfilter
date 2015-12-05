//
// csvfilter, Copyright (c) 2015, plnu
//

#include "headers.h"

#include <sstream>
#include <iostream>
#include <utility>
#include <string>

#include "filterExpression/lexer.h"

/**
 * @brief constructor
 *
 * Construct the mappings between headers.
 *
 * Initialisation of this class may fail for two reasons:
 *
 *  - Headers in the input file are expected to be unique
 *  - The requested output headers must exist in the input
 *
 * Check Headers::ok to ensure this is true.
 *
 * @param headerLine     The headers from the input file, pre-parsed.
 * @param outputHeaders  The output headers that were specified on the command
 *                       line. Obtained from CmdOptions.
 */
Headers::Headers(const LineParser& headerLine,
                 const std::vector<std::string>& outputHeaders)
    :ok_(true),
     errorText_(""),
     headerIndexes_(),
     expressionTokens_(),
     originalHeaders_(),
     outCols_() {

    std::list<std::pair<const std::string, int>> tokensToTransform;

    for (int i = 0; ok_ && i < headerLine.fieldCount(); i++) {
        std::string h = headerLine.field(i)->asString();
        originalHeaders_.push_back(h);
        if (headerIndexes_.find(h) != headerIndexes_.end()) {
            tokensToTransform.push_back(std::make_pair(h, i));
        } else {
            headerIndexes_[h] = i;
            if (!Lexer::isIdentifier(h)) {
                tokensToTransform.push_back(std::make_pair(h, i));
            }
        }
    }

    if (ok_) {
        generateExpressionTokens(tokensToTransform);
        ok_ = readOutputHeaders(outputHeaders);
    }
}

/**
 * @brief Was initialisation successful?
 *
 * Check whether the class was successfully initialised. If this function
 * returns false then Headers::errText will contain a description of the error.
 *
 * @return  true if initialisation was successful, false otherwise
 *
 * @see Headers::errText
 */
bool Headers::ok() const {
    return ok_;
}

/**
 * @brief An error message
 *
 * @return  A description of the error if Headers::ok is false, an empty string
 *          otherwise
 *
 */
const std::string& Headers::errText() const {
    return errorText_;
}

/**
 * @brief Print a summary of the headers.
 *
 * This function prints a summary of the input headers to stdout. This summary
 * includes the header names and their aliases, if any.
 *
 */
void Headers::printHeaders() const {
    std::string headerTitle = "Header";

    int maxLength = headerTitle.length();
    for (int i = 0; i < originalHeaders_.size(); i++) {
        int length = originalHeaders_[i].size();
        if (length > maxLength) {
            maxLength = length;
        }
    }
    // padding between the columns
    maxLength += 2;

    printHeaderLine(headerTitle, "Alias", maxLength);
    printHeaderLine("======", "=====", maxLength);

    for (int i = 0; i < originalHeaders_.size(); i++) {
        std::string header = originalHeaders_[i];
        std::string alias = "";
        auto it = expressionTokens_.find(i);
        if (it != expressionTokens_.end()) {
            alias = (*it).second;
        }
        printHeaderLine(header, alias, maxLength);
    }
}

/**
 * @brief Get the index of an input column
 *
 * Given a header, return the index of that column in the input file.
 *
 * @param column  The header of a column
 *
 * @return  The index of that column, or -1 if the column isn't in the input
 *          file.
 *
 */
int Headers::indexOf(const std::string& column) const {
    int ret = -1;
    auto it = headerIndexes_.find(column);
    if (it != headerIndexes_.end()) {
        ret = (*it).second;
    }
    return ret;
}

/**
 * @brief The number of columns in the output file.
 *
 * @return  The number of columns in the output file.
 *
 */
int Headers::outColCount() const {
    return outCols_.size();
}

/**
 * @brief  Maps between output and input columns
 *
 * Given an output column, return the index of the input column that is used to
 * populate it. i.e. if outColIdx(2) = 5, then the data from the 5th input
 * column is used to populate the 3nd output column.
 *
 * @param  inIdx  The index of an output column
 *
 * @return  The index of the input column that supplies the data
 *
 */
int Headers::outColIdx(int inIdx) const {
    int ret = -1;
    if (inIdx >= 0 && inIdx < outColCount()) {
        ret = outCols_[inIdx];
    }
    return ret;
}

/**
 * @brief Make aliases for headers that are not valid in expressions
 * 
 * This function takes a list of headers that cannot be used in filter
 * expressions (because they use invalid characters or are duplicates) and
 * creates aliases for them. These aliases are added into expressionTokens_ and
 * headerIndexes_, so can be used in the same way as normal headers.
 *
 * @param tokensToTransform  A map from header to index in the row. This map
                             must only contain headers that need to have
                             aliases assigned.
 *
 */
void Headers::generateExpressionTokens(
    const std::list<std::pair<const std::string, int>>& tokensToTransform) {
    auto it = tokensToTransform.begin();

    while (it != tokensToTransform.end()) {
        bool done = false;
        std::string header = (*it).first;
        int index = (*it).second;

        const std::string candidate = Lexer::makeValidIdentifier(header);

        if (headerIndexes_.find(candidate) == headerIndexes_.end()) {
            done = true;
            headerIndexes_[candidate] = index;
            expressionTokens_[index] = candidate;
        }

        int i = 1;
        while (!done) {
            std::string newCandidate = candidate + std::to_string(i);
            if (headerIndexes_.find(newCandidate) == headerIndexes_.end()) {
                done = true;
                headerIndexes_[newCandidate] = index;
                expressionTokens_[index] = newCandidate;
            }
            i++;
        }
        it++;
    }
}

bool Headers::readOutputHeaders(
    const std::vector<std::string>& outputHeaders) {

    bool valid = true;
    if (outputHeaders.size() == 0) {
        for (int i = 0; i < headerIndexes_.size(); i++) {
            outCols_.push_back(i);
        }
    } else {
        for (auto it = outputHeaders.begin();
             valid && it != outputHeaders.end();
             it++) {
            int idx = indexOf(*it);
            if (idx < 0) {
                std::stringstream msg;
                msg << "No such column \"" << *it << "\"";
                errorText_ = msg.str();
                valid = false;
            } else {
                outCols_.push_back(idx);
            }
        }
    }
    return valid;
}

/**
 *
 * @brief Helper for Headers:printHeaders.
 *
 * This helper for Headers::printHeaders prints a single header, padding it
 * appropriately.
 *
 */
void Headers::printHeaderLine(const std::string& header,
                              const std::string& alias,
                              int headerWidth) const {
    std::cout << header;
    if (alias.size() > 0) {
        for (int i = header.size(); i < headerWidth; i++) {
            std::cout << " ";
        }
    }
    std::cout << alias << std::endl;
}
