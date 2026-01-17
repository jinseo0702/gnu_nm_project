#include <stdio.h>
#include <stdlib.h>

int main()
{
    //what is printf return Values Hello..? A-->5
    printf("\n%d\n", printf("Hello"));
    //printf error return Values..? A --> -1 fotmat에 초과값이 들어간경우 
    printf("\n%d\n", printf("%2147483649d\n", 42));
    //printf %c의경우 3번 뽑으면 다 H가 뽑히는건가? --> YES~!
    char *case_perc = "Hello World";
    printf("%d", printf("%c\n", *case_perc));//\n까지 총 2byte를 return 한다.
    printf("%c\n", *case_perc);
    printf("%c\n", *case_perc);
    //printf %s의 경우 3번 뽑으면 다 Hello World가 뽑히는건가? -->
    printf("%d", printf("%s\n", case_perc));//\n까지 총 12byte를 return 한다.
    printf("%s\n", case_perc);
    printf("%s\n", case_perc);
    //printf %p의 경우를 살펴보자 --> 디버깅으로 확인해 보니 포인터가 가르키는 주소가 출력된다. "Hello World"의 첫 주소 H의 주소
    printf("%p\n", case_perc);
    //포인터 자신의 주소를 직접 출력해 보자 --> 다른걸 확인 할수 있다. ptr의 직접적인 주소가 출력된다. 
    printf("%p\n", &case_perc);
    //printf로 NULL을 가르키면 어떤게 뽑히는지 알아보자 --> (nil)이라는 값이 출력된다. like NULL
    printf("%p\n", (void *)0);
    //printf로 overflow max를 해보자
    printf("%p\n", (void *)-1);
    //printf로 void pointer (void *)1의 주소를 가르킨다면? --> 0x1이리는 주소가 출력된다. 직접적으로 값이 나오는걸 알수 있다.
    printf("%p\n", (void *)1);
    //printf("%d", printf("%s\n", &(void *)1));--> 이렇게 하면 segment error 가 난다.
    //NULL을 직접 박아버려도 (nil)이 뜬다. 반환 값은 어떻게 될까? -->
    printf("(nil)'s return values == %d\n", printf("%p\n", NULL));
    //그렇다면 nil자체를 박아 버려도 사용 가능할까? 불가능 하다.
    //printf로 NULL을 출력해보자.
    //printf("%s\n", NULL); --> segment error가 나온다.
    //NULL의 return 값은 어떻게 될까? --> \n 포함 7byte가 나오는걸 확인 할수 있다.
    char *case_null = malloc(8);
    case_null[0] = 0;
    printf("NULL's return values == %d\n", printf("%s\n", *case_null));
    free(case_null);
    //그렇다면 그냥 빈 값이 나오는 경우는 어떤 값을 return 하는걸까? --> 1을 retrun 하는것을 알수 있다.
    printf("%d\n", printf("%c", NULL));
    //printf의 %d에 대해서 알아 보자.. 1. 오버플로우 2. 음수 오버플로우의 경우
        printf("---start %%d value\n");
    printf("case normal +1 == %d\n", 1);
    printf("case normal -1 == %d\n", -1);
    printf("case normal  0 == %d\n", 0);
    printf("int max printf return values == %d\n", printf("%d\n", 2147483647));//이 둘의 경우 반환값이 어떻게 될까? --> \n포함 11
    printf("int negative_max printf return values == %d\n", printf("%d\n", -2147483648));//\n포함 12인걸 확인 할 수 있다.
    //그렇다면 overflow가되면 어떻게 되는지 알아보도록 하자.
    printf("%d\n", 2147483648);//-->-2147483648가 출력이 된다.
    printf("%d\n", -2147483649);//-->2147483647가 출력이 된다.
    printf("print 16base %d\n", 0xff);
    printf("print 8base %d\n", 010);
    printf("print 16base %d\n", 0xf + 15);// 이렇게 더하는것까지 가능하다.
    //%i의 경우를 보자 --> %d랑 아예 같은값을 공유하는것을 알수 있다.
        printf("---start %%i value\n");
    printf("case normal +1 == %i\n", 1);
    printf("case normal -1 == %i\n", -1);
    printf("case normal  0 == %i\n", 0);
    printf("int max printf return values == %i\n", printf("%i\n", 2147483647));//이 둘의 경우 반환값이 어떻게 될까? --> \n포함 11
    printf("int negative_max printf return values == %i\n", printf("%i\n", -2147483648));//\n포함 12인걸 확인 할 수 있다.
    //그렇다면 overflow가되면 어떻게 되는지 알아보도록 하자.
    printf("%i\n", 2147483648);//-->-2147483648가 출력이 된다.
    printf("%i\n", -2147483649);//-->2147483647가 출력이 된다.
    printf("print 16base %i\n", 0xff);
    printf("print 8base %i\n", 010);
    //printf의 %u를 알아 보자 unsigned int로 숫자를 받아들인다고 한다.
    printf("---start %%u value\n");
    printf("case normal +1 == %u\n", 1);
    printf("case normal -1 == %u\n", -1);//overflow가 난걸 알수 있다.
    printf("case normal  0 == %u\n", 0);
    printf("int max printf return values == %u\n", printf("%u\n", 2147483647));//이 둘의 경우 반환값이 어떻게 될까? --> \n포함 11
    printf("int negative_max printf return values == %u\n", printf("%u\n", -2147483648));//\n포함 11인걸 확인 할 수 있다. --> 결과값은 2147483648으로 비트들의 숫자가 바뀐걸 눈으로 볼 수 있다.
    //그렇다면 overflow가되면 어떻게 되는지 알아보도록 하자.
    printf("%u\n", 2147483648);//-->-2147483648가 출력이 된다.
    printf("%u\n", -2147483649);//-->2147483647가 출력이 된다.
    printf("print 16base %u\n", 0xff);
    printf("print 8base %u\n", 010);
    //printf의 %x에 대해서 알아 보자.. 1. 오버플로우 2. 음수 오버플로우의 경우
    printf("---start %%x value\n");
    printf("case normal +1 == %x\n", 1);
    printf("case normal -1 == %x\n", -1);
    printf("case normal  0 == %x\n", 0);
    printf("int max printf return values == %d\n", printf("%x\n", 2147483647));//이 둘의 경우 반환값이 어떻게 될까? --> \n포함 11
    printf("int negative_max printf return values == %d\n", printf("%x\n", -2147483648));//\n포함 12인걸 확인 할 수 있다.
    //그렇다면 overflow가되면 어떻게 되는지 알아보도록 하자.
    printf("%x\n", 2147483648);//-->-2147483648가 출력이 된다.
    printf("%x\n", -2147483649);//-->2147483647가 출력이 된다.
    //printf의 %X에 대해서 알아 보자.. 1. 오버플로우 2. 음수 오버플로우의 경우
    printf("---start %%X value\n");
    printf("case normal +1 == %X\n", 1);
    printf("case normal -1 == %X\n", -1);
    printf("case normal  0 == %X\n", 0);
    printf("int max printf return values == %d\n", printf("%X\n", 2147483647));//이 둘의 경우 반환값이 어떻게 될까? --> \n포함 11
    printf("int negative_max printf return values == %d\n", printf("%X\n", -2147483648));//\n포함 12인걸 확인 할 수 있다.
    //그렇다면 overflow가되면 어떻게 되는지 알아보도록 하자.
    printf("%X\n", 2147483648);//-->-2147483648가 출력이 된다.
    printf("%X\n", -2147483649);//-->2147483647가 출력이 된다.
    printf("%X\n", 010);//8진법 또한 구해야한다.
    printf("%X\n", 0xff);
    printf("%X\n", 255);
    printf("---start %%c value\n");
    printf("printf A Hex == 0x41 == %c\n", 0x41);
    printf("printf A Oct == 0101 == %c\n", 0101);
    printf("printf A Dec == 65 == %c\n", 65);
    printf("printf NULL -->%c\n", NULL);//printf NULL -->^@ NULL 값이 찍히는 걸 알수 있다.
    printf("printf NULL -->%c\n", 0);//printf NULL -->^@ NULL 값이 찍히는 걸 알수 있다.
    printf("printf 129 -->%c\n", 129);//printf 129 -->M-^A$ 오버 플로우가 난걸 확인 할 수있다. 
    return (0);
}