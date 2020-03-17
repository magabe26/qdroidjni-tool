/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#include "java.h"
#include <sstream>
#include "whitespace_utils.h"

/** ------------------------------------------
 * @brief The Method_parameter class
 */
Java_method_parameter::Java_method_parameter(std::string name, std::string type)
{
    name_ = name;
    type_ = type;
}

std::string Java_method_parameter::name() const
{
    return name_;
}

std::string Java_method_parameter::type() const
{
    return type_;
}

bool Java_method_parameter::is_valid() const{
    return  (!type_.empty());
}

std::string Java_method_parameter::to_string() const{
    return type_ + " "+ name_;
}



/** ------------------------------------------
 * @brief The Native_method class
 */
Java_native_method::Java_native_method(){}

Java_native_method::Java_native_method(const Java_native_method &other){
    is_public_ = other.is_public_;
    is_private_ = other.is_private_;
    is_static_ = other.is_static_;
    name_ = other.name_;
    return_type_ = other.return_type_;
    package_name_ = other.package_name_;

    method_parameters_.clear();

    for(std::vector<Java_native_method>::size_type i = 0; i < other.method_parameters_.size(); i++){
        auto param = other.method_parameters_.at(i);
        if(param.is_valid()){
            method_parameters_.push_back(param);
        }
    }

    imports_map_.clear();
    auto iterator = other.imports_map_.begin();
    while (iterator != other.imports_map_.end()) {
        imports_map_[iterator->first] = iterator->second;
        iterator++;
    }
}

bool Java_native_method::is_private() const
{
    return is_private_;
}

bool Java_native_method::is_public() const
{
    return is_public_;
}

bool Java_native_method::is_valid() const
{
    return ((!name_.empty()) &&
            (!return_type_.empty()) &&
            (is_public_ || is_private_) );
}

std::string Java_native_method::name() const
{
    return name_;
}

std::string Java_native_method::return_type() const
{
    return return_type_;
}

bool Java_native_method::is_static() const
{
    return is_static_;
}

std::vector<Java_method_parameter> Java_native_method::method_parameters() const
{
    return method_parameters_;
}

void Java_native_method::add_method_parameter(const Java_method_parameter &parameter)
{
    if(parameter.is_valid()){
        method_parameters_.push_back(parameter);
    }
}

std::string Java_native_method::to_string() const{
    std::stringstream sstream;

    sstream << ((is_static_) ? "static " : "");
    sstream << ((is_public_) ? "public " : "private ");
    sstream << "native ";
    sstream << return_type_ + " ";
    sstream << name_;
    sstream << "(";
    for(std::vector<Java_method_parameter>::size_type i = 0; i < method_parameters_.size(); i++){
        sstream << method_parameters_.at(i).to_string();
        if(i != (method_parameters_.size() - 1)){
            sstream << ",";
        }
    }
    sstream << ");";

    return sstream.str();
}

std::map<std::string, std::string> Java_native_method::imports_map() const
{
    return imports_map_;
}

std::string Java_native_method::package_name() const
{
    return package_name_;
}




/** ---------------------------------------------
 * @brief The Java_native_class class
 */
Java_native_class::Java_native_class():
    methods_{new std::vector<Java_native_method>}{}

Java_native_class::Java_native_class(const Java_native_class &other)
{
    package_name_ = other.package_name_;
    class_name_ = other.class_name_;

    if(methods_ == nullptr){
        methods_ = new std::vector<Java_native_method>();
    }

    methods_->clear();

    for(std::vector<Java_native_method>::size_type i = 0; i < other.methods_->size(); i++){
        methods_->push_back(other.methods_->at(i));
    }


    imports_.clear();

    for(std::vector<Java_native_method>::size_type i = 0; i < other.imports_.size(); i++){
        imports_.push_back(other.imports_.at(i));
    }

}

Java_native_class::~Java_native_class()
{
    if(methods_ != nullptr){
        methods_->clear();
        delete  methods_;
        methods_ = nullptr;
    }
}

std::vector<Java_native_method> *Java_native_class::methods() const
{
    return methods_;
}

std::string Java_native_class::full_class_name() const
{
    return package_name_+ "." +class_name_;
}

std::string Java_native_class::class_name() const
{
    return class_name_;
}

std::string Java_native_class::package_name() const
{
    return package_name_;
}

std::vector<std::string> Java_native_class::imports() const
{
    return imports_;
}

std::string Java_native_class::java_file_path() const
{
    return java_file_path_;
}

std::string Java_native_class::methods_as_string(){
    std::stringstream sstream;
    if(methods_ != nullptr){
        for(std::vector<Java_native_method>::size_type i = 0; i < methods_->size(); i++){
            sstream << methods_->at(i).to_string() << std::endl;
        }
    }
    return  sstream.str();
}

std::string Java_native_class::to_string() const{
    std::stringstream sstream;
    sstream << "package " << package_name_ << ";";
    Whitespace_utils::write_newline(sstream,2);
    for(std::vector<std::string>::size_type i = 0; i< imports_.size(); i++){
        sstream <<"import "<< imports_.at(i) << ";";
        Whitespace_utils::write_newline(sstream,1);
    }
    Whitespace_utils::write_newline(sstream,2);

    sstream << "class " << class_name_ <<"{";
    Whitespace_utils::write_newline(sstream,2);

    if(methods_ != nullptr){
        for(std::vector<Java_native_method>::size_type i = 0; i < methods_->size(); i++){
            Whitespace_utils::write_space(sstream,2);
            sstream << methods_->at(i).to_string();
            Whitespace_utils::write_newline(sstream,1);
        }
    }
    sstream <<"}";

    return sstream.str();
}

bool Java_native_class::is_valid(){
    return (!java_file_path_.empty());
}
