/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#include "whitespace_utils.h"

void Whitespace_utils::write_space(std::stringstream &sstream, int count){
    for(int i = 0; i < count; i++){
        sstream << " ";
    }
}

void Whitespace_utils::write_newline(std::stringstream &sstream, int count){
    for(int i = 0; i < count; i++){
        sstream << "\n";
    }
}

void Whitespace_utils::write_tab(std::stringstream &sstream, int count){
    for(int i = 0; i < count; i++){
        sstream << "\t";
    }
}
