cmake_minimum_required(VERSION 4.2)
project(PyAmore)

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

Python_add_library(PyAmore
        SHARED
        Src/Core/Core.ixx
        Src/Core/Types.ixx
        Src/Core/Log.ixx
        Src/Core/Hook.ixx
        Src/Platform/Platform.ixx
        Src/PyAmoreTransform.ixx
        Src/PyAmoreTransform.cpp
)

if (UNIX)
    target_sources(PyAmore PRIVATE
            Src/Platform/Platform.Unix.cpp
    )
endif()



#Lib Detour
include_directories(External/LibDetour/src)
target_sources(PyAmore PRIVATE
        External/LibDetour/src/libdetour.h
        External/LibDetour/src/libdetour.c
)