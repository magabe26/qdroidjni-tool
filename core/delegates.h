/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#ifndef DELEGATES_H
#define DELEGATES_H

#include "context.h"
#include "result.h"
#include "errors.h"
#include "def.h"
#include <vector>
#include <memory>


/**
 *--------------------------------------------
 * @brief The DelegateInterface class
 */
class DelegateInterface{
public:
    DelegateInterface(std::string name = "");
    DelegateInterface (const DelegateInterface &other);

    virtual  ~DelegateInterface();

    bool operator ==(const DelegateInterface& other);
    bool operator !=(const DelegateInterface& other);

    DelegateInterface& operator =(const DelegateInterface &other);

    virtual Result firstMatch(std::string input, pos start = 0) = 0;
    virtual std::string firstStringMatch(std::string input, pos start = 0) = 0;
    virtual Result lastMatch(std::string input, pos start = 0) = 0;
    virtual std::string lastStringMatch(std::string input, pos start = 0) = 0;
    virtual std::vector<std::string> allStringMatches(std::string input, pos start = 0) = 0;
    virtual std::vector<Success> allMatches(std::string input, pos start = 0) =0;
    virtual Result parse(std::string input, pos start = 0) = 0;
    virtual std::shared_ptr<DelegateInterface> makeShared() = 0;
    static  void deleter(DelegateInterface* obj);
    virtual bool isNull();
    std::string name() const;

protected:
    pos pos1_;
    pos pos2_;
    int i1_;
    int i2_;
    char c1_;
    char c2_;
    std::string s1_;
    std::string s2_;
private:
    std::string name_;
    virtual Result parseOn(Context context) = 0;
};



/**
 *---------------------------------------
 * @brief The NullDelegate class
 */
class NullDelegate : public DelegateInterface{
    // DelegateInterface interface
public:
    NullDelegate(std::string message = "NullDelegate");

    Result firstMatch(std::string, pos);
    std::string firstStringMatch(std::string, pos);
    Result lastMatch(std::string, pos start);
    std::string lastStringMatch(std::string, pos);
    std::vector<std::string> allStringMatches(std::string, pos);
    std::vector<Success> allMatches(std::string input, pos start);
    Result parse(std::string input, pos start);
    std::shared_ptr<DelegateInterface> makeShared();

private:
   Result parseOn(Context context);
   std::string message_;
};


static const std::shared_ptr<DelegateInterface>  nullDelegate = NullDelegate().makeShared();


/**
  *-----------------------------------
 * @brief The Delegate class
 */

class Delegate : public DelegateInterface{

public:

    Delegate(std::string name = "",std::shared_ptr<DelegateInterface> d1 = nullDelegate,std::shared_ptr<DelegateInterface> d2 = nullDelegate);
    virtual  ~Delegate(){}

    bool operator ==(const Delegate& other);
    bool operator !=(const Delegate& other);

    Delegate& operator =(const Delegate &other);

    Result firstMatch(std::string input, pos start = 0);
    std::string firstStringMatch(std::string input, pos start = 0);
    Result lastMatch(std::string input, pos start = 0);
    std::string lastStringMatch(std::string input, pos start = 0);
    std::vector<std::string> allStringMatches(std::string input, pos start = 0);
    std::vector<Success> allMatches(std::string input, pos start = 0);
    Result parse(std::string input, pos start = 0);
    bool isNull();

protected:
    std::shared_ptr<DelegateInterface> d1_; //do not use to test in operator == or !=
    std::shared_ptr<DelegateInterface> d2_; //do not use to test in operator == or !=
    std::shared_ptr<std::vector<std::shared_ptr<Delegate>>> skippers_; //do not use to test in operator == or !=

    void copySkippers(const Delegate& from);

private:
    virtual Result parseOn(Context context) = 0;
    bool isSkipParser();
    pos getQualifiedPosition(std::string input, pos start);
    static void skippersDeleter(std::vector<std::shared_ptr<Delegate>> *skippers);
    Result parseImpl(std::string input, pos start);
};



/**
 *--------------------------------------
 * @brief The TestDelegate class
 */
class SuccessDelegate:public  Delegate{

    // Delegate interface
public:
    SuccessDelegate(pos start, pos end);
    // Delegate interface
    std::shared_ptr<DelegateInterface> makeShared();

private:
    Result parseOn(Context);
};
/**
 *--------------------------------------------------
 * @brief The SeqParserDelegate class
 */
class SeqParserDelegate: public Delegate{

public:
    SeqParserDelegate(std::shared_ptr<DelegateInterface> first
                      , std::shared_ptr<DelegateInterface> second);

    // DelegateInterface interface
    std::shared_ptr<DelegateInterface> makeShared();

    // Delegate interface
private:
    Result parseOn(Context context);
};

/**
 *--------------------------------------------------
 * @brief The OrParserDelegate class
 */
class OrParserDelegate: public Delegate{

public:
    OrParserDelegate(std::shared_ptr<DelegateInterface> first
                     ,std::shared_ptr<DelegateInterface> second);

    // DelegateInterface interface
    std::shared_ptr<DelegateInterface> makeShared();

    // Delegate interface
private:
    Result parseOn(Context context);
};

/**
 * ---------------------------------------------
 * @brief The StarParserDelegate class
 */
class StarParserDelegate: public Delegate{

public:
    StarParserDelegate(std::shared_ptr<DelegateInterface> delegate);

    // DelegateInterface interface
    std::shared_ptr<DelegateInterface> makeShared();

    // Delegate interface
private:
    Result parseOn(Context context);
};

/**
 *-------------------------------------------------
 * @brief The PlusParserDelegate class
 */
class PlusParserDelegate: public Delegate{

public:
    PlusParserDelegate(std::shared_ptr<DelegateInterface> delegate);

    // DelegateInterface interface
    std::shared_ptr<DelegateInterface> makeShared();

    // Delegate interface
private:
    Result parseOn(Context context);
};

/**
 *-------------------------------------
 * @brief The OptionalParserDelegate class
 */
class OptionalParserDelegate: public Delegate{

public:
    OptionalParserDelegate(std::shared_ptr<DelegateInterface> delegate);

    // DelegateInterface interface
    std::shared_ptr<DelegateInterface> makeShared();

    // Delegate interface
private:
    Result parseOn(Context context);
};

/**
 *--------------------------------------------------
 * @brief The RepeatParserDelegate class
 */
class RepeatParserDelegate: public Delegate{

public:
    RepeatParserDelegate(std::shared_ptr<DelegateInterface> delegate, int min, int max);

    // DelegateInterface interface
    std::shared_ptr<DelegateInterface> makeShared();

    // Delegate interface
private:
    Result parseOn(Context context);
};


/**
  *---------------------------------------------------
 * @brief The SkipParserDelegate class
 */
class SkipParserDelegate: public Delegate{

public:
    SkipParserDelegate(std::shared_ptr<DelegateInterface> delegate);

    // DelegateInterface interface
    std::shared_ptr<DelegateInterface> makeShared();

    // Delegate interface
private:
    Result parseOn(Context context);
};


#endif // DELEGATES_H
