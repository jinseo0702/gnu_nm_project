int shared_data = 29;
int shared_bss;

int shared_add(int left, int right)
{
	return left + right + shared_data;
}
