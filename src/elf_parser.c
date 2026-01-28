#include "../include/nm.h"
#include <elf.h>
#include <stdint.h>

const char *safe_get_string(const t_unit *unit, uint64_t strtab_offset, uint64_t strtab_size, uint32_t str_offset)
{
	const char *strtab;
	uint64_t remaining;

	if (str_offset >= strtab_size)
		return NULL;
	if (!CHECK_RANGE(strtab_offset + str_offset, 1, unit->limit))
		return NULL;
	strtab = (const char *)MOVE_ADDRESS(unit->base, strtab_offset);
	remaining = strtab_size - str_offset;
	if (!ft_memchr(strtab + str_offset, '\0', remaining))
		return NULL;
	return strtab + str_offset;
}

static uint64_t check_debug_symbol(t_MetaData *meta)
{
	uint16_t idx;
	uint64_t offset;
	if (meta->elf_class == ELFCLASS64)
	{
		idx = meta->ElfN_Ehdr.Ehdr64->e_shstrndx;
		offset = meta->ElfN_Shdr.Shdr64[idx].sh_offset;
	}
	else
	{
		idx = meta->ElfN_Ehdr.Ehdr32->e_shstrndx;
		offset = meta->ElfN_Shdr.Shdr32[idx].sh_offset;
	}
	return offset;
}

static uint64_t check_debug_section_size(t_MetaData *meta)
{
	uint16_t idx;
	uint64_t sh_size;
	if (meta->elf_class == ELFCLASS64)
	{
		idx = meta->ElfN_Ehdr.Ehdr64->e_shstrndx;
		sh_size = meta->ElfN_Shdr.Shdr64[idx].sh_size;
	}
	else
	{
		idx = meta->ElfN_Ehdr.Ehdr32->e_shstrndx;
		sh_size = meta->ElfN_Shdr.Shdr32[idx].sh_size;
	}
	return sh_size;
}

static uint32_t check_section_name(t_MetaData *meta, uint16_t st_shndx)
{
	uint32_t sh_name;
	if (meta->elf_class == ELFCLASS64) sh_name = meta->ElfN_Shdr.Shdr64[st_shndx].sh_name;
	else sh_name = meta->ElfN_Shdr.Shdr64[st_shndx].sh_name;
	return sh_name;
}

static int verify_elf_header(const t_unit *unit, t_MetaData *meta)
{
	const Elf64_Ehdr *ehdr64;
	const Elf32_Ehdr *ehdr32;

	if (!CHECK_RANGE(0, sizeof(Elf64_Ehdr), unit->limit))
		return FAIL_PATH;
	ehdr64 = (const Elf64_Ehdr *)unit->base;
	if (ehdr64->e_ident[EI_MAG0] != ELFMAG0 || ehdr64->e_ident[EI_MAG1] != ELFMAG1 ||
		ehdr64->e_ident[EI_MAG2] != ELFMAG2 || ehdr64->e_ident[EI_MAG3] != ELFMAG3)
		return FAIL_PATH;
	if (ehdr64->e_ident[EI_DATA] != ELFDATA2LSB)
		return FAIL_PATH;
	if (ehdr64->e_ident[EI_CLASS] != ELFCLASS32 && ehdr64->e_ident[EI_CLASS] != ELFCLASS64)
		return FAIL_PATH;
	meta->elf_class = ehdr64->e_ident[EI_CLASS];
	if (meta->elf_class == ELFCLASS64)
	{
		meta->ElfN_Ehdr.Ehdr64 = ehdr64;
		if (ehdr64->e_type != ET_REL && ehdr64->e_type != ET_EXEC && ehdr64->e_type != ET_DYN)
			return FAIL_PATH;
		if (ehdr64->e_machine != EM_X86_64 && ehdr64->e_machine != EM_386)
			return FAIL_PATH;
		if (ehdr64->e_shoff == 0 || ehdr64->e_shnum == 0)
			return FAIL_PATH;
		if (!CHECK_RANGE(ehdr64->e_shoff, ehdr64->e_shnum * ehdr64->e_shentsize, unit->limit))
			return FAIL_PATH;
	}
	else
	{
		ehdr32 = (const Elf32_Ehdr *)unit->base;
		meta->ElfN_Ehdr.Ehdr32 = ehdr32;
		if (ehdr32->e_type != ET_REL && ehdr32->e_type != ET_EXEC && ehdr32->e_type != ET_DYN)
			return FAIL_PATH;
		if (ehdr32->e_machine != EM_X86_64 && ehdr32->e_machine != EM_386)
			return FAIL_PATH;
		if (ehdr32->e_shoff == 0 || ehdr32->e_shnum == 0)
			return FAIL_PATH;
		if (!CHECK_RANGE(ehdr32->e_shoff, ehdr32->e_shnum * ehdr32->e_shentsize, unit->limit))
			return FAIL_PATH;
	}
	return OK;
}

static int find_symbol_table(const t_unit *unit, t_MetaData *meta)
{
	uint16_t i;
	const Elf64_Shdr *shdr64;
	const Elf32_Shdr *shdr32;
	uint64_t sh_type;
	int found_symtab;
	// int found_dynsym;
	uint16_t symtab_idx;
	// uint16_t dynsym_idx;
	uint16_t shnum;

	found_symtab = 0;
	// found_dynsym = 0;
	symtab_idx = 0;
	// dynsym_idx = 0;
	if (meta->elf_class == ELFCLASS64)
	{
		meta->ElfN_Shdr.Shdr64 = (const Elf64_Shdr *)MOVE_ADDRESS(unit->base, meta->ElfN_Ehdr.Ehdr64->e_shoff);
		shnum = meta->ElfN_Ehdr.Ehdr64->e_shnum;
	}
	else
	{
		meta->ElfN_Shdr.Shdr32 = (const Elf32_Shdr *)MOVE_ADDRESS(unit->base, meta->ElfN_Ehdr.Ehdr32->e_shoff);
		shnum = meta->ElfN_Ehdr.Ehdr32->e_shnum;
	}
	i = 0;
	while (i < shnum)
	{
		if (meta->elf_class == ELFCLASS64)
		{
			shdr64 = &meta->ElfN_Shdr.Shdr64[i];
			sh_type = shdr64->sh_type;
		}
		else
		{
			shdr32 = &meta->ElfN_Shdr.Shdr32[i];
			sh_type = shdr32->sh_type;
		}
		if (sh_type == SHT_SYMTAB)
		{
			found_symtab = 1;
			symtab_idx = i;
			break;
		}
		// if (sh_type == SHT_DYNSYM)
		// {
		// 	found_dynsym = 1;
		// 	dynsym_idx = i;
		// }
		i++;
	}
	if (found_symtab)
		i = symtab_idx;
	// else if (found_dynsym)
	// 	i = dynsym_idx;
	else
	{
		NM_LOG(unit->display_name, "no symbols");
		return FAIL_PATH;
	}
	if (meta->elf_class == ELFCLASS64)
	{
		shdr64 = &meta->ElfN_Shdr.Shdr64[i];
		if (shdr64->sh_link >= shnum)
			return FAIL_PATH;
		meta->strtab_offset = meta->ElfN_Shdr.Shdr64[shdr64->sh_link].sh_offset;
		meta->strtab_size = meta->ElfN_Shdr.Shdr64[shdr64->sh_link].sh_size;
		meta->ElfN_Sym.Sym64 = (const Elf64_Sym *)MOVE_ADDRESS(unit->base, shdr64->sh_offset);
		if (shdr64->sh_entsize == 0)
			meta->sym_count = shdr64->sh_size / sizeof(Elf64_Sym);
		else
			meta->sym_count = shdr64->sh_size / shdr64->sh_entsize;
	}
	else
	{
		shdr32 = &meta->ElfN_Shdr.Shdr32[i];
		if (shdr32->sh_link >= shnum)
			return FAIL_PATH;
		meta->strtab_offset = meta->ElfN_Shdr.Shdr32[shdr32->sh_link].sh_offset;
		meta->strtab_size = meta->ElfN_Shdr.Shdr32[shdr32->sh_link].sh_size;
		meta->ElfN_Sym.Sym32 = (const Elf32_Sym *)MOVE_ADDRESS(unit->base, shdr32->sh_offset);
		if (shdr32->sh_entsize == 0)
			meta->sym_count = shdr32->sh_size / sizeof(Elf32_Sym);
		else
			meta->sym_count = shdr32->sh_size / shdr32->sh_entsize;
	}
	return OK;
}

static int load_symbols(const t_unit *unit, t_MetaData *meta, t_NmSymData **symbols, t_NmShdrData **shdr_cache)
{
	uint64_t i;
	const Elf64_Sym *sym64;
	const Elf32_Sym *sym32;
	const Elf64_Shdr *shdr64;
	const Elf32_Shdr *shdr32;
	uint16_t shnum;

	*symbols = malloc(sizeof(t_NmSymData) * meta->sym_count);
	if (!*symbols)
		return FAIL_PATH;
	if (meta->elf_class == ELFCLASS64)
		shnum = meta->ElfN_Ehdr.Ehdr64->e_shnum;
	else
		shnum = meta->ElfN_Ehdr.Ehdr32->e_shnum;
	*shdr_cache = malloc(sizeof(t_NmShdrData) * shnum);
	if (!*shdr_cache)
	{
		free(*symbols);
		return FAIL_PATH;
	}
	i = 0;
	while (i < shnum)
	{
		if (meta->elf_class == ELFCLASS64)
		{
			shdr64 = &meta->ElfN_Shdr.Shdr64[i];
			(*shdr_cache)[i].sh_type = shdr64->sh_type;
			(*shdr_cache)[i].sh_flags = shdr64->sh_flags;
		}
		else
		{
			shdr32 = &meta->ElfN_Shdr.Shdr32[i];
			(*shdr_cache)[i].sh_type = shdr32->sh_type;
			(*shdr_cache)[i].sh_flags = shdr32->sh_flags;
		}
		i++;
	}
	i = 0;
	while (i < meta->sym_count)
	{
		if (meta->elf_class == ELFCLASS64)
		{
			sym64 = &meta->ElfN_Sym.Sym64[i];
			(*symbols)[i].st_value = sym64->st_value;
			(*symbols)[i].st_size = sym64->st_size;
			(*symbols)[i].st_name = sym64->st_name;
			(*symbols)[i].st_shndx = sym64->st_shndx;
			(*symbols)[i].st_info_type = ELF64_ST_TYPE(sym64->st_info);
			(*symbols)[i].st_info_bind = ELF64_ST_BIND(sym64->st_info);
		}
		else
		{
			sym32 = &meta->ElfN_Sym.Sym32[i];
			(*symbols)[i].st_value = sym32->st_value;
			(*symbols)[i].st_size = sym32->st_size;
			(*symbols)[i].st_name = sym32->st_name;
			(*symbols)[i].st_shndx = sym32->st_shndx;
			(*symbols)[i].st_info_type = ELF32_ST_TYPE(sym32->st_info);
			(*symbols)[i].st_info_bind = ELF32_ST_BIND(sym32->st_info);
		}
		(*symbols)[i].name = safe_get_string(unit, meta->strtab_offset, meta->strtab_size, (*symbols)[i].st_name);
		if (!(*symbols)[i].name)
			(*symbols)[i].name = "";
		(*symbols)[i].type = classify_symbol(unit, meta, &(*symbols)[i], *shdr_cache);
		if ((*symbols)[i].st_value == 0 && (*symbols)[i].type != 'a' && (*symbols)[i].type != 'U' && (*symbols)[i].type != 'w') {
			uint64_t offset = check_debug_symbol(meta);
			uint64_t size = check_debug_section_size(meta);
			uint32_t sh_name = check_section_name(meta, (*symbols)[i].st_shndx);
			if (offset != 0 && size != 0) {
				(*symbols)[i].name = safe_get_string(unit, offset, size, sh_name);
				if (!(*symbols)[i].name)
					(*symbols)[i].name = "";
			}
		}
		i++;
	}
	return OK;
}

int process_elf_unit(const t_unit *unit, uint32_t opt, int multiple_files)
{
	t_MetaData meta;
	t_NmSymData *symbols;
	t_NmShdrData *shdr_cache;
	int ret;

	ft_memset(&meta, 0, sizeof(t_MetaData));
	symbols = NULL;
	shdr_cache = NULL;
	if (verify_elf_header(unit, &meta) != OK)
		return FAIL_PATH;
	if (find_symbol_table(unit, &meta) != OK)
		return FAIL_PATH;
	if (load_symbols(unit, &meta, &symbols, &shdr_cache) != OK)
		return FAIL_PATH;
	ret = sort_and_print_symbols(symbols, meta.sym_count, opt, unit->display_name, multiple_files);
	free(symbols);
	free(shdr_cache);
	return ret;
}
