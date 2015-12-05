//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef CSVFILTER_FILE_READER_H
#define CSVFILTER_FILE_READER_H

#include <string>
#include <stdio.h>

/**
 * @brief Reads a file
 *
 * Class that reads the contents of a file, a line at a time.
 *
 */
class FileReader {
public:
    FileReader(const std::string& name);
    ~FileReader();
    
    bool ok() const;
    const std::string& errText() const;

    char* getLine();

private:
    // copy and assignment opterators
    FileReader(const FileReader& other);
    FileReader& operator=(const FileReader& other);

    void setError(const std::string& msg);
    void closeFile();
    char* line_;
    FILE* file_;
    bool ok_;
    std::string errText_;
};

#endif // CSVFILTER_FILE_READER_H
