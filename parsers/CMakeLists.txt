cmake_minimum_required(VERSION 3.17)
cmake_policy(VERSION 3.17)

project(magabe_parser-parsers CXX)

include(CMakePackageConfigHelpers)

set(magabe_parser-parsers_VERSION "1.0" )

add_library(magabe_parser-parsers STATIC)

get_filename_component(PROJECT_CURRENT_DIR "./../.." ABSOLUTE)
set(magabe_parser-core_DIR  "${PROJECT_CURRENT_DIR}/magabe_parser-core-${CMAKE_BUILD_TYPE}/core" )
list(APPEND CMAKE_PREFIX_PATH "${magabe_parser-core_DIR}/lib/cmake")

set(magabe_parser-parsers_DIR  "${PROJECT_CURRENT_DIR}/magabe_parser-parsers-${CMAKE_BUILD_TYPE}" )

find_package(magabe_parser-core 1.0 REQUIRED)

target_sources(magabe_parser-parsers
    PRIVATE

    char_parsers.cpp
    any_character_parser.cpp
    all_input_parser.cpp
    pattern_parser.cpp
    space_parsers.cpp
    string_parser.cpp
  )

target_link_libraries(magabe_parser-parsers
    PUBLIC
    magabe_parser-core::magabe_parser-core
    )

target_compile_features(magabe_parser-parsers
    PUBLIC
    cxx_std_11
    )

write_basic_package_version_file("magabe_parser-parsersConfigVersion.cmake"
    VERSION "${magabe_parser-parsers_VERSION}"
    COMPATIBILITY SameMajorVersion
    )

configure_file("magabe_parser-parsersConfig.cmake.in"
                "${magabe_parser-parsers_DIR}/lib/cmake/magabe_parser-parsersConfig.cmake" @ONLY
    )

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/magabe_parser-parsersConfigVersion.cmake"
    DESTINATION "${magabe_parser-parsers_DIR}/lib/cmake"
    )

add_custom_command(TARGET magabe_parser-parsers
    POST_BUILD
     COMMAND ${CMAKE_COMMAND}
     ARGS -E chdir ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_MAKE_PROGRAM} install
     COMMENT "INSTALLING target 'magabe_parser-parsers' to ${magabe_parser-parsers_DIR}"
)

install(DIRECTORY "./"
    DESTINATION "${magabe_parser-parsers_DIR}/include"
    PATTERN "*.cpp" EXCLUDE
    PATTERN "*.txt" EXCLUDE
    PATTERN "*.cmake.*" EXCLUDE
    )

install(TARGETS magabe_parser-parsers EXPORT magabe_parser-parsersTargets
     LIBRARY DESTINATION "${magabe_parser-parsers_DIR}/lib"
     ARCHIVE DESTINATION "${magabe_parser-parsers_DIR}/lib"
     RUNTIME DESTINATION "${magabe_parser-parsers_DIR}/bin"
     INCLUDES DESTINATION "${magabe_parser-parsers_DIR}/include"
    )

install(EXPORT magabe_parser-parsersTargets
    FILE magabe_parser-parsersTargets.cmake
    NAMESPACE magabe_parser-parsers::
    DESTINATION "${magabe_parser-parsers_DIR}/lib/cmake"
    )
