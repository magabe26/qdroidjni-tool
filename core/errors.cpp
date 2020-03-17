/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#include "errors.h"

/**
 *--------------------------------------------------
 * @brief The Error class
 */
Error::Error(std::string message):message_{message}{}

Error::Error(const Error &other){
    message_ = other.message_;
}

Error::~Error(){}

std::string Error::toString(){
    return message_;
}


/**
 *--------------------------------------------------
 * @brief The RangeError class
 */
RangeError::RangeError(pos invalidValue, pos minValue, pos maxValue, std::string name, std::string message):Error(message){
    start_ = minValue;
    end_ = maxValue;
    this->invalidValue_ = invalidValue;
    this->name_ = name;
}

pos RangeError::start() const
{
    return start_;
}

pos RangeError::end() const
{
    return end_;
}

std::string RangeError::message() const
{
    if(message_.empty()){

        std::string explanation = "";
        if (end_ > start_) {
            explanation = ": Not in range " + std::to_string(start_) + ".." + std::to_string(end_)+ ", inclusive" ;
        } else if (end_ < start_) {
            explanation = ": Valid value range is empty";
        } else {
            explanation = ": Only valid value is " + std::to_string(start_);
        }
        return explanation;
    }else{
        return message_;
    }
}

std::string RangeError::toString(){
    return message();
}

pos RangeError::checkValidRange(pos start, pos end, pos length, std::string startName, std::string endName, std::string message) {
    if (start > length) {
        if(startName.empty())
            startName = "start";
        throw RangeError(start, 0, length, startName, message);
    }

    if (start > end || end > length) {
        if(endName.empty())
            endName = "end";
        throw RangeError(end, start, length, endName, message);
    }
    return end;
}
