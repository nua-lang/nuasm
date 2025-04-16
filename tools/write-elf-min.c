#include <elf.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// "hello world!\n"
uint8_t seg_data[] = {
    0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77,
    0x6f, 0x72, 0x6c, 0x64, 0x21, 0x0a,
};

uint8_t seg_text_64[] = {
    0xb8, 0x01, 0x00, 0x00, 0x00,  // mov    $0x1,%eax
    0xbf, 0x01, 0x00, 0x00, 0x00,  // mov    $0x1,%edi
    0xbe, 0x78, 0x00, 0x40, 0x00,  // movabs $0x400078,%rsi
    0xba, 0x0d, 0x00, 0x00, 0x00,  // mov    $0xd,%edx
    0x0f, 0x05,                    // syscall
    0xb8, 0x3c, 0x00, 0x00, 0x00,  // mov    $0x3c,%eax
    0xbf, 0x00, 0x00, 0x00, 0x00,  // mov    $0x0,%edi
    0x0f, 0x05,                    // syscall
};

uint8_t seg_text_32[] = {
    0xb8, 0x04, 0x00, 0x00, 0x00,  // mov    $0x4,%eax
    0xbb, 0x01, 0x00, 0x00, 0x00,  // mov    $0x1,%ebx
    0xb9, 0x54, 0x80, 0x04, 0x08,  // mov    $0x8048054,%ecx
    0xba, 0x0d, 0x00, 0x00, 0x00,  // mov    $0xd,%edx
    0xcd, 0x80,                    // int    $0x80
    0xb8, 0x01, 0x00, 0x00, 0x00,  // mov    $0x1,%eax
    0xbb, 0x00, 0x00, 0x00, 0x00,  // mov    $0x0,%ebx
    0xcd, 0x80,                    // int    $0x80
};

#define entry_64 0x400000
#define data_offset_64 (sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr))
#define text_offset_64 (data_offset_64 + sizeof(seg_data))

#define entry_32 0x8048000
#define data_offset_32 (sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr))
#define text_offset_32 (data_offset_32 + sizeof(seg_data))

Elf64_Ehdr ehdr_64 = {
    .e_ident     = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3, ELFCLASS64, ELFDATA2LSB,
                    EV_CURRENT, ELFOSABI_NONE},
    .e_type      = ET_EXEC,
    .e_machine   = EM_X86_64,
    .e_version   = EV_CURRENT,
    .e_entry     = entry_64 + text_offset_64,
    .e_phoff     = sizeof(Elf64_Ehdr),
    .e_shoff     = 0x0,
    .e_flags     = 0x0,
    .e_ehsize    = sizeof(Elf64_Ehdr),
    .e_phentsize = sizeof(Elf64_Phdr),
    .e_phnum     = 1,
    .e_shentsize = sizeof(Elf64_Shdr),
    .e_shnum     = 0,
    .e_shstrndx  = 0,
};

Elf32_Ehdr ehdr_32 = {
    .e_ident     = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3, ELFCLASS32, ELFDATA2LSB,
                    EV_CURRENT, ELFOSABI_NONE},
    .e_type      = ET_EXEC,
    .e_machine   = EM_386,
    .e_version   = EV_CURRENT,
    .e_entry     = entry_32 + text_offset_32,
    .e_phoff     = sizeof(Elf32_Ehdr),
    .e_shoff     = 0x0,
    .e_flags     = 0x0,
    .e_ehsize    = sizeof(Elf32_Ehdr),
    .e_phentsize = sizeof(Elf32_Phdr),
    .e_phnum     = 1,
    .e_shentsize = sizeof(Elf32_Shdr),
    .e_shnum     = 0,
    .e_shstrndx  = 0,
};

Elf64_Phdr phdr_text_64 = {
    .p_type   = PT_LOAD,
    .p_flags  = PF_R | PF_X,
    .p_offset = data_offset_64,
    .p_vaddr  = entry_64 + data_offset_64,
    .p_paddr  = entry_64 + data_offset_64,
    .p_filesz = sizeof(seg_text_64) + sizeof(seg_data),
    .p_memsz  = sizeof(seg_text_64) + sizeof(seg_data),
    .p_align  = 0x1000,
};

Elf32_Phdr phdr_text_32 = {
    .p_type   = PT_LOAD,
    .p_flags  = PF_R | PF_X,
    .p_offset = data_offset_32,
    .p_vaddr  = entry_32 + data_offset_32,
    .p_paddr  = entry_32 + data_offset_32,
    .p_filesz = sizeof(seg_text_32) + sizeof(seg_data),
    .p_memsz  = sizeof(seg_text_32) + sizeof(seg_data),
    .p_align  = 0x1000,
};

static void usage(const char* exe) {
  fprintf(stderr, "usage: %s [[elf32|elf64] output]\n", exe);
  exit(1);
}

int main(int argc, char* argv[]) {
  FILE* output    = stdout;
  uint8_t is_64   = 0;
  const char* exe = argv[0];

  if (argc > 3) {
    usage(exe);
  }

  argv++;
  argc--;

  if (argc == 2) {
    if (strcmp(argv[0], "elf64") == 0) {
      is_64 = 1;
    } else if (strcmp(argv[0], "elf32") == 0) {
      is_64 = 0;
    } else {
      usage(exe);
    }

    argv++;
    argc--;
  }

  if (argc == 1) {
    output = fopen(argv[0], "wb");
    if (!output) {
      fprintf(stderr, "failed to open %s: %s", argv[1], strerror(errno));
      usage(exe);
    }
  }

  if (is_64) {
    fwrite(&ehdr_64, 1, sizeof(ehdr_64), output);
    fwrite(&phdr_text_64, 1, sizeof(phdr_text_64), output);
    fwrite(seg_data, 1, sizeof(seg_data), output);
    fwrite(seg_text_64, 1, sizeof(seg_text_64), output);
  } else {
    fwrite(&ehdr_32, 1, sizeof(ehdr_32), output);
    fwrite(&phdr_text_32, 1, sizeof(phdr_text_32), output);
    fwrite(seg_data, 1, sizeof(seg_data), output);
    fwrite(seg_text_32, 1, sizeof(seg_text_32), output);
  }
  fclose(output);
  return 0;
}
