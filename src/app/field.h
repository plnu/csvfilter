//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef CSVFILTER_FIELD_H
#define CSVFILTER_FIELD_H
#include <memory>


/**
 *
 * @brief A single field from the CSV file.
 *
 */
class Field {
public:
    Field(const char* rawVal);
    ~Field();

    void reset(const char* rawVal);

    const char* asString();
    bool asNumber(double& val);
    const char* raw() const;
private:
    typedef enum {
        YES,
        NO,
        UNKNOWN
    } Maybe;
    Field(const Field& other);
    Field& operator=(const Field& other);

    const char* rawVal_;
    char* stringVal_;
    Maybe canBeNumber_;
    double doubleVal_;
};

typedef std::shared_ptr<Field> FieldRef;

#endif //CSVFILTER_FIELD_H
