/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#include "configuration_file.h"
#include "file.h"
#include "error.h"
#include "string_utils.h"
#include "common_file_writer.h"
#include "java_class_files.h"
#include "qt.h"
#include "qt_class_files.h"
#include "comment_parser.h"

void print_message(std::string message){
    std::cout << message << std::endl;
}

/**  -----------------------------------------
 * @brief The Config_instruction class
 */

bool Config_instruction_group::is_valid(){
    return ((!dir_.empty()) && (!instructions_.empty()));
}


Config_instruction::Config_instruction(){}

Config_instruction::Config_instruction(const Config_instruction &other){
    qt_class_name_ = other.qt_class_name_;
    qt_file_prefix_ = other.qt_file_prefix_;
    java_file_path_ = other.java_file_path_;
}

bool Config_instruction::operator ==(const Config_instruction &other)
{
    return ((qt_class_name_ == other.qt_class_name_) &&
            (qt_file_prefix_ == other.qt_file_prefix_) &&
            (java_file_path_ == other.java_file_path_));
}

bool Config_instruction::operator !=(const Config_instruction &other)
{
    return ((qt_class_name_ != other.qt_class_name_) ||
            (qt_file_prefix_ != other.qt_file_prefix_) ||
            (java_file_path_ != other.java_file_path_));
}

std::string Config_instruction::qt_class_name() const
{
    return qt_class_name_;
}

std::string Config_instruction::qt_file_prefix() const
{
    return qt_file_prefix_;
}

std::string Config_instruction::java_file_path() const
{
    return java_file_path_;
}

bool Config_instruction::is_valid(){
    return ((!qt_class_name_.empty()) &&
            (!qt_file_prefix_.empty()) &&
            (!java_file_path_.empty()));
}

bool Config_instruction::execute(std::string qt_dir)
{

    auto j_n_class = Java_class_files::parse(java_file_path_);
    if(j_n_class.is_valid() && is_valid()){
        Qt_class qt_class(qt_class_name_,qt_file_prefix_,qt_dir,j_n_class);

        if(qt_class.is_valid()){
            Qt_class_files qt_files(qt_class);
            if(qt_files.write_header_file() &&
                    qt_files.write_source_file()) {
                print_message("\"" + qt_class.source_file_path() + "\" written successfully");
                print_message("\"" + qt_class.header_file_path() + "\" written successfully");
                return true;
            } else{
                return  false;
            }


        }else{
            if(qt_class.c_native_methods().empty()){
                print_error(java_file_path_ + "contains no native methods, it's corresponding qt files will not be written");
            }
        }
    }
    return false;
}

/** -------------------------------------
 * @brief The Config_instruction_group class
 */
Config_instruction_group::Config_instruction_group(){}

Config_instruction_group::Config_instruction_group(const Config_instruction_group &other){
    dir_ = other.dir_;

    instructions_.clear();
    for(std::vector<Config_instruction>::size_type i = 0; i < other.instructions_.size() ; i++){
        instructions_.push_back(other.instructions_.at(i));
    }

}

bool Config_instruction_group::operator ==(const Config_instruction_group &other)
{
    if((instructions_.size() != other.instructions_.size()) || (dir_ != other.dir_)){
        return false;
    }

    for(std::vector<Config_instruction>::size_type i = 0;i < other.instructions_.size();i++){
        if(instructions_.at(i) != other.instructions_.at(i)){
            return false;
        }
    }

    return true;
}

Config_instruction_group::~Config_instruction_group(){}

std::vector<Config_instruction> Config_instruction_group::instructions() const
{
    return instructions_;
}

std::string Config_instruction_group::dir() const
{
    return dir_;
}

bool Config_instruction_group::execute(){

    if(is_valid()){
        bool written = false;
        std::map<std::string,std::string> classes_to_include;

        for(std::vector<Config_instruction>::size_type i = 0;i < instructions_.size(); i++){
            auto instruction = instructions_.at(i);
            if(instruction.execute(dir_)){

                classes_to_include[instruction.qt_class_name()] = instruction.qt_file_prefix() + ".h";
                if(!written){
                    written = true;
                }
            }
        }
        if(written){
            bool native_common_h_created = false;
            bool native_register_h_created = false;
            bool native_register_cpp_created =false;
            if(Common_file_writer::write_native_common_h(dir_)){
                native_common_h_created = true;
                print_message("\"" + dir_ + "/" + native_common + ".h\" written successfully");
            }
            if(Common_file_writer::write_native_register_h(dir_)){
                native_register_h_created = true;
                print_message("\"" + dir_ + "/" + native_register + ".h\" written successfully");
            }
            if(Common_file_writer::write_native_register_cpp(dir_,classes_to_include)){
                native_register_cpp_created = true;
                print_message("\"" + dir_ + "/" + native_register + ".cpp\" written successfully");
            }
            return (native_common_h_created && native_register_h_created && native_register_cpp_created);
        }
    }else{
        print_error("Error failed to generate qt files");

    }
    return false;
}

/** -------------------------------------------------
 * @brief The Configuration_file class
 */
std::vector<Config_instruction_group> Configuration_file::parse(std::string config_file_path)
{
    auto config_error = [config_file_path](std::string txt){
        print_error("Error in \""+ config_file_path + "\" detected!,"+txt);
    };

    std::vector<Config_instruction_group> c_i_groups;
    auto config_file_content = File::read(config_file_path);

    //remove comments
    config_file_content = comment().removeFrom(config_file_content);

    if(!config_file_content.empty()){
        auto c_i_group_markers = string("dir:",false).allMatches(config_file_content);
        auto c_i_group_count = c_i_group_markers.size();

        if(c_i_group_count == 0){
            print_error("Error ,\"" + config_file_path + "\" is not a valid configuration file");
        }else{
            for(std::vector<Success>::size_type i = 0; i < c_i_group_count ; i++){
                pos start = c_i_group_markers.at(i).start();
                pos end;
                if(i < (c_i_group_count - 1)){
                    end = c_i_group_markers.at(i + 1).start();
                }else{
                    end = config_file_content.length();
                }
                auto config = config_file_content.substr(start,end - start);
                auto tmp =  String_utils::trim(string("dir:",false).removeFrom(config));
                auto match = chaR('(').firstMatch(tmp);
                if(match.isSuccess()){
                    auto dir = String_utils::trim(tmp.substr(0,match.success().start()));
                    if(dir.empty()){
                        config_error("Configuration instruction group with no destination dir found!");
                    }else{
                        Parser quote = chaR('"').oR(chaR('\''));
                        Config_instruction_group c_i_group;
                        c_i_group.dir_ = quote.removeFrom(dir);

                        auto add_instractions = [&c_i_group,&quote](std::string instruction) -> std::string{
                            auto tmp = chaR('(').oR(chaR(')')).removeFrom(instruction);
                            auto delimeter1 = String_utils::str_index_in(tmp,",");
                            auto delimeter2 = String_utils::str_index_in(tmp,"<");
                            auto class_name = String_utils::trim(tmp.substr(0,delimeter1));
                            auto file_prefix = String_utils::trim(tmp.substr(delimeter1+1,delimeter2 - (delimeter1+1)));
                            auto java_file = String_utils::trim(tmp.substr(delimeter2+1,tmp.length() - (delimeter2+1)));

                            if(quote.hasMatch(java_file)){
                                java_file = quote.seq(any(quote).plus()).seq(quote).firstStringMatch(java_file);
                                java_file = quote.removeFrom(java_file);
                            }

                            Config_instruction c_i ;
                            c_i.qt_class_name_ = class_name;
                            c_i.qt_file_prefix_ = file_prefix;
                            c_i.java_file_path_ = java_file;
                            c_i_group.instructions_.push_back(c_i);

                            return instruction;
                        };

                        chaR('(').seq(any(chaR(',')).plus().seq(chaR(','))).seq(any(chaR('<')).plus().seq(chaR('<')))
                                .seq(any(chaR(')')).plus().seq(chaR(')'))).replaceInMapped(tmp,add_instractions);
                        if(c_i_group.is_valid()){
                            c_i_groups.push_back(c_i_group);
                        }
                    }

                }else{
                    config_error("Configuration instruction group with no instruction(s) found!");
                }
            }
        }

    }else{
        print_error("Error ,\"" + config_file_path + "\" is not a valid configuration file");
    }

    return  c_i_groups;
}
