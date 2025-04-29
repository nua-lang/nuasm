/**
 * Nuasm is an assembler.
 * Copyright (c) 2025, Niz <mn@furzoom.com>, Furzoom.com
 * https://furzoom.com
 * https://github.com/nua-lang/nuasm
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "nelf.h"

typedef struct File {
  FILE* input;
  uint8_t* buf;
  uint64_t size;
  uint64_t pos;
} File;

const uint8_t kMagic[]         = {0x7f, 0x45, 0x4c, 0x46};
const uint32_t kBufSize        = 16 * 1024;
const uint8_t kElfIdent        = 16;
const uint8_t kElfIdentPadding = 8;

File* NewFile() {
  File* file = calloc(1, sizeof(File));
  if (file == NULL) {
    fprintf(stderr, "alloc File failed\n");
    return file;
  }

  return file;
}

void FreeFile(File* file) {
  if (file == NULL) {
    return;
  }

  fclose(file->input);
  free(file->buf);

  free(file);
}

static int OpenFile(File* file, const char* filename) {
  if (file->input != NULL) {
    fprintf(stderr, "previous input is opened\n");
    return -1;
  }

  file->input = fopen(filename, "rb");
  if (file->input == NULL) {
    fprintf(stderr, "open %s failed: %s\n", filename, strerror(errno));
    return -1;
  }

  file->pos  = 0;
  file->size = 0;
  if (file->buf == NULL) {
    file->buf = malloc(kBufSize);
    if (file->buf == NULL) {
      fprintf(stderr, "alloc buf failed\n");
      return -1;
    }
  }

  return 0;
}

int ReadMoreData(File* file) {
  if (file->pos != 0) {
    memmove(file->buf, file->buf + file->pos, file->pos);
    file->size -= file->pos;
    file->pos = 0;
  }

  size_t n =
      fread(file->buf + file->pos, 1, kBufSize - file->size, file->input);
  if (n == 0) {
    fprintf(stderr, "read input error: %s\n", strerror(errno));
    return -1;
  }

  file->size += n;

  return 0;
}

static uint8_t ReadUint8(File* file) {
  uint8_t n = *(uint8_t*)(file->buf + file->pos);
  file->pos++;
  return n;
}

static uint16_t ReadUint16(File* file) {
  uint16_t n = *(uint16_t*)(file->buf + file->pos);
  file->pos += 2;
  return n;
}

static uint32_t ReadUint32(File* file) {
  uint32_t n = *(uint32_t*)(file->buf + file->pos);
  file->pos += 4;
  return n;
}

static int ParseHeaderIdent(File* file) {
  if (file->pos + kElfIdent > file->size) {
    if (ReadMoreData(file) == -1) {
      fprintf(stderr, "not enough data\n");
      return -1;
    }
  }

  if (memcmp(file->buf + file->pos, kMagic, sizeof(kMagic)) != 0) {
    fprintf(stderr, "not valid ELF file: %#08x\n",
            *(uint32_t*)(file->buf + file->pos));
    return -1;
  }
  file->pos += sizeof(kMagic);

  uint8_t val = ReadUint8(file);
  /* Class. 1 => 32-bit, 2 => 64-bit */
  if (val != 1) {
    fprintf(stderr, "unsupported class: %#x\n", val);
    return -1;
  }

  /* Endian. 1 => Little, 2 => Big */
  if ((val = ReadUint8(file)) != 1) {
    fprintf(stderr, "unsupported endian: %#x\n", val);
    return -1;
  }

  /* Version. 1 => Current */
  if ((val = ReadUint8(file)) != 1) {
    fprintf(stderr, "unsupported version: %#x\n", val);
    return -1;
  }

  /* OS ABI. 0 => SysV */
  if ((val = ReadUint8(file)) != 0) {
    fprintf(stderr, "unsupported OS ABI: %#x\n", val);
    return -1;
  }

  file->pos += kElfIdentPadding;

  return 0;
}

static int ParseElfHeader(File* file) {
  if (ParseHeaderIdent(file) == -1) {
    return -1;
  }

  /*

  return 0;
}

int Parse(File* input, const char* filename) {
  if (input == NULL) {
    fprintf(stderr, "invalid File %p\n", input);
    return -1;
  }

  if (OpenFile(input, filename) == -1) {
    return -1;
  }

  if (input->size == input->pos) {
    if (ReadMoreData(input) == -1) {
      return -1;
    }
  }

  if (ParseElfHeader(input) == -1) {
    return -1;
  }

  return 0;
}