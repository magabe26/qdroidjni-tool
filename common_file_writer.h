/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#ifndef COMMON_FILE_WRITER_H
#define COMMON_FILE_WRITER_H

#include <string>
#include <map>

static const char * native_common = "native_common";
static const char * native_register = "native_register";

class Common_file_writer
{
public:
    Common_file_writer();
    static bool write_native_common_h(std::string dest_dir);
    static bool write_native_register_h(std::string dest_dir);
    static bool write_native_register_cpp(std::string dest_dir, const std::map<std::string, std::string> &classes);

};

#endif // COMMON_FILE_WRITER_H
