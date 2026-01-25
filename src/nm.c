#include "../include/nm.h"
#include <stdint.h>

const char *Error_table[] = {
#define X(id, str) [id] = str,
	ERROR_LIST
#undef X
};

typedef struct s_symbol_section {
	uint64_t sym_offset;
	uint64_t sym_count;
	uint64_t sym_entsize;
	uint64_t strtab_offset;
	uint64_t strtab_size;
} t_symbol_section;

static uint8_t prepare_metadata(t_MetaData *meta);
static int handle_symbols(t_MetaData *meta, uint32_t optionFlags);
static int locate_symbol_section(t_MetaData *meta, t_symbol_section *section);
static int scan_symbol_type(t_MetaData *meta, uint64_t target, t_symbol_section *section);
static void classify_symbol(const t_MetaData *meta, t_NmSymData *symbol);
static void get_section_properties(const t_MetaData *meta, size_t index, uint64_t *type, uint64_t *flags);
static size_t build_visible_list(t_NmSymData *symbols, size_t symbolCount, t_NmSymData **visibleList, uint32_t optionFlags);
static void sort_symbol_pointers(t_NmSymData **list, size_t start, size_t end, uint32_t optionFlags);
static int compare_symbols(const t_NmSymData *lhs, const t_NmSymData *rhs, uint32_t optionFlags);
static int compare_names(const char *lhs, const char *rhs);
static uint8_t symbol_visible(uint32_t optionFlags, unsigned char symbolType);
static void print_symbol(const t_NmSymData *symbol, uint32_t optionFlags);

int main(int argc, char **argv)
{
	t_path_list pathList = {0};
	uint32_t optionFlags = 0;
	int status = EXIT_SUCCESS;
	if (parse_arguments(argc, argv, &optionFlags, &pathList) != 0)
		status = EXIT_FAILURE;
	else
		process_paths(&pathList, optionFlags);
	release_path_list(&pathList);
	return status;
}

int parse_arguments(int argc, char **argv, uint32_t *optionFlags, t_path_list *pathList)
{
	char **storage = ft_calloc((size_t)argc, sizeof(char *));
	if (storage == NULL) {
		NM_LOG(ERR_MALLOC);
		return -1;
	}
	pathList->entries = storage;
	pathList->count = 0;
	for (int index = 1; index < argc; ++index) {
		char *argument = argv[index];
		if (argument[0] == '-' && argument[1] != '\0') {
			for (int cursor = 1; argument[cursor] != '\0'; ++cursor) {
				char option = argument[cursor];
				if (option == 'a')
					SETOPT(*optionFlags, OPT_a);
				else if (option == 'g')
					SETOPT(*optionFlags, OPT_g);
				else if (option == 'u')
					SETOPT(*optionFlags, OPT_u);
				else if (option == 'r')
					SETOPT(*optionFlags, OPT_r);
				else if (option == 'P')
					SETOPT(*optionFlags, OPT_P);
				else if (option == 'n')
					SETOPT(*optionFlags, OPT_n);
				else {
					errno = EINVAL;
					NM_LOG(ERR_UNKNOWN_OPTION);
					release_path_list(pathList);
					return -1;
				}
			}
		} else {
			char *duplicate = ft_strdup(argument);
			if (duplicate == NULL) {
				NM_LOG(ERR_MALLOC);
				release_path_list(pathList);
				return -1;
			}
			pathList->entries[pathList->count++] = duplicate;
		}
	}
	if (pathList->count == 0) {
		char *duplicate = ft_strdup("a.out");
		if (duplicate == NULL) {
			NM_LOG(ERR_MALLOC);
			release_path_list(pathList);
			return -1;
		}
		pathList->entries[pathList->count++] = duplicate;
	}
	return 0;
}

void release_path_list(t_path_list *pathList)
{
	if (pathList->entries == NULL)
		return;
	for (size_t index = 0; index < pathList->count; ++index)
		free(pathList->entries[index]);
	free(pathList->entries);
	pathList->entries = NULL;
	pathList->count = 0;
}

int process_paths(const t_path_list *pathList, uint32_t optionFlags)
{
	if (pathList == NULL)
		return -1;
	for (size_t index = 0; index < pathList->count; ++index) {
		if (pathList->count > 1)
			ft_printf("%s:\n", pathList->entries[index]);
		handle_path(pathList->entries[index], optionFlags);
	}
	return 0;
}

int handle_path(const char *path, uint32_t optionFlags)
{
	struct stat statTemp;
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		NM_LOG(ERR_OPEN);
		return -1;
	}
	if (fstat(fd, &statTemp) < 0) {
		NM_LOG(ERR_FSTAT);
		close(fd);
		return -1;
	}
	if (!S_ISREG(statTemp.st_mode)) {
		close(fd);
		return -1;
	}
	if ((uint64_t)statTemp.st_size < sizeof(Elf32_Ehdr)) {
		close(fd);
		return -1;
	}
	const unsigned char *mapping = mmap(NULL, (size_t)statTemp.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (mapping == MAP_FAILED) {
		NM_LOG(ERR_MMAP);
		close(fd);
		return -1;
	}
	t_MetaData meta;
	ft_bzero(&meta, sizeof(meta));
	meta.base = mapping;
	meta.file_limit = (uint64_t)statTemp.st_size;
	if (!prepare_metadata(&meta)) {
		munmap((void *)mapping, (size_t)statTemp.st_size);
		close(fd);
		return -1;
	}
	handle_symbols(&meta, optionFlags);
	munmap((void *)mapping, (size_t)statTemp.st_size);
	close(fd);
	return 0;
}

static uint8_t prepare_metadata(t_MetaData *meta)
{
	if (!CHECK_RANGE(0, EI_NIDENT, meta->file_limit)) {
		errno = EINVAL;
		NM_LOG(ERR_FORMAT);
		return 0;
	}
	const unsigned char *ident = meta->base;
	if (ft_memcmp(ident, ELFMAG, SELFMAG) != 0) {
		errno = EINVAL;
		NM_LOG(ERR_FORMAT);
		return 0;
	}
	if (ident[EI_DATA] != ELFDATA2LSB) {
		errno = EINVAL;
		NM_LOG(ERR_FORMAT);
		return 0;
	}
	uint8_t elfClass = ident[EI_CLASS];
	if (elfClass != ELFCLASS32 && elfClass != ELFCLASS64) {
		errno = EINVAL;
		NM_LOG(ERR_FORMAT);
		return 0;
	}
	meta->elf_class = elfClass;
	if (elfClass == ELFCLASS32) {
		if (!CHECK_RANGE(0, sizeof(Elf32_Ehdr), meta->file_limit)) {
			errno = EINVAL;
			NM_LOG(ERR_FORMAT);
			return 0;
		}
		const Elf32_Ehdr *ehdr = (const Elf32_Ehdr *)meta->base;
		meta->ElfN_Ehdr.Ehdr32 = ehdr;
		if (ehdr->e_shoff == 0 || ehdr->e_shnum == 0) {
			errno = EINVAL;
			NM_LOG(ERR_FORMAT);
			return 0;
		}
		if (ehdr->e_machine != EM_386 && ehdr->e_machine != EM_X86_64) {
			errno = EINVAL;
			NM_LOG(ERR_FORMAT);
			return 0;
		}
		if (ehdr->e_type != ET_REL && ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN) {
			errno = EINVAL;
			NM_LOG(ERR_FORMAT);
			return 0;
		}
		uint64_t span = (uint64_t)ehdr->e_shentsize * ehdr->e_shnum;
		if (!CHECK_RANGE(ehdr->e_shoff, span, meta->file_limit)) {
			errno = EINVAL;
			NM_LOG(ERR_FORMAT);
			return 0;
		}
		if (ehdr->e_shentsize < sizeof(Elf32_Shdr)) {
			errno = EINVAL;
			NM_LOG(ERR_FORMAT);
			return 0;
		}
		meta->sectionBase = (const unsigned char *)MOVE_ADDRESS(meta->base, ehdr->e_shoff);
		meta->sectionEntrySize = ehdr->e_shentsize;
		meta->sectionCount = ehdr->e_shnum;
		meta->ElfN_Shdr.Shdr32 = (const Elf32_Shdr *)meta->sectionBase;
	} else {
		if (!CHECK_RANGE(0, sizeof(Elf64_Ehdr), meta->file_limit)) {
			errno = EINVAL;
			NM_LOG(ERR_FORMAT);
			return 0;
		}
		const Elf64_Ehdr *ehdr = (const Elf64_Ehdr *)meta->base;
		meta->ElfN_Ehdr.Ehdr64 = ehdr;
		if (ehdr->e_shoff == 0 || ehdr->e_shnum == 0) {
			errno = EINVAL;
			NM_LOG(ERR_FORMAT);
			return 0;
		}
		if (ehdr->e_machine != EM_386 && ehdr->e_machine != EM_X86_64) {
			errno = EINVAL;
			NM_LOG(ERR_FORMAT);
			return 0;
		}
		if (ehdr->e_type != ET_REL && ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN) {
			errno = EINVAL;
			NM_LOG(ERR_FORMAT);
			return 0;
		}
		uint64_t span = (uint64_t)ehdr->e_shentsize * ehdr->e_shnum;
		if (!CHECK_RANGE(ehdr->e_shoff, span, meta->file_limit)) {
			errno = EINVAL;
			NM_LOG(ERR_FORMAT);
			return 0;
		}
		if (ehdr->e_shentsize < sizeof(Elf64_Shdr)) {
			errno = EINVAL;
			NM_LOG(ERR_FORMAT);
			return 0;
		}
		meta->sectionBase = (const unsigned char *)MOVE_ADDRESS(meta->base, ehdr->e_shoff);
		meta->sectionEntrySize = ehdr->e_shentsize;
		meta->sectionCount = ehdr->e_shnum;
		meta->ElfN_Shdr.Shdr64 = (const Elf64_Shdr *)meta->sectionBase;
	}
	return 1;
}

static int handle_symbols(t_MetaData *meta, uint32_t optionFlags)
{
	t_symbol_section section = {0};
	if (locate_symbol_section(meta, &section) != 0)
		return -1;
	if (section.sym_count == 0)
		return -1;
	t_NmSymData *symbolBuffer = ft_calloc((size_t)section.sym_count, sizeof(t_NmSymData));
	if (symbolBuffer == NULL) {
		NM_LOG(ERR_MALLOC);
		return -1;
	}
	t_NmSymData **visibleList = ft_calloc((size_t)section.sym_count, sizeof(t_NmSymData *));
	if (visibleList == NULL) {
		NM_LOG(ERR_MALLOC);
		free(symbolBuffer);
		return -1;
	}
	const unsigned char *strtabBase = (const unsigned char *)MOVE_ADDRESS(meta->base, section.strtab_offset);
	for (uint64_t index = 0; index < section.sym_count; ++index) {
		uint64_t entryOffset = section.sym_offset + index * section.sym_entsize;
		if (!CHECK_RANGE(entryOffset, section.sym_entsize, meta->file_limit))
			continue;
		const unsigned char *entryBase = (const unsigned char *)MOVE_ADDRESS(meta->base, entryOffset);
		if (entryBase == NULL)
			continue;
		t_NmSymData *symbol = &symbolBuffer[index];
		if (meta->elf_class == ELFCLASS32) {
			const Elf32_Sym *raw = (const Elf32_Sym *)entryBase;
			symbol->st_value = raw->st_value;
			symbol->st_size = raw->st_size;
			symbol->st_name = raw->st_name;
			symbol->st_shndx = raw->st_shndx;
			symbol->st_info_type = ELF32_ST_TYPE(raw->st_info);
			symbol->st_info_bind = ELF32_ST_BIND(raw->st_info);
		} else {
			const Elf64_Sym *raw = (const Elf64_Sym *)entryBase;
			symbol->st_value = raw->st_value;
			symbol->st_size = raw->st_size;
			symbol->st_name = raw->st_name;
			symbol->st_shndx = raw->st_shndx;
			symbol->st_info_type = ELF64_ST_TYPE(raw->st_info);
			symbol->st_info_bind = ELF64_ST_BIND(raw->st_info);
		}
		symbol->name = "";
		if (symbol->st_name < section.strtab_size) {
			const char *candidate = (const char *)(strtabBase + symbol->st_name);
			if (ft_memchr(candidate, '\0', section.strtab_size - symbol->st_name) != NULL)
				symbol->name = candidate;
		}
		classify_symbol(meta, symbol);
	}
	size_t visibleCount = build_visible_list(symbolBuffer, (size_t)section.sym_count, visibleList, optionFlags);
	if (visibleCount > 1)
		sort_symbol_pointers(visibleList, 0, visibleCount, optionFlags);
	for (size_t index = 0; index < visibleCount; ++index)
		print_symbol(visibleList[index], optionFlags);
	free(symbolBuffer);
	free(visibleList);
	return 0;
}

static int locate_symbol_section(t_MetaData *meta, t_symbol_section *section)
{
	if (scan_symbol_type(meta, SHT_SYMTAB, section) == 0)
		return 0;
	if (scan_symbol_type(meta, SHT_DYNSYM, section) == 0)
		return 0;
	errno = ENOENT;
	NM_LOG(ERR_SECTION);
	return -1;
}

static int scan_symbol_type(t_MetaData *meta, uint64_t target, t_symbol_section *section)
{
	for (size_t index = 1; index < meta->sectionCount; ++index) {
		const unsigned char *entry = meta->sectionBase + index * meta->sectionEntrySize;
		uint64_t sh_type;
		uint64_t sh_offset;
		uint64_t sh_size;
		uint64_t sh_link;
		uint64_t sh_entsize;
		if (meta->elf_class == ELFCLASS32) {
			const Elf32_Shdr *shdr = (const Elf32_Shdr *)entry;
			sh_type = shdr->sh_type;
			sh_offset = shdr->sh_offset;
			sh_size = shdr->sh_size;
			sh_link = shdr->sh_link;
			sh_entsize = shdr->sh_entsize;
		} else {
			const Elf64_Shdr *shdr = (const Elf64_Shdr *)entry;
			sh_type = shdr->sh_type;
			sh_offset = shdr->sh_offset;
			sh_size = shdr->sh_size;
			sh_link = shdr->sh_link;
			sh_entsize = shdr->sh_entsize;
		}
		if (sh_type != target)
			continue;
		if (sh_link >= meta->sectionCount)
			continue;
		if (!CHECK_RANGE(sh_offset, sh_size, meta->file_limit))
			continue;
		uint64_t entrySize = sh_entsize;
		if (entrySize == 0)
			entrySize = (meta->elf_class == ELFCLASS32) ? sizeof(Elf32_Sym) : sizeof(Elf64_Sym);
		if (entrySize == 0 || sh_size < entrySize)
			continue;
		uint64_t count = sh_size / entrySize;
		if (count == 0)
			continue;
		const unsigned char *linkEntry = meta->sectionBase + sh_link * meta->sectionEntrySize;
		uint64_t strtab_offset;
		uint64_t strtab_size;
		if (meta->elf_class == ELFCLASS32) {
			const Elf32_Shdr *link = (const Elf32_Shdr *)linkEntry;
			strtab_offset = link->sh_offset;
			strtab_size = link->sh_size;
		} else {
			const Elf64_Shdr *link = (const Elf64_Shdr *)linkEntry;
			strtab_offset = link->sh_offset;
			strtab_size = link->sh_size;
		}
		if (!CHECK_RANGE(strtab_offset, strtab_size, meta->file_limit))
			continue;
		if (strtab_size == 0)
			continue;
		section->sym_offset = sh_offset;
		section->sym_count = count;
		section->sym_entsize = entrySize;
		section->strtab_offset = strtab_offset;
		section->strtab_size = strtab_size;
		if (meta->elf_class == ELFCLASS32)
			meta->ElfN_Sym.Sym32 = (const Elf32_Sym *)MOVE_ADDRESS(meta->base, sh_offset);
		else
			meta->ElfN_Sym.Sym64 = (const Elf64_Sym *)MOVE_ADDRESS(meta->base, sh_offset);
		return 0;
	}
	return -1;
}

static void classify_symbol(const t_MetaData *meta, t_NmSymData *symbol)
{
	uint16_t shndx = symbol->st_shndx;
	unsigned char bind = symbol->st_info_bind;
	unsigned char type = symbol->st_info_type;
	if (shndx == SHN_UNDEF) {
		if (bind == STB_WEAK) {
			if (type == STT_OBJECT)
				symbol->type = 'v';
			else
				symbol->type = 'w';
			return;
		}
		symbol->type = 'U';
		return;
	}
	if (bind == STB_WEAK) {
		if (type == STT_OBJECT)
			symbol->type = 'V';
		else
			symbol->type = 'W';
		return;
	}
	if (bind == STB_GNU_UNIQUE) {
		symbol->type = 'u';
		return;
	}
	if (shndx == SHN_ABS) {
		symbol->type = (bind == STB_LOCAL) ? 'a' : 'A';
		return;
	}
	if (shndx == SHN_COMMON) {
		symbol->type = 'C';
		return;
	}
	if (type == STT_GNU_IFUNC) {
		symbol->type = 'i';
		return;
	}
#ifdef SHT_X86_64_UNWIND
	if (meta->elf_class == ELFCLASS64) {
		uint64_t sectionType = 0;
		uint64_t sectionFlags = 0;
		if (shndx < meta->sectionCount) {
			get_section_properties(meta, shndx, &sectionType, &sectionFlags);
			if (sectionType == SHT_X86_64_UNWIND) {
				symbol->type = 'p';
				return;
			}
		}
	}
#endif
	uint64_t sectionType = 0;
	uint64_t sectionFlags = 0;
	get_section_properties(meta, shndx, &sectionType, &sectionFlags);
	if (sectionType == SHT_NOBITS && (sectionFlags & SHF_ALLOC) && (sectionFlags & SHF_WRITE)) {
		symbol->type = (bind == STB_LOCAL) ? 'b' : 'B';
		return;
	}
	if (sectionType == SHT_PROGBITS && (sectionFlags & SHF_ALLOC) && (sectionFlags & SHF_WRITE)) {
		symbol->type = (bind == STB_LOCAL) ? 'd' : 'D';
		return;
	}
	if (sectionType == SHT_PROGBITS && (sectionFlags & SHF_ALLOC) && (sectionFlags & SHF_EXECINSTR)) {
		symbol->type = (bind == STB_LOCAL) ? 't' : 'T';
		return;
	}
	if (sectionType == SHT_PROGBITS && (sectionFlags & SHF_ALLOC) && !(sectionFlags & SHF_WRITE)) {
		symbol->type = (bind == STB_LOCAL) ? 'r' : 'R';
		return;
	}
	if (sectionType == SHT_PROGBITS) {
		if (sectionFlags == 0) {
			symbol->type = 'N';
			return;
		}
		if ((sectionFlags & SHF_WRITE) == 0) {
			symbol->type = 'n';
			return;
		}
	}
	symbol->type = '?';
}

static void get_section_properties(const t_MetaData *meta, size_t index, uint64_t *type, uint64_t *flags)
{
	*type = 0;
	*flags = 0;
	if (index >= meta->sectionCount)
		return;
	const unsigned char *entry = meta->sectionBase + index * meta->sectionEntrySize;
	if (meta->elf_class == ELFCLASS32) {
		const Elf32_Shdr *shdr = (const Elf32_Shdr *)entry;
		*type = shdr->sh_type;
		*flags = shdr->sh_flags;
	} else {
		const Elf64_Shdr *shdr = (const Elf64_Shdr *)entry;
		*type = shdr->sh_type;
		*flags = shdr->sh_flags;
	}
}

static size_t build_visible_list(t_NmSymData *symbols, size_t symbolCount, t_NmSymData **visibleList, uint32_t optionFlags)
{
	size_t count = 0;
	for (size_t index = 0; index < symbolCount; ++index) {
		if (symbol_visible(optionFlags, symbols[index].type))
			visibleList[count++] = &symbols[index];
	}
	return count;
}

static void sort_symbol_pointers(t_NmSymData **list, size_t start, size_t end, uint32_t optionFlags)
{
	if (end <= start + 1)
		return;
	size_t left = start;
	size_t right = end - 1;
	t_NmSymData *pivot = list[start + (end - start) / 2];
	while (left <= right) {
		while (compare_symbols(list[left], pivot, optionFlags) < 0)
			++left;
		while (compare_symbols(list[right], pivot, optionFlags) > 0)
			--right;
		if (left <= right) {
			t_NmSymData *temp = list[left];
			list[left] = list[right];
			list[right] = temp;
			++left;
			if (right == 0)
				break;
			--right;
		}
	}
	if (start < right + 1)
		sort_symbol_pointers(list, start, right + 1, optionFlags);
	if (left < end)
		sort_symbol_pointers(list, left, end, optionFlags);
}

static int compare_symbols(const t_NmSymData *lhs, const t_NmSymData *rhs, uint32_t optionFlags)
{
	int order = 0;
	if (HASOPT(optionFlags, OPT_n)) {
		if (lhs->st_value < rhs->st_value)
			order = -1;
		else if (lhs->st_value > rhs->st_value)
			order = 1;
		else
			order = compare_names(lhs->name, rhs->name);
	} else {
		order = compare_names(lhs->name, rhs->name);
	}
	if (HASOPT(optionFlags, OPT_r))
		order = -order;
	return order;
}

static int compare_names(const char *lhs, const char *rhs)
{
	const char *left = lhs ? lhs : "";
	const char *right = rhs ? rhs : "";
	size_t len = ft_strlen(left) > ft_strlen(right) ? ft_strlen(left) : ft_strlen(right);
	return ft_strncmp(left, right, len);
}

static uint8_t symbol_visible(uint32_t optionFlags, unsigned char symbolType)
{
	if (HASOPT(optionFlags, OPT_u))
		return (symbolType == 'U' || symbolType == 'w');
	if (HASOPT(optionFlags, OPT_g))
		return (symbolType == 'A' || symbolType == 'B' || symbolType == 'D' || symbolType == 'R' || symbolType == 'T' || symbolType == 'W' || symbolType == 'w' || symbolType == 'U');
	if (HASOPT(optionFlags, OPT_a))
		return 1;
	return (symbolType != 'a' && symbolType != 'N');
}

static void make_padding_zero(uint64_t value, char *buf) {
	if (value == 0) {
		ft_memset(buf, ' ', 16);
		return ;
	}
	int i = 15;
	const char *base = "0123456789abcdef";

	while (i >= 0) {
		buf[i] = base[value & 0xF]; // 마지막 4비트 추출
		value >>= 4;                // 4비트 오른쪽으로 밀기
		i--;
	}
	buf[16] = '\0';
}

static void print_symbol(const t_NmSymData *symbol, uint32_t optionFlags)
{
	char displayType = symbol->type ? symbol->type : '?';
	const char *displayName = symbol->name ? symbol->name : "";
	if (HASOPT(optionFlags, OPT_P))
		ft_printf("%s %c %x %x\n", displayName, displayType, (unsigned long long)symbol->st_value, (unsigned long long)symbol->st_size);
	else{
		char buf[17] = {0,};
		make_padding_zero(symbol->st_value, buf);
		ft_printf("%s %c %s\n", buf, displayType, displayName);
	}
}
