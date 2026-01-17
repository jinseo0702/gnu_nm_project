#ifndef DEBUG_H
#define DEBUG_H

#include <elf.h>
#include <sys/stat.h>


#ifdef DEBUG

void print_stat(struct stat *temp);
void print_e_ident(unsigned char e_ident[]);
void print_elf_64(Elf64_Ehdr *elf_64);
void print_elf_32(Elf32_Ehdr *elf_32);
void prints_elf64_shdr(Elf64_Ehdr *elf_64);

# define DBG_STAT(st) print_stat((st))
# define DBG_E_IDENT(st) print_e_ident((st))
# define DBG_ELF64(st) print_elf_64((st))
# define DBG_ELF32(st) print_elf_32((st))
# define DBG_ELF64_SHDR(st) prints_elf64_shdr((st))

#else

# define DBG_STAT(st) do {} while (0)
# define DBG_E_IDENT(st) do {} while (0)
# define DBG_ELF64(st) do {} while (0)
# define DBG_ELF32(st) do {} while (0)
# define DBG_ELF64_SHDR(st) do {} while (0)
#endif


#endif