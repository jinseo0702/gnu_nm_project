#include <stdio.h>
#include <stdarg.h>

void	printValues(char *types, ...)
{
	va_list	ap;//가변인자 포인터 선언.
	int	i = 0;

	va_start(ap, types);//이렇게 하면 메모리주소는 type의후부터 나오는건가?
	while (types[i] != '\0')
	{
		switch (types[i])
		{
			case 'i':
				printf("%d ", va_arg(ap, int));
				break ;
			case 'd':
				printf("%f ", va_arg(ap, double));
				break ;
			case 'c':
				printf("%c ", (char)va_arg(ap, int));
				break ;
			case 's':
				printf("%s ", va_arg(ap, char *));
				break ;
			default:
				break ;
		}
		i++;
	}
	va_end(ap);

	printf("\n");
}

int	main()
{
	printValues("i", 10);
	printValues("ci", 'a', 10);
	printValues("cdi", 1.234567, 'a', 10);
	printValues("sicd", "Hello World", 10, 'a', 1.234567);

	return (0);
}
