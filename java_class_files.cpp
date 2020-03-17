/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#include "java_class_files.h"
#include "parsers/parsers.h"
#include "file.h"
#include <iostream>
#include "string_utils.h"


Java_class_files::Java_class_files(){}

Java_native_class Java_class_files::parse(const std::string &java_file_path){
    Java_native_class jnclass;
    std::string java_class_content = File::read(java_file_path);
    if(java_class_content.empty()){
        print_error("Failed to read '" + java_file_path +"' or the file may be empty.");
        return jnclass;
    }

    jnclass.java_file_path_ = java_file_path;
    auto tmp_package_name = package_parser().firstStringMatch(java_class_content);

    if(!tmp_package_name.empty()){
        jnclass.package_name_ = String_utils::trim(string("package").oR(chaR(';')).removeFrom(tmp_package_name));

        jnclass.class_name_ = String_utils::rtrim(string("class").seq(space()).removeFrom(string("class").seq(space()).seq(name_parser()).firstStringMatch(java_class_content)));

        auto tmp_imports = import_parser().allStringMatches(java_class_content);
        for(std::vector<std::string>::size_type i = 0; i < tmp_imports.size(); i++){
            jnclass.imports_.push_back(String_utils::trim(string("import").oR(chaR(';')).removeFrom(tmp_imports.at(i))));
        }

        std::map<std::string,std::string> imports_map;
        for(std::vector<std::string>::size_type i = 0; i <  jnclass.imports_.size(); i++){
            auto import = jnclass.imports_.at(i);
            auto lastMatch = chaR('.').lastMatch(import);
            if(lastMatch.isSuccess()){
                auto import_class = String_utils::trim(import.substr(lastMatch.success().end()));
                if(!import_class.empty()){
                    imports_map[import_class]  = import;
                }
            }
        }

        //remove commented methods
        java_class_content = string("//").seq(spaceOptional()).seq(native_method_parser()).removeFrom(java_class_content);

        std::vector<std::string> tmp_native_methods = native_method_parser().allStringMatches(java_class_content);
        for(std::vector<std::string>::size_type i = 0; i < tmp_native_methods.size(); i++){
            Java_native_method method;
            const std::string method_declaration = tmp_native_methods.at(i);

            method.is_public_ = is_public_native_method(method_declaration);
            method.is_private_ = is_private_native_method(method_declaration);
            method.is_static_ = is_static_native_method(method_declaration);
            method.return_type_ = space().removeFrom((public_parser().oR(private_parser())
                                                      .oR(native_parser()).oR(static_parser())).seq(space())
                                                     .oR(name_parser().seq(method_parameters_enclosed_parser()).seq(chaR(';'))).removeFrom(method_declaration));



            auto tmp_name = (public_parser().oR(private_parser())
                             .oR(native_parser()).oR(static_parser())).seq(space())
                    .oR(method_parameters_enclosed_parser().seq(chaR(';'))).removeFrom(method_declaration);

            auto tmp_list = data_type_parser().oR(name_parser()).allStringMatches(tmp_name);
            if(tmp_list.size() == 2){
                method.name_ = String_utils::trim(tmp_list[1]);
            }

            auto tmp_params = chaR('(').oR(chaR(')')).removeFrom(method_parameters_enclosed_parser().firstStringMatch(method_declaration));
            auto paramParser = spaceOptional().seq((data_type_parser().seq(space()).seq(name_parser())).seq(spaceOptional()).seq(chaR(',').optional()));

            tmp_params = space().replaceIn(tmp_params," ");
            auto tmp_params_list = paramParser.allStringMatches(tmp_params);
            for(std::vector<std::string>::size_type i = 0; i < tmp_params_list.size(); i++){
                auto tmp_list = data_type_parser().oR(name_parser()).allStringMatches(tmp_params_list.at(i));
                if(tmp_list.size() == 2){
                    Java_method_parameter param(String_utils::trim(tmp_list[1]),String_utils::trim(tmp_list[0]));
                    if(param.is_valid()){
                        method.add_method_parameter(param);
                    }
                }
            }

            //add import maps
            std::map<std::string,std::string>::iterator iterator= imports_map.begin();
            while (iterator != imports_map.end()) {
                method.imports_map_[iterator->first.c_str()] = iterator->second;
                iterator++;
            }

            method.package_name_ = jnclass.package_name_;

            jnclass.methods_->push_back(method);
        }

    }else{
        print_error("No package name found,the java file must contain a package name");
    }
    return jnclass;
}


bool Java_class_files::is_native_method(std::string method_declaration)
{
    return public_native_method_parser().oR(private_native_method_parser()).hasMatch(method_declaration);
}

bool Java_class_files::is_public_native_method(std::string method_declaration)
{
    return (!is_private_native_method(method_declaration));
}

bool Java_class_files::is_private_native_method(std::string method_declaration)
{
    return private_native_method_parser().hasMatch(method_declaration);
}

bool Java_class_files::is_static_native_method(std::string method_declaration)
{
    return (is_native_method(method_declaration) && static_parser().hasMatch(method_declaration));
}

void Java_class_files::print_error(std::string error)
{
    std::cerr << "Java_native_class_parser::ERROR -> " << error <<  std::endl;
}

Parser Java_class_files::public_parser(){
    return string("public");
}

Parser Java_class_files::private_parser()
{
    return string("private");
}

Parser Java_class_files::native_parser()
{
    return string("native");
}

Parser Java_class_files::static_parser()
{
    return string("static");
}


Parser Java_class_files::data_type_parser()
{
    return letter().oR(chaR('_')).seq((word() | chaR('.') | chaR('_') | chaR('-')).star()).seq(spaceOptional().seq(string("[]")).optional());
}

Parser Java_class_files::name_parser()
{
    return letter().oR(chaR('_')).seq((word() | chaR('_') | chaR('-')).star());
}

Parser Java_class_files::package_parser()
{
    return string("package")
            .seq(space())
            .seq(letter().oR(chaR('_')).seq((word() | chaR('.') | chaR('_') | chaR('-')).star()))
            .seq(spaceOptional())
            .seq(chaR(';'));
}

Parser Java_class_files::import_parser()
{
    return string("import")
            .seq(space())
            .seq(data_type_parser())
            .seq(spaceOptional())
            .seq(chaR(';'));
}

Parser Java_class_files::public_native_method_parser()
{
    auto public_with_no_identifier = (static_parser().seq(space()).optional())
            .seq(native_parser())
            .seq(space())
            .seq(data_type_parser())
            .seq(space())
            .seq(name_parser())
            .seq(method_parameters_enclosed_parser())
            .seq(spaceOptional())
            .seq(chaR(';'));

    auto public_identifier = public_parser();
    return (method_parser(public_identifier) | public_with_no_identifier);
}

Parser Java_class_files::private_native_method_parser()
{
    auto private_identifier = private_parser();
    return method_parser(private_identifier);
}

Parser Java_class_files::native_method_parser()
{
    return public_native_method_parser() | private_native_method_parser();
}

Parser Java_class_files::method_parameters_parser()
{
    auto param = spaceOptional().seq((data_type_parser().seq(space()).seq(name_parser())).seq(spaceOptional()).seq(chaR(',').optional())).seq(spaceOptional());
    return  (param.plus() | spaceOptional());
}

Parser Java_class_files::method_parameters_enclosed_parser()
{
    return chaR('(').seq(method_parameters_parser()).seq(chaR(')'));
}


/**
 * @brief method_
 * @param modifier could be public or private
 * @return
 */

Parser Java_class_files::method_parser(Parser& modifier)
{
    return  (modifier.seq(space()).seq(static_parser()) | static_parser().seq(space()).seq(modifier) | modifier)
            .seq(space())
            .seq(native_parser())
            .seq(space())
            .seq(data_type_parser())
            .seq(space())
            .seq(name_parser())
            .seq(method_parameters_enclosed_parser())
            .seq(spaceOptional())
            .seq(chaR(';'));
}




