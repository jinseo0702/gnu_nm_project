IMPLEMENTATION DIAGRAM

[COREPRINCIPLES]
- handle_path()는 절대 바로 ELF로 간주 하지 않는다.
- 반드시 아래 순서를 고정한다.

[PROCESSINGFLOW]
1. open + fstat + mmap → unit(base=mmap, limit=file_size)
2. detect_format(unit)
    - AR → process_ar_archive(unit)
    - ELF → process_elf_unit(unit)
    - else → ERR_FORMAT (해당 path 실패, 다음 path)
3. process_ar_archive는 member payload마다 process_elf_unit(member_unit) 호출
4. 정책 결정 금지: AR인데도 ELF처럼 처리 같은 우회는 금지.

[MODULERESPONSIBILITY]
1. 파일명은 예시고, 책임이 핵심입니다.
    * A) arg.c / arg.h
        - 옵션 파싱,. path list 구성
        - unkwon option -> fatal 반환
        - path 없으면 자동으로 "a.out" 자동 등록
    * B) io_unit.c / io_unit.h (리소스/단위/추상화)
        - 현재 해서 단위 를 표현하는 t_unit 도입
            - const unsigned char *base
            - uint64_t limit;
            - const char *display_name(ar member 출력용)
        - 이 모듈은 리소스 라이프사이클 표준화를 담당(에러 시 누가 무엇을 해제하는지 고정)
    * C) format_router.c
        - detect_format(base, limit) :
            - AR magic(ARMAG) 검사
            - ELF magic(ELFMAG) 검사
        - 들 다 아니면 UNKNOWN
    * D) ar_parser.c
        - ar archive 해석 :
            - string table(필요 시)확보
            - member header 순회
            - member payload unit 생성 (base / limit 재설정)
            - payload에서 ELF magic 재검사 후 process_elf_unit 호출
        - ar_fmag 불일치 시 : ar 해석 중단 + path 실패 (다음 path로)
    * E) elf_parser.c
        - ELF 헤더 검증 및 t_MetaData 준비
        - EI_CALSS 에 따라서 해당하는 멤버선택
        - section header scan
        - symbol section 선택(symtab/dynsym)
        - strtab 검증 및 심볼 raw 로드(t_NmSymData array)
    * F) sym_classify.c
        - Symbol Type Determination Rules (A/a/B/b/C/D/d/i/N/n/p/R/r/T/t/U/u/V/v/W/w/?)
        - Unused Types
            - c, g, G, s, S, -, I
        - A/a : 
            - If `st_shndx` is `SHN_ABS`
            - If `ELFN_ST_BIND` of `st_info` is `STB_LOCAL`: a
            - If `ELFN_ST_BIND` of `st_info` is `STB_GLOBAL`: A
        - B/b:
            - If `sh_type` is `SHT_NOBITS` AND
            - `(sh_flag & SHF_ALLOC) != 0`
            - `(sh_flag & SHF_WRITE) != 0`
            - If `ELFN_ST_BIND` of `st_info` is `STB_LOCAL`: b
            - If `ELFN_ST_BIND` of `st_info` is `STB_GLOBAL`: B
        - C:
            - If `sh_type` is `SHT_NOBITS` AND
            - `(sh_flag & SHF_ALLOC) != 0`
            - `(sh_flag & SHF_WRITE) != 0`
            - If `st_shndx` is `SHN_COMMON`
        - D/d:
            - If `sh_type` is `SHT_PROGBITS` AND
            - `(sh_flag & SHF_ALLOC) != 0`
            - `(sh_flag & SHF_WRITE) != 0`
            - If `ELFN_ST_BIND` of `st_info` is `STB_LOCAL`: d
            - If `ELFN_ST_BIND` of `st_info` is `STB_GLOBAL`: D
        - i: 
            - If `ELFN_ST_TYPE` of `st_info` is `STT_GNU_IFUNC`
        - N:
            - If `sh_type` is `SHT_PROGBITS` AND
            - `sh_flag` is 0 (none)
        - n:
            - If `sh_type` is `SHT_PROGBITS` AND
            - `(sh_flag & SHF_WRITE) == 0` for `sh_flag`
        - p:
            - Here, p is a symbol type character, not an option (only `-P` exists as an option).
            - Consider 64-bit criteria only.
            - If `sh_type` is `SHT_X86_64_UNWIND`, it is p.
        - R/r:
            - If `sh_type` is `SHT_PROGBITS` AND
            - `(sh_flag & SHF_WRITE) == 0` for `sh_flag` AND
            - `sh_flag` is `SHF_ALLOC`
            - If `ELFN_ST_BIND` of `st_info` is `STB_LOCAL`: r
            - If `ELFN_ST_BIND` of `st_info` is `STB_GLOBAL`: R
        - T/t:
            - If `sh_type` is `SHT_PROGBITS` AND
            - `sh_flag` is `SHF_ALLOC` AND `SHF_EXECINSTR`
            - If `ELFN_ST_BIND` of `st_info` is `STB_LOCAL`: t
            - If `ELFN_ST_BIND` of `st_info` is `STB_GLOBAL`: T
        - U: 
            - If `st_shndx` is `SHN_UNDEF` AND
            - `ELFN_ST_BIND` of `st_info` is NOT `STB_WEAK`K 이 아닌경우
        - u:
            - If `ELFN_ST_BIND` of `st_info` is `STB_GNU_UNIQUE`
        - V/v : 
            - When `ELFN_ST_BIND` of `st_info` is `STB_WEAK`
            - If `ELFN_ST_TYPE` of `st_info` is `STT_OBJECT` AND
            - If `st_shndx` is `SHN_UNDEF` (undefined): v
            - If `st_shndx` is defined: V
        - W/w :
            - When `ELFN_ST_BIND` of `st_info` is `STB_WEAK`
            - If `ELFN_ST_TYPE` of `st_info` is NOT `STT_OBJECT`
            - If `st_shndx` is `SHN_UNDEF` (undefined): w
            - If `st_shndx` is defined: W
        - ?:
            - Cases other than the above. 
    * G) sort_filter_print.c
        - 옵션 우선순위에 따라 visible list 구성
        - 정렬(n/r)
        - 출력(P/일반)
        - quick sort 만들어서 사용 swap 은 stack 메모리 활용

[FUNCCONTRACT]
- 공통 반환 규약
    - OK: 계속 진행
    - SKIP_UNIT: 현재 unit만 건너뛰고 상위 루프로 복귀
    - FAIL_PATH: 현재 path 처리 실패(상위에서 다음 path)
    - FATAL: 프로그램 종료해야 하는 수준(unknown option 같은 케이스 전용)
- 프로그램 종료하지 않는다 정책을 코드로 복잡하게 만들지 않기 위해, 대부분은 FAIL_PATH/SKIP_UNIT로 표현하고, FATAL은 진짜 제한적으로 사용.

[CoreContractExample]
- int process_path(const char *path, uint32_t opt);
    - mmap 까지 성공하면 t_unit 생성
    - detect_fomat 호출
    - 마지막에 항상 munmap/close 정리(단일 cleanup 블록)
- int process_elf_unit(const t_unit *unit, uint32_t opt);
    - unit base/limit만 사용 (절대 외부 파일 크기 참조 금지)
    - t_MetaData 초기화 및 검증
    - symbols 로드/정렬/출력
- int process_ar_archive(const t_unit *unit, uint32_t opt);
    - ar magic 검사 통과가 전제
    - member loop 내부에서 매 member마다
        - check header range
        - name parsing
        - create payload unit (reflect align)
        - check payload ELF magic
            - pass process_elf_unit(payload_unit)
            - if ar_fmag check fail is FAIL_PATH

[RULETABLE]
- ranege rule (global)
| item | rule |
| --- | --- |
| Required Before Access | CHECK_RANGE(offset,size,limit) |
| Inspection sequence | offset <= limit first, and then size <= limit-offset |
| MOVE ADDRESS | MOVE_ADDRESS(base,offset)is not check range (Pre-call inspection required) |
| base mean | Start current interpretation unit (file mmap or member payload) |

- ar Member Circulation Rules
| stage | input | check | result |
| --- | --- | --- | --- |
| ar magic | unit base | memcmp(base, ARMAG, SARMAG)==0 | or FAIL_PATH |
| member header | member_off | CHECK_RANGE(member_off, sizeof(struct ar_hdr), limit) | or FAIL_PATH |
| ar_fmag | ar_hdr.ar_fmag | memcmp(ar_fmag, ARFMAG, 2)==0 | or FAIL_PATH |
| member size | ar_hdr.ar_size | decimal parse + range | payload size |
| payload unit | base+payload_off | payload limit=member_size | payload에서 ELF magic re check |
| alignment | member_end | and then member_off = align2(member_end) | even padding |

- ar_name parsing rule (fixed 16byte)
| case | term | extract name |
| --- | --- | --- |
| inline name | ar_name is not "/<digits>" | '/' at the end of ar_name[16] or empty trim|
| string table | ar_name is "/<digits>" format  | Restore digits from string table to offset |
| Prohibition of termination | No '\0' | Must handle 16-byte only |

- archive format 에서 "//" 는 기억해서 필여한 member payload name 추출

[DEFINITIONOFDONE]
- 아래는 코드 실행 없이도 정적으로 확인 가능한 DoD입니다.

- DoD-ELF (필수 8)
    1. ELF magic/클래스/엔디안 검증이 process_elf_unit 초반에 존재
    2. e_shoff/e_shnum/e_shentsize 검증 및 e_shoff + e_shnum*e_shentsize range 검사 존재
    3. section header 접근 전에 반드시 range 검사(엔트리 단위)
    4. symtab 우선, 없으면 dynsym 시도 로직 존재
    5. sh_link < e_shnum 검증 존재
    6. sym 엔트리 size가 0이면 sizeof로 치환하는 로직 존재
    7. strtab 접근 전에 st_name < strtab_size 및 memchr 널 확인 존재
    8. base/limit이 “unit 기준”으로만 사용됨(파일 전체 기준 참조 금지)

- DoD-AR (필수 7)
    1. AR magic 검사 후에만 ar 파서 진입
    2. ar_fmag ft_memcmp 검사 존재, 실패 시 FAIL_PATH
    3. member payload를 unit(base/limit)로 재정의
    4. payload에서 ELF magic 재검사 존재
    5. member offset 계산에 align2 반영
    6. ar_name은 16바이트 내에서만 파싱(널 종결 가정 없음)
    7. "/<digits>"일 때만 string table 복원, 그 외는 inline trim

- DoD-정책/리소스 (필수 6)
    1. unknown option은 fatal 처리
    2. 시스템 오류는 path 실패로 처리하되 프로그램 전체 종료는 하지 않음
    3. 모든 분기에서 open/mmap/malloc 자원 해제가 보장되는 cleanup 구조 존재
    4. free 후 NULL guard 적용(가능한 범위에서)
    5. CHECK_RANGE 없이 MOVE_ADDRESS 결과를 역참조하는 코드가 없음
    6. 옵션 우선순위(a/g/u > n > P)대로 필터/정렬/출력 적용


[DATASTRUCT]
```c
typedef struct s_unit{
    const unsigned char *base;
    uint64_t limit;
    const char * display_name;
}s_unit;

//meta data cache
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
    uint64_t strtab_offset; //strtab_offset은 현재 심볼 테이블(symtab/dynsym)이 참조하는 문자열 테이블(.strtab/.dynstr)의 파일 오프셋이다.
    uint8_t elf_class; // bit type 을정한다.
}t_MetaData;

//Type section header cache
typedef struct s_NmShdrData{
    uint64_t sh_type;
    uint64_t sh_flags;
}t_NmShdrData;

//정렬/필터/출력용 normalized
typedef struct s_NmSymData{
    uint64_t st_value;
    uint64_t st_size;
    uint32_t st_name;
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

[INLINE:MACRO]
```c
    // 포인터 산술연산
    //MOVE_ADDRESS는 범위검사를 하지 않는다. 호출 전 CHECK_RANGE를 통과해야 한다.
        static inline const void *MOVE_ADDRESS(const void *base, uint64_t offset) {
            if (base == NULL) return NULL;
            const unsigned char *p = (const unsigned char*)base;
            return (const void*)(p + offset);
        }
    // 범위체크
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