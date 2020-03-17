/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#include "delegates.h"
#include <iostream>

static const char *nullDelegateName = "NullDelegate";
static const char *skipParserDelegate = "SkipParserDelegate";
static const char *successDelegate = "SuccessDelegate";
static const char *seqParserDelegate = "SeqParserDelegate";
static const char *orParserDelegate = "OrParserDelegate";
static const char *starParserDelegate = "StarParserDelegate";
static const char *plusParserDelegate = "PlusParserDelegate";
static const char *optionalParserDelegate = "OptionalParserDelegate";
static const char *repeatParserDelegate = "RepeatParserDelegate";

static const char *skipParser = "SkipParser";

/**
 * @brief DelegateInterface::name
 * @return
 */
DelegateInterface::DelegateInterface(std::string name)
    :pos1_{npos}
    ,pos2_{npos}
    ,i1_{-1}
    ,i2_{-1}
    ,c1_{' '}
    ,c2_{' '}
    ,s1_{std::string()}
    ,s2_{std::string()}
    ,name_{name}
{}

DelegateInterface::DelegateInterface(const DelegateInterface &other)
{
    name_ = other.name_;
    pos1_ = other.pos1_;
    pos2_ = other.pos2_;
    i1_ = other.i1_;
    i2_ = other.i2_;
    c1_ = other.c1_;
    c2_ = other.c2_;
    s1_ = other.s1_;
    s2_ = other.s2_;

}

DelegateInterface::~DelegateInterface() {}

bool DelegateInterface::operator ==(const DelegateInterface &other)
{
    return (this == &other) || ((name_ == other.name_) &&
                                (pos1_ == other.pos1_) &&
                                (pos2_ == other.pos2_) &&
                                (i1_ == other.i1_) &&
                                (i2_ == other.i2_) &&
                                (c1_ == other.c1_) &&
                                (c2_ == other.c2_) &&
                                (s1_ == other.s1_) &&
                                (s2_ == other.s2_));
}

bool DelegateInterface::operator !=(const DelegateInterface &other)
{
    return ((name_ != other.name_) ||
            (pos1_ != other.pos1_) ||
            (pos2_ != other.pos2_) ||
            (i1_ != other.i1_) ||
            (i2_ != other.i2_) ||
            (c1_ != other.c1_) ||
            (c2_ != other.c2_) ||
            (s1_ != other.s1_) ||
            (s2_ != other.s2_));
}

DelegateInterface &DelegateInterface::operator =(const DelegateInterface &other)
{
    name_ = other.name_;
    pos1_ = other.pos1_;
    pos2_ = other.pos2_;
    i1_ = other.i1_;
    i2_ = other.i2_;
    c1_ = other.c1_;
    c2_ = other.c2_;
    s1_ = other.s1_;
    s2_ = other.s2_;
    return *this;
}

void DelegateInterface::deleter(DelegateInterface *obj){
    if(obj != nullptr){
#ifdef DEBUG_MESSAGE
        std::string name = obj->name_;
        long long addr= reinterpret_cast<long long>(obj);
#endif
        delete obj;
        obj = nullptr;
#ifdef DEBUG_MESSAGE
        std::cout << "DelegateInterface::deleter : " << name <<"::" << addr;
#endif
    }
}

bool DelegateInterface::isNull(){
    return ((pos1_ == npos) &&
            (pos2_ == npos) &&
            (i1_ == -1) &&
            (i2_ == -1) &&
            (c1_ == ' ') &&
            (c2_ == ' ') &&
            (s1_.empty()) &&
            (s2_.empty()));
}

std::string DelegateInterface::name() const {return name_;}




/**
*---------------------------------------
* @brief The NullDelegate class
*/

NullDelegate::NullDelegate(std::string message)
    :DelegateInterface(nullDelegateName/*to pass the null delegate test*/)
    ,message_{message}{}

Result NullDelegate::firstMatch(std::string , pos start){
    return  Failure(start,message_);
}

std::string NullDelegate::firstStringMatch(std::string , pos ){
    return "";
}

Result NullDelegate::lastMatch(std::string , pos start){
    return  Failure(start,message_);
}

std::string NullDelegate::lastStringMatch(std::string , pos ){
    return "";
}

std::vector<std::string> NullDelegate::allStringMatches(std::string, pos){
    return std::vector<std::string>();
}

std::vector<Success> NullDelegate::allMatches(std::string , pos ){
    return std::vector<Success>();
}

Result NullDelegate::parse(std::string , pos start){
    return  Failure(start,message_);
}

std::shared_ptr<DelegateInterface> NullDelegate::makeShared(){
    return std::shared_ptr<DelegateInterface>(new NullDelegate(),&DelegateInterface::deleter);
}

Result NullDelegate::parseOn(Context context){
    return  Failure(context.position(),message_);
}


/**
  *-----------------------------------------
 * @brief The Delegate class
 */

Delegate::Delegate(std::string name,std::shared_ptr<DelegateInterface> d1,std::shared_ptr<DelegateInterface> d2)
    :DelegateInterface(name)
    ,d1_ {d1}
    ,d2_ {d2}
    ,skippers_(std::shared_ptr<std::vector<std::shared_ptr<Delegate>>>(new std::vector<std::shared_ptr<Delegate>>(),&Delegate::skippersDeleter)){}

bool Delegate::operator ==(const Delegate &other){
    return (DelegateInterface::operator==(other) &&
            (skippers_->size() == other.skippers_->size()));
}

bool Delegate::operator !=(const Delegate &other){
    return (DelegateInterface::operator!=(other)||
            (skippers_->size() != other.skippers_->size()));
}

Delegate& Delegate::Delegate::operator =(const Delegate &other){
    DelegateInterface::operator=(other);
    d1_ = other.d1_;
    d2_ = other.d2_;
    return *this;
}

Result Delegate::firstMatch(std::string input, pos start) {
    auto position =  getQualifiedPosition(input,start);

    std::string message;
    while (position < input.length()) {
        Result result = parseImpl(input, position);
        if (result.isSuccess()) {
            return result;
        } else {
            message = result.failure().message();
            position++;
        }
    }
    return Failure(position, message);
}

std::string Delegate::firstStringMatch(std::string input, pos start) {
    const Result result = firstMatch(input, start);
    if (result.isSuccess()) {
        return result.success().value();
    } else {
        return std::string();
    }
}

Result Delegate::lastMatch(std::string input, pos start) {
    auto matches = allMatches(input, start);
    return (!matches.empty()) ? Result(matches.at(matches.size() - 1))  : Failure(start);
}

std::string Delegate::lastStringMatch(std::string input, pos start) {
    Result match = lastMatch(input, start);
    if (match.isSuccess()) {
        return match.success().value();
    } else {
        return std::string();
    }
}

std::vector<std::string> Delegate::allStringMatches(std::string input, pos start) {
    auto list = std::vector<std::string>();
    const auto matches = allMatches(input, start);
    for(auto match : matches){
        list.push_back(match.value());
    }
    return  list;
}

std::vector<Success> Delegate::allMatches(std::string input, pos start) {
    auto list = std::vector<Success>();
    auto position =  getQualifiedPosition(input,start);

    while (position < input.length()) {
        Result result = parseImpl(input, position);
        if (result.isSuccess()) {
            const std::string value = result.success().value();
            Success success = result.success();
            if (!value.empty()) {
                list.push_back(success);
            }
            if (position == success.end()) {
                position++;
            } else {
                position = success.end();
            }
        } else {
            position++;
        }
    }
    return list;
}

Result Delegate::parse(std::string input, pos start) {
    return parseImpl(input,getQualifiedPosition(input,start));
}

Result Delegate::parseImpl(std::string input, pos start) {
    auto position =  start;
    RangeError::checkValidRange(position, input.length(), input.length());
    Result result = parseOn(Context(input, position));
    if (result.isSuccess()) {
        Success success = result.success();
        const auto start = success.start();
        const auto end = success.end();
        RangeError::checkValidRange(start, end, input.length());
        return success.setValue(input.substr(start,end - start));
    } else {
        return result;
    }
}

bool Delegate::isNull()
{
    const auto d1Null = ((dynamic_cast<NullDelegate*>(d1_.get()) == nullptr) || d1_->isNull());
    const auto d2Null = ((dynamic_cast<NullDelegate*>(d2_.get()) == nullptr) || d2_->isNull());
    return (DelegateInterface::isNull() || (d1Null && d2Null));
}

void Delegate::copySkippers(const Delegate &from)
{
    skippers_ = from.skippers_;
}

pos Delegate::getQualifiedPosition(std::string input, pos start){
    if(!skippers_->empty()){
        int index = 0;
        for(auto skipper : *skippers_.get()){
            if(skipper->isSkipParser()){
                try {
                    Result result =  skipper->parse(input,start);
                    if(result.isSuccess()){
                        if(result.success().end() > start){
                            start = result.success().end();

                        }
                    }
                } catch (...) {}
            }
            index++;
        }
        skippers_->clear();
    }

    return start;
}

void Delegate::skippersDeleter(std::vector<std::shared_ptr<Delegate> > *skippers){
    if(skippers != nullptr){
#ifdef DEBUG_MESSAGE
        long long addr = reinterpret_cast<long long>(skippers);
#endif
        skippers->clear();
        delete skippers;
        skippers = nullptr;
#ifdef DEBUG_MESSAGE
        std::cout << "skippers list #"<<addr << "deleted";
#endif
    }
}

/**
*--------------------------------------
* @brief The TestDelegate class
*/
SuccessDelegate::SuccessDelegate(pos start, pos end)
    :Delegate(successDelegate){
    pos1_ = start;
    pos2_ = end;
}

Result SuccessDelegate::parseOn(Context ){
    return  Success(pos1_,pos2_);
}

std::shared_ptr<DelegateInterface> SuccessDelegate::makeShared(){
    return std::shared_ptr<DelegateInterface>(new SuccessDelegate(pos1_,pos2_),&DelegateInterface::deleter);
}


/**
*--------------------------------------------------
* @brief The SeqParserDelegate class
*/
SeqParserDelegate::SeqParserDelegate(std::shared_ptr<DelegateInterface> first, std::shared_ptr<DelegateInterface> second)
    :Delegate(seqParserDelegate,first,second){}

Result SeqParserDelegate::parseOn(Context context){
    const std::string buffer = context.buffer();
    const auto start = context.position();
    auto position = context.position();

    if(d1_->name() == skipParser){
        try {
            return d2_->parse(buffer, position);
        } catch (Error e) {
            return Failure(position,e.toString());
        }
    }

    try {
        Result result = d1_->parse(buffer, position);
        if (result.isSuccess()) {
            position = result.success().end();
            try {
                result = d2_->parse(buffer, position);
                if (result.isSuccess()) {
                    position = result.success().end();

                } else {
                    return result;
                }
            } catch (Error e) {
                return Failure(position,e.toString());
            }
        } else {
            return result;
        }
    } catch (Error e) {
        return Failure(position,e.toString());
    }

    return Success(start, position).setValue(buffer.substr(start,position - start));

}

std::shared_ptr<DelegateInterface> SeqParserDelegate::makeShared(){
    return std::shared_ptr<DelegateInterface>(new SeqParserDelegate(d1_, d2_),&DelegateInterface::deleter);
}



/**
*--------------------------------------------------
* @brief The OrParserDelegate class
*/
OrParserDelegate::OrParserDelegate(std::shared_ptr<DelegateInterface> first, std::shared_ptr<DelegateInterface> second)
    :Delegate(orParserDelegate,first,second){}

Result OrParserDelegate::parseOn(Context context){

    try {
        Result result = d1_->parse(context.buffer(), context.position());
        if (result.isFailure()) {
            try {
                result = d2_->parse(context.buffer(), context.position());
            } catch (Error e) {
                result = Failure(context.position(), e.toString());
            }
        }
        return result;
    } catch (Error e) {
        try {
            return d2_->parse(context.buffer(), context.position());
        } catch (Error e) {
            return Failure(context.position(), e.toString());
        }
    }
}

std::shared_ptr<DelegateInterface> OrParserDelegate::makeShared(){
    return std::shared_ptr<DelegateInterface>(new OrParserDelegate(d1_,d2_),&DelegateInterface::deleter);
}


/**
* ---------------------------------------------
* @brief The StarParserDelegate class
*/
StarParserDelegate::StarParserDelegate(std::shared_ptr<DelegateInterface> delegate)
    :Delegate(starParserDelegate,delegate){}

Result StarParserDelegate::parseOn(Context context){
    const auto start = context.position();
    auto end = context.position();
    try {
        Result result = d1_->parse(context.buffer(), end);
        while (result.isSuccess()) {
            end = result.success().end();
            try {
                result = d1_->parse(context.buffer(), end);
            } catch (Error e) {
                break;
            }
        }
        return Success(start, end);
    } catch (Error e) {
        return Success(start, end);
    }
}

std::shared_ptr<DelegateInterface> StarParserDelegate::makeShared(){
    return std::shared_ptr<DelegateInterface>(new StarParserDelegate(d1_),&DelegateInterface::deleter);
}


/**
*-------------------------------------------------
* @brief The PlusParserDelegate class
*/
PlusParserDelegate::PlusParserDelegate(std::shared_ptr<DelegateInterface> delegate)
    :Delegate(plusParserDelegate,delegate){}

Result PlusParserDelegate::parseOn(Context context){
    const auto start = context.position();
    auto end = context.position();
    bool success = false;

    try {
        Result result = d1_->parse(context.buffer(), end);
        std::string message("");
        while (result.isSuccess()) {
            end = result.success().end();
            if (!success) {
                success = true;
            }
            try {
                result = d1_->parse(context.buffer(), end);
            } catch (Error e) {
                end = result.failure().position();
                message = e.toString();
                break;
            }
        }
        if (success) {
            return Success(start, end);
        } else {
            return Failure(end,message);
        }
    } catch (Error e) {
        return Failure(context.position(),e.toString());
    }
}

std::shared_ptr<DelegateInterface> PlusParserDelegate::makeShared(){
    return std::shared_ptr<DelegateInterface>(new PlusParserDelegate(d1_),&DelegateInterface::deleter);
}


/**
*-------------------------------------
* @brief The OptionalParserDelegate class
*/
OptionalParserDelegate::OptionalParserDelegate(std::shared_ptr<DelegateInterface> delegate)
    :Delegate(optionalParserDelegate,delegate){}

Result OptionalParserDelegate::parseOn(Context context){
    const auto start = context.position();
    auto end = context.position();
    try {
        Result result = d1_->parse(context.buffer(), end);
        if (result.isSuccess()) {
            return Success(start, result.success().end());
        } else {
            return Success(start, end);
        }
    } catch (Error e) {
        return Success(start, end);
    }
}

std::shared_ptr<DelegateInterface> OptionalParserDelegate::makeShared(){
    return std::shared_ptr<DelegateInterface>(new OptionalParserDelegate(d1_),&DelegateInterface::deleter);
}


/**
*--------------------------------------------------
* @brief The RepeatParserDelegate class
*/
RepeatParserDelegate::RepeatParserDelegate(std::shared_ptr<DelegateInterface> delegate, int min, int max)
    :Delegate(repeatParserDelegate,delegate){
    i1_ = min;
    i2_ = max;
}

Result RepeatParserDelegate::parseOn(Context context){
    const auto min = i1_;
    const auto max = i2_;

    const std::string buffer = context.buffer();
    const auto start = context.position();
    auto end = context.position();
    bool firstLoopEndedWithFailure = false;
    bool success = false;
    std::string message("");

    for (int i = 0; i < min; i++) {
        try {
            const Result result = d1_->parse(buffer, end);
            if (result.isSuccess()) {
                end = result.success().end();
                if (!success) {
                    success = true;
                }
            } else {
                message = result.failure().message();
                firstLoopEndedWithFailure = true;
                break;
            }
        } catch (Error e) {
            message = e.toString();
            firstLoopEndedWithFailure = true;
            break;
        }
    }

    if (firstLoopEndedWithFailure || (!success)) {
        return Failure(start,message);
    } else {
        if (max <= min) {
            return Success(start, end);
        } else {
            for (int i = 0; i < (max - min); i++) {
                try {
                    const Result result = d1_->parse(buffer, end);
                    if (result.isSuccess()) {
                        end = result.success().end();
                    }
                } catch (Error e) {
                    break;
                }
            }
            return Success(start, end);
        }
    }
}

std::shared_ptr<DelegateInterface> RepeatParserDelegate::makeShared(){
    return std::shared_ptr<DelegateInterface>(new RepeatParserDelegate(d1_,i1_,i2_),&DelegateInterface::deleter);
}

/**
 *---------------------------------------------------
* @brief The SkipParserDelegate class
*/
SkipParserDelegate::SkipParserDelegate(std::shared_ptr<DelegateInterface> delegate)
    :Delegate(skipParserDelegate,delegate){}

Result SkipParserDelegate::parseOn(Context context){
    const std::string buffer = context.buffer();
    const pos position = context.position();
    try {
        const Result result = d1_->parse(buffer, position);
        if (result.isSuccess()) {
            auto success = result.success();
            return Success(success.end(), success.end());
        } else {
            return Success(position, position);
        }
    } catch (Error e) {
        return Success(position, position);
    }
}

std::shared_ptr<DelegateInterface> SkipParserDelegate::makeShared(){
    return std::shared_ptr<DelegateInterface>(new SkipParserDelegate(d1_),&DelegateInterface::deleter);
}
