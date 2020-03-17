/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#ifndef PARSER_H
#define PARSER_H

#define PARSER_NAME(name) Parser(#name)

#include "def.h"
#include "context.h"
#include "result.h"
#include "errors.h"
#include "delegates.h"
#include <functional>

typedef std::function<std::string (std::string match)> Replace;

/**
 * @brief strIndexIn returns the index of @param str in   @param input start at  @param start
 * @param input
 * @param str
 * @param start
 * @return
 */
pos strIndexIn(std::string input,std::string str,pos start);

/**
 *--------------------------------------------------
 * @brief The Parser class
 */
class Parser: public Delegate{

public:
    Parser(std::shared_ptr<DelegateInterface> delegate, std::string name);
    Parser(std::string name = "", std::shared_ptr<DelegateInterface> d1 = nullDelegate, std::shared_ptr<DelegateInterface> d2 = nullDelegate);
    Parser(const Parser &other);

    bool operator ==(const Parser& other);
    bool operator !=(const Parser& other);

    Delegate& operator =(const Delegate &other);

    Parser seq(const Parser &parser);
    Parser operator &(const Parser &parser);
    Parser oR(const Parser &parser);
    Parser operator |(const Parser &other);
    Parser optional();
    Parser star();
    Parser plus();
    Parser repeat(int min, int max = -1);
    Parser skip();
    virtual std::string removeFrom(std::string input, pos start = 0, int count = -1);
    virtual std::string replaceIn(std::string input, std::string replacement, pos start = 0, int count = -1);
    virtual std::string replaceInMapped(std::string input,Replace replace, pos start = 0, int count = -1);
    bool hasMatch(std::string input, pos start = 0);
    bool isNull();

    // DelegateInterface interface
    std::shared_ptr<DelegateInterface> makeShared();

private:
    // Delegate interface
    virtual Result parseOn(Context context);
};


/**
 *---------------------------
 * @brief The NullParser class
 */
class NullParser : public Parser{
public:
    NullParser();
};


/**
  *--------------------------------------
 * @brief The DebugParser class
 */
class DebugParser: public Parser {

public:
    DebugParser();

    // DelegateInterface interface
    std::shared_ptr<DelegateInterface> makeShared();

    // Delegate interface
private:
    Result parseOn(Context context);
};

DebugParser debug();


/**
 *-----------------------------------
 * @brief The WrapParser class
 */
class WrapParser: public Parser{
public:
    WrapParser(std::shared_ptr<DelegateInterface> d);

    std::shared_ptr<DelegateInterface> makeShared() override;
private:
    Result parseOn(Context context) override;
};

WrapParser wrap(const Parser& another);


#endif // PARSER_H
