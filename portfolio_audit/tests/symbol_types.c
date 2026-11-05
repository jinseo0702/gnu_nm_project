int global_data = 42;
int global_bss;
static int local_data = 7;
static int local_bss;
const int global_rodata = 11;
static const int local_rodata = 13;

int common_symbol;

void global_function(void)
{
}

static void local_function(void)
{
}

extern void undefined_function(void);
extern int undefined_object;

void *undefined_references[] = {
	(void *)&undefined_function,
	(void *)&undefined_object,
};

__attribute__((weak)) int weak_defined_object = 17;

__attribute__((weak)) void weak_defined_function(void)
{
}

extern int weak_undefined_object __attribute__((weak));
extern void weak_undefined_function(void) __attribute__((weak));

void *weak_undefined_references[] = {
	(void *)&weak_undefined_object,
	(void *)&weak_undefined_function,
};

static void ifunc_implementation(void)
{
}

static void *ifunc_resolver(void)
{
	return (void *)&ifunc_implementation;
}

void indirect_function(void) __attribute__((ifunc("ifunc_resolver")));

int force_local_references(void)
{
	local_function();
	return local_data + local_bss + local_rodata;
}
