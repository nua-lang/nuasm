# Nuasm is an assembler.
# Copyright (c) 2025, Niz <mn@furzoom.com>, Furzoom.com
# https://furzoom.com
# https://github.com/nua-lang/nuasm
#

# TARGET: hello-c
add_executable(hello-c hello-c.c)
target_link_libraries(hello-c -static)
add_custom_command(TARGET hello-c
        POST_BUILD
        COMMAND strip $<TARGET_FILE:hello-c>
        COMMENT "stripping hello-c"
        VERBATIM
)

# TARGET: hello-asm
add_custom_target(hello-asm ALL
        DEPENDS hello-asm.asm
)

add_custom_command(TARGET hello-asm
        PRE_BUILD
        COMMAND mkdir -p ${PROJECT_BINARY_DIR}/cache
        COMMAND nasm -f elf64 -o ${PROJECT_BINARY_DIR}/cache/hello-asm.o ${CMAKE_CURRENT_SOURCE_DIR}/hello-asm.asm
        COMMAND ld ${PROJECT_BINARY_DIR}/cache/hello-asm.o -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hello-asm
        COMMAND strip ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hello-asm
        COMMENT "assembly hello-asm"
        VERBATIM
)

# TARGET: hello-elf
set(HELLO_ELF_SRC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hello-elf)
add_executable(write-elf write-elf.c)
add_custom_target(hello-elf ALL
        DEPENDS ${HELLO_ELF_SRC}
)
add_custom_command(OUTPUT ${HELLO_ELF_SRC}
        PRE_BUILD
        DEPENDS write-elf
        COMMAND mkdir -p ${PROJECT_BINARY_DIR}/cache
        COMMAND $<TARGET_FILE:write-elf> ${HELLO_ELF_SRC}
        COMMAND chmod +x ${HELLO_ELF_SRC}
        COMMENT "generating ${HELLO_ELF_SRC}"
        VERBATIM
)

# TARGET: hello-elf-min
set(HELLO_ELF_MIN_SRC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hello-elf-min)
add_executable(write-elf-min write-elf-min.c)
add_custom_target(hello-elf-min ALL
        DEPENDS ${HELLO_ELF_MIN_SRC}
)
add_custom_command(OUTPUT ${HELLO_ELF_MIN_SRC}
        PRE_BUILD
        DEPENDS write-elf-min
        COMMAND mkdir -p ${PROJECT_BINARY_DIR}/cache
        COMMAND $<TARGET_FILE:write-elf-min> ${HELLO_ELF_MIN_SRC}
        COMMAND chmod +x ${HELLO_ELF_MIN_SRC}
        COMMENT "generating ${HELLO_ELF_MIN_SRC}"
        VERBATIM
)


