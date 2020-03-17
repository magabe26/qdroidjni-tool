/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#include "file.h"
#include <fstream>
#include <iostream>
#include <stdio.h>

inline void clear(char *pc,int len){
    for(int i = 0;i < len ; i++){
        *pc = '\0';
        pc++;
    }
}

std::string File::read(std::string path){
    std::ifstream in(path);
    std::string content;
    if(in.is_open()){
        while (!in.eof()) {
            char c[2];
            clear(c,2);
            in.read(c,1);
            content.append(c);
        }
    }
    return content;
}

bool File::exists(std::string path)
{
    std::ifstream in(path);
    const bool b = in.is_open();
    return b;
}

bool File::write(std::string content, std::string path)
{
    std::ofstream file(path);
    if(!file.is_open()){
        return false;
    }

    file << content;
    file.flush();
    return (!file.fail());
}

