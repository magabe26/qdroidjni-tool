cmake_minimum_required(VERSION 3.17)
cmake_policy(VERSION 3.17)

project(magabe_parser-core CXX)

include(CMakePackageConfigHelpers)

set(magabe_parser-core_VERSION "1.0" )

get_filename_component(PROJECT_CURRENT_DIR "./../.." ABSOLUTE)
set(magabe_parser-core_DIR  "${PROJECT_CURRENT_DIR}/magabe_parser-core-${CMAKE_BUILD_TYPE}/core" )

add_library(magabe_parser-core STATIC)

target_sources(magabe_parser-core
    PRIVATE
    context.cpp
    errors.cpp
    result.cpp
    delegates.cpp
    parser.cpp
    utils.cpp
  )


set_property(TARGET magabe_parser-core
    PROPERTY COMPILE_DEFINITIONS_DEBUG DEBUG_MESSAGE
    )

target_compile_features(magabe_parser-core
    PUBLIC
    cxx_std_11
    )

configure_file("magabe_parser-coreConfig.cmake.in"
                "${magabe_parser-core_DIR}/lib/cmake/magabe_parser-coreConfig.cmake" @ONLY
    )


write_basic_package_version_file("magabe_parser-coreConfigVersion.cmake"
    VERSION "${magabe_parser-core_VERSION}"
    COMPATIBILITY SameMajorVersion
    )

add_custom_command(TARGET magabe_parser-core
    POST_BUILD
     COMMAND ${CMAKE_COMMAND}
     ARGS -E chdir ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_MAKE_PROGRAM} install
     COMMENT "INSTALLING target 'magabe_parser-core' to ${magabe_parser-core_DIR}"
)

install(DIRECTORY "./"
    DESTINATION ${magabe_parser-core_DIR}
    PATTERN "*.cpp" EXCLUDE
    PATTERN "*.txt" EXCLUDE
    PATTERN "*.cmake.*" EXCLUDE
    )

install(TARGETS magabe_parser-core EXPORT magabe_parser-coreTargets
     LIBRARY DESTINATION "${magabe_parser-core_DIR}/lib"
     ARCHIVE DESTINATION "${magabe_parser-core_DIR}/lib"
     RUNTIME DESTINATION "${magabe_parser-core_DIR}/bin"
     INCLUDES DESTINATION "${PROJECT_CURRENT_DIR}/magabe_parser-core-${CMAKE_BUILD_TYPE}/"
    )

install(EXPORT magabe_parser-coreTargets
    FILE magabe_parser-coreTargets.cmake
    NAMESPACE magabe_parser-core::
    DESTINATION "${magabe_parser-core_DIR}/lib/cmake"
    )

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/magabe_parser-coreConfigVersion.cmake"
    DESTINATION "${magabe_parser-core_DIR}/lib/cmake"
    )
