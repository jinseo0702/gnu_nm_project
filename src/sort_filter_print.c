#include "../include/nm.h"

static int is_visible(const t_NmSymData *sym, uint32_t opt)
{
	int show_all;
	int show_external;
	int show_undefined;

	if ( sym->type == 'U' && (!sym->name || sym->name[0] == '\0'))
		return 0;
	show_all = HASOPT(opt, OPT_a);
	if (!show_all)
	{
		if (sym->st_info_type == STT_FILE || sym->st_info_type == STT_SECTION)
			return 0;
		if (!sym->name || sym->name[0] == '\0')
			return 0;
	}
	show_external = HASOPT(opt, OPT_g);
	show_undefined = HASOPT(opt, OPT_u);
	if (!show_all && !show_external && !show_undefined)
		return 1;
	if (show_undefined)
	{
		if (sym->type == 'w' || sym->type == 'U')
			return 1;
		return 0;
	}
	if (show_external)
	{
		if (sym->type == 'A' || sym->type == 'B' || sym->type == 'D' || 
			sym->type == 'R' || sym->type == 'T' || sym->type == 'W' || 
			sym->type == 'w' || sym->type == 'U' || sym->type == 'V')
			return 1;
		return 0;
	}
	if (show_all)
	{
		return 1;
	}
	return 1;
}

static int str_compare(const char *s1, const char *s2)
{
	while (*s1 && *s2 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (unsigned char)*s1 - (unsigned char)*s2;
}

static int compare_by_name(const t_NmSymData *a, const t_NmSymData *b)
{
	int cmp;

	cmp = str_compare(a->name, b->name);
	if (cmp != 0)
		return cmp;
	if (a->st_value < b->st_value)
		return -1;
	if (a->st_value > b->st_value)
		return 1;
	return 0;
}

static int compare_by_value(const t_NmSymData *a, const t_NmSymData *b)
{
	if (a->st_value == 0 && b->st_value == 0) 
	{
		if ((a->type != 'U' && a->type != 'w')  && (b->type == 'U' || b->type == 'w'))
			return 1;
		else if ((a->type == 'U' || a->type == 'w') && (b->type != 'U' && b->type != 'w'))
			return -1;
	}
	if (a->st_value < b->st_value)
		return -1;
	if (a->st_value > b->st_value)
		return 1;
	return str_compare(a->name, b->name);
}

static void swap_symbols(t_NmSymData *a, t_NmSymData *b)
{
	t_NmSymData tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static int partition(t_NmSymData *arr, int low, int high, int (*cmp)(const t_NmSymData *, const t_NmSymData *))
{
	t_NmSymData *pivot;
	int i;
	int j;

	pivot = &arr[high];
	i = low - 1;
	j = low;
	while (j < high)
	{
		if (cmp(&arr[j], pivot) < 0)
		{
			i++;
			swap_symbols(&arr[i], &arr[j]);
		}
		j++;
	}
	swap_symbols(&arr[i + 1], &arr[high]);
	return i + 1;
}

static void quicksort(t_NmSymData *arr, int low, int high, int (*cmp)(const t_NmSymData *, const t_NmSymData *))
{
	int pi;

	if (low < high)
	{
		pi = partition(arr, low, high, cmp);
		quicksort(arr, low, pi - 1, cmp);
		quicksort(arr, pi + 1, high, cmp);
	}
}

static void write_hex(uint64_t addr, char *dest, uint8_t class) {
	int i;
	if (class == ELFCLASS64)
		i = 15;
	else i = 7;
    const char *base = "0123456789abcdef";

    while (i >= 0) {
        dest[i] = base[addr & 0xF]; // 마지막 4비트 추출
        addr >>= 4;                // 4비트 오른쪽으로 밀기
        i--;
    }
	if (class == ELFCLASS64)
    	dest[16] = '\0';
	else dest[8] = '\0';
}

static void print_symbol(const t_NmSymData *sym, uint32_t opt)
{
	if (HASOPT(opt, OPT_P))
	{
		ft_printf("%s %c", sym->name, sym->type);
		if (sym->type == 'U' || sym->type == 'w' || sym->type == 'v')
			ft_printf("\n");
		else{
			ft_printf(" %x", sym->st_value);
			if (sym->st_size != 0) {
				ft_printf(" %x", sym->st_size);
			}
			ft_printf("\n");
		}
	}
	else
	{
		char arr[17] = {0,};
		if (sym->type == 'U' || sym->type == 'w' || sym->type == 'v'){
			if (sym->elf_class == ELFCLASS64)
				ft_memset(arr, ' ', 16);
			else ft_memset(arr, ' ', 8);
			ft_printf("%s %c %s\n", arr , sym->type, sym->name);
		}
		else{
			write_hex(sym->st_value, arr, sym->elf_class);
			ft_printf("%s %c %s\n", arr, sym->type, sym->name);
		}
	}
}

int sort_and_print_symbols(t_NmSymData *symbols, uint64_t count, uint32_t opt, const char *filename, int multiple_files)
{
	uint64_t i;
	t_NmSymData *visible;
	uint64_t visible_count;
	uint64_t j;

	if (multiple_files && filename)
		ft_printf("\n%s:\n", filename);
	visible = malloc(sizeof(t_NmSymData) * count);
	if (!visible)
		return FAIL_PATH;
	visible_count = 0;
	i = 0;
	while (i < count)
	{
		if (is_visible(&symbols[i], opt))
		{
			visible[visible_count] = symbols[i];
			visible_count++;
		}
		i++;
	}
	if (visible_count > 1)
	{
		if (HASOPT(opt, OPT_n))
			quicksort(visible, 0, visible_count - 1, compare_by_value);
		else
			quicksort(visible, 0, visible_count - 1, compare_by_name);
	}
	if (HASOPT(opt, OPT_r) && visible_count > 1)
	{
		i = 0;
		j = visible_count - 1;
		while (i < j)
		{
			swap_symbols(&visible[i], &visible[j]);
			i++;
			j--;
		}
	}
	i = 0;
	while (i < visible_count)
	{
		print_symbol(&visible[i], opt);
		i++;
	}
	free(visible);
	return OK;
}
