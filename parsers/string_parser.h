/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#ifndef STRING_PARSER_H
#define STRING_PARSER_H

#include "core/core.h"

/**
 * @brief The StringParser class
 */
class StringParser: public Parser {
public:

    StringParser(std::string string,bool caseSensitive = true);
    StringParser(const StringParser& other );

    virtual ~StringParser();

    // DelegateInterface interface
    std::shared_ptr<DelegateInterface> makeShared();

private:
    std::string string_;
    bool caseSensitive_;
    pos failedAt_;

    // Delegate interface
    Result parseOn(Context context);
};

StringParser string(
        std::string string,
        bool caseSensitive = true
        );


#endif // STRING_PARSER_H
