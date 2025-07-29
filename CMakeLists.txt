
cmake_minimum_required(VERSION 3.20...4.2)

project("QkTraits" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library("QkTraits" INTERFACE
    "include/QkTraits/StringLiteral.h"
    "include/QkTraits/View.h"
    "include/QkTraits/TypeList.h"
    "include/QkTraits/TypeTraits.h"
    "include/QkTraits/PointerTraits.h"
)


target_include_directories("QkTraits" INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)


# Testing Setup (GoogleTest)
if(PROJECT_IS_TOP_LEVEL)
    enable_testing()

    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )

    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "RunTests")
endif()
