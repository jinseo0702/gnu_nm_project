#ifndef NM_H
#define NM_H

#include <ar.h>
#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../libft/libft.h"
#include "../ft_printf/libftprintf.h"

typedef enum e_nm_option {
	OPT_a = (1 << 0),
	OPT_g = (1 << 1),
	OPT_u = (1 << 2),
	OPT_r = (1 << 3),
	OPT_P = (1 << 4),
	OPT_n = (1 << 5),
} t_nm_option;

typedef struct s_unit {
	const unsigned char *base;
	uint64_t limit;
	const char *display_name;
} t_unit;

typedef struct s_MetaData {
	union {
		const Elf32_Ehdr *Ehdr32;
		const Elf64_Ehdr *Ehdr64;
	} ElfN_Ehdr;
	union {
		const Elf32_Shdr *Shdr32;
		const Elf64_Shdr *Shdr64;
	} ElfN_Shdr;
	union {
		const Elf32_Sym *Sym32;
		const Elf64_Sym *Sym64;
	} ElfN_Sym;
	uint64_t strtab_offset;
	uint64_t strtab_size;
	uint64_t sym_count;
	uint8_t elf_class;
} t_MetaData;

typedef struct s_NmShdrData {
	uint64_t sh_type;
	uint64_t sh_flags;
} t_NmShdrData;

typedef struct s_NmSymData {
	uint64_t st_value;
	uint64_t st_size;
	uint32_t st_name;
	uint16_t st_shndx;
	uint8_t st_info_type;
	uint8_t st_info_bind;
	unsigned char type;
	const char *name;
	uint8_t elf_class;
} t_NmSymData;

typedef enum e_result {
	OK = 0,
	SKIP_UNIT = 1,
	FAIL_PATH = 2,
	FATAL = 3
} t_result;

typedef enum e_format {
	FMT_UNKNOWN = 0,
	FMT_ELF = 1,
	FMT_AR = 2
} t_format;

#define ERROR_LIST \
	X(ERR_MALLOC, "ERR_MALLOC") \
	X(ERR_OPEN, "ERR_OPEN") \
	X(ERR_FSTAT, "ERR_FSTAT") \
	X(ERR_MMAP, "ERR_MMAP") \
	X(ERR_FORMAT, "ERR_FORMAT") \
	X(ERR_ELF_MAGIC, "ERR_ELF_MAGIC") \
	X(ERR_ELF_CLASS, "ERR_ELF_CLASS") \
	X(ERR_ELF_DATA, "ERR_ELF_DATA") \
	X(ERR_ELF_MACHINE, "ERR_ELF_MACHINE") \
	X(ERR_ELF_TYPE, "ERR_ELF_TYPE") \
	X(ERR_NO_SYMBOLS, "ERR_NO_SYMBOLS") \
	X(ERR_AR_MAGIC, "ERR_AR_MAGIC") \
	X(ERR_AR_FMAG, "ERR_AR_FMAG")

typedef enum e_error {
	#define X(id, str) id,
	ERROR_LIST
	#undef X
	ERROR_END
} t_error;

static inline const void *MOVE_ADDRESS(const void *base, uint64_t offset)
{
	if (base == NULL)
		return NULL;
	const unsigned char *p = (const unsigned char *)base;
	return (const void *)(p + offset);
}

static inline uint8_t CHECK_RANGE(uint64_t offset, uint64_t size, uint64_t limit)
{
	if (offset > limit)
		return 0;
	return (size <= (limit - offset));
}

#define HASOPT(flag, option) (((flag) & (option)) != 0)
#define SETOPT(flag, option) ((flag) |= (option))

const char *get_error_msg(t_error err);
void print_error(t_error err, const char *path);

int parse_arguments(int argc, char **argv, uint32_t *opts, char ***paths, int *path_count);
int process_path(const char *path, uint32_t opt, int multiple_files);
t_format detect_format(const t_unit *unit);
int process_elf_unit(const t_unit *unit, uint32_t opt, int multiple_files);
int process_ar_archive(const t_unit *unit, uint32_t opt);
unsigned char classify_symbol(const t_unit *unit, const t_MetaData *meta, const t_NmSymData *sym, const t_NmShdrData *shdr_cache);
int sort_and_print_symbols(t_NmSymData *symbols, uint64_t count, uint32_t opt, const char *filename, int multiple_files);
const char *safe_get_string(const t_unit *unit, uint64_t strtab_offset, uint64_t strtab_size, uint32_t str_offset);

#ifdef DEBUG
#define NM_LOG(str, str2) real_print_error(str, str2, __FILE__, __LINE__)
#else
#define NM_LOG(str, str2) real_print_error(str, str2, NULL, 0)
#endif

static inline void real_print_error(const char *str, const char *str2, const char *file, int line){
	ft_fprintf(2, "ft_nm: '%s': %s\n", str ,str2);
	if(file != NULL) ft_fprintf(2, "file : %s line : %d\n", file, line);
}

#endif
