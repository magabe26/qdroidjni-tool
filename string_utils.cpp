/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#include "string_utils.h"
#include "parsers/parsers.h"

std::string String_utils::ltrim(std::string str){
    std::string tmp = str;
    auto lresult = space().firstMatch(str);
    if(lresult.isSuccess()){
        auto start = lresult.success().start();
        auto end = lresult.success().end();
        if(start == 0){
            tmp = tmp.substr(end,tmp.size() - end);
        }
    }
    return  tmp;
}

std::string String_utils::rtrim(std::string str){
    std::string tmp = str;
    auto rresult = space().lastMatch(str);
    if(rresult.isSuccess()){
        auto start = rresult.success().start();
        auto end = rresult.success().end();
        if(end == str.length()){
            tmp = tmp.substr(0,tmp.size() - (end-start));
        }
    }
    return  tmp;
}

std::string String_utils::trim(std::string str)
{
    return rtrim(ltrim(str));
}

pos String_utils::str_index_in(std::string input, std::string str, pos start){
    if(input.empty() || str.empty()){
        return npos;
    }
    pos count = 0;
    pos index = npos;
    for(pos inputIndex = 0;start < input.length() ; start++){
        if(count == str.length()){
            return index;
        }
        if(input[start] == str[inputIndex]){
            if(count == 0){
                index = start;
            }
            count++;
            inputIndex++;
        }else{
            count = 0;
            inputIndex = 0;
            index = npos;
        }
    }
    return index;
}

std::string String_utils::to_upper(std::string str)
{
    for(std::string::size_type i = 0; i < str.size() ; i ++){
        str[i] = static_cast<char>(toupper(str[i])) ;
    }
    return str;
}

std::string String_utils::to_lower(std::string str)
{
    for(std::string::size_type i = 0; i < str.size() ; i ++){
        str[i] = static_cast<char>(tolower(str[i])) ;
    }
    return str;
}
