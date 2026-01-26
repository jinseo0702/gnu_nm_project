#include "../include/nm.h"

static const char *g_error_table[] = {
	#define X(id, str) [id] = str,
	ERROR_LIST
	#undef X
};

const char *get_error_msg(t_error err)
{
	if (err >= ERROR_END)
		return "UNKNOWN_ERROR";
	return g_error_table[err];
}

void print_error(t_error err, const char *path)
{
	if (path)
		ft_fprintf(2, "ft_nm: %s: %s\n", path, get_error_msg(err));
	else
		ft_fprintf(2, "ft_nm: %s\n", get_error_msg(err));
}

int main(int argc, char **argv)
{
	uint32_t opts;
	char **paths;
	int path_count;
	int ret;
	int i;

	opts = 0;
	paths = NULL;
	path_count = 0;
	ret = parse_arguments(argc, argv, &opts, &paths, &path_count);
	if (ret == FATAL)
		return 1;
	if (path_count == 0)
	{
		paths = malloc(sizeof(char *));
		if (!paths)
			return 1;
		paths[0] = "a.out";
		path_count = 1;
	}
	i = 0;
	while (i < path_count)
	{
		process_path(paths[i], opts, path_count > 1);
		i++;
	}
	if (paths && paths[0] != argv[0])
		free(paths);
	return 0;
}
