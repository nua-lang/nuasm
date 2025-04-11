# Nuasm is an assembler.
# Copyright (c) 2025, Niz <mn@furzoom.com>, Furzoom.com
# https://furzoom.com
# https://github.com/nua-lang/nuasm
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

PROJECT(Nuasm C)

SET(C_FLAGS
        -g
        -Wall
        -Wextra
        -Werror
        -Wconversion
        -Wpointer-arith
        -Wshadow
        -Wwrite-strings
)

STRING(REPLACE ";" " " CMAKE_C_FLAGS "${C_FLAGS}")

OPTION(NUASM_ASAN "enable sanitizer?" OFF)
if (NUASM_ASAN)
    SET(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} " -fsanitize=address")
endif ()

SET(CMAKE_C_FLAGS_DEBUG "-O0")
SET(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

SET(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

SET(BUILD_TYPE "DEBUG")
IF (CMAKE_BUILD_TYPE)
    STRING(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
ENDIF ()

MESSAGE(STATUS "C_FLAGS = " ${CMAKE_C_FLAGS} " " ${CMAKE_C_FLAGS_${BUILD_TYPE}})

ADD_SUBDIRECTORY(tools)
