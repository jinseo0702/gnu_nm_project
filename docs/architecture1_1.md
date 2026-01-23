nm 설계를 해보자

알아낸 결과 32/64 비트의 차이점은  st_info , st_other 의 해석 차이만 존재한다. 즉 이 과정만 고려하면 문제 없이 출력을 할 수 있다는 것이다.
1.0ver의 문제점은 구조체의 정규화가 되지 않았다. 
    - 구조체가 bit type 에 따라 존재하고 그 구조체에 맞는 함수들이 존재해야한다
    - 구조체를 하나로 통일을 할 수 있는 방법은 무엇 일까?
        * 내가 어떤 정보를 사용하고 어떤일을 하는지 알아보자.
            - ElfN_Shdr
                * sh_type : 이 멤버는 섹션의 내용과 의미를 분류합니다.
                    - 32 : uint32_t
                    - 64 : uint32_t
                * sh_flags : 섹션은 다양한 속성을 설명하는 1비트 플래그를 지원합니다.
                    - 32 : uint32_t
                    - 64 : uint64_t
            - ElfN_Sym
                * st_name : 심볼 문자열 테이블(symbol string table)에 대한 인덱스를 보유한다.
                    - 32 : uint32_t
                    - 64 : uint32_t                
                * st_value : 
                    - 32 : Elf32_Addr
                    - 64 : Elf64_Addr                
                * st_size :
                    - 32 : uint32_t
                    - 64 : uint64_t                
                * st_info :
                    - 32 : unsigned char
                    - 64 : unsigned char                
                * st_shndx :
                    - 32 : uint16_t
                    - 64 : uint16_t                


## 의사코드를 작성한다.

```c
//필요한 구조체
typedef struct s_Nm32Sym{
    Elf32_Sym    *Sym;
    const char  *SymName;
    Elf32_Addr   st_value;
    Elf32_Shdr  *section;
    uint32_t      st_size;
    uint16_t    st_shndx;
    char        type;
    char        print;
}t_Nm32Sym;

typedef struct s_Nm64Sym{
    Elf64_Sym    *Sym;
    const char  *SymName;
    Elf64_Addr   st_value;
    Elf64_Shdr  *section;
    uint64_t      st_size;
    uint16_t    st_shndx;
    char        type;
    char        print;
}t_Nm64Sym;

```

- 필수적인 내용
    1. malloc 등 시스템적 오류가 아니라면 프로그램을 종료하지 않는다.
    2. 오류는 (x macro) enum 을 사용하고 lookup table을 사용한다.
    3. 변수는 최대 4개의 인자를 받을 수 있다.
    4. 변수이름 명사로 지정한다. 카멜 케이스 를 사용한다.
    5. 함수이름은 행동을 알려줄 수 있어야한다. 스네이크 케이스 를 사용한다.
    6. libft 와 ft_printf를 사용해야 한다.
    7. 지시서에 적힌 내용을 이행해야하며, 지시서를 벗어나는 행동은 하면 안된다.
    8. 지시서에 적힌 내용이 아니라면 반드시 주석과 설명을 해야한다.
    9. 코드는 가독성이 좋게 만들어야한다.
    10. ElfN_Ehdr, ElfN_Addr, ElfN_Shdr, ElfN_Sym 의 Elf'N' 에서 'N' 은 elf 에서 디코딩한 bit 를 따른다. t_NmNSym 의 t_Nm'N'Sym 또한 elf 에서 디코딩한 bit 를 따른다.
    11. bit 에 따라서 구조체를 만들거나 분기가 많이 생기는경우 최대한 깔끔하고 복잡하지 않게 만든다.
    12. 각 함수는 모듈화 시켜야하며 재활용할 수 있어야한다.
    13. 코드는 사람이 이해하기 쉬워야하며 안전성을 최대 목표로 한다.
    14. 코드의 검수는 작성을 지시한 사람에게 책임이 있으며 코드 작성자는 코드 검수를 하지 않는다.
    15. 누수가 나면 안되고 만약 시스템적 오류라면 steal reachable 은 상관 없다.
    16. 만약 다음 분기로 넘어가야한다면 최상단 char **path 빼고 할당받거나 fd 가 열렸거나 mmap으로 할당을 받았다면 반드시 반납을 해야한다.
    17. 비트 Mask 에서 우선순위를 잘 생각해서 만들어야 한다. 
    18. achive 형식인(.a) 오브젝트로 인식하지 않는다 오류를 반환해야한다. 
    ```c
    #ifndef LIBFT_H
    # define LIBFT_H
    # include <stdlib.h>
    # include <unistd.h>
    # include "./gnl_check_bonus/get_next_line.h"
    # include "./gnl_check_bonus/get_next_line_bonus.h"

    size_t		ft_strlen(const char *str);
    size_t		ft_strlcpy(char *dst, const char *src, size_t size);
    size_t		ft_strlcat(char *dst, const char *src, size_t size);
    size_t		ft_strchr_len(const char *s, int c);
    int			ft_atoi(const char *nptr);
    long long	ft_atoi_longlong(const char *nptr);
    double		ft_atof(char *nptr);
    int			ft_isalpha(int c);
    int			ft_isdigit(int c);
    int			ft_isalnum(int c);
    int			ft_strrstr(const char *s1, const char *s2, int len);
    int			ft_isascii(int c);
    int			ft_isprint(int c);
    int			ft_isinstr(int c, const char *str);
    int			ft_toupper(int c);
    int			ft_tolower(int c);
    int			ft_strncmp(const char *s1, const char *s2, size_t n);
    int			ft_memcmp(const void *s1, const void *s2, size_t n);
    int			ft_isspace(char c);
    int			ft_onlyisspace(char *str);
    void		ft_bzero(void *s, size_t n);
    void		*ft_memset(void *s, int c, size_t n);
    void		*ft_memcpy(void *dest, const void *src, size_t n);
    void		*ft_memmove(void *dest, const void *src, size_t n);
    void		*ft_memchr(const void *s, int c, size_t n);
    void		*ft_calloc(size_t nmemb, size_t size);
    void		ft_putchar_fd(char c, int fd);
    void		ft_putstr_fd(char *s, int fd);
    void		ft_putendl_fd(char *s, int fd);
    void		ft_putnbr_fd(int n, int fd);
    void		ft_freenull(char **str);
    void		ft_free_two(char **arry);
    char		*ft_strchr(const char *s, int c);
    char		*ft_strrchr(const char *s, int c);
    char		*ft_strnstr(const char *big, const char *little, size_t len);
    char		*ft_strdup(const char *s);
    char		*ft_strdup_flag(const char *s, int *status);
    char		*ft_substr(char const *s, unsigned int start, size_t len);
    char		*ft_strjoin(char const *s1, char const *s2);
    char		*ft_strtrim(char const *s1, char const *set);
    char		**ft_split(char const *s, char c);
    char		**ft_split_str(char const *s, char *c);
    char		*ft_itoa(int n);
    char		*ft_strmapi(char const *s, char (*f)(unsigned int, char));
    char		*ft_strndup(const char *s, size_t n);
    void		ft_striteri(char *s, void (*f)(unsigned int, char*));

    #endif
    ```
    ```c
    #ifndef LIBFTPRINTF_H
    # define LIBFTPRINTF_H
    # include <unistd.h>
    # include <stdarg.h>

    void	*ft_memset(void *s, int c, size_t n);
    int		ft_printf(const char *str, ...);
    int		ft_putstr(const char *str);
    int		ft_putchar(unsigned char c);
    int		ft_putaddress(void *ptr);
    int   ft_putaddress_upper(void *ptr);
    int		ft_putnbr(int n);
    int		ft_putnbr_un(unsigned int n);
    int		ft_put_hex(unsigned int n);
    int		ft_put_hex_upper(unsigned int n);
    int		ft_put_octal(unsigned int n);
    int		ft_put_space(unsigned int n);

    int   ft_fprintf(size_t fd, const char *str, ...);
    int   ft_fputstr(const char *s, size_t fd);
    int   ft_fputchar(unsigned char c, size_t fd);
    int	  ft_fputaddress(void *ptr, size_t fd);
    int   ft_fputaddress_upper(void *ptr, size_t fd);
    int	  ft_fputnbr(int n, size_t fd);
    int	  ft_fputnbr_un(unsigned int n, size_t fd);
    int	  ft_fput_hex(unsigned int n, size_t fd);
    int	  ft_fput_hex_upper(unsigned int n, size_t fd);
    int	  ft_fput_octal(unsigned int n, size_t fd);
    int	  ft_fput_space(unsigned int n, size_t fd);

    #endif
    ```

포인터 산술 연산 이동 매크로
# define MOVE_ADDRESS(base, offset) ((unsigned char *)(base) + (offset))

1. 프로그램시작시 argc 를 확인한다.
    - argc를 활용해서 path를 담을 수 있는 char **path 을 만들고 ft_calloc(argc, sizeof(char *)); 를 할당한다.
        * 이차원 포인터를 안전하게 해제할 수 있는 free함수가 존재해야한다. 할당을 해제하면 그 주소값을 다시 NULL 로 채워 넣는 행동을 해야한다.
    - argc == 1
        * a.out 이라는 파일의 이름을 기본으로 찾고 없다면 오류를 반환한다.
        * path_cnt = 1 이된다.
        * path 일때 path[path_cnt] = ft_strdup("a.out") 를 활용해서 path를 할당한다.
    - argc > 1
        * 파싱을 시작한다.
            * i > 1 : argv[i][0] 이 '-' 이고 argv[i] 문자열의 크기가 1이상 이라면 (n > 0) argv[i][n] 부터 끝까지 option 으로 인식한다. 만약 argv[i] 문자열의 크기가 1 이라면 argv[i] 는 경로로 판단한다.
            * option 으로 간주했다면 'a', 'g', 'u', 'r', 'p', 'n' 은 option Argument로 인식한다. option Argument 는 중복이 존재 할 수있다.
            * option 의 인자값이 이 'a', 'g', 'u', 'r', 'p', 'n' 포함되지 않는다면 Error 메세지를 반환 하고 프로그램을 종료한다.
                - option Argument 의 중복을 처리하기 위해서 bitmask 를 활용한다.
                    - (1 << 1) OPT_a 
                    - (1 << 2) OPT_g
                    - (1 << 3) OPT_u
                    - (1 << 4) OPT_r
                    - (1 << 5) OPT_p
                    - (1 << 6) OPT_n
                    - 비트 마스킹을 활용하기 위한 도구 매크로 또한 활용한다.
                    - HASOPT(opt, flag) (((opt) & (flag)) != 0)
                    - SETOPT(opt, flag) ((opt) |= (flag))
            * path 일때 path[path_cnt] = ft_strdup(argv[i]) 를 활용해서 path를 할당한다.
            * path의 갯수를 count 하기 위한 path_cnt 에 path가 나올때마다 1을 증가시킨다.
            * path_cnt 가 0 이고 option 끝까지 문제가 없이 도달 했다면 argc == 1 일때를 수행한다.
    - 반복문으로 index < path_cnt (index는 1 씩 증가) 로반복한다.
        - int fd = open(path[index], O_RDONLY) 로 파일의 유무를 확인한다. (어떤한 이유로 이분기를 끝내야 하고 fd가 열려있다면 반드시 닫아 줘야한다)
            * 오류가 있다면 Error message를 반환 하고 다음 path 를 찾는다.
        - struct stat stat_temp; 를 생성하고 fstat(fd, &stat_temp)를 찾는다.
            * 오류가 있다면 Error message를 반환 하고 다음 path 를 찾는다.
        - stat_temp  S_ISREG(stat_temp.st_mode) 로 REGULAR 인지 확인한다.
            * 오류가 있다면 Error message를 반환 하고 다음 path 를 찾는다.
        - stat_temp  stat_temp.st_size > sizeof(ElfN_Ehdr) 로 유효한 사이즈인지를 확인한다.
            * false가 나오면 Error message를 반환 하고 다음 path 를 찾는다.
            * true 라면 stat_temp.st_size 를 기억한다.
        - ElfN_Ehdr *ElfN = mmap(NULL, stat_temp.st_size, PROT_READ , MAP_PRIVATE, fd, 0) 을 활용해서 현재 open 된 fd 를 찾고 읽기 전용으로 복사를 해온다.
            * 오류가 있다면 Error message를 반환 하고 다음 path 를 찾는다.
        - unsigned char *e_ident = ElfN->e_ident;
            * if(e_ident[EI_MAG0] == 0x7F && e_ident[EI_MAG1] == E && e_ident[EI_MAG2] == L && e_ident[EI_MAG3] == F) 
                - false 라면 Error message를 반환 하고 다음 path 를 찾는다.
            * if(e_ident[EI_CLASS] == ELFCLASS32 || e_ident[EI_CLASS] == ELFCLASS64) 
                - false 라면 Error message를 반환 하고 다음 path 를 찾는다.
                - true 라면 bit의 유형을 기억한다.
            * if(e_ident[EI_OSABI] == ELFOSABI_LINUX)
                - false 라면 Error message를 반환 하고 다음 path 를 찾는다.
        - uint16_t e_type = ElfN->e_type;
            * if(e_type == ET_REL || e_type == ET_EXEC || e_type == ET_DYN)
                - false 라면 Error message를 반환 하고 다음 path 를 찾는다.
        - uint16_t e_machine = ElfN->e_machine;
            * if(e_machine == EM_386 || e_machine == EM_X86_64)
                - false 라면 Error message를 반환 하고 다음 path 를 찾는다.
                - true 라면
                    아까 기억한 bit의 유형 비트를 확인한다. 이후 e_machine 의 비트유형과 같은지 비교.
                        - false 라면 Error message를 반환 하고 다음 path 를 찾는다.
        - uintN e_shoff = ElfN->e_shoff; 비트유형에 맞는 자료형을 가져 온다.
            * if (e_shoff == 0)
                - true 라면 section header table 이 존재하지 않는다고 판단하고 message 를 message를 반환 하고 다음 path 를 찾는다.
        - 
            ```c
            uint64_t total_shdr_size = (uint64_t)(ElfN->e_shnum * ElfN->e_shentsize);
            uint64_t shdr_end_offset = (uint64_t)(ElfN->e_shoff + total_shdr_size);
            uint64_t file_limit = (uint64_t)stat_temp.st_size;
            ```
        - if (shdr_end_offset > file_limit)
            - true 라면 Error message를 반환 하고 다음 path 를 찾는다.
        - ElfN_Shdr *shdr =  (ElfN_Shdr *)MOVE_ADDRESS(ElfN, e_shoff) 로 이동한다.
            *
            ```c
                for (int i = 0; i < elf_64->e_shnum; i++) {
                    if (shdr[i].sh_type == SHT_SYMTAB) {
                        sym_section_temp = &shdr_64[i];
                        break;
                    }
                }
            ```
            * if (sym_section_temp == NULL)
                - true 라면 message 를 반환하고 다음 path 를 찾는다.
            * 
            ```c
                uint64_t shdr_end_offset = (uint64_t)(sym_section_temp->sh_offset + sym_section_temp->sh_size)
                uint64_t file_limit = (uint64_t)stat_temp.st_size;
            ```
            * if (shdr_end_offset > file_limit)
                - true 라면 Error message를 반환 하고 다음 path 를 찾는다.
            * if ( sym_section_temp->sh_size == 0 || sym_section_temp->sh_entsize)
                - true 라면 Error message를 반환 하고 다음 path 를 찾는다.
            * long cnt = sym_section_temp->sh_size / sym_section_temp->sh_entsize; 로 cnt 를 구한다.
            * t_NmNSym *symData = ft_calloc(cnt, sizeof(t_NmNSym))
            * ElfN_Sym *Sym =  (ElfN_Sym *)MOVE_ADDRESS(ElfN, sym_section_temp->sh_offset) 으로 이동한다.
            * char *Sym_str = (char *)MOVE_ADDRESS(ElfN, shdr[sym_section_temp->sh_link].sh_offset);
            ```c
                for(int i = 0; i < cnt; i++){
                    symData[i].Sym = &Sym[i];
                    symData[i].SymName = &Sym_str[Sym[i].st_name];
                    symData[i].st_value = Sym[i].st_value;
                    symData[i].st_shndx = Sym[i].st_shndx;
                    symData[i].section =  NULL;
                    if ( symData[i].st_shndx != SHN_UNDEF &&  symData[i].st_shndx < SHN_LORESERVE) symData[i].section =  &shdr[symData[i].st_shndx];
                    symData[i].st_size = Sym[i].st_size;
                    symData[i].type = 0;
                    symData[i].print = 0;
                }
            ```
            * 반복문을 돌면서 Type 을 정한다.
            ```c
                for(int i = 0; i < cnt; i++){
                    ...
                }

            ```
            * type 에는 우선 순위가 존재한다. 고로 반복문과 switch 문을 사용해서 다음과 같은 우선순위를 맞춘다

            | 기존 상태 | 새로운 심볼 | 결과 | 우선순위 |
            | --- | --- | --- | --- |
            | Weak Def | **Strong Def** | Strong Def 선택 | Strong > Weak |
            | Strong Def | Weak Def | Strong Def 유지 | Strong > Weak |
            | Dynamic Def | **Regular Def** | Regular Def 선택 | Regular > Dynamic |
            | Regular Def | Dynamic Def | Regular Def 유지 | Regular > Dynamic |
            | Common | **Def** | Def 선택 | Def > Common |
            | Def | Common | Def 유지 | Def > Common |
            | Undef | **Def** | Def 선택 | Def > Undef |
            | Undef | **Common** | Common 선택 | Common > Undef |
            | **Strong Def** | Strong Def | Multiple Definition Error | 동일 레벨 충돌 |
            
            * type 을 정하는 Rule 은 다음 과 같다.

            ```text
                사용안하는것
                c, g, G, s, S

                사용하는 symbol type

                A/a : 
                ElfN_Sym 의 st_shndx 가 SHN_ABS 인경우
                shdr_N->st_info 의 ELFN_ST_BIND 가 STB_LOCAL 인경우 : a
                shdr_N->st_info 의 ELFN_ST_BIND 가 STB_GLOBAL 인경우 : A

                B/b:
                ElfN_Shdr의 sh_type 이 SHT_NOBITS  인경우 &&
                ElfN_Shdr의
                (sh_flag & SHF_ALLOC) == 1 
                (sh_flag & SHF_WRITE) == 1 
                shdr_N->st_info 의 ELFN_ST_BIND 가 STB_LOCAL 인경우 : b
                shdr_N->st_info 의 ELFN_ST_BIND 가 STB_GLOBAL 인경우 : B

                C:
                ElfN_Shdr의 sh_type 이 SHT_NOBITS  인경우 &&
                ElfN_Shdr의
                (sh_flag & SHF_ALLOC) == 1 
                (sh_flag & SHF_WRITE) == 1 
                ElfN_Sym 의 st_shndx 가 SHN_COMMON 인경우

                D/d:
                ElfN_Shdr의 sh_type 이 SHT_PROGBITS  인경우 &&
                ElfN_Shdr의
                (sh_flag & SHF_ALLOC) == 1 
                (sh_flag & SHF_WRITE) == 1 
                shdr_N->st_info 의 ELFN_ST_BIND 가 STB_LOCAL 인경우 : d
                shdr_N->st_info 의 ELFN_ST_BIND 가 STB_GLOBAL 인경우 : D

                i: 
                ElfN_Sym -> st_info 의 ELFN_ST_TYPE 가 STT_GNU_IFUNC 인 경우

                I: 
                구현하지 않겠다. 
                심볼은 다른 심볼에 대한 간접 참조(indirect reference)입니다. 
                위 사항을 구현 방법은 현재로써는 어렵기때문에 구현하지 않겠다

                N:
                ElfN_Shdr 의 sh_type 이 SHT_PROGBITS 이면서
                sh_flag 가 0(none) 인경우

                n:
                ElfN_Shdr 의 sh_type 이 SHT_PROGBITS 이면서
                sh_flag 이 (sh_flag & SHF_WRITE) == 0  인경우

                p:
                64bit기준만 고려 한다.
                ElfN_Shdr 의 sh_type 이 SHT_X86_64_UNWIND 이면 p 이다.

                R/r:
                ElfN_Shdr 의 sh_type 이 SHT_PROGBITS 이면서
                sh_flag 이 (sh_flag & SHF_WRITE) == 0  이면서
                sh_flag 이 SHF_ALLOC 인 경우
                shdr_N->st_info 의 ELFN_ST_BIND 가 STB_LOCAL 인경우 : r
                shdr_N->st_info 의 ELFN_ST_BIND 가 STB_GLOBAL 인경우 : R

                T/t:
                ElfN_Shdr 의 sh_type 이 SHT_PROGBITS 이면서
                sh_flag 이 SHF_ALLOC  이면서 SHF_EXECINSTR 인경우
                shdr_N->st_info 의 ELFN_ST_BIND 가 STB_LOCAL 인경우 : t
                shdr_N->st_info 의 ELFN_ST_BIND 가 STB_GLOBAL 인경우 : T

                U: 
                ElfN_Sym 의 st_shndx 가 SHN_UNDEF 인경우 그리고
                symbol ->st_info 의 ELFN_ST_BIND 가  STB_WEAK 이 아닌경우

                u:
                shdr_N->st_info 의 ELFN_ST_BIND 가 STB_GNU_UNIQUE 인경우

                V/v : 
                The symbol is a weak object. 를 근거로 
                symbol ->st_info 의 ELFN_ST_BIND 가  STB_WEAK 일때 
                symbol ->st_info 의 ELFN_ST_TYPE 이 STT_OBJECT 이고
                symbol->st_shndx 가 SHN_UNDEF(정의 되지 않았다면) v
                symbol->st_shndx 가 정의 되었다면 V

                W/w :
                 man page 보면 not been tagged as a weak object symbol 를 근거로
                symbol ->st_info 의 ELFN_ST_BIND 가  STB_WEAK 일때 
                symbol ->st_info 의 ELFN_ST_BIND 가  STB_WEAK 아닐때
                symbol->st_shndx 가 SHN_UNDEF(정의 되지 않았다면) w
                symbol->st_shndx 가 정의 되었다면 W

                -:
                N 으로 통합이 되었음

                ?:
                위 내용이 아닌경우 
            ```

        - option 을 적용한다.

        ```
        1. 필터링 순위: -a, -g, -u
        2. 정렬 순위: -n
        3. 형식 순위: -P

        a:
        type N, a 을 출력에 포함 시킵니다.
        g:
        type 이 대문자 'A', 'B', 'D', 'R', 'T', 'W', 'w'
        u:
        type == U || type == w 만 출력 합니다.


        n:
        quick sort 를 사용한다.
        symData->st_value 를 기준으로 오름차순
        n이 없다면 
        const char  *SymName 기준으로 오름 차순

        r:
        quick sort 를 사용한다.
        n 이 존재한다면
        symData->st_value 를 기준으로 내림 차순
        n 이 존재하지 않는다면
        const char  *SymName 기준으로 내림 차순
        ```

            * a 옵션 있다면

            ```c
                for(int i = 0; i < cnt; i++){
                    symData[i].print = 1;
                }
            ```

            * a 옵션이 없는경우

            ```c
                for(int i = 0; i < cnt; i++){
                    if(symData[i].type != 'N' && \
                        symData[i].type != 'a' && \
                    ) symData[i].print = 1;
                    else {
                        symData[i].print = 0;
                    }
                }
            ```            

            * g 옵션이 있다면

            ```c
                for(int i = 0; i < cnt; i++){
                    if(symData[i].type == 'A' || \
                        symData[i].type == 'B' || \
                        symData[i].type == 'D' || \
                        symData[i].type == 'R' || \
                        symData[i].type == 'T' || \
                        symData[i].type == 'W' || \
                        symData[i].type == 'w' || \
                    ) symData[i].print = 1;
                    else {
                        symData[i].print = 0;
                    }
                }
            ```

            * u 옵션이 있다면

            ```c
                for(int i = 0; i < cnt; i++){
                    if(symData[i].type == 'U' || \
                        symData[i].type == 'w' || \
                    ) symData[i].print = 1;
                    else {
                        symData[i].print = 0;
                    };
                }
            ```

            * n 옵션이 있는경우
                - r 옵션이 있는경우
                    * symData[i]->st_value 를 기준으로 t_NmNSym *symData 를 내림 차순
                - r 옵션이 없는경우
                    * symData[i]->st_value 를 기준으로 t_NmNSym *symData 를 오름 차순
            
            * n 옵션이 없는경우
                - const char  *SymName 기준으로 t_NmNSym *symData 를 오름 차순
            
            * r 옵션이 있는경우
                - n 옵션이 있는경우
                    * n 과 r 이 있는경우와 같음
                - n 옵션이 없는경우
                    * const char  *SymName 기준으로 t_NmNSym *symData 를 내림 차순

        - 정렬은 기본적으로 quickSort를 사용
        ```c
        // 1. Swap 로직: 구조체 직접 대입 (memcpy보다 직관적이고 효율적)
        void swap(t_NmNSym *arr, int i, int j) {
            t_NmNSym temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }

        // 이름 비교 함수 (ASCII 순)
        int compare_name(t_NmNSym *a, t_NmNSym *b) {
            return ft_strncmp(a->SymName, b->SymName, 214748);
        }

        // 주소 비교 함수 (보완된 로직: 주소 같으면 이름순)
        int compare_numeric(t_NmNSym *a, t_NmNSym *b) {
            if (a->st_value < b->st_value) return -1;
            if (a->st_value > b->st_value) return 1;
            // 주소가 같으면 이름순으로 정렬 (nm의 표준 동작)
            return compare_name(a, b);
        }

        // 통합 파티션 로직
        int partition(t_NmNSym *arr, int start, int end, int is_numeric) {
            t_NmNSym pivot = arr[(start + end) / 2];

            while (start <= end) {
                if (is_numeric) {
                    while (compare_numeric(&arr[start], &pivot) < 0) start++;
                    while (compare_numeric(&arr[end], &pivot) > 0) end--;
                } else {
                    while (compare_name(&arr[start], &pivot) < 0) start++;
                    while (compare_name(&arr[end], &pivot) > 0) end--;
                }

                if (start <= end) {
                    swap(arr, start, end);
                    start++;
                    end--;
                }
            }
            return start;
        }

        // 퀵 정렬 메인 함수
        void quickSort(t_NmNSym *arr, int start, int end, int is_numeric) {
            if (start >= end) return;

            int p = partition(arr, start, end, is_numeric);
            quickSort(arr, start, p - 1);
            quickSort(arr, p, end);
        }

        // 2. 내림차순(-r) 처리: 정렬 완료 후 배열을 뒤집음
        void reverse_symbols(t_NmNSym *arr, int size) {
            int i = 0;
            int j = size - 1;
            while (i < j) {
                swap(arr, i, j);
                i++;
                j--;
            }
        }
        ```

    - 출력할 시에 path_cnt > 1 이상인경우 처음 한번 "<path>:\n" 을 출력한다.
        - 출력형식 P 옵션이 없는경우
            * t_NmNSym 구조체에서 print == 1 인 것만 출력한다
        ```c
        void write_hex(ElfN_Addr addr, char *dest) {
            int i = 15;
            const char *base = "0123456789abcdef";

            while (i >= 0) {
                dest[i] = base[addr & 0xF]; // 마지막 4비트 추출
                addr >>= 4;                // 4비트 오른쪽으로 밀기
                i--;
            }
            dest[16] = '\0';
        }
        ```
        ```c

        char address[17] = {0,};
        write_hex(st_value, address);
        if (!(type == U || type == w)){
            ft_printf("%s ", address);
        }
        else {
            ft_printf("%*", 17);
        }
        ft_printf("%c ", type);
        ft_printf("%S\n", SymName);
        ```

        - 출력형식 P 옵션이 있는경우
                    * t_NmNSym 구조체에서 print == 1 인 것만 출력한다
        ```c
        ft_printf("%S ", SymName);
        ft_printf("%c ", type);
        if (st_value != 0) ft_printf("%x ", st_value);
        if (st_size != 0) ft_printf("%x ", st_size);
        ft_printf("%c ", type);


        ```
    - 출력이 완료 되었다면 이 path 안에서 에서 메모리를 할당받거나 open 한 fd 가 있다면 free 또는 close 를 한다. (반복문으로 index < path_cnt (index는 1 씩 증가) 로반복한다.) 로 돌아간다.

    - 전부 종료가 되었다면 char **path 를 해제한다.