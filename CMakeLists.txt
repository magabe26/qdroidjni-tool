cmake_minimum_required(VERSION 3.17)
cmake_policy(VERSION 3.17)

project(magabe_parser CXX)

add_subdirectory(core)
add_subdirectory(parsers)

get_filename_component(PROJECT_CURRENT_DIR "./.." ABSOLUTE)
set(magabe_parser-parsers_DIR  "${PROJECT_CURRENT_DIR}/magabe_parser-parsers-${CMAKE_BUILD_TYPE}" )
list(APPEND CMAKE_PREFIX_PATH "${magabe_parser-parsers_DIR}/lib/cmake")

find_package(magabe_parser-parsers 1.0 REQUIRED)
find_package(magabe_parser-core 1.0 REQUIRED)

add_executable(qdroidjni-tool)

target_sources(qdroidjni-tool
    PRIVATE
    main.cpp
    file.cpp
    debug.cpp
    java.cpp
    java_class_files.cpp
    string_utils.cpp
    whitespace_utils.cpp
    common_file_writer.cpp
    qt.cpp
    configuration_file.cpp
    error.cpp
    qt_class_files.cpp
    function_parser.cpp
    comment_parser.cpp


    )

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_compile_definitions(qdroidjni-tool
        PRIVATE
         "PRINT_DEBUG_MESSAGE"
        )

endif()

target_link_libraries(qdroidjni-tool
    PUBLIC
    magabe_parser-parsers::magabe_parser-parsers
    )


add_custom_command(TARGET qdroidjni-tool
    POST_BUILD
    COMMAND make
    ARGS install
    COMMENT "FINISHED!! executable 'qdroidjni-tool' is saved in ${PROJECT_CURRENT_DIR}/qdroidjni-tool-${CMAKE_BUILD_TYPE}"
)

install(TARGETS qdroidjni-tool
     RUNTIME DESTINATION "${PROJECT_CURRENT_DIR}/qdroidjni-tool-${CMAKE_BUILD_TYPE}"
    )
