/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#ifndef JAVA_NATIVE_CLASS_PARSER_H
#define JAVA_NATIVE_CLASS_PARSER_H

#include "core/core.h"
#include "java.h"

/**  ------------------------------------------------
 * @brief The Java_class_files class
 */
class Java_class_files
{
public:
    Java_class_files();
    static Java_native_class parse(const std::string& java_file_path);

private:

    static bool is_native_method(std::string method_declaration);
    static bool is_public_native_method(std::string method_declaration);
    static bool is_private_native_method(std::string method_declaration);
    static bool is_static_native_method(std::string method_declaration);
    static void print_error(std::string error);
    static Parser public_parser();
    static Parser private_parser();
    static Parser native_parser();
    static Parser data_type_parser();
    static Parser name_parser();
    static Parser package_parser();
    static Parser import_parser();
    static Parser public_native_method_parser();
    static Parser private_native_method_parser();
    static Parser native_method_parser();
    static Parser method_parameters_parser();
    static Parser method_parameters_enclosed_parser();
    static Parser static_parser();
    static Parser method_parser(Parser &modifier);
};

#endif // JAVA_NATIVE_CLASS_PARSER_H
