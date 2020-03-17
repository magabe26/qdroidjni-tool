/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#include "parser.h"
#include <iostream>

static const char *skipParserDelegate = "SkipParserDelegate";
static const char *debugParser = "DebugParser";
static const char *wrapParser = "WrapParser";

static const char *skipParser = "SkipParser";
static const char *optionalParser = "OptionalParser";
static const char *starParser = "StarParser";
static const char *plusParser = "PlusParser";
static const char *repeatParser = "RepeatParser";

pos strIndexIn(std::string input, std::string str, pos start){
    if(input.empty() || str.empty()){
        return npos;
    }
    pos count = 0;
    pos index = npos;
    for(pos inputIndex = 0;start < input.length() ; start++){
        if(count == str.length()){
            return index;
        }
        if(input[start] == str[inputIndex]){
            if(count == 0){
                index = start;
            }
            count++;
            inputIndex++;
        }else{
            count = 0;
            inputIndex = 0;
            index = npos;
        }
    }
    return index;
}

/**
 *--------------------------------------------------
 * @brief The Parser class
 */
Parser::Parser(std::shared_ptr<DelegateInterface> delegate, std::string name)
    :Delegate(name,delegate){}

Parser::Parser(std::string name, std::shared_ptr<DelegateInterface> d1, std::shared_ptr<DelegateInterface> d2)
    :Delegate(name,d1,d2)

{
}

/**
 * @brief   Parser::Parser , A very important method that helps assigning example: Parser p = CharParser() works,
 *          it make it possible for the appropriate parserOn method to be called
 * @param other
 */
Parser::Parser(const Parser &other)
    :Delegate(other.name()){
    d1_ = (other.d1_->name() == nullDelegate->name()) ? const_cast<Parser&>(other).makeShared() : other.d1_;
}

bool Parser::operator ==(const Parser &other){
    return Delegate::operator==(other);
}

bool Parser::operator !=(const Parser &other){
    return Delegate::operator!=(other);
}

Delegate &Parser::operator =(const Delegate &other){
    Delegate::operator=(other);
    return *this;
}

Parser Parser::seq(const Parser &parser)
{
    auto newParser = Parser(SeqParserDelegate(makeShared(),const_cast<Parser&>(parser).makeShared()).makeShared(),"SeqParser");
    newParser.copySkippers(*this);
    return  newParser;
}

Parser Parser::operator &(const Parser &parser){
    return seq(parser);
}

Parser Parser::oR(const Parser &parser){
    auto newParser = Parser(OrParserDelegate(makeShared(),const_cast<Parser&>(parser).makeShared()).makeShared(),"OrParser");
    newParser.copySkippers(*this);
    return  newParser;

}

Parser Parser::operator |(const Parser &other){
    return oR(other);
}

Parser Parser::optional() {
    auto newParser = Parser(OptionalParserDelegate(makeShared()).makeShared(),optionalParser);
    newParser.copySkippers(*this);
    return  newParser;

}

Parser Parser::star(){
    auto newParser = Parser(StarParserDelegate(makeShared()).makeShared(),starParser);
    newParser.copySkippers(*this);
    return  newParser;

}

Parser Parser::plus(){
    auto newParser = Parser(PlusParserDelegate(makeShared()).makeShared(),plusParser);
    newParser.copySkippers(*this);
    return  newParser;

}

Parser Parser::repeat(int min, int max){
    auto newParser = Parser(RepeatParserDelegate(makeShared(),min,max).makeShared(),repeatParser);
    newParser.copySkippers(*this);
    return  newParser;
}

bool Delegate::isSkipParser() {
    return (name() == skipParser || (name() == skipParserDelegate));
}

Parser Parser::skip(){
    auto skipper = SkipParserDelegate(makeShared()).makeShared();
    auto newParser = Parser(skipper,skipParser);
    newParser.skippers_->push_back(std::shared_ptr<Delegate>(new SkipParserDelegate(makeShared())));
    return newParser;
}

/**
 * @brief removeFrom ,removes success parser results from @param input
 * @param input
 * @param start
 * @param count , if set -1 ,it removes until end of @param input
 * @return
 */

std::string Parser::removeFrom(std::string input, pos start, int count) {
    return replaceIn(input,"",start,count);
}

/**
 * @brief Parser::replaceIn
 * @param input
 * @param replacement
 * @param start
 * @param count , if set -1 ,it removes until end of @param input
 * @return
 */
std::string Parser::replaceIn(std::string input,std::string replacement, pos start, int count) {

    Replace replace = [&replacement](std::string) ->std::string {
        return   replacement;
    };
    return replaceInMapped(input,replace,start,count);
}


/**
 * @brief Parser::replaceInMapped
 * @param input
 * @param replace
 * @param start
 * @param count , if set -1 ,it removes until end of @param input
 * @return
 */
std::string Parser::replaceInMapped(std::string input, Replace replace, pos start, int count) {
    std::string output = input;

    if (start < output.length()) {
        auto matches = allStringMatches(output, start);
        if (matches.empty()) {
            return output;
        }

        int c = 0;
        auto pos = start;
        for (auto match : matches) {
            const auto index = strIndexIn(output,match,pos);
            if(index != npos){
                const std::string replacement = replace(match);
                output.replace(index,match.length(), replacement);
                c++;
                pos += replacement.length();
                if ((count != -1) && (count == c)) {
                    break;
                }
            }

        }
        return output;
    } else {
        return output;
    }
}

bool Parser::hasMatch(std::string input, pos start)
{
    return firstMatch(input,start).isSuccess();
}

bool Parser::isNull(){
    return Delegate::isNull();
}

Result Parser::parseOn(Context context){
    try {
        if((d1_.get() == nullptr) && (nullDelegate->name() != name())){
            return Failure(context.position(),std::string("Could not parse because either"
                                                          "\n 1: a '"+name()+"' instance was "
                                                                             "returned from a method/function as a base class 'Parser' instance."
                                                                             "\n 2: a 'Parser' variable was used to hold an instance of "+name()+
                                                          "\nTo fix this ,Change that return type or variable type from "
                                                          "'Parser' to '"+name()+"'"));
        }else{
            return d1_->parse(context.buffer(),context.position());
        }
    } catch (Error e) {
        return Failure(context.position(),e.toString());
    }
}

std::shared_ptr<DelegateInterface> Parser::makeShared(){
    return std::shared_ptr<DelegateInterface>(new Parser(name(),d1_,d2_),&DelegateInterface::deleter);
}



/**
 *---------------------------
 * @brief The NullParser class
 */
NullParser::NullParser(){}


/**
  *--------------------------------------
 * @brief The DebugParser class
 */
DebugParser::DebugParser(): Parser(debugParser){}

Result DebugParser::parseOn(Context context){
    std::cout << ">>> DebugParser:\n input = "<< context.buffer() << "\n\nposition = " << context.position() <<" <<<";
    return Success(context.position(), context.position());
}

std::shared_ptr<DelegateInterface> DebugParser::makeShared(){
    return std::shared_ptr<DelegateInterface>(new DebugParser(),&DelegateInterface::deleter);
}


DebugParser debug() {return DebugParser();}



/**
 *-----------------------------------
 * @brief The WrapParser class
 */
WrapParser::WrapParser(std::shared_ptr<DelegateInterface> d):Parser(wrapParser,d){

}

std::shared_ptr<DelegateInterface> WrapParser::makeShared(){
    return std::shared_ptr<DelegateInterface>(new WrapParser(d1_),&DelegateInterface::deleter);
}

Result WrapParser::parseOn(Context context){
    try {
        return d1_->parse(context.buffer(),context.position());
    } catch (Error e) {
        return  Failure(context.position(),e.toString());
    }
}

WrapParser wrap(const Parser &another){
    return WrapParser(const_cast<Parser&>(another).makeShared());
}
