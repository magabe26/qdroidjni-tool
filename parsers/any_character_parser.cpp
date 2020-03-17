/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#include "any_character_parser.h"


/**
 *------------------------------------------
 * @brief The AnyCharacterParser class
 */

AnyCharacterParser::AnyCharacterParser(std::shared_ptr<DelegateInterface> end, std::string except)
    :Parser("AnyCharacterParser",end){
    s1_ = except;
}

AnyCharacterParser::~AnyCharacterParser(){}

Result AnyCharacterParser::parseOn(Context context) {
    const std::string buffer = context.buffer();
    const auto position = context.position();
    Result match = d1_->firstMatch(buffer, position);
    if (match.isSuccess()) {
        const auto start = position;
        const auto end = match.success().start();
        const std::string except = s1_;
        if (start < end) {
            if(except.empty()){
                return Success(start,start+1);
            }

            const auto pattern = std::string("([^").append(except).append("])");
            if (matchPattern(buffer[start],pattern)) {
                return Success(start,start+1);
            }else{
                return Failure(start, "AnyCharacterParser: unwanted character found");
            }
        }else{
            return Failure(start, "AnyCharacterParser: character not found");
        }
    } else {
        return Failure(position, "AnyCharacterParser: end not found");
    }

}

std::shared_ptr<DelegateInterface> AnyCharacterParser::makeShared(){
    return std::shared_ptr<DelegateInterface>(new AnyCharacterParser(d1_,s1_),&DelegateInterface::deleter);
}


AnyCharacterParser any(const Parser &end, std::string except) {
    return AnyCharacterParser(const_cast<Parser&>(end).makeShared(), except);
}

