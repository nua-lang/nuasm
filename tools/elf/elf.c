/**
 * Nuasm is an assembler.
 * Copyright (c) 2025, Niz <mn@furzoom.com>, Furzoom.com
 * https://furzoom.com
 * https://github.com/nua-lang/nuasm
 */

#include <stdio.h>

#include "nelf.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("usage: %s <filename>\n", argv[0]);
    return -1;
  }

  File* file = NewFile();
  if (file == NULL) {
    return -1;
  }

  if (Parse(file, argv[1]) == -1) {
    FreeFile(file);
    return -1;
  }

  printf("%s is a supported ELF file\n", argv[1]);

  FreeFile(file);
  return 0;
}