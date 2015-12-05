//
// csvfilter, Copyright (c) 2015, plnu
//

#include "fileReader.h"

#include <sstream>
#include <stdio.h>
#include <string.h>

static const int MAX_LINE_LENGTH = 1024 * 1024;

/**
 * @brief Constructor
 *
 * Attempts to open the file and prepare it for reading. Use FileReader::ok to
 * check whether the file has been successfully opened, and FileReader::errText
 * to obtain an error message if the open fails.
 *
 * @param name  The name of the file that should be read.
 *
 */
FileReader::FileReader(const std::string& name)
    :line_(nullptr),
     file_(nullptr),
     ok_(true),
     errText_("") {
    line_ = new char[MAX_LINE_LENGTH];

    if (name == "") {
        file_ = stdin;
    } else {
        file_ = fopen(name.c_str(), "r");
        if (file_ == nullptr) {
            std::stringstream msg;
            msg << "Failed to open " << name << ": " << strerror(errno);
            setError(msg.str());
        }
    }
}

FileReader::~FileReader() {
    delete[] line_;
}

/**
 * @brief Was the file opened successfully?
 *
 * @return  true if the file was opened successfully, false otherwise.
 *
 */
bool FileReader::ok() const {
    return ok_;
}

/**
 * @brief Error description.
 *
 * If FileReader::ok returns false then this function can be used to get a
 * description of the error.
 *
 * @return  A description of the error
 *
 */
const std::string& FileReader::errText() const {
    return errText_;
}

/**
 * @brief Get the next line from the file.
 *
 * Get the next line from the file. The returned char* is valid until the next
 * call to FileReader::getLine. Other classes (especially LineParser) will edit
 * the line in-place to save string copies.
 *
 * @return  The next line from the file, or NULL if the end of the file has been
 *          reached.
 */
char* FileReader::getLine() {
    char* ret = nullptr;
    if (file_ != nullptr) {
        ret = fgets(line_, MAX_LINE_LENGTH, file_);
        if (ret == nullptr) {
            if (ferror(file_)) {
                std::stringstream msg;
                msg << "Error reading file" << ": " << strerror(errno);
                setError(msg.str());
            } else {
                // eof
            }
            closeFile();
        } else {
            // we need to check we had enough room for the complete line, and
            // strip the newline if necessary
            int length = strlen(line_);
            if (length == MAX_LINE_LENGTH - 1) {
                ret = nullptr;
                setError("Line too long");
                closeFile();
            } else {
                if (line_[length - 1] == '\n') {
                    line_[length - 1] = '\0';
                }
            }
        }
    }
    return ret;
}

void FileReader::setError(const std::string& msg) {
    errText_ = msg;
    ok_ = false;
}

void FileReader::closeFile() {
    fclose(file_); // best effort - we'll ignore errors
    file_ = nullptr;
}
