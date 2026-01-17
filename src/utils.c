#include "../include/debug.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>
#include "../ft_printf/libftprintf.h"

static const char *e_ident_ei_class[] = {
    [ELFCLASSNONE] = "ELFCLASSNONE : This class is invalid.",
    [ELFCLASS32] = "ELFCLASS32 : This defines the 32-bit architecture.  It supports machines with files and virtual address spaces up to 4 Gigabytes.",
    [ELFCLASS64] = "ELFCLASS64 : This defines the 64-bit architecture",
};

static const char *e_ident_ei_data[] = {
    [ELFDATANONE] = "ELFDATANONE : Unknown data format.",
    [ELFDATA2LSB] = "ELFDATA2LSB : Two's complement, little-endian.",
    [ELFDATA2MSB] = "ELFDATA2MSB : Two's complement, big-endian.",
};

static const char *e_ident_ei_version[] = {
    [EV_NONE] = "EV_NONE : Invalid version.",
    [EV_CURRENT] = "EV_CURRENT : Current version.",
};

static const char *e_ident_ei_osabi[] = {
    [ELFOSABI_NONE] = "ELFOSABI_NONE : Same as ELFOSABI_SYSV / UNIX System V ABI",
    [ELFOSABI_HPUX] = "ELFOSABI_HPUX : HP-UX ABI",
    [ELFOSABI_NETBSD] = "ELFOSABI_NETBSD : NetBSD ABI",
    [ELFOSABI_LINUX] = "ELFOSABI_LINUX : Linux ABI",
    [ELFOSABI_SOLARIS] = "ELFOSABI_SOLARIS : Solaris ABI",
    [ELFOSABI_IRIX] = "ELFOSABI_IRIX : IRIX ABI",
    [ELFOSABI_FREEBSD] = "ELFOSABI_FREEBSD : FreeBSD ABI",
    [ELFOSABI_TRU64] = "ELFOSABI_TRU64 : TRU64 UNIX ABI",
    [ELFOSABI_ARM] = "ELFOSABI_ARM : ARM architecture ABI",
    [ELFOSABI_STANDALONE] = "ELFOSABI_STANDALONE : Stand-alone (embedded) ABI",
};

static const char *e_type_str[] = {
    [ET_NONE] = "ET_NONE : An unknown type.",
    [ET_REL] = "ET_REL : A relocatable file.",
    [ET_EXEC] = "ET_EXEC : An executable file.",
    [ET_DYN] = "ET_DYN : A shared object.",
    [ET_CORE] = "ET_CORE : A core file.",
};

static const char *e_machine_str[] = {
    [EM_NONE] = "EM_NONE An unknown machine",
    [EM_M32] = "EM_M32 AT&T WE 32100",
    [EM_SPARC] = "EM_SPARC Sun Microsystems SPARC",
    [EM_386] = "EM_386 Intel 80386",
    [EM_68K] = "EM_68K Motorola 68000",
    [EM_88K] = "EM_88K Motorola 88000",
    [EM_860] = "EM_860 Intel 80860",
    [EM_MIPS] = "EM_MIPS MIPS RS3000 (big-endian only)",
    [EM_PARISC] = "EM_PARISC HP/PA",
    [EM_SPARC32PLUS] = "EM_SPARC32PLUS SPARC with enhanced instruction set",
    [EM_PPC] = "EM_PPC PowerPC",
    [EM_PPC64] = "EM_PPC64 PowerPC 64-bit",
    [EM_S390] = "EM_S390 IBM S/390",
    [EM_ARM] = "EM_ARM Advanced RISC Machines",
    [EM_SH] = "EM_SH Renesas SuperH",
    [EM_SPARCV9] = "EM_SPARCV9 SPARC v9 64-bit",
    [EM_IA_64] = "EM_IA_64 Intel Itanium",
    [EM_X86_64] = "EM_X86_64 AMD x86-64",
    [EM_VAX] = "EM_VAX DEC Vax",
};

static const char *e_version_str[] = {
    [EV_NONE] = "EV_NONE : Invalid version",
    [EV_CURRENT] = "EV_CURRENT : Current version",
};

void print_stat(struct stat *temp) {
    ft_printf("struct stat {\n\
               dev_t     st_dev;     %d         /* ID of device containing file */ \n\
               ino_t     st_ino;     %d        /* Inode number */ \n\
               mode_t    st_mode;    %o        /* File type and mode */ \n\
               nlink_t   st_nlink;   %d       /* Number of hard links */ \n\
               uid_t     st_uid;     %u      /* User ID of owner */ \n\
               gid_t     st_gid;     %u      /* Group ID of owner */ \n\
               dev_t     st_rdev;    %d      /* Device ID (if special file) */ \n\
               off_t     st_size;    %d      /* Total size, in bytes */ \n\
               blksize_t st_blksize; %d     /* Block size for filesystem I/O */ \n\
               blkcnt_t  st_blocks;  %d    /* Number of 512B blocks allocated */ \n\
}\n", temp->st_dev, temp->st_ino, temp->st_mode, temp->st_nlink, temp->st_uid, temp->st_gid, temp->st_rdev, temp->st_size, temp->st_blksize, temp->st_blocks);
}

/*
              EI_MAG0 \n\
              EI_MAG1 \n\
              EI_MAG2 \n\
              EI_MAG3 \n\
              EI_CLASS \n\
              EI_DATA \n\
              EI_VERSION \n\
              EI_OSABI \n\
              EI_ABIVERSION \n\
              EI_PAD \n\
              EI_NIDENT \n\
*/

void print_e_ident(unsigned char e_ident[]){
    ft_printf("\n\nPrint  e_ident[] ----\n\
            EI_MAG0 %X\n\
              EI_MAG1 %c\n\
              EI_MAG2 %c\n\
              EI_MAG3 %c\n\
              EI_CLASS %s\n\
              EI_DATA %s\n\
              EI_VERSION %s\n\
              EI_OSABI %s\n\
              EI_ABIVERSION %d\n\
              EI_PAD %d\n\
              EI_NIDENT %d\n\
        ", e_ident[EI_MAG0], e_ident[EI_MAG1], e_ident[EI_MAG2], e_ident[EI_MAG3]\
        , e_ident_ei_class[e_ident[EI_CLASS]] \
        , e_ident_ei_data[e_ident[EI_DATA]] \
        , e_ident_ei_version[e_ident[EI_VERSION]] \
        , e_ident_ei_osabi[e_ident[EI_OSABI]] \
        , e_ident[EI_ABIVERSION], e_ident[EI_PAD], EI_NIDENT
    );
}

void print_elf_64(Elf64_Ehdr *elf_64) {
 ft_printf("\n\n\
typedef struct { \n\
               unsigned char e_ident[EI_NIDENT]; \n\
               uint16_t      e_type; -> %s \n\
               uint16_t      e_machine; -> %s \n\
               uint32_t      e_version; -> %s \n\
               Elf64_Addr     e_entry; -> %P\n\
               Elf64_Off      e_phoff; -> %P\n\
               Elf64_Off      e_shoff; -> %P\n\
               uint32_t      e_flags; -> %P\n\
               uint16_t      e_ehsize; -> %P\n\
               uint16_t      e_phentsize; -> %P\n\
               uint16_t      e_phnum; -> %P\n\
               uint16_t      e_shentsize; -> %P\n\
               uint16_t      e_shnum; -> %P\n\
               uint16_t      e_shstrndx; -> %P\n\
} Elf64_Ehdr; \n", e_type_str[elf_64->e_type] \
        , e_machine_str[elf_64->e_machine] \
        , e_version_str[elf_64->e_version] \
        , elf_64->e_entry \
        , elf_64->e_phoff \
        , elf_64->e_shoff \
        , elf_64->e_flags \
        , elf_64->e_ehsize \
        , elf_64->e_phentsize \
        , elf_64->e_phnum \
        , elf_64->e_shentsize \
        , elf_64->e_shnum \
        , elf_64->e_shstrndx \
        );
}

void print_elf_32(Elf32_Ehdr *elf_32) {
    ft_printf("\
        typedef struct { \n\
        unsigned char e_ident[EI_NIDENT]; \n\
        uint16_t      e_type; \n\
        uint16_t      e_machine; \n\
        uint32_t      e_version; \n\
        Elf32_Addr     e_entry; \n\
        Elf32_Off      e_phoff; \n\
        Elf32_Off      e_shoff; \n\
        uint32_t      e_flags; \n\
        uint16_t      e_ehsize; \n\
        uint16_t      e_phentsize; \n\
        uint16_t      e_phnum; \n\
        uint16_t      e_shentsize; \n\
        uint16_t      e_shnum; \n\
        uint16_t      e_shstrndx; \n\
    } Elf32_Ehdr; \n");
}

static const char *Elf64_Shdr_sh_type[] = {
    [SHT_NULL] = "SHT_NULL : This value marks the section header as inactive.",
    [SHT_PROGBITS] = "SHT_PROGBITS : This section holds information defined by the program, whose format and meaning are determined solely by the program.",
    [SHT_SYMTAB] = "SHT_SYMTAB : This  section  holds a symbol table.",
    [SHT_STRTAB] = "SHT_STRTAB : This section holds a string table.",
    [SHT_RELA] = "SHT_RELA : This section holds relocation entries with explicit addends, such as type Elf32_Rela for the 32-bit class of object files.",
    [SHT_HASH] = "SHT_HASH : This section holds a symbol hash table.  An object participating in dynamic linking must contain a symbol hash table.",
    [SHT_DYNAMIC] = "SHT_DYNAMIC : This section holds information for dynamic linking.  An object file may have only one dynamic section.",
    [SHT_NOTE] = "SHT_NOTE : This section holds notes (ElfN_Nhdr).",
    [SHT_NOBITS] = "SHT_NOBITS : A section of this type occupies no space in the file but otherwise resembles SHT_PROGBITS.",
    [SHT_REL] = "SHT_REL : This section holds relocation offsets without explicit addends, such as type Elf32_Rel for the 32-bit class of object files.",
    [SHT_SHLIB] = "SHT_SHLIB : This section is reserved but has unspecified semantics.",
    [SHT_DYNSYM] = "SHT_DYNSYM : This section holds a minimal set of dynamic linking symbols.",
    // [SHT_LOPROC] = "SHT_LOPROC : Values in the inclusive range [SHT_LOPROC, SHT_HIPROC] are reserved for processor-specific semantics.",
    // [SHT_LOUSER] = "SHT_LOUSER : This value specifies the lower bound of the range of indices reserved for application programs.",
    // [SHT_HIUSER] = "SHT_HIUSER : This value specifies the upper bound of the range of indices reserved for application programs.",
};

static const char *Elf64_Shdr_sh_flags[] = {
    [SHF_WRITE] = "SHF_WRITE : This section contains data that should be writable during process execution.",
    [SHF_ALLOC] = "SHF_ALLOC : This section occupies memory during process execution.  Some control sections do not reside in the memory image of an object file.  This attribute is off for those sections.",
    [SHF_EXECINSTR] = "SHF_EXECINSTR : This section contains executable machine instructions.",
};

/*
           typedef struct { \n\
               uint32_t   sh_name; \n\
               uint32_t   sh_type; \n\
               uint64_t   sh_flags; \n\
               Elf64_Addr sh_addr; \n\
               Elf64_Off  sh_offset; \n\
               uint64_t   sh_size; \n\
               uint32_t   sh_link; \n\
               uint32_t   sh_info; \n\
               uint64_t   sh_addralign; \n\
               uint64_t   sh_entsize; \n\
           } Elf64_Shdr; \n\
*/

static void print_elf64_shdr(Elf64_Shdr *shdr_64) {
    const char *ptr;
    if (shdr_64->sh_type < 12) {
        ptr = Elf64_Shdr_sh_type[shdr_64->sh_type];
    }
    else {
        ptr = "Out of range";
    }
    ft_printf(" \n\
typedef struct { \n\
               uint32_t   sh_name; -> %P \n\
               uint32_t   sh_type; -> %s \n\
               uint64_t   sh_flags; -> %P \n\
               Elf64_Addr sh_addr; -> %P \n\
               Elf64_Off  sh_offset; -> %P \n\
               uint64_t   sh_size; -> %P \n\
               uint32_t   sh_link; -> %P \n\
               uint32_t   sh_info; -> %P \n\
               uint64_t   sh_addralign; -> %P \n\
               uint64_t   sh_entsize; -> %P \n\
           } Elf64_Shdr; \n\
", shdr_64->sh_name \
, ptr \
, Elf64_Shdr_sh_flags[shdr_64->sh_flags] \
, shdr_64->sh_addr \
, shdr_64->sh_offset \
, shdr_64->sh_size \
, shdr_64->sh_link \
, shdr_64->sh_info \
, shdr_64->sh_addralign \
, shdr_64->sh_entsize \
);
}

void prints_elf64_shdr(Elf64_Ehdr *elf_64) {
    Elf64_Shdr *shdr_64 =  (Elf64_Shdr *)((unsigned char *)elf_64 + elf_64->e_shoff);
    for (int i = 0; i < elf_64->e_shnum; i++) {
        print_elf64_shdr(&shdr_64[i]);
    }
}