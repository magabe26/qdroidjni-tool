/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include <vector>

class Debug
{
public:
    static void print(const std::string& text);
    static void print(const std::vector<std::string>& list);
    static void print(unsigned long long u);
};

#endif // DEBUG_H
