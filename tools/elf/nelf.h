/**
 * Nuasm is an assembler.
 * Copyright (c) 2025, Niz <mn@furzoom.com>, Furzoom.com
 * https://furzoom.com
 * https://github.com/nua-lang/nuasm
 */

#ifndef NELF_H_
#define NELF_H_

#include <stdint.h>
#include <stdio.h>

typedef struct File File;

typedef enum E_Type {
  kENone,
  kERel,
  kEExec,
  kEDyn,
  kECore,
} E_Type;

File* NewFile();
void FreeFile(File* file);

int Parse(File* input, const char* filename);

#endif