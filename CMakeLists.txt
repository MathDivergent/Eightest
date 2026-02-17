# [[Module][Root]]
cmake_minimum_required(VERSION 3.16)

project(Eightest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LIBRARY_PREFIX "")


# [[Module][Options]]
option(EIGHTEST_BUILD_SHARED_LIBS "Build shared libraies by Default" ON)
option(EIGHTEST_DEFAULT_STAT_HANDLER "Build with printf stat handler by Default" ON)


# [[Module][Defaults]]
if(EIGHTEST_BUILD_SHARED_LIBS)
    set(PROJECT_LIBS_TYPE SHARED)
else()
    set(PROJECT_LIBS_TYPE STATIC)
endif()

if(WIN32)
    set(EIGHTEST_MODULE_EXPORT "__declspec(dllexport)")
    set(EIGHTEST_MODULE_IMPORT "__declspec(dllimport)")
elseif(LINUX OR APPLE)
    set(EIGHTEST_MODULE_EXPORT "__attribute__((visibility(\"default\")))")
    set(EIGHTEST_MODULE_IMPORT "")
endif()

if(LINUX OR APPLE)
    set(EIGHTEST_VISIBILITY_HIDDEN C_VISIBILITY_PRESET hidden CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN ON)
endif()


# [[Module][Binaries]]
file(GLOB_RECURSE PROJECT_SOURCES_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
add_library(Eightest ${PROJECT_LIBS_TYPE} ${PROJECT_SOURCES_FILES})

target_include_directories(Eightest PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")


# [[Module][Configurations]]
if(EIGHTEST_BUILD_SHARED_LIBS)
    target_compile_definitions(Eightest PRIVATE "EIGHTEST_API=${EIGHTEST_MODULE_EXPORT}" INTERFACE "EIGHTEST_API=${EIGHTEST_MODULE_IMPORT}")

    if(LINUX OR APPLE)
        set_target_properties(Eightest PROPERTIES ${EIGHTEST_VISIBILITY_HIDDEN})
    endif()
else()
    target_compile_definitions(Eightest PUBLIC "EIGHTEST_API=")
endif()

if(EIGHTEST_DEFAULT_STAT_HANDLER)
    target_compile_definitions(Eightest PUBLIC "EIGHTEST_DEFAULT_STAT_HANDLER=")
endif()

target_compile_options(Eightest PRIVATE -fno-rtti)


# [[Launcher][Defaults]]
if(EIGHTEST_RUN_MODULE)
    if(LINUX)
        set(EIGHTEST_RPATH "$ORIGIN")
    elseif(APPLE)
        set(EIGHTEST_RPATH "@loader_path/../Frameworks")
    endif()
endif()


# [[Launcher][Binaries]]
if(EIGHTEST_RUN_MODULE)
    add_executable(EightestLauncher "${CMAKE_CURRENT_SOURCE_DIR}/test/Launcher.cpp")

    target_link_libraries(EightestLauncher PUBLIC Eightest)
    if(LINUX)
        target_link_libraries(EightestLauncher PRIVATE dl)
    endif()
endif()


# [[Launcher][Configurations]]
if(EIGHTEST_RUN_MODULE)
    if(LINUX OR APPLE)
        set_target_properties(EightestLauncher PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "${EIGHTEST_RPATH}")
    endif()

    target_compile_definitions(EightestLauncher PUBLIC "EIGHTEST_RUN_MODULE=\"${EIGHTEST_RUN_MODULE}\"")
endif()
