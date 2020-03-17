/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#include "char_parsers.h"

/**
 *-----------------------------------------------
 * @brief The CharParser class
 */
CharParser::CharParser(unsigned char c):PARSER_NAME(CharParser) {
    char_ = c;
}

CharParser::CharParser(const CharParser &other)
    :PARSER_NAME(CharParser)
    ,char_ {other.char_}{

}

CharParser::~CharParser(){}

Result CharParser::parseOn(Context context) {
    const std::string buffer = context.buffer();
    const auto position = context.position();
    if (position < buffer.length()) {
        if (buffer[position] == char_) {
            const auto start = position;
            const auto end = start + 1;
            return Success(start, end);
        } else {
            return Failure(position);
        }
    }
    return Failure(buffer.length());
}

std::shared_ptr<DelegateInterface> CharParser::makeShared(){
    return std::shared_ptr<DelegateInterface>(new CharParser(char_),&DelegateInterface::deleter);
}


/**
 *--------------------------------------------------
 * @brief The LastCharParserDelegate class
 */
LastCharParserDelegate::LastCharParserDelegate():PARSER_NAME(LastCharParserDelegate){}

Result LastCharParserDelegate::parseOn(Context context) {
    const std::string buffer = context.buffer();
    const auto position = context.position();
    if (position == buffer.length() - 1) {
        return Success(position, buffer.length());
    } else {
        return Failure(position, "Not the end of input");
    }
}

std::shared_ptr<DelegateInterface> LastCharParserDelegate::makeShared(){
    return std::shared_ptr<DelegateInterface>(new LastCharParserDelegate(),&DelegateInterface::deleter);
}


/**
 *--------------------------------------------------
 * @brief The LastCharParser class
 */
LastCharParser::LastCharParser():PARSER_NAME(LastCharParser){}

Result LastCharParser::parseOn(Context context) {
    try {
        return LastCharParserDelegate().plus().parse(context.buffer(),context.position());
    } catch (Error e) {
        return  Failure(context.position(),e.toString());
    }
}

std::shared_ptr<DelegateInterface> LastCharParser::makeShared(){
    return std::shared_ptr<DelegateInterface>(new LastCharParser(),&DelegateInterface::deleter);
}


/**
 *----------------------------------------------------
 * @brief The FirstCharParser class
 */
FirstCharParser::FirstCharParser():PARSER_NAME(FirstCharParser){}

Result FirstCharParser::parseOn(Context context) {
    const auto position = context.position();
    if (position == 0) {
        return Success(position, 1);
    } else {
        return Failure(position, "Not the end of input");
    }
}

std::shared_ptr<DelegateInterface> FirstCharParser::makeShared(){
    return std::shared_ptr<DelegateInterface>(new FirstCharParser(),&DelegateInterface::deleter);
}

CharParser chaR(unsigned char c) {
    return CharParser(c);
}

FirstCharParser firstChar()  {
    return FirstCharParser();
}

LastCharParser lastChar() {
    return LastCharParser();
}

Parser remainingChars() { return any(lastChar()).star().seq(lastChar()); }

Parser charsBefore(Parser parser) {return any(parser).star();}

Parser newLine() { return  chaR('\n'); }
