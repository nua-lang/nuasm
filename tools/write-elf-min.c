#include <elf.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// "hello world!\n"
uint8_t seg_data[] = {
    0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77,
    0x6f, 0x72, 0x6c, 0x64, 0x21, 0x0a,
};

uint8_t seg_text[] = {
    0xb8, 0x01, 0x00, 0x00, 0x00,  // mov    $0x1,%eax
    0xbf, 0x01, 0x00, 0x00, 0x00,  // mov    $0x1,%edi
    0xbe, 0x78, 0x00, 0x40, 0x00,  // movabs $0x400078,%rsi
    0xba, 0x0d, 0x00, 0x00, 0x00,  // mov    $0xd,%edx
    0x0f, 0x05,                    // syscall
    0xb8, 0x3c, 0x00, 0x00, 0x00,  // mov    $0x3c,%eax
    0xbf, 0x00, 0x00, 0x00, 0x00,  // mov    $0x0,%edi
    0x0f, 0x05,                    // syscall
};

Elf64_Ehdr ehdr = {
    .e_ident     = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3, ELFCLASS64, ELFDATA2LSB,
                    EV_CURRENT, ELFOSABI_NONE},
    .e_type      = ET_EXEC,
    .e_machine   = EM_X86_64,
    .e_version   = EV_CURRENT,
    .e_entry     = 0x400085,
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

Elf64_Phdr phdr_text = {
    .p_type   = PT_LOAD,
    .p_flags  = PF_R | PF_X,
    .p_offset = 0x78,
    .p_vaddr  = 0x400078,
    .p_paddr  = 0x400078,
    .p_filesz = sizeof(seg_text) + sizeof(seg_data),
    .p_memsz  = sizeof(seg_text) + sizeof(seg_data),
    .p_align  = 0x1000,
};

int main(int argc, char* argv[]) {
  FILE* output = stdout;
  if (argc > 2) {
    fprintf(stderr, "usage: %s [output]\n", argv[0]);
    return 1;
  } else if (argc == 2) {
    output = fopen(argv[1], "wb");
    if (!output) {
      fprintf(stderr, "failed to open %s: %s", argv[1], strerror(errno));
      return 1;
    }
  }
  fwrite(&ehdr, 1, sizeof(ehdr), output);
  fwrite(&phdr_text, 1, sizeof(phdr_text), output);
  fwrite(seg_data, 1, sizeof(seg_data), output);
  fwrite(seg_text, 1, sizeof(seg_text), output);
  fclose(output);
  return 0;
}