/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#ifndef ERRORS_H
#define ERRORS_H

#include "def.h"

/**
 *--------------------------------------------------
 * @brief The Error class
 */
class Error
{

public:
    Error(std::string message);
    Error(const Error& other);
    virtual ~Error();
    virtual std::string toString();

protected:
    std::string message_;
};



/**
 *--------------------------------------------------
 * @brief The RangeError class
 */
class RangeError : public Error{
public:


    RangeError(pos invalidValue, pos minValue, pos maxValue,
               std::string name = std::string(""), std::string message= std::string(""));

    /** The minimum value that [value] is allowed to assume. */
    pos start() const;

    /** The maximum value that [value] is allowed to assume. */
    pos end() const;

    /** Message describing the problem. */
    std::string message() const;

    std::string toString();
    static pos checkValidRange(pos start, pos end, pos length,
                               std::string startName = std::string(""),
                               std::string endName = std::string(""), std::string message= std::string(""));
private:
    pos start_;
    pos end_;
    /** The invalid value. */
    pos invalidValue_;
    /** Name of the invalid argument, if available. */
    std::string name_;
};

#endif // ERRORS_H
