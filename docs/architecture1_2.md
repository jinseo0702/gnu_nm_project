#nm 설계를 해보자
#1.2 ver


```c
//Up Scale copy 를 이용해서 32bit elf 정보를 정규화한다.
//필요한 구조체
//base는 현재 해석 단위의 시작 주소이다 (일반 ELF: mmap 시작, archive member: member payload 시작)
typedef struct s_MetaData{
    union {
        const Elf32_Ehdr    *Ehdr32;
        const Elf64_Ehdr    *Ehdr64;
    }ElfN_Ehdr;
    union {
        const Elf32_Shdr    *Shdr32;
        const Elf64_Shdr    *Shdr64;
    }ElfN_Shdr;
    union {
        const Elf32_Sym    *Sym32;
        const Elf64_Sym    *Sym64;
    }ElfN_Sym;
    const unsigned char *base;
    uint64_t strtab_offset; //strtab_offset은 현재 심볼 테이블(symtab/dynsym)이 참조하는 문자열 테이블(.strtab/.dynstr)의 파일 오프셋이다.
    uint64_t file_limit; //struct stat 의  off_t     st_size; 이다.
    uint8_t elf_class; // bit type 을정한다.
}t_MetaData;

typedef struct s_NmShdrData{
    uint64_t sh_type;
    uint64_t sh_flags;
}t_NmShdrData;

typedef struct s_NmSymData{
    uint64_t st_value;
    uint64_t st_size;
    uint32_t st_name; //st_name 으로 추출한 문자열은 은 함수로만 접근하며 범위를 체크하는 조건을 넣어야한다.
    uint16_t st_shndx;
    uint8_t st_info_type;
    uint8_t st_info_bind;
    unsigned char type;
}t_NmSymData;

typedef enum e_nm_option {
    OPT_a = (1 << 0),
    OPT_g = (1 << 1),
    OPT_u = (1 << 2),
    OPT_r = (1 << 3),
    OPT_P = (1 << 4),
    OPT_n = (1 << 5),
}t_nm_option;

```

- 필수적인 내용
    1. malloc 등 시스템적 오류가 아니라면 프로그램을 종료하지 않는다. 알수 없는 옵션은 fatal 로 간주한다.
    2. 오류는 (x macro) enum 을 사용하고 lookup table을 사용한다.
    3. 변수는 최대 4개의 인자를 받을 수 있다.
    4. 변수이름 명사로 지정한다. 카멜 케이스 를 사용한다.
    5. 함수이름은 행동을 알려줄 수 있어야한다. 스네이크 케이스 를 사용한다.
    6. libft 와 ft_printf를 사용해야 한다. 또한 libft ,ft_printf 디렉토리안에 있는 내용은 수정하지 않는다. 
    7. 지시서에 적힌 내용을 이행해야하며, 지시서를 벗어나는 행동은 하면 안된다.
    9. 코드는 가독성이 좋게 만들어야한다.
    11. bit 에 따라서 구조체를 만들거나 분기가 많이 생기는경우 최대한 깔끔하고 복잡하지 않게 만든다.
    12. 각 함수는 모듈화 시켜야하며 재활용할 수 있어야한다.
    13. 코드는 사람이 이해하기 쉬워야하며 안전성을 최대 목표로 한다.
    14. 코드의 검수는 작성을 지시한 사람에게 책임이 있으며 코드 작성자는 코드 검수를 하지 않는다.
    15. 누수가 나면 안되고 만약 시스템적 오류라면 steal reachable 은 상관 없다.
    16. 만약 다음 분기로 넘어가야한다면 최상단 char **path 빼고 할당받거나 fd 가 열렸거나 mmap으로 할당을 받았다면 반드시 반납을 해야한다.
    17. 비트 Mask 에서 우선순위를 잘 생각해서 만들어야 한다. 
    18. archive format 은 ar.h 에 존재하는 format 을따르며, MagicNumber 를 확인한다.
        - 또한 archive member의 payload(base+member_offset, size=ar_size)를 독립적인 file_limit으로 보고, 그 payload에서 ELF magic을 다시 검사한다.
        - ar 멤버 offset 계산 시 짝수 패딩(align) 반영한다.
        - ar_name은 16바이트 안에서만 파싱한다. 끝은 '/ 또는 공백 패딩 트림으로 처리한다.
        - "/<digits>"일 때만 string table에서 복원 그 외는 ar_name[16] 필드에서 직접 trim해서 이름 추출
        - "/<digits>" 는 문자열의 끝에 0x20으로 종료된다.
        - ar_name 은 '\0' 로 끝나지 않고 문자열'/'로 종료된다.
        - ar_fmag 을 memcmp 로 반드시 확인하며 없다면 심각한 오류로 판단하고 ar format 에 할당된 자원을 반납하고 다른 path를 해석한다.
    20. elf format 의 Magic number 는 반드시 확인한다.
    21. 경로의 분기 또는 오류로 인해서 다음 경로로 이동 또는 상황을 진행해야한다면 그 분기에서 할당받은 자원은 반납해야한다.
        - malloc 계열
        - mmap
        - open
    22. malloc 계열의 자원을 반납하는경우 자원반납을 한다음 NULL 가드를 통해서 안정선을 높인다.
    23. 문자열 복사는 ft_strdup() 을 사용한다.
    24. libft, ft_printf 디렉토리내 함수를 제외한 사용가능한 함수는 아래와 같다.
        - open(2)
        - close(2)
        - mmap(2)
        - munmap(2)
        - write(2)
        - fstat(2)
        - malloc(3)
        - free(3)
        - exit(3)
        - perror(3)
        - strerror(1)
        - getpagesize(2)
    25. linux 기반 x86_32, x86_64를 해석한다.
    26. data는 자료형이 다르다면 up Scale 을통해서 비교한다. 기준은 uint64_t 이다.
    27. offset을 더하는 base는 "현재 해석 단위의 시작 주소"이며, 일반 ELF는 mmap 시작, archive member는 member payload 시작이다.
    28. libft의 경로는 ./libft
    29. ft_printf의 경로는 ./ft_printf
    30. 작업해야하는 디렉토리는
        - ./src
        - ./include
    31. 코드 실행 를 실행해서 검증은 하지 않는다.
    32. 코드의 설명이나 주석은 달지 않는다.
    33. 모든 접근은 offset <= file_limit 및 size <= file_limit - offset 형태로 검사한다.
    34. 섹션 요약 배열은 타입 판정에서 shdr 원본 접근을 줄이기 위한 캐시다.
    35. CHECK_RANGE는 offset <= file_limit을 먼저 검사하고, 그 다음 size <= file_limit - offset을 검사한다.
    36. MOVE_ADDRESS는 const base를 받아 const pointer를 반환한다.
---

- 매크로 and static inline func  

    ```c
    // 포인터 산술연산
    //아래 정의는 개념 예시이며, 실제 구현은 36을 따른다
    //MOVE_ADDRESS는 범위검사를 하지 않는다. 호출 전 CHECK_RANGE를 통과해야 한다.
        static inline const void *MOVE_ADDRESS(const void *base, uint64_t offset) {
            if (base == NULL) return NULL;
            const unsigned char *p = (const unsigned char*)base;
            return (const void*)(p + offset);
        }
    // 범위체크
    //아래 정의는 개념 예시이며, 실제 구현은 35을 따른다
        static inline uint8_t CHECK_RANGE(uint64_t offset, uint64_t size, uint64_t limit) {
                if (offset > limit) return 0;
                return (size <= (limit - offset));
        }
    // option check
        #define HASOPT(flag, option) (((flag) & (option)) != 0)
        #define SETOPT(flag, option) ((flag) |= (option))
    // error enum
        #define ERROR_LIST \
            X(ERR_MALLOC, "ERR_MALLOC") \
            X(ERR_OPENDIR, "ERR_OPENDIR") \
            ...

        typedef enum e_error {
            #define X(id, str) id,
            ERROR_LIST
            #undef X
            ERROR_END
        } t_error;

        const char *Error_table[] = {
            #define X(id, str) [id] = str,
            ERROR_LIST
            #undef X
        }

        #ifdef DEBUG
            #define NM_LOG(err) real_print_error(err, __FILE__, __LINE__)
        #else
            #define NM_LOG(err) real_print_error(err, NULL, 0)
        #endif

        static inline void real_print_error(t_error err, const char *file, int line) {
            ft_fprintf(2, "nm [%s] -> %s\n",get_error_msg(err) ,strerror(errno));
            if(file != NULL) ft_fprintf(2, "file : %s line : %d\n", file, line);
        }
    ```

## nm 프로세스 관련 사항

- 프로그램시작시 argc 를 확인한다.
    * argc를 활용해서 path를 담을 수 있는 이차원 동적메모리를 ft_calloc 을 통해서 할당한다.
    * path_cnt 라는 변수를 통해서 path의 갯수를 확인한다.
    * argc == 1
        - a.out 이라는 파일의 이름을 기본으로 찾고 없다면 오류를 반환한다.
        - path_cnt = 1 이된다.
        - path 일때 path[path_cnt] = ft_strdup("a.out") 를 활용해서 path를 할당한다.
    * argc > 1
        - option 의 기준은 문자열의 0번째가 '-' 이고 문자열의 크기가 2 이상으로 한다.
        - option 의 인자값은 중복이 허용된다.
        - option 은 비트마스크를 통해서 옵션을 파싱한다.
        - option 의 기준이 아닌경우 path로 판단한다.
        - path 인 경우 path[path_cnt] 에 문자열을 복사하고 path_cnt 를 증가시킨다.
        - 만약 path는 없고 option 만 존재한다면 argc == 1 로직을 따른다.
        - 옵션만 있는경우 무조건 값은 a.out 이란 elf format 으로 해석한다. 
- path_cnt 를 기준으로 반복해서 nm 로직을 실행한다.
    * t_MetaData 를 사용해서 연산의 대상(cpu bit type 별) 안전성을 추구한다.
        - class에 따라 해석할 구조체 타입(Ehdr/Shdr/Sym) 을 선택한다
        - 이동은 base + offset(바이트 단위)로 한다
    * open
        - O_RDONLY 로 파일을 읽기 모드로만 확인한다.
    * fstat
        - st_mode 를통해서 st_mode REGULAR 인지 확인한다.
        - stat_temp.st_size >= sizeof(Elf32_Ehdr) 먼저 체크하고,
        - class가 64면 stat_temp.st_size >= sizeof(Elf64_Ehdr)도 체크
        - file_limit = (uint64_t)stat_temp.st_size;
    * mmap
        - EI_CLASS 결정 후 Ehdr32/64로 캐스팅한 뒤 e_shentsize를 검증한다.
        - mmap 결과는 meta.base로 받는다
        - EI_CLASS 결정 후 meta.ElfN_Ehdr.Ehdr32/Ehdr64에 base를 캐스팅해 저장한다
        - mmap(NULL, file_limit, PROT_READ , MAP_PRIVATE, fd, 0);
        - 읽기전용과 복사본으로만 확용한다.
        - e_ident
            * magic 넘버는 반드시 확인한다.
            * EI_CALSS : ELFCLASS32 , ELFCLASS64 
            * EI_DATA : ELFDATA2LSB
        - e_type
            * ET_REL, ET_EXEC, ET_DYN
        - e_machine
            * EM_386, EM_X86_64
        - e_shoff
            * 값이 0 이라면 오류로 판단하고 다음 경로분기로 이동한다.
        - e_shnum * e_shentsize + e_shoff
            * e_shnum == 0 이라면 오류로 판단한다.
            * 위 값들을 기준으로 매크로를 활용해서 file_limit 과 범위 체크를한다.
- 포인터 산술연산과 반복문을 통해서 section header 를 찾는다.
    * sh_type 을 통해서 우선적으로 SHT_SYMTAB 를 찾는다.
        - 존재 하지 않는다면 SHT_DYNSYM 을 확인한다.
        - 선택된 심볼 테이블 섹션(=symtab 또는 dynsym)의 sh_offset으로 meta.ElfN_Sym을 세팅한다
        - 만약 둘다 없다면 메세지를 출력하고 다음 경로 분기로 이동한다.
        - Shdr은 e_shoff 검증 후 base+e_shoff를 캐스팅하여 meta에 저장한다
        - Sym은 symtab 섹션 발견 후 sh_offset 검증 후 base+sh_offset을 캐스팅하여 meta에 저장한다
    * section 범위 체크를 한다
        - sh_offset + sh_size , file_limit
        - sh_size == 0
        - sh_entsize == 0
        - sh_link < e_shnum
        - SHT_SYMTAB/DYNSYM의 cnt 계산은 resolved_entsize(0이면 sizeof로 치환한 값)로 수행한다.
        - shdr[sh_link] 의 값 또한 section 범위체크를 한다.
            * sh_entsize == 0 이면 sizeof(ElfN_Sym)로 간주
    * t_NmShdrData
        - 고정크기의 동적할당 배열을 만들고 반복문을 통해서 ft_calloc 을 사용한다.
        - 각 구조체에 값을 채워 넣는다.
        - 0 < index < e_shnum 을 지켜야한다.
    * SHT_SYMTAB or SHT_DYNSYM 의 값들을 저장하기 위한 구조체를 할당한다.
        - sh_size / sh_entsize 로 cnt룰 구한다.
        - cnt 를 기반으로 t_NmSymData 의 ft_calloc 으로 고정크기 배열을만들어서 사용한다.
        - 각 symbol table의 값의 range 유효성검사를 하며 type 을 제외한 값을 채워 넣는다.
        - symbol 의 이름이 있는 section 이 shdr[sh_link].sh_offset 의 st_name 이 file_limit 의 범위를 확인해야한다.
        - ft_memchr(strtab+st_name, '\0', file_limit-(strtab_offset+st_name)) 로 문자열의 NULL이 있는지 확인한다.
- symbol type 정하기
    *  type 에는 우선 순위가 존재한다. 고로 반복문과 switch 문을 사용해서 다음과 같은 우선순위를 맞춘다
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
    (sh_flag & SHF_ALLOC) != 0
    (sh_flag & SHF_WRITE) != 0 
    shdr_N->st_info 의 ELFN_ST_BIND 가 STB_LOCAL 인경우 : b
    shdr_N->st_info 의 ELFN_ST_BIND 가 STB_GLOBAL 인경우 : B

    C:
    ElfN_Shdr의 sh_type 이 SHT_NOBITS  인경우 &&
    ElfN_Shdr의
    (sh_flag & SHF_ALLOC) != 0 
    (sh_flag & SHF_WRITE) != 0
    ElfN_Sym 의 st_shndx 가 SHN_COMMON 인경우

    D/d:
    ElfN_Shdr의 sh_type 이 SHT_PROGBITS  인경우 &&
    ElfN_Shdr의
    (sh_flag & SHF_ALLOC) != 0
    (sh_flag & SHF_WRITE) != 0 
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
    여기서 p는 옵션이 아니라 symbol type 문자이며, 옵션은 -P만 존재한다.
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
    symbol ->st_info 의 ELFN_ST_TYPE 가  STT_OBJECT 아닐때
    symbol->st_shndx 가 SHN_UNDEF(정의 되지 않았다면) w
    symbol->st_shndx 가 정의 되었다면 W

    -:
    N 으로 통합이 되었음

    ?:
    위 내용이 아닌경우 
    ```

- option
    * 필수 숙지 사항
        - n 옵션이 없다면 Symbolname 을 기준으로 정렬한다.
        - n 옵션이 있다면 st_value를 기준으로 정렬한다.
    * 옵션은 다음과 같은 우선 순위가 존재한다. 같은 우선순위 안이라면 교집합 으로 동작한다.
        1. 'a' , 'g' , 'u'
        2. 'n'
        3. 'P'

    * a : type N, a 을 출력에 포함 시킵니다.
    * g : type 'A', 'B', 'D', 'R', 'T', 'W', 'w', 'U'
    * u : type == U || type == w 만 출력 합니다.
    * n : quick sort 로 구현하고 swap 은 stack 메모리의 temp 를 활용한다.
        - st_value 가 동일한 상황이라면 이름순서로 정렬한다.
        - 이름 추출 실패 시 빈 문자열 취급
    * r : 정렬을 뒤집는다.
        - n 유무에 따라서 reverse 정렬 기준이 변경 된다.
    * P : POSIX.2 표준 출력 형식을 사용한다.
    * none option
        - a 옵션의 조건을 제외한 내용을 출력한다.

- 다음 경로의 분기 점이 없는 상황이라면 path를 담았던 이차원 배열을 해제 하고 종료한다.