#include "../include/nm.h"

static uint64_t parse_ar_size(const char *ar_size_str)
{
	uint64_t size;
	int i;

	size = 0;
	i = 0;
	while (i < 10 && ar_size_str[i] >= '0' && ar_size_str[i] <= '9')
	{
		size = size * 10 + (ar_size_str[i] - '0');
		i++;
	}
	return size;
}

static const char *extract_ar_name(const struct ar_hdr *hdr, const char *strtab, uint64_t strtab_size)
{
	static char name_buf[17];
	static char long_name_buf[4095] = {0,};
	int i;
	uint64_t offset;
	const char *name_ptr;
	int j;

	if (hdr->ar_name[0] == '/' && hdr->ar_name[1] >= '0' && hdr->ar_name[1] <= '9')
	{
		offset = 0;
		i = 1;
		while (i < 16 && hdr->ar_name[i] >= '0' && hdr->ar_name[i] <= '9')
		{
			offset = offset * 10 + (hdr->ar_name[i] - '0');
			i++;
		}
		if (strtab && offset < strtab_size)
		{
			name_ptr = strtab + offset;
			i = 0;
			while (i < 4095 && offset + i < strtab_size && name_ptr[i] != '/' && name_ptr[i] != '\n' && name_ptr[i] != '\0')
			{
				long_name_buf[i] = name_ptr[i];
				i++;
			}
			long_name_buf[i] = '\0';
			return long_name_buf;
		}
	}
	i = 0;
	while (i < 16 && hdr->ar_name[i] != '/' && hdr->ar_name[i] != ' ')
	{
		name_buf[i] = hdr->ar_name[i];
		i++;
	}
	name_buf[i] = '\0';
	j = i - 1;
	while (j >= 0 && name_buf[j] == ' ')
	{
		name_buf[j] = '\0';
		j--;
	}
	return name_buf;
}

int process_ar_archive(const t_unit *unit, uint32_t opt)
{
	uint64_t member_off;
	const struct ar_hdr *hdr;
	uint64_t member_size;
	uint64_t payload_off;
	t_unit member_unit;
	const char *member_name;
	const char *strtab;
	uint64_t strtab_size;

	if (!CHECK_RANGE(0, SARMAG, unit->limit))
		return FAIL_PATH;
	member_off = SARMAG;
	strtab = NULL;
	strtab_size = 0;
	while (member_off < unit->limit)
	{
		if (!CHECK_RANGE(member_off, sizeof(struct ar_hdr), unit->limit))
			break;
		hdr = (const struct ar_hdr *)MOVE_ADDRESS(unit->base, member_off);
		if (ft_memcmp(hdr->ar_fmag, ARFMAG, 2) != 0)
			return FAIL_PATH;
		member_size = parse_ar_size(hdr->ar_size);
		payload_off = member_off + sizeof(struct ar_hdr);
		if (!CHECK_RANGE(payload_off, member_size, unit->limit))
			break;
		member_name = extract_ar_name(hdr, strtab, strtab_size);
		if (ft_memcmp(hdr->ar_name, "//", 2) == 0)
		{
			strtab = (const char *)MOVE_ADDRESS(unit->base, payload_off);
			strtab_size = member_size;
		}
		else if (!(hdr->ar_name[0] == '/' && ft_strlen(hdr->ar_name) == 1))
		{
			member_unit.base = (const unsigned char *)MOVE_ADDRESS(unit->base, payload_off);
			member_unit.limit = member_size;
			member_unit.display_name = member_name;
			if (detect_format(&member_unit) == FMT_ELF)
			{
				ft_printf("\n%s:\n", member_name);
				process_elf_unit(&member_unit, opt, 0);
			}
		}
		member_off = payload_off + member_size;
		if (member_off % 2)
			member_off++;
	}
	return OK;
}
