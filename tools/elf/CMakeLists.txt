# Nuasm is an assembler.
# Copyright (c) 2025, Niz <mn@furzoom.com>, Furzoom.com
# https://furzoom.com
# https://github.com/nua-lang/nuasm
#

SET(DELF_SRC
        parse.c
)

ADD_LIBRARY(nelf ${DELF_SRC})

ADD_EXECUTABLE(elf elf.c)
TARGET_LINK_LIBRARIES(elf PRIVATE nelf)
