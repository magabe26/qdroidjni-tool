/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#ifndef CONFIGURATION_FILE_PARSER_H
#define CONFIGURATION_FILE_PARSER_H
#include "parsers/parsers.h"
#include "debug.h"


/**  -----------------------------------------
 * @brief The Config_instruction class
 */
class Config_instruction{

public:
    Config_instruction();
    Config_instruction(const Config_instruction& other);

    bool operator ==(const Config_instruction& other);
    bool operator !=(const Config_instruction &other);

    std::string qt_class_name() const;
    std::string qt_file_prefix() const;
    std::string java_file_path() const;
    bool is_valid();
    bool execute(std::string qt_dir);

private:
    friend class Configuration_file;

    std::string qt_class_name_;
    std::string qt_file_prefix_;
    std::string java_file_path_;
};


/** -------------------------------------
 * @brief The Config_instruction_group class
 */
class Config_instruction_group{
public:
    Config_instruction_group();
    Config_instruction_group(const Config_instruction_group& other);
    bool operator ==(const Config_instruction_group& other);

    ~Config_instruction_group();
    std::string dir() const;
    std::vector<Config_instruction> instructions() const;
    bool is_valid();
    bool execute();

private:
    friend class Configuration_file;

    std::string dir_;
    std::vector<Config_instruction> instructions_;
};

/** -------------------------------------------------
 * @brief The Configuration_file class
 */
class Configuration_file
{

public:
    static std::vector<Config_instruction_group> parse(std::string config_file_path);

};

#endif // CONFIGURATION_FILE_PARSER_H
