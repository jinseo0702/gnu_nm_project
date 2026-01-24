#nm 설계를 해보자
#1.2 ver

```c
//Up Scale copy 를 이용해서 32bit elf 정보를 정규화한다.
//필요한 구조체
typedef struct s_NmShdrData{
    uint64_t sh_type;
    uint64_t sh_flags;
}s_NmShdrData;

typedef struct s_NmSymData{
    uint64_t st_value;
    uint64_t st_size;
    uint64_t strtab_offset;
    uint32_t st_name;
    uint16_t st_shndx;
    uint8_t st_info_type;
    uint8_t st_info_bind;
}t_NmSymData;

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
    11. bit 에 따라서 구조체를 만들거나 분기가 많이 생기는경우 최대한 깔끔하고 복잡하지 않게 만든다.
    12. 각 함수는 모듈화 시켜야하며 재활용할 수 있어야한다.
    13. 코드는 사람이 이해하기 쉬워야하며 안전성을 최대 목표로 한다.
    14. 코드의 검수는 작성을 지시한 사람에게 책임이 있으며 코드 작성자는 코드 검수를 하지 않는다.
    15. 누수가 나면 안되고 만약 시스템적 오류라면 steal reachable 은 상관 없다.
    16. 만약 다음 분기로 넘어가야한다면 최상단 char **path 빼고 할당받거나 fd 가 열렸거나 mmap으로 할당을 받았다면 반드시 반납을 해야한다.
    17. 비트 Mask 에서 우선순위를 잘 생각해서 만들어야 한다. 
    18. 