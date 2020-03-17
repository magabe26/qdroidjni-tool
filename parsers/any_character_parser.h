/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#ifndef ANY_CHARACTER_PARSER_H
#define ANY_CHARACTER_PARSER_H
#include "core/core.h"

/**
 *------------------------------------------
 * @brief The AnyCharacterParser class
 */
class AnyCharacterParser: public Parser {
public:
    AnyCharacterParser(std::shared_ptr<DelegateInterface> end,std::string except);
    AnyCharacterParser(const AnyCharacterParser& other);
    virtual ~AnyCharacterParser();

    // DelegateInterface interface
    std::shared_ptr<DelegateInterface> makeShared();

    // Delegate interface
private:
    Result parseOn(Context context);
};


AnyCharacterParser any(const Parser& end,std::string except = std::string());

#endif // ANY_CHARACTER_PARSER_H
