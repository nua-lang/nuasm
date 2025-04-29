#include <elf.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t shstrtab[] = {'\0', '.', 's', 'h', 's', 't', 'r', 't', 'a', 'b', '\0'};

Elf64_Ehdr ehdr_64 = {
    .e_ident     = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3, ELFCLASS64, ELFDATA2LSB,
                    EV_CURRENT, ELFOSABI_NONE},
    .e_type      = ET_REL,
    .e_machine   = EM_X86_64,
    .e_version   = EV_CURRENT,
    .e_entry     = 0,
    .e_phoff     = 0x0,
    .e_shoff     = sizeof(Elf64_Ehdr),
    .e_flags     = 0x0,
    .e_ehsize    = sizeof(Elf64_Ehdr),
    .e_phentsize = sizeof(Elf64_Phdr),
    .e_phnum     = 0,
    .e_shentsize = sizeof(Elf64_Shdr),
    .e_shnum     = 2,
    .e_shstrndx  = 1,
};

Elf32_Ehdr ehdr_32 = {
    .e_ident     = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3, ELFCLASS32, ELFDATA2LSB,
                    EV_CURRENT, ELFOSABI_NONE},
    .e_type      = ET_REL,
    .e_machine   = EM_386,
    .e_version   = EV_CURRENT,
    .e_entry     = 0,
    .e_phoff     = 0x0,
    .e_shoff     = sizeof(Elf32_Ehdr),
    .e_flags     = 0x0,
    .e_ehsize    = sizeof(Elf32_Ehdr),
    .e_phentsize = sizeof(Elf32_Phdr),
    .e_phnum     = 0,
    .e_shentsize = sizeof(Elf32_Shdr),
    .e_shnum     = 2,
    .e_shstrndx  = 1,
};

Elf32_Shdr shdr_empty_32    = {0};
Elf64_Shdr shdr_empty_64    = {0};

Elf32_Shdr shdr_shstrtab_32 = {
    .sh_name      = 1,
    .sh_type      = SHT_STRTAB,
    .sh_flags     = 0,
    .sh_addr      = 0,
    .sh_offset    = sizeof(Elf32_Ehdr) + 2 * sizeof(Elf32_Shdr),
    .sh_size      = sizeof(shstrtab),
    .sh_link      = 0,
    .sh_info      = 0,
    .sh_addralign = 0,
    .sh_entsize   = 0,
};

Elf64_Shdr shdr_shstrtab_64 = {
    .sh_name      = 1,
    .sh_type      = SHT_STRTAB,
    .sh_flags     = 0,
    .sh_addr      = 0,
    .sh_offset    = sizeof(Elf64_Ehdr) + 2 * sizeof(Elf64_Shdr),
    .sh_size      = sizeof(shstrtab),
    .sh_link      = 0,
    .sh_info      = 0,
    .sh_addralign = 0,
    .sh_entsize   = 0,
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
    fwrite(&shdr_empty_64, 1, sizeof(shdr_empty_64), output);
    fwrite(&shdr_shstrtab_64, 1, sizeof(shdr_shstrtab_64), output);
    fwrite(shstrtab, 1, sizeof(shstrtab), output);
  } else {
    fwrite(&ehdr_32, 1, sizeof(ehdr_32), output);
    fwrite(&shdr_empty_32, 1, sizeof(shdr_empty_32), output);
    fwrite(&shdr_shstrtab_32, 1, sizeof(shdr_shstrtab_32), output);
    fwrite(shstrtab, 1, sizeof(shstrtab), output);
  }
  fclose(output);
  return 0;
}
