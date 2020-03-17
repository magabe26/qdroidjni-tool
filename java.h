/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#ifndef JAVA_TYPES_H
#define JAVA_TYPES_H

#include <string>
#include <vector>
#include <map>

/** ------------------------------------------
 * @brief The Method_parameter class
 */
class Java_method_parameter{

public:
    explicit Java_method_parameter(std::string name,
                                   std::string type);
    std::string name() const;
    std::string type() const;
    bool is_valid() const;
    std::string to_string() const;

private:
    std::string name_;
    std::string type_;
};


/** ------------------------------------------
 * @brief The Native_method class
 */
class Java_native_method{

public:
    Java_native_method();
    Java_native_method(const Java_native_method& other);

    bool is_private() const;
    bool is_public() const;
    bool is_valid() const;

    std::string name() const;
    std::string return_type() const;
    bool is_static() const;
    std::vector<Java_method_parameter> method_parameters() const;
    void add_method_parameter(const Java_method_parameter& parameter);
    std::string to_string() const;
    std::map<std::string, std::string> imports_map() const;
    std::string package_name() const;

private:
    friend class Java_class_files;

    bool is_public_;
    bool is_private_;
    bool is_static_;
    std::string name_;
    std::string return_type_;
    std::vector<Java_method_parameter> method_parameters_;
    std::map<std::string,std::string> imports_map_;
    std::string package_name_;
};


/** ---------------------------------------------
 * @brief The Java_native_class class
 */
class Java_native_class{
public:
    Java_native_class();
    Java_native_class(const Java_native_class&  other);

    ~Java_native_class();

    std::vector<Java_native_method> *methods() const;
    std::string full_class_name() const;
    std::string class_name() const;
    std::string package_name() const;
    std::vector<std::string> imports() const;
    std::string java_file_path() const;
    std::string methods_as_string();
    std::string to_string() const;
    bool is_valid();

private:
    friend class Java_class_files;

    std::string package_name_;
    std::string class_name_;
    std::vector<Java_native_method> *methods_;
    std::vector<std::string> imports_;
    std::string java_file_path_;
};


#endif // JAVA_TYPES_H
