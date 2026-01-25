#ifndef NM_H
#define NM_H

#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ar.h>

#include "../ft_printf/libftprintf.h"
#include "../libft/libft.h"

#define HASOPT(flag, option) (((flag) & (option)) != 0)
#define SETOPT(flag, option) ((flag) |= (option))

static inline const void *MOVE_ADDRESS(const void *base, uint64_t offset)
{
	if (base == NULL)
		return NULL;
	const unsigned char *pointer = (const unsigned char *)base;
	return (const void *)(pointer + offset);
}

static inline uint8_t CHECK_RANGE(uint64_t offset, uint64_t size, uint64_t limit)
{
	if (offset > limit)
		return 0;
	return size <= limit - offset;
}

#define ERROR_LIST \
	X(ERR_MALLOC, "ERR_MALLOC") \
	X(ERR_OPEN, "ERR_OPEN") \
	X(ERR_FSTAT, "ERR_FSTAT") \
	X(ERR_MMAP, "ERR_MMAP") \
	X(ERR_FORMAT, "ERR_FORMAT") \
	X(ERR_SECTION, "ERR_SECTION") \
	X(ERR_SYMBOL, "ERR_SYMBOL") \
	X(ERR_UNKNOWN_OPTION, "ERR_UNKNOWN_OPTION")

typedef enum e_error {
#define X(id, str) id,
	ERROR_LIST
#undef X
	ERROR_END
} t_error;

extern const char *Error_table[];

static inline const char *get_error_msg(t_error err)
{
	if (err < ERROR_END)
		return Error_table[err];
	return "ERR_UNKNOWN";
}

static inline void real_print_error(t_error err, const char *file, int line)
{
	ft_fprintf(2, "nm [%s] -> %s\n", get_error_msg(err), strerror(errno));
	if (file != NULL)
		ft_fprintf(2, "file : %s line : %d\n", file, line);
}

#ifdef DEBUG
#define NM_LOG(err) real_print_error(err, __FILE__, __LINE__)
#else
#define NM_LOG(err) real_print_error(err, NULL, 0)
#endif

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
	const unsigned char *base;
	const unsigned char *sectionBase;
	uint64_t file_limit;
	uint64_t sectionCount;
	uint64_t sectionEntrySize;
	uint64_t strtab_offset;
	uint64_t strtab_size;
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
} t_NmSymData;

typedef enum e_nm_option {
	OPT_a = (1 << 0),
	OPT_g = (1 << 1),
	OPT_u = (1 << 2),
	OPT_r = (1 << 3),
	OPT_P = (1 << 4),
	OPT_n = (1 << 5),
} t_nm_option;

typedef struct s_path_list {
	char **entries;
	size_t count;
} t_path_list;

int parse_arguments(int argc, char **argv, uint32_t *optionFlags, t_path_list *pathList);
void release_path_list(t_path_list *pathList);
int process_paths(const t_path_list *pathList, uint32_t optionFlags);
int handle_path(const char *path, uint32_t optionFlags);

#endif
