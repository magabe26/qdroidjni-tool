/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#ifndef FUNCTION_PARSER_H
#define FUNCTION_PARSER_H

#include "parsers/parsers.h"

//fn_declaration is a Parser that parse a function declaration in the form of 'return_type function(arguments)'
class Function_parser : public Parser{
public:

    Function_parser(std::shared_ptr<DelegateInterface> fn_declaration);
    std::shared_ptr<DelegateInterface> makeShared();

    // DelegateInterface interface
private:
    Result parseOn(Context context);
};

Parser fn_declaration(std::string class_name);
Function_parser fn(std::string class_name);
Function_parser fn(const Parser &fn_declaration);

#endif // FUNCTION_PARSER_H
