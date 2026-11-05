int executable_data = 23;
int executable_bss;

static int helper(int value)
{
	return value + executable_data;
}

int main(void)
{
	return helper(executable_bss);
}
