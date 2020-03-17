/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#ifndef RESULT_H
#define RESULT_H

#include "def.h"

/**
 *  ---------------------------------------------------
 * @brief The Success class
 */
class Success{

public:
    Success(pos start, pos end);
    Success(const Success &other);

    Success &operator=(const Success &other);

    bool operator ==(Success other);
    bool operator !=(Success other);

    pos start() const;
    pos end() const;

    std::string value() const;
    Success  setValue(const std::string &value);

    std::string toString();
private:
    pos start_;
    pos end_;
    std::string value_;

};

/**
 *  ---------------------------------------------------
 * @brief The Failure class
 */
class Failure{

    // Result interface
public:
    Failure(pos position, std::string message = std::string());
    Failure(const Failure &other);

    Failure &operator=(const Failure &other);

    bool operator ==(Failure other);
    bool operator !=(Failure other);

    pos position() const;

    std::string message() const;

    std::string toString();

private:
    pos position_;
    std::string message_;
};
/**
 *  ---------------------------------------------------
 * @brief The Result class
 */
class Result
{
public:
    Result(const Result &other);

    Result(const Success &success);
    Result(const Failure &failure);


    bool isSuccess() const;
    bool isFailure() const;

    Success success() const;
    Failure failure() const;

    std::string toString();

private:
    Success success_;
    Failure failure_;
    bool isSuccess_;
    bool isFailure_;
};

#endif // RESULT_H
