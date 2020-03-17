/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#include "result.h"
#include <assert.h>

/**
 *  ---------------------------------------------------
 * @brief The Success class
 */

Success::Success(pos start, pos end){
    start_ = start;
    end_ = end;
}

Success::Success(const Success &other){
    start_ = other.start_;
    end_ = other.end_;
    value_ = other.value_;
}

Success &Success::operator=(const Success &other){
    start_ = other.start_;
    end_ = other.end_;
    value_ = other.value_;
    return *this;
}

bool Success::operator ==(Success other) {
    return  ((start_ == other.start_) &&
             (end_ == other.end_) &&
             (value_ == other.value_));
}

bool Success::operator !=(Success other) {
    return  ((start_ != other.start_) ||
             (end_ != other.end_) ||
             (value_ != other.value_));
}

pos Success::start() const
{
    return start_;
}

pos Success::end() const
{
    return end_;
}

std::string Success::value() const
{
    return value_;
}

Success Success::setValue(const std::string &value)
{
    if(value_.empty()){
        value_ = value;
    }
    return  *this;
}

std::string Success::toString() {
    return ">>> Success:  "+value_+
            " (start: "+std::to_string(start_)+
            " - end: "+std::to_string(end_)+") <<<";
}



/**
 *  ---------------------------------------------------
 * @brief The Failure class
 */
Failure::Failure(pos position, std::string message) {
    position_ = position;
    message_ = message;
}

Failure::Failure(const Failure &other){
    position_ = other.position_;
    message_ = other.message_;
}



Failure &Failure::operator=(const Failure &other){
    position_ = other.position_;
    message_ = other.message_;
    return *this;
}

bool Failure::operator ==(Failure other) {
    return  ((position_ == other.position_) &&
             (message_ == other.message_));
}

bool Failure::operator !=(Failure other) {
    return  ((position_ != other.position_) ||
             (message_ != other.message_));
}

pos Failure::position() const
{
    return position_;
}

std::string Failure::message() const
{
    return message_;
}

std::string Failure::toString() {
    return ">>> Failure:  (position: "+std::to_string(position_)+")\n"+message_+" <<<\n";
}

/**
 *  ---------------------------------------------------
 * @brief The Result class
 */

Result::Result(const Result &other)
    :success_ {other.success_}
    ,failure_{other.failure_}
{
    isSuccess_ = other.isSuccess_;
    isFailure_ = other.isFailure_;
}



Result::Result(const Success &success)
    :success_{success}
    ,failure_{Failure(0)}{
    isSuccess_ = true;
    isFailure_ = false;
}

Result::Result(const Failure &failure)
    :success_{Success(0,0)}
    ,failure_{failure}{
    isSuccess_ = false;
    isFailure_ = true;
}

bool Result::isSuccess() const{
    return  isSuccess_;
}

bool Result::isFailure() const{
    return  isFailure_;
}

Success Result::success() const
{
    assert(isSuccess_);
    return success_;
}

Failure Result::failure() const
{
    assert(isFailure_);
    return failure_;
}

std::string Result::toString(){
    return (isSuccess_)? success_.toString() : failure_.toString();
}

