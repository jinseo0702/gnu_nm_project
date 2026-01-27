#include "../include/nm.h"

static unsigned char classify_by_shndx(const t_NmSymData *sym, const t_NmShdrData *shdr_cache)
{
	uint64_t sh_type;
	uint64_t sh_flags;

	if (sym->st_shndx == SHN_ABS)
	{
		if (sym->st_info_bind == STB_LOCAL)
			return 'a';
		return 'A';
	}
	if (sym->st_shndx == SHN_COMMON)
		return 'C';
	if (sym->st_shndx == SHN_UNDEF)
	{
		if (sym->st_info_bind == STB_WEAK)
		{
			if (sym->st_info_type == STT_OBJECT)
				return 'v';
			return 'w';
		}
		return 'U';
	}
	sh_type = shdr_cache[sym->st_shndx].sh_type;
	sh_flags = shdr_cache[sym->st_shndx].sh_flags;
	if (sh_type == SHT_X86_64_UNWIND)
		return 'p';
	if (sh_type == SHT_NOBITS && (sh_flags & SHF_ALLOC) && (sh_flags & SHF_WRITE))
	{
		if (sym->st_info_bind == STB_LOCAL)
			return 'b';
		return 'B';
	}
	if ((sh_flags & SHF_ALLOC) && (sh_flags & SHF_EXECINSTR))
	{
		if (sym->st_info_bind == STB_LOCAL)
			return 't';
		return 'T';
	}
	if ((sh_flags & SHF_ALLOC) == SHF_ALLOC && (sh_flags & SHF_WRITE) == SHF_WRITE)
	{
		if (sym->st_info_bind == STB_LOCAL)
			return 'd';
		return 'D';
	}
	if ((sh_flags & SHF_ALLOC) && !(sh_flags & SHF_WRITE))
	{
		if (sym->st_info_bind == STB_LOCAL)
			return 'r';
		return 'R';
	}
	if (sh_type == SHT_PROGBITS && sh_flags == 0)
		return 'N';
	if (sh_type == SHT_PROGBITS)
		return 'n';
	return '?';
}

unsigned char classify_symbol(const t_unit *unit, const t_MetaData *meta, const t_NmSymData *sym, const t_NmShdrData *shdr_cache)
{
	(void)unit;
	(void)meta;
	if (sym->st_info_type == STT_GNU_IFUNC)
		return 'i';
	if (sym->st_info_bind == STB_GNU_UNIQUE)
		return 'u';
	if (sym->st_info_bind == STB_WEAK)
	{
		if (sym->st_shndx == SHN_UNDEF)
		{
			if (sym->st_info_type == STT_OBJECT)
				return 'v';
			return 'w';
		}
		if (sym->st_info_type == STT_OBJECT)
			return 'V';
		return 'W';
	}
	return classify_by_shndx(sym, shdr_cache);
}
