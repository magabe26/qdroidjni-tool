/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#include "pattern_parser.h"


PatternParser::PatternParser(const char *pattern)
    : PARSER_NAME(PatternParser) {
    pattern_ = pattern;
}

PatternParser::PatternParser(const PatternParser &other)
    : PARSER_NAME(PatternParser){
    pattern_ = other.pattern_;
}

Result PatternParser::parseOn(Context context) {
    const std::string buffer = context.buffer();
    const auto position = context.position();

    if(position < buffer.length()){
        const auto pattern = std::string("([").append(pattern_).append("])");
        if (matchPattern(buffer[position],pattern)) {
            return Success(position, position+1);
        }else{
            return Failure(position);
        }
    }else{
        return Failure(buffer.length());
    }
}

std::shared_ptr<DelegateInterface> PatternParser::makeShared(){
    return std::shared_ptr<DelegateInterface>(new PatternParser(pattern_),&DelegateInterface::deleter);
}

PatternParser pattern(const char *pattern) {return  PatternParser(pattern);}

PatternParser letter() {return pattern("a-zA-Z");}

PatternParser digit() {return  pattern("0-9");}

Parser word() { return (letter() | digit()); }
