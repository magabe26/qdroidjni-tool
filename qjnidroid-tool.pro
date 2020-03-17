TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

#print debug messages
DEFINES += PRINT_DEBUG_MESSAGE

SOURCES += \
        comment_parser.cpp \
        common_file_writer.cpp \
        configuration_file.cpp \
        core/context.cpp \
        core/delegates.cpp \
        core/errors.cpp \
        core/parser.cpp \
        core/result.cpp \
        core/utils.cpp \
        debug.cpp \
        error.cpp \
        file.cpp \
        function_parser.cpp \
        java.cpp \
        java_class_files.cpp \
        main.cpp \
        parsers/all_input_parser.cpp \
        parsers/any_character_parser.cpp \
        parsers/char_parsers.cpp \
        parsers/pattern_parser.cpp \
        parsers/space_parsers.cpp \
        parsers/string_parser.cpp \
        qt.cpp \
        qt_class_files.cpp \
        string_utils.cpp \
        whitespace_utils.cpp

HEADERS += \
    comment_parser.h \
    common_file_writer.h \
    configuration_file.h \
    core/context.h \
    core/core.h \
    core/def.h \
    core/delegates.h \
    core/errors.h \
    core/parser.h \
    core/result.h \
    core/utils.h \
    debug.h \
    error.h \
    file.h \
    function_parser.h \
    java.h \
    java_class_files.h \
    parsers/all_input_parser.h \
    parsers/any_character_parser.h \
    parsers/char_parsers.h \
    parsers/parsers.h \
    parsers/pattern_parser.h \
    parsers/space_parsers.h \
    parsers/string_parser.h \
    qt.h \
    qt_class_files.h \
    string_utils.h \
    whitespace_utils.h

DISTFILES += \
    CMakeLists.txt \
    config.cmake \
    core/CMakeLists.txt \
    core/magabe_parser-coreConfig.cmake.in \
    parsers/CMakeLists.txt \
    parsers/magabe_parser-parsersConfig.cmake.in
