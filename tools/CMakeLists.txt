# Nuasm is an assembler.
# Copyright (c) 2025, Niz <mn@furzoom.com>, Furzoom.com
# https://furzoom.com
# https://github.com/nua-lang/nuasm
#

# TARGET: hello-c
ADD_EXECUTABLE(hello-c hello-c.c)
TARGET_LINK_LIBRARIES(hello-c -static)
ADD_CUSTOM_COMMAND(TARGET hello-c
        POST_BUILD
        COMMAND strip $<TARGET_FILE:hello-c>
        COMMENT "stripping hello-c"
        VERBATIM
)

# TARGET: hello-asm
IF(IS_x64)
    SET(NASM_FORMAT elf64)
    SET(HELLO_ASM_SRC hello-asm-64.asm)
ELSE()
    SET(NASM_FORMAT elf32)
    SET(HELLO_ASM_SRC hello-asm-32.asm)
ENDIF()

ADD_CUSTOM_TARGET(hello-asm ALL
        DEPENDS ${HELLO_ASM_SRC}
)

ADD_CUSTOM_COMMAND(TARGET hello-asm
        PRE_BUILD
        COMMAND mkdir -p ${PROJECT_BINARY_DIR}/cache
        COMMAND nasm -f ${NASM_FORMAT} -o ${PROJECT_BINARY_DIR}/cache/hello-asm.o ${CMAKE_CURRENT_SOURCE_DIR}/${HELLO_ASM_SRC}
        COMMAND ld ${PROJECT_BINARY_DIR}/cache/hello-asm.o -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hello-asm
        COMMAND strip ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hello-asm
        COMMENT "assembly hello-asm"
        VERBATIM
)

# TARGET: hello-elf
SET(ELF_TARGET ${NASM_FORMAT})
SET(HELLO_ELF_SRC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hello-elf)
ADD_EXECUTABLE(write-elf write-elf.c)
ADD_CUSTOM_TARGET(hello-elf ALL
        DEPENDS ${HELLO_ELF_SRC}
)
ADD_CUSTOM_COMMAND(OUTPUT ${HELLO_ELF_SRC}
        PRE_BUILD
        DEPENDS write-elf
        COMMAND mkdir -p ${PROJECT_BINARY_DIR}/cache
        COMMAND $<TARGET_FILE:write-elf> ${ELF_TARGET} ${HELLO_ELF_SRC}
        COMMAND chmod +x ${HELLO_ELF_SRC}
        COMMENT "generating ${HELLO_ELF_SRC}"
        VERBATIM
)

# TARGET: hello-elf-min
SET(HELLO_ELF_MIN_SRC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hello-elf-min)
ADD_EXECUTABLE(write-elf-min write-elf-min.c)
ADD_CUSTOM_TARGET(hello-elf-min ALL
        DEPENDS ${HELLO_ELF_MIN_SRC}
)
ADD_CUSTOM_COMMAND(OUTPUT ${HELLO_ELF_MIN_SRC}
        PRE_BUILD
        DEPENDS write-elf-min
        COMMAND mkdir -p ${PROJECT_BINARY_DIR}/cache
        COMMAND $<TARGET_FILE:write-elf-min> ${ELF_TARGET} ${HELLO_ELF_MIN_SRC}
        COMMAND chmod +x ${HELLO_ELF_MIN_SRC}
        COMMENT "generating ${HELLO_ELF_MIN_SRC}"
        VERBATIM
)


