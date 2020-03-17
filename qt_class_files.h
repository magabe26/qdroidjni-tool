/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#ifndef QT_CLASS_FILES_H
#define QT_CLASS_FILES_H

#include <string>
#include "parsers/parsers.h"
#include "function_parser.h"
#include "qt.h"

class Qt_class_files
{
public:
    Qt_class_files(Qt_class& qt_class);
    bool write_source_file();
    std::string read_source_file();
    bool write_header_file();
    std::string read_header_file();
    bool header_file_exist();
    bool source_file_exist();
    Parser jni_type_parser();
    Function_parser native_function_parser();
    std::vector<C_native_method> get_native_methods_from_content(std::string source_file_content);

    Qt_method get_init_method() const;
    Qt_method get_constructor_method() const;
    Qt_method get_instance_method() const;
    std::vector<Qt_method> get_other_qt_methods() const;
    std::string android_specific_search_context(std::string source_file_content);
    std::string platiform_independent_search_context(std::string source_file_content);
    Parser static_variable_parser();
private:
    Qt_class& qt_class_;

    Parser jni_env_pointer_parser();

    std::string method_table_string(std::string table_name = "method_table") const;
    std::string source_file_content() const;
    std::string header_file_content() const;
    void write_native_methods_to_source_file(std::stringstream& stream) const;
    void write_init_method_source_file(std::stringstream &stream,std::string table_name = "method_table") const;
    void write_includes(std::stringstream& stream) const;
    void write_constructor_method(std::stringstream& stream,std::string code = "") const;
    void write_instance_method(std::stringstream &stream) const;
    std::map<std::string, std::string> get_signature_map_from_content(std::string source_file_content);
    void copy_qt_methods_from_source(std::string source_file_content);
    void write_platiform_independent_includes(std::stringstream &stream) const;
    void write_android_specific_includes(std::stringstream &stream) const;
    void copy_includes(std::string source_file_content);
    std::string get_comment(const std::string &function_with_comment, Parser &fn_boundary);
    std::vector<std::string> platiform_independent_static_variables(std::string source_file_content);
    std::vector<std::string> android_specific_static_variables(std::string source_file_content);
    void copy_static_variables(std::string source_file_content);
    void write_platiform_independent_static_variables_to_source_file(std::stringstream &stream) const;
    void write_android_specific_static_variables_to_source_file(std::stringstream &stream) const;
};

#endif // QT_CLASS_FILES_H
