/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include "core/core.h"

class String_utils
{   
public:
    static std::string ltrim(std::string str);
    static std::string rtrim(std::string str);
    static std::string trim(std::string str);
    static pos str_index_in(std::string input, std::string str, pos start = 0);
    static std::string to_upper(std::string str);
    static std::string to_lower(std::string str);
};

#endif // STRING_UTILS_H
