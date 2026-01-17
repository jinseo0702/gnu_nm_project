#include <stdio.h>
#include <string.h>
#include "./ver1.3/libft.h"

int	main()
{
	//strlen
	printf("---------------------------------ft_strlen--------------------------------------\n");
	size_t	a;
	a = ft_strlen("Hello World!");
	printf("Hello World! len is %ld\n", a);
	//atoi
	printf("---------------------------------ft_atoi--------------------------------------\n");
	printf("%d\n",ft_atoi(" \f \t \v \n \r  -123456ab567"));
	//isalpha
	printf("---------------------------------ft_isalpha-------------------------------------\n");
	printf("a is %d, A is %d, 1 is %d\n", ft_isalpha('a'), ft_isalpha('A'), ft_isalpha('1'));
	//isdigit
	printf("---------------------------------ft_isdigit--------------------------------------\n");
	printf("28 is %d\n", ft_isdigit(28));
	printf("'1' is %d\n", ft_isdigit('1'));
	//isalnum
	printf("---------------------------------ft_isalnum---------------------------------------\n");
	printf("a == %d\n", ft_isalnum('a'));
	printf("1 == %d\n", ft_isalnum('1'));
	//memset
	printf("---------------------------------ft_memset----------------------------------------\n");
	char buffer[21];
	char *string;

	ft_memset(buffer, 0, sizeof(buffer));
	string = (char *) ft_memset(buffer,'A', 10);
	printf("Buffer contents: %s\n", string);
	ft_memset(buffer+10, 'B', 10);
	printf("Buffer contents: %s\n", buffer);
	//isascii
	printf("---------------------------------ft_isascii---------------------------------------\n");
	for (int a = 0; a < 200; a++)
		printf("%d is ascii ?? == %d\n", a, ft_isascii(a));
	//isprint
	printf("---------------------------------ft_isprint---------------------------------------\n");
	for	(int a = 0; a < 128; a++)
		printf("%c is print ascii == %d\n", a, ft_isprint(a));
	//bzero
	printf("---------------------------------ft_bzero---------------------------------------\n");
	char barr[20]; 
	ft_bzero(barr, 20);
	for (int a = 0; a < 20; a++)
		printf ("%darr take %d\n", a,barr[a]);
	//memcpy
	printf("----------------------ft_strlcpy&&ft_memcpy---------------------------------------\n");
	char	mcarr[20];
	char	mcbrr[20];
	char	*mcptr;
	char	*mccpy;
	ft_bzero(mcarr, 20);
	mcptr = mcarr;
	size_t li = ft_strlcpy(mcptr, "Hello World!", 13);
	ft_strlcpy(mcbrr, "12312312312123123fasd", 20);
	printf("strlcpy return value is %lu == 12\n", li);
	mccpy = (char *)ft_memcpy(mcbrr, mcptr, 5);
	printf("%s\n", mccpy);
	//memcpy
	printf("-----------------------ft_memcpy--------------------------------------------------\n");
	char	memcpyarr[20] = "Im jin Hello";
	char	memcpybrr[20] = "Im lee Mello";
	
	printf("original memcpyarr == %s\n", memcpyarr);
	printf("original memcpybrr == %s\n", memcpybrr);
	ft_memcpy(memcpybrr + 3, memcpyarr + 3, 3);
	printf("change memcpybrr == %s\n", memcpybrr);
	//memmove
	printf("-----------------------ft_memcpy--------------------------------------------------\n");
	printf("overlap case memmove if you want memcpy case. you change fd\n");
	char	*memmoveptr;
	char	memmovearr[100];

	ft_bzero(memmovearr, 100);
	ft_strlcpy(memmovearr, "123456789", 9);
	memmoveptr = (char *)malloc(sizeof(char) * 100);
	memmoveptr = memmovearr;
	memmoveptr = (char *)ft_memmove(memmoveptr, memmoveptr + 1, 6);
	printf("memmove is very safe %s\n", memmoveptr);

	printf("general case\n");
	char	movearr[20] = "Im jin Hello";
	char	movebrr[20] = "Im lee Mello";
	
	printf("original memcpyarr == %s\n", movearr);
	printf("original memcpybrr == %s\n", movebrr);
	ft_memmove(movebrr + 3, movearr + 3, 3);
	printf("change memcpybrr == %s\n", movebrr);
	//strlcat
	printf("----------------------ft_strlcat---------------------------------------------------\n");
	char lcatarr[] = "7890";
	char lcatbrr[50] = "56";

	char lcatarr1[] = "7890";
	char lcatbrr1[50] = "56";
	size_t num = ft_strlcat(lcatbrr, lcatarr, 1);
	printf("%s\n", lcatbrr);
	printf("%lu\n", num);
	size_t num2 = ft_strlcat(lcatbrr1, lcatarr1, 7);
	printf("%s\n", lcatbrr1);
	printf("%lu\n", num2);
	//touuper and tolower
	printf("----------------------ft_toupper&&lower---------------------------------------------\n");
	char aup = ft_toupper('c');
	char blow = ft_tolower('A');
	printf("c change upper %c\n", aup);
	printf("A change lower %c\n", blow);
	//strchr
	printf("----------------------ft_strchr---------------------------------------------\n");
	const char *strcc = "Try not. Do, or do not. There is no try.";
	char targetcc = '\0';
	const char *resultcc = strcc;

		printf("Found '%c' starting at '%s'\n", targetcc, resultcc);
	//strrchr
	printf("----------------------ft_strrchr---------------------------------------------\n");
	const char *strrc = "Try not. Do, or do not. There is no try?";
	char targetrc = '\0';
	const char *resultrc = strrc;

	resultrc = ft_strrchr(resultrc, targetrc);
	printf("Found '%c' starting at '%s'\n", targetrc, resultrc);
	//strncmp
	printf("----------------------ft_strncmp---------------------------------------------\n");
	char cmparr[] = "123123123";
	char cmpbrr[] = "12";
	printf("%d\n", ft_strncmp(cmparr, cmpbrr, 10));
	printf("%d\n", strncmp(cmparr, cmpbrr, 10));
	//strnstr
	printf("----------------------ft_strstr---------------------------------------------\n");
    char *strbig = "Hello my name is not jikwon. My name is jin nice to meet you!";                 
    char *strlittle = "jin";
	size_t strnlen = ft_strlen(strbig);

	printf("strbig is %s\nstrlittle is %s\n", strbig, strlittle);
	printf("ft_strnstr jin nice to meet you! == %s\n", ft_strnstr(strbig, strlittle, strnlen));
	//memchr
	printf("----------------------ft_memchr---------------------------------------------\n");
	char*mempch;
	char memstr[] = "Example string";
	mempch = (char*)ft_memchr(memstr, 'p', ft_strlen(memstr));
	printf("'p' found at position %s.\n", mempch);
	//memcmp
	printf("----------------------ft_memchr---------------------------------------------\n");
	char memcmparr[] = "123123123";
	char memcmpbrr[] = "12";
	printf("%d\n", ft_memcmp(memcmparr, memcmpbrr, 48));
	printf("%d\n", memcmp(memcmparr, memcmpbrr, 48));
	//calloc
	printf("----------------------ft_calloc---------------------------------------------\n");
	char *castr = ft_calloc(20, sizeof(char));
	strcpy(castr, "Hello World!");
	for (int a = 0; a < 20; a++)
		printf("%d ", castr[a]);
	printf("\n");
	//strdup
	printf("----------------------ft_strdup---------------------------------------------\n");
	char *src = "Hello my friends";                                                                 
	char *fuck = ft_strdup(src);

	for (int a = 0; a < 20; a++)
		printf("%d, ", fuck[a]);
	printf("\n");
	free(ft_strdup(src));
	//substr
	printf("----------------------ft_substr---------------------------------------------\n");
	char *substr1;
	substr1 = ft_substr("lorem ipsum dolor sit amet", 10, 100);
	printf("%s\n", substr1);
	free(substr1);
	printf("----------------------ft_strtrim---------------------------------------------\n");
	printf("%s \n", ft_strtrim("a1sd1asd1asfdasfasdfasdfas1", "asd"));

	//ft_split
	printf("----------------------ft_split---------------------------------------------\n");
	char	*split = "Hello my name is jin";
	char	**split2 = (char **)ft_split(split, ' ');
	for (int a = 0; a != NULL; a++)
		printf("%s\n", split2[a]);
	return (0);
}
























