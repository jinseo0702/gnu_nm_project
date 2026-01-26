#include "../include/nm.h"

int process_path(const char *path, uint32_t opt, int multiple_files)
{
	int fd;
	struct stat st;
	void *map;
	t_unit unit;
	t_format fmt;
	int ret;

	fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		ft_fprintf(2, "ft_nm: '%s': %s\n", path, strerror(errno));
		return FAIL_PATH;
	}
	if (fstat(fd, &st) < 0)
	{
		ft_fprintf(2, "ft_nm: '%s': %s\n", path, strerror(errno));
		close(fd);
		return FAIL_PATH;
	}
	if (st.st_size == 0)
	{
		close(fd);
		return FAIL_PATH;
	}
	map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (map == MAP_FAILED)
	{
		ft_fprintf(2, "ft_nm: '%s': %s\n", path, strerror(errno));
		close(fd);
		return FAIL_PATH;
	}
	unit.base = (const unsigned char *)map;
	unit.limit = (uint64_t)st.st_size;
	unit.display_name = path;
	fmt = detect_format(&unit);
	if (fmt == FMT_ELF)
		ret = process_elf_unit(&unit, opt, multiple_files);
	else if (fmt == FMT_AR)
		ret = process_ar_archive(&unit, opt);
	else
	{
		ft_fprintf(2, "ft_nm: %s: file format not recognized\n", path);
		ret = FAIL_PATH;
	}
	munmap(map, st.st_size);
	close(fd);
	return ret;
}
