/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#include "qt.h"
#include <sstream>
#include "whitespace_utils.h"
#include "parsers/parsers.h"
#include <map>
#include "debug.h"
#include "string_utils.h"


std::string to_jni_type(std::string java_type){
    if(java_type.empty()){
        return "";
    }
    auto test = [java_type](std::string jni_type)-> bool{
        return  string(string("[]").removeFrom(java_type)).hasMatch(jni_type);
    };



    auto jni_type  = [java_type](std::string hint)-> std::string{
        if(string("[]").hasMatch(java_type)){
            return hint + "Array";
        }else{
            return hint;
        }
    };

    if(test("boolean")){
        return jni_type("jboolean");
    }else if(test("char")){
        return jni_type("jchar");
    }else if(test("byte")){
        return jni_type("jbyte");
    }else if(test("short")){
        return jni_type("jshort");
    }else if(test("int")){
        return jni_type("jint");
    }else if(test("long")){
        return jni_type("jlong");
    }else if(test("float")){
        return jni_type("jfloat");
    }else if(test("double")){
        return jni_type("jdouble");
    }else if(test("void")){
        return jni_type("void");
    }else if(test("Object")){
        return jni_type("jobject");
    }else if(test("Class")){
        return jni_type("jclass");
    }else if(test("String")){
        return jni_type("jstring");
    }else if(test("array")){
        return jni_type("jarray");
    }else if(test("Throwable")){
        return jni_type("jthrowable");
    }else{
        return jni_type("jobject");
    }
}

std::string to_java_type(std::string jni_type){
    if(jni_type.empty()){
        return "";
    }
    auto test = [jni_type](std::string java_type)-> bool{
        return  string(string("Array").removeFrom(jni_type)).hasMatch(java_type);
    };



    auto java_type  = [jni_type](std::string hint)-> std::string{
        if(string("Array").hasMatch(jni_type)){
            return hint + "[]";
        }else{
            return hint;
        }
    };

    if(test("jboolean")){
        return java_type("boolean");
    }else if(test("jchar")){
        return java_type("char");
    }else if(test("jbyte")){
        return java_type("byte");
    }else if(test("jshort")){
        return java_type("short");
    }else if(test("jint")){
        return java_type("int");
    }else if(test("jlong")){
        return java_type("long");
    }else if(test("jfloat")){
        return java_type("float");
    }else if(test("jdouble")){
        return java_type("double");
    }else if(test("void")){
        return java_type("void");
    }else if(test("jobject")){
        return java_type("Object");
    }else if(test("jclass")){
        return java_type("Class");
    }else if(test("jstring")){
        return java_type("String");
    }else if(test("jarray")){
        return java_type("array");
    }else if(test("jthrowable")){
        return java_type("Throwable");
    }else{
        return java_type("");
    }
}


std::string do_mangling(std::string java_type,const std::map<std::string,std::string>& imports_map,std::string package_name){
    if(java_type.empty()){
        return "";
    }
    auto test = [java_type](std::string c_type)-> bool{
        return  string(string("[]").removeFrom(java_type)).hasMatch(c_type);
    };



    auto c_type  = [java_type](std::string hint)-> std::string{
        if(string("[]").hasMatch(java_type)){
            return "[" + hint;
        }else{
            return hint;
        }
    };

    if(test("boolean")){
        return c_type("Z");
    }else if(test("char")){
        return c_type("C");
    }else if(test("byte")){
        return c_type("B");
    }else if(test("short")){
        return c_type("S");
    }else if(test("int")){
        return c_type("I");
    }else if(test("long")){
        return c_type("J");
    }else if(test("float")){
        return c_type("F");
    }else if(test("double")){
        return c_type("D");
    }else if(test("void")){
        return c_type("V");
    }else if(test("String")){
        return c_type("Ljava/lang/String;");
    }else{
        if(chaR('.').hasMatch(java_type)){
            return "L" + c_type(chaR('.').replaceIn(java_type,"/")) + ";";
        }else{//use imported targets
            auto i = imports_map.find(java_type);
            if(i != imports_map.end()){
                return "L" + c_type(chaR('.').replaceIn(i->second,"/")) + ";";
            }else{//use  'home' package
                return "L" + c_type(chaR('.').replaceIn(package_name,"/")) + "/"+ java_type + ";";
            }
        }
    }

}

/** -----------------------------------------
 * @brief The Jni_native_method_parameter class
 */
Jni_native_method_parameter::Jni_native_method_parameter(){}

Jni_native_method_parameter::Jni_native_method_parameter(const Java_method_parameter &j_m_parameter)
{
    name_ = j_m_parameter.name();
    type_ = to_jni_type(j_m_parameter.type());
}

Jni_native_method_parameter::Jni_native_method_parameter(const Jni_native_method_parameter &other)
{
    name_ = other.name_;
    type_ = other.type_;
}

std::string Jni_native_method_parameter::name() const
{
    return  name_;
}

bool Jni_native_method_parameter::is_valid() const
{
    return (!type_.empty());
}

std::string Jni_native_method_parameter::to_string() const
{
    return (name_.empty()) ? type_ : type_ + " "+ name_;
}

std::string Jni_native_method_parameter::type() const
{
    return type_;
}



/** --------------------------------------
 * @brief The Jni_native_method class
 */
Jni_native_method::Jni_native_method()
    : use_jni_env_parameter_{true}
    , use_class_or_object_parameter_{true}
{}

Jni_native_method::Jni_native_method(const Java_native_method &j_n_method)
{
    name_ = j_n_method.name();
    return_type_ = to_jni_type(j_n_method.return_type());
    represent_static_java_native_ = j_n_method.is_static();
    use_jni_env_parameter_ = true;
    use_class_or_object_parameter_ = true;

    auto params = j_n_method.method_parameters();
    std::stringstream stream;
    stream << "(";
    for(std::vector<Java_method_parameter>::size_type i = 0; i < params.size(); i++){
        auto param  = params.at(i);
        stream << do_mangling(param.type(),j_n_method.imports_map(),j_n_method.package_name());
        method_parameters_.push_back(Jni_native_method_parameter(param));
    }
    stream << ")";
    stream << do_mangling(j_n_method.return_type(),j_n_method.imports_map(),j_n_method.package_name());
    signature_ = stream.str();
}

Jni_native_method::Jni_native_method(const Jni_native_method &other)
{
    name_ = other.name_;
    return_type_ = other.return_type_;
    represent_static_java_native_ = other.represent_static_java_native_;
    signature_ = other.signature_;
    function_code_ = other.function_code_;
    comment_ = other.comment_;
    use_jni_env_parameter_ = other.use_jni_env_parameter_;
    use_class_or_object_parameter_ = other.use_class_or_object_parameter_;
    jni_env_parameter_ = other.jni_env_parameter_;
    class_or_object_parameter_ = other.class_or_object_parameter_;

    method_parameters_.clear();

    for(std::vector<Java_method_parameter>::size_type i = 0; i < other.method_parameters_.size(); i++){
        auto param = other.method_parameters_.at(i);
        if(param.is_valid()){
            method_parameters_.push_back(Jni_native_method_parameter(param));
        }
    }
}

bool Jni_native_method::operator ==(const Jni_native_method &other){
    return ((name_ == other.name_) &&
            (signature_ == other.signature_) &&
            (function_code_ == other.function_code_)&&
            (represent_static_java_native_ == other.represent_static_java_native_));
}

bool Jni_native_method::operator !=(const Jni_native_method &other){
    return ((name_ != other.name_) ||
            (signature_ != other.signature_) ||
            (function_code_ != other.function_code_) ||
            (represent_static_java_native_ != other.represent_static_java_native_));
}

bool Jni_native_method::is_valid() const
{
    return ((!name_.empty())  && (!return_type_.empty()));
}

std::string Jni_native_method::name() const
{
    return name_;
}

std::string Jni_native_method::return_type() const
{
    return return_type_;
}

std::vector<Jni_native_method_parameter> Jni_native_method::method_parameters() const
{
    return method_parameters_;
}

std::string Jni_native_method::to_string(Format format) const
{
    if(!is_valid()){
        return "";
    }

    std::stringstream stream;
    if(comment_.empty()){
        stream << "/*";
        Whitespace_utils::write_newline(stream,1);
        stream << "*Method: " << name_;
        Whitespace_utils::write_newline(stream,1);
        stream << "*Signature: " << signature_;
        Whitespace_utils::write_newline(stream,1);
        stream << "*/";
    }else{
        stream << comment_;
    }

    Whitespace_utils::write_newline(stream,1);
    stream << return_type_;
    Whitespace_utils::write_space(stream,1);
    stream << name_;
    stream << "(";

    if(format == Format::one){
        std::stringstream d;d<<return_type_ << " " << name_ << "(";
        int param_distance = static_cast<int>(d.str().size());
        Whitespace_utils::write_newline(stream,1);
        Whitespace_utils::write_space(stream,param_distance);
        stream <<  ((use_jni_env_parameter_) ? (jni_env_parameter_.empty() ? "JNIEnv* env," : ("JNIEnv* "+jni_env_parameter_+",")) : "JNIEnv*,");
        Whitespace_utils::write_newline(stream,1);
        Whitespace_utils::write_space(stream,param_distance);
        if(represent_static_java_native_){
            stream << ((use_class_or_object_parameter_) ? ((class_or_object_parameter_.empty()) ? "jclass cl" : ("jclass " + class_or_object_parameter_)) : "jclass");
        }else{
            stream << ((use_class_or_object_parameter_) ? ((class_or_object_parameter_.empty()) ? "jobject obj" : ("jobject " + class_or_object_parameter_)) : "jobject");
        }
        if(!method_parameters_.empty()){
            stream <<",";
        }
        for(std::vector<Jni_native_method_parameter>::size_type i = 0; i < method_parameters_.size(); i++){
            Whitespace_utils::write_newline(stream,1);
            Whitespace_utils::write_space(stream,param_distance);
            stream << method_parameters_.at(i).to_string();
            if(i != (method_parameters_.size() - 1)){
                stream << ",";
            }
        }
        Whitespace_utils::write_newline(stream,1);
        Whitespace_utils::write_space(stream,param_distance - 1);
        stream << ")";
    }else{
        stream <<  ((use_jni_env_parameter_) ? (jni_env_parameter_.empty() ? "JNIEnv* env," : ("JNIEnv* " + jni_env_parameter_ + ",")) : "JNIEnv*,");
        Whitespace_utils::write_space(stream,1);
        if(represent_static_java_native_){
            stream << ((use_class_or_object_parameter_) ? ((class_or_object_parameter_.empty()) ? "jclass cl" : ("jclass " + class_or_object_parameter_)) : "jclass");
        }else{
            stream << ((use_class_or_object_parameter_) ? ((class_or_object_parameter_.empty()) ? "jobject obj" : ("jobject " + class_or_object_parameter_)) : "jobject");
        }
        if(!method_parameters_.empty()){
            stream <<",";
        }
        for(std::vector<Jni_native_method_parameter>::size_type i = 0; i < method_parameters_.size(); i++){
            Whitespace_utils::write_space(stream,1);
            stream << method_parameters_.at(i).to_string();
            if(i != (method_parameters_.size() - 1)){
                stream << ",";
            }
        }
        stream << ")";
    }

    Whitespace_utils::write_newline(stream,1);
    stream << "{";
    if(function_code_.empty()){
        Whitespace_utils::write_newline(stream,2);
    }else{
        stream << function_code_;
    }
    stream << "}";
    Whitespace_utils::write_newline(stream,2);


    return stream.str();
}

std::string Jni_native_method::signature() const
{
    return signature_;
}

bool Jni_native_method::are_same(const Jni_native_method &m1,const Jni_native_method &m2)
{
    return ((m1.name_ == m2.name_) &&
            (m1.signature_ == m2.signature_));
}

/** -----------------------
 * @brief The Qt_method class
 */
Qt_method::Qt_method(std::string class_name,std::string definition,std::string comment)
    :class_name_{class_name}
    ,definition_{definition}
    ,comment_{comment}
{}

std::string Qt_method::definition() const
{
    return definition_;
}

bool Qt_method::is_constructor() const
{
    return is_method_test_(class_name_);
}

bool Qt_method::is_init_method() const
{
    return is_method_test_("init");
}

bool Qt_method::is_instance_method() const
{
    return is_method_test_("instance");
}

std::string Qt_method::comment() const
{
    return comment_;
}

bool Qt_method::is_valid() const
{
    return ((!definition_.empty()) && (!class_name_.empty()));
}

bool Qt_method::has_comment(){
    return (!comment_.empty());
}

std::string Qt_method::method_name(){
    if(!is_valid()){
        return "";
    }
    auto match =  string(class_name_).seq(spaceOptional()).seq(string("::"))
            .seq(any(chaR('(')).plus()).firstStringMatch(definition_);
    return String_utils::trim(string(class_name_).seq(spaceOptional()).seq(string("::")).removeFrom(match));
}

bool Qt_method::is_method_test_(std::string method_name) const
{
    if(is_valid()){
        return   string(class_name_).seq(string("::")).seq(string(method_name))
                .seq(spaceOptional()).seq(chaR('(')).hasMatch(definition_);
    }else{
        return false;
    }
}

/** ------------------------------
 * @brief The Qt_class class
 */
Qt_class::Qt_class(const std::string &class_name, const std::string &file_prefix, const std::string &destination_dir, const Java_native_class &j_n_class){
    class_name_ = (!class_name.empty())? class_name : j_n_class.class_name();
    file_prefix_ = file_prefix;
    destination_dir_ = destination_dir;
    java_class_full_path_ = chaR('.').replaceIn(j_n_class.full_class_name(),"/");

    //
    auto j_n_methods = j_n_class.methods();
    if(j_n_methods != nullptr){
        for(std::vector<Java_native_method>::size_type i = 0;i< j_n_methods->size(); i++){
            auto j_n_method = j_n_methods->at(i);
            jni_native_methods_.push_back(Jni_native_method(j_n_method));
        }
    }


}

Qt_class::Qt_class(const Qt_class &other){
    class_name_ = other.class_name_;

    jni_native_methods_.clear();
    for(std::vector<Jni_native_method>::size_type i  = 0; i < other.jni_native_methods_.size() ; i++){
        jni_native_methods_.push_back(other.jni_native_methods_.at(i));
    }

    qt_methods_.clear();
    for(std::vector<Qt_method>::size_type i  = 0; i < other.qt_methods_.size() ; i++){
        qt_methods_.push_back(other.qt_methods_.at(i));
    }

    java_class_full_path_ = other.java_class_full_path_;
    file_prefix_ = other.file_prefix_;
    destination_dir_ = other.destination_dir_;

    platiform_independent_includes_.clear();
    for(std::vector<std::string>::size_type i  = 0; i < other.platiform_independent_includes_.size() ; i++){
        platiform_independent_includes_.push_back(other.platiform_independent_includes_.at(i));
    }

    android_specific_includes_.clear();
    for(std::vector<std::string>::size_type i  = 0; i < other.android_specific_includes_.size() ; i++){
        android_specific_includes_.push_back(other.android_specific_includes_.at(i));
    }

    android_specific_static_variables_.clear();
    for(std::vector<std::string>::size_type i  = 0; i < other.android_specific_static_variables_.size() ; i++){
        android_specific_static_variables_.push_back(other.android_specific_static_variables_.at(i));
    }

    platiform_independent_static_variables_.clear();
    for(std::vector<std::string>::size_type i  = 0; i < other.platiform_independent_static_variables_.size() ; i++){
        platiform_independent_static_variables_.push_back(other.platiform_independent_static_variables_.at(i));
    }
}

std::string Qt_class::header_file_path() const
{
    return destination_dir_ + "/"+ file_prefix_ + ".h" ;
}

std::string Qt_class::source_file_path() const
{
    return destination_dir_ + "/"+ file_prefix_ + ".cpp" ;
}

std::string Qt_class::class_name() const
{
    return class_name_;
}

std::vector<Jni_native_method> Qt_class::c_native_methods() const
{
    return jni_native_methods_;
}

bool Qt_class::is_valid(){
    return  ((!class_name_.empty()) &&
             (!jni_native_methods_.empty()) &&
             (!java_class_full_path_.empty()) &&
             (!file_prefix_.empty()) &&
             (!destination_dir_.empty()));
}

std::string Qt_class::file_prefix() const
{
    return file_prefix_;
}

std::vector<Qt_method> Qt_class::qt_methods() const
{
    return qt_methods_;
}
