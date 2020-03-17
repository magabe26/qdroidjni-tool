/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#include "string_parser.h"

StringParser::StringParser(std::string string, bool caseSensitive)
    :PARSER_NAME(StringParser)
    ,string_{string}
    ,caseSensitive_{caseSensitive}
    ,failedAt_ {npos}{

}

StringParser::StringParser(const StringParser &other):PARSER_NAME(StringParser){
    string_ = other.string_;
    caseSensitive_ = other.caseSensitive_;
    failedAt_ = other.failedAt_;
}

StringParser::~StringParser(){}

Result StringParser::parseOn(Context context) {
    const std::string buffer = context.buffer();
    if (context.position() < buffer.length()) {
        if ((failedAt_ != npos) && (context.position() <= failedAt_)) {
            return Failure(context.position(), "StringParser::Failed , No match found");
        }
        const pos start = context.position();
        pos end = context.position();

        for (pos i = 0; i < string_.length(); i++, end++) {
            const bool match = caseSensitive_
                    ? (string_[i] == buffer[end])
                    : ((toupper(string_[i]) == buffer[end]) ||
                       (tolower(string_[i]) == buffer[end]));

            if (!match) {
                failedAt_ = end;
                return Failure(failedAt_, "StringParser:: Failed , No match found");
            }
        }
        failedAt_ = npos;
        return Success(start, end);
    } else {
        return Failure(buffer.length(), "StringParser:: Failed , No match found");
    }
}

std::shared_ptr<DelegateInterface> StringParser::makeShared(){
    return std::shared_ptr<DelegateInterface>(new StringParser(string_,caseSensitive_),&DelegateInterface::deleter);
}

StringParser string(std::string string, bool caseSensitive) {return StringParser(string,caseSensitive);}
