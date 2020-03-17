/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#ifndef JAVA_NATIVE_TO_QT_CONVETER_H
#define JAVA_NATIVE_TO_QT_CONVETER_H

#include "java_class_files.h"
#include "parsers/parsers.h"

std::string to_jni_type(std::string java_type);
std::string to_java_type(std::string jni_type);
std::string do_mangling(std::string java_type, const std::map<std::string,std::string>& imports_map, std::string package_name);

/** -----------------------------------------
 * @brief The C_native_method_parameter class
 */
class C_native_method_parameter{

public:
    C_native_method_parameter();
    explicit C_native_method_parameter(const Java_method_parameter& j_m_parameter);
    C_native_method_parameter(const C_native_method_parameter& other);

    std::string name() const;
    bool is_valid() const;
    std::string to_string() const;
    std::string type() const;

private:
    friend class Qt_class_files;

    std::string name_;
    std::string type_;
};


/** --------------------------------------
 * @brief The C_native_method class
 */
class C_native_method{

public:
    enum  Format{
        one,
        two
    };

    C_native_method();
    C_native_method(const Java_native_method& j_n_method);
    C_native_method(const C_native_method& other);
    bool operator ==(const C_native_method& other);
    bool operator !=(const C_native_method& other);
    bool is_valid() const;
    std::string name() const;
    std::string return_type() const;
    std::vector<C_native_method_parameter> method_parameters() const;
    std::string to_string(Format format = Format::two) const;
    std::string signature() const;
    static bool are_same(const C_native_method &m1, const C_native_method &m2);

private:
    friend class Qt_class_files;

    std::string name_;
    std::string return_type_;
    bool represent_static_java_native_;
    std::vector<C_native_method_parameter> method_parameters_;
    std::string signature_;
    std::string function_code_;
    std::string comment_;
    bool use_jni_env_parameter_;
    bool use_class_or_object_parameter_;
    std::string jni_env_parameter_;
    std::string class_or_object_parameter_;
};


/** -----------------------
 * @brief The Qt_method class
 */
class Qt_method{

public:
    Qt_method(std::string class_name,std::string definition,std::string comment);
    std::string definition() const;
    bool is_constructor() const;
    bool is_init_method() const;
    bool is_instance_method() const;
    std::string comment() const;
    bool is_valid() const;
    bool has_comment();
    std::string method_name();

private:
    friend class Qt_class_files;
    std::string class_name_;
    std::string definition_;
    std::string comment_;

    bool is_method_test_(std::string method_name) const;
};

/** ------------------------------
 * @brief The Qt_class class
 */
class Qt_class{

public:
    Qt_class(const std::string& class_name,
             const std::string& file_prefix,
             const std::string& destination_dir,
             const Java_native_class& j_n_class);

    Qt_class(const Qt_class& other );
    std::string header_file_path() const;
    std::string source_file_path() const;
    std::string class_name() const;
    std::vector<C_native_method> c_native_methods() const;
    bool is_valid();
    std::string file_prefix() const;
    std::vector<Qt_method> qt_methods() const;

private:
    friend class Qt_class_files;

    std::string class_name_;
    std::vector<C_native_method> c_native_methods_;
    std::vector<Qt_method> qt_methods_;
    std::string java_class_full_path_;
    std::string file_prefix_;
    std::string destination_dir_;
    std::vector<std::string> platiform_independent_includes_;
    std::vector<std::string> android_specific_includes_;
    std::vector<std::string> platiform_independent_static_variables_;
    std::vector<std::string> android_specific_static_variables_;

};

#endif // JAVA_NATIVE_TO_QT_CONVETER_H
