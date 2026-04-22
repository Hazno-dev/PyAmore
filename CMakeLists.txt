cmake_minimum_required(VERSION 4.2)
project(PyAmore LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(Python 3.7
        EXACT
        COMPONENTS
        Interpreter
        Development.Embed
)

message(STATUS "Python Info:")
message(STATUS "Library: ${Python_LIBRARY}")
message(STATUS "Library Dirs: ${Python_LIBRARY_DIRS}")
message(STATUS "Libraries: ${PYTHON_LIBRARIES}")
message(STATUS "Executable: ${Python_EXECUTABLE}")
message(STATUS "Include Dir: ${PYTHON_INCLUDE_DIR}")
message(STATUS "Include Dirs: ${PYTHON_INCLUDE_DIRS}")

#Project
include_directories(Src/)
include_directories(${PYTHON_INCLUDE_DIRS})

Python_add_library(PyAmore SHARED)

target_sources(PyAmore
        PUBLIC
        FILE_SET HEADERS FILES
        Src/Core/Hook.h
        PUBLIC
        FILE_SET CXX_MODULES FILES
        Src/Core/Core.ixx
        Src/Core/Types.ixx
        Src/Core/Log.ixx
        Src/Core/Hook.ixx
        Src/Platform/Platform.ixx
        Src/Amore/Transform.ixx
        Src/Amore/Encryption/Encryption.ixx
        Src/Amore/Encryption/Encryption.SymmetricSXOR.ixx
)

target_sources(PyAmore
        PRIVATE
        Src/Amore/Transform.cpp
        Src/Amore/Transform.Builtins.cpp)

if (UNIX)
    target_sources(PyAmore
            PRIVATE
            Src/Platform/Platform.Unix.cpp)
endif()

set_target_properties(PyAmore PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY lib/
        LIBRARY_OUTPUT_NAME "PyAmore"
        PREFIX ""
)

#Lib Detour
include_directories(External/LibDetour/src)
target_sources(PyAmore
        PUBLIC
        External/LibDetour/src/libdetour.h
        PRIVATE
        External/LibDetour/src/libdetour.c
)