/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#ifndef WHITESPACE_UTILS_H
#define WHITESPACE_UTILS_H

#include <sstream>

class Whitespace_utils
{
public:
    static void write_space(std::stringstream &sstream, int count);
    static void write_newline(std::stringstream &sstream, int count);
    static void write_tab(std::stringstream &sstream, int count);
};

#endif // WHITESPACE_UTILS_H
