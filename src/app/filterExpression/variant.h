//
// csvfilter, Copyright (c) 2015, plnu
//

#ifndef CSVFILTER_VARIANT_H
#define CSVFILTER_VARIANT_H

#include <string>
#include <memory>

class Variant;
typedef std::shared_ptr<Variant> VariantRef;

/**
 * @brief A simple variant type.
 *
 * This is a simple, immutable variant type that is used when evaluating
 * expressions. Along with the usual types (numbers, booleans, and strings), it
 * supports an error type that is used when an expression cannot be evaluated.
 *
 * To create a Variant use the type-specific static methods Variant::string,
 * Variant::error, Variant::number and Variant::boolean.
 *
 */
class Variant {
public:
    ~Variant();

    /**
     * @brief  The type contained by a variant.
     *
     * The type contained by a variant.
     */
    typedef enum {
        NUMBER, /**< A number, which is stored as a double */
        BOOLEAN, /**< A boolean */
        STRING, /**< A string */
        ERROR /**< An error message */
    } Type;

    Type type() const;
    double numberVal() const;
    bool booleanVal() const;
    const char* charVal() const;

    void resetToNumber(double num);
    void resetToString(const char* val);
    void resetToString(const std::string& val);
    void resetToError(const std::string& err);
    void resetToBoolean(bool b);
    
    static VariantRef string(const char* val);
    static VariantRef string(const std::string& val);
    static VariantRef error(const char* val);
    static VariantRef error(const std::string& val);
    static VariantRef number(double val);
    static VariantRef boolean(bool val);
    
private:
    Variant(Type t, const char* val);
    Variant(bool val);
    Variant(double val);

    void clear();
    union {
        double numberVal_;
        bool boolVal_;
        char* charVal_; // strings cannot be used in unions
    } val_;
    Type type_;
};

std::ostream& operator<< (std::ostream &out, Variant::Type t);


#endif // CSVFILTER_VARIANT_H
