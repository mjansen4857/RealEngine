cmake_minimum_required(VERSION 3.15)
project(RealEngine)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(RealEngine
        WIN32
        src/RealEngine.rc
        src/RealException.cpp
        src/RealException.h
        src/RealWin.h
        src/main.cpp
        src/resource.h
        src/Window.cpp
        src/Window.h)
