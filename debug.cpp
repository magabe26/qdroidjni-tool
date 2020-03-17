/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#include "debug.h"
#include <iostream>

void unused(std::string){}
void unused(std::vector<std::string>){}
void unused(unsigned long long){}

void Debug::print(const std::string &text)
{

#ifdef PRINT_DEBUG_MESSAGE
    std::cout << text << std::endl;
#else
    unused(text);
#endif
}

void Debug::print(const std::vector<std::string> &list)
{
#ifdef PRINT_DEBUG_MESSAGE
    for(std::vector<std::string>::size_type i =0;i < list.size();i++){
        print(list.at(i));
    }
#else
    unused(list);
#endif
}

void Debug::print(unsigned long long u)
{
#ifdef PRINT_DEBUG_MESSAGE
    std::cout << u << std::endl;
#else
    unused(u);
#endif
}
