/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#include "comment_parser.h"

Parser comment(){
    auto single_line = (string("//").seq(any(string("//") | chaR('\n')).star())).seq(spaceOptional()).plus();
    auto muilt_line = string("/*").seq(any(string("*/")).plus()).seq(string("*/"));
    return (single_line | muilt_line);
}
