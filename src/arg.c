#include "../include/nm.h"

static int is_valid_option(char c)
{
	return (c == 'a' || c == 'g' || c == 'u' || c == 'r' || c == 'P' || c == 'n');
}

static int parse_option_string(const char *str, uint32_t *opts)
{
	int i;

	i = 1;
	while (str[i])
	{
		if (!is_valid_option(str[i]))
		{
			ft_fprintf(2, "ft_nm: invalid option -- '%c'\n", str[i]);
			return FATAL;
		}
		if (str[i] == 'a')
			SETOPT(*opts, OPT_a);
		else if (str[i] == 'g')
			SETOPT(*opts, OPT_g);
		else if (str[i] == 'u')
			SETOPT(*opts, OPT_u);
		else if (str[i] == 'r')
			SETOPT(*opts, OPT_r);
		else if (str[i] == 'P')
			SETOPT(*opts, OPT_P);
		else if (str[i] == 'n')
			SETOPT(*opts, OPT_n);
		i++;
	}
	return OK;
}

// int parse_arguments(int argc, char **argv, uint32_t *opts, char ***paths, int *path_count)
// {
// 	int i;
// 	int path_idx;

// 	*opts = 0;
// 	*path_count = 0;
// 	i = 1;
// 	while (i < argc && argv[i][0] == '-' && argv[i][1] != '\0')
// 	{
// 		if (parse_option_string(argv[i], opts) == FATAL)
// 			return FATAL;
// 		i++;
// 	}
// 	*path_count = argc - i;
// 	if (*path_count == 0)
// 		return OK;
// 	*paths = malloc(sizeof(char *) * (*path_count));
// 	if (!*paths)
// 		return FATAL;
// 	path_idx = 0;
// 	while (i < argc)
// 	{
// 		(*paths)[path_idx] = argv[i];
// 		path_idx++;
// 		i++;
// 	}
// 	return OK;
// }

int parse_arguments(int argc, char **argv, uint32_t *opts, char ***paths, int *path_count)
{
	int option;
	int path_idx;
	int i;

	*opts = 0;
	*path_count = 0;
	option = 0;
	i = 0;
	for (int index = 1; index < argc; index++) {
		if (argv[index][0] == '-' && ft_strlen(argv[index]) > 1) {
			option++;
			if (parse_option_string(argv[index], opts) == FATAL)
			return FATAL;
		}
		else i++;
	}
	*path_count = i;
	if (*path_count == 0)
		return OK;
	*paths = malloc(sizeof(char *) * (*path_count));
	if (!*paths)
		return FATAL;
	path_idx = 0;
	for (int index = 1; index < argc; index++) {
		if (!(argv[index][0] == '-' && ft_strlen(argv[index]) > 1)) {
			(*paths)[path_idx] = argv[index];
			path_idx++;			
		}
	}
	return OK;
}

