/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#ifndef FILE_READER_H
#define FILE_READER_H

#include <string>

struct File{
    static std::string read(std::string path);
    static bool exists(std::string path);
    static bool write(std::string content,std::string path);
};

#endif // FILE_READER_H
