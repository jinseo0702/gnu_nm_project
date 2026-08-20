#include "../include/nm.h"

t_format detect_format(const t_unit *unit)
{
	if (!unit || !unit->base)
		return FMT_UNKNOWN;
	if (unit->limit >= SARMAG)
	{
		if (ft_memcmp(unit->base, ARMAG, SARMAG) == 0)
			return FMT_AR;
	}
	if (unit->limit >= SELFMAG)
	{
		if (ft_memcmp(unit->base, ELFMAG, SELFMAG) == 0)
			return FMT_ELF;
	}
	return FMT_UNKNOWN;
}
