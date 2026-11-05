# ft_nm Architecture — Static Audit

Evidence class: 이 문서의 source-evidence 기반 구조와 제어 흐름은 **VERIFIED FROM CODE**다.
정적 검증 공백이 실제 crash/오출력으로 이어지는지는 **INFERENCE/UNKNOWN**이며 baseline
전에는 runtime failure로 단정하지 않는다.

## 요약

`ft_nm`은 path를 read-only로 열어 전체 파일을 `mmap`하고, magic으로 regular ELF와
regular Unix archive를 분기한 뒤, ELF의 `SHT_SYMTAB`을 공통 내부 심볼 구조체로
정규화하여 분류·필터·정렬·출력하는 단일 프로세스 CLI다.

```text
CLI / option parsing
  -> each path (or a.out)
  -> open + fstat + mmap(PROT_READ, MAP_PRIVATE)
  -> magic router
       -> ELF32/ELF64
            -> ELF header checks
            -> section table + SHT_SYMTAB selection
            -> symbol/string/section metadata load
            -> symbol type classification
            -> visibility filter
            -> name/value quicksort + optional reverse
            -> default/POSIX output
       -> regular ar
            -> member header iteration + align2
            -> GNU long-name table lookup
            -> ELF member -> same ELF pipeline
  -> munmap + close
```

## Component map

| Component | Responsibility | Source evidence |
|---|---|---|
| Entry/error table | CLI 시작, 기본 `a.out`, path 순회 | `src/nm.c:3-55` |
| Argument parser | 6개 short option bitmask, path 수집 | `src/arg.c:3-102` |
| I/O unit | `open`/`fstat`/`mmap`, format dispatch, cleanup | `src/io_unit.c:3-52` |
| Format router | `ARMAG` 또는 `ELFMAG` 비교 | `src/format_router.c:3-18` |
| Archive parser | member 순회, long-name table, ELF member 전달 | `src/ar_parser.c:3-114` |
| ELF parser | header 검사, `SHT_SYMTAB`, symbol/string load | `src/elf_parser.c:5-312` |
| Symbol classifier | bind/type/section flag를 출력 문자로 변환 | `src/sym_classify.c:3-82` |
| Filter/sort/print | visibility, name/value quicksort, `-r`, `-P` | `src/sort_filter_print.c:3-251` |

## Core data model

- `t_unit`은 현재 해석 단위의 `base`, byte `limit`, 출력 이름을 보관한다.
  일반 파일에서는 mmap 전체, archive member에서는 payload가 독립된 unit이다
  (`include/nm.h:27-31`, `src/ar_parser.c:100-106`).
- `t_MetaData`는 ELF32/64 header/section/symbol pointer와 string-table 위치,
  symbol count, class를 보관한다 (`include/nm.h:33-50`).
- ELF32/64 symbol은 `t_NmSymData`의 64-bit value/size와 공통 bind/type 필드로
  정규화된다 (`include/nm.h:57-67`, `src/elf_parser.c:236-264`).
- `CHECK_RANGE`는 `offset > limit`을 먼저 검사한 뒤 `size <= limit - offset`을
  사용해 덧셈 overflow를 피한다 (`include/nm.h:112-117`). 다만 모든 pointer
  접근 전에 이 helper가 적용되지는 않는다.

## ELF path

1. 최소 크기, ELF magic, little-endian, class를 검사한다.
2. `ET_REL`/`ET_EXEC`/`ET_DYN`과 `EM_386`/`EM_X86_64`만 허용한다.
3. section table의 전체 범위를 검사한다.
4. 첫 `SHT_SYMTAB`을 선택하고 `sh_link`의 string table 위치를 취한다.
5. ELF32/64 symbol과 section metadata를 공통 구조체 배열로 복사한다.
6. symbol name 또는 `STT_SECTION`의 section name을 가져오고 타입 문자를 분류한다.
7. visibility filter, sort, output을 수행한다.

근거: `src/elf_parser.c:63-194`, `src/elf_parser.c:197-280`,
`src/elf_parser.c:283-312`.

## Archive path

`SARMAG` 다음부터 fixed `struct ar_hdr`를 읽고, decimal size, payload 범위,
2-byte trailer magic, odd-size padding을 처리한다. `//` member는 GNU long-name
table로 저장하고 `/<digits>`는 해당 offset에서 이름을 복원한다. payload magic이
ELF일 때만 공통 ELF path로 전달한다 (`src/ar_parser.c:65-113`).

## Error and exit behavior

- path-level 오류는 대체로 `FAIL_PATH`를 반환하고 다음 path로 진행할 수 있다.
- archive 내부 ELF의 `process_elf_unit` 반환값은 검사하지 않으며, archive loop가 끝나면
  `OK`를 반환한다 (`src/ar_parser.c:103-113`). 따라서 member-level parse/할당/출력 실패는
  archive 결과로 전파되지 않는다.
- `main`은 `process_path`의 반환값을 수집하지 않으므로, invalid option 또는 path-array
  allocation 실패가 아닌 path/format 오류 뒤에도 최종 `0`을 반환하는 source 흐름이다
  (`src/nm.c:36-55`).
- 오류 문자열 enum/table은 있지만 실제 path 오류의 다수는 `NM_LOG`에 직접 문자열을
  전달한다 (`src/nm.c:3-23`, `src/io_unit.c:12-47`).
- 정상 I/O path에서는 mapping과 fd가 회수된다 (`src/io_unit.c:49-50`).

## Important design characteristics

1. **Bounded unit abstraction** — standalone mmap과 archive member를 모두 `t_unit(base,
   limit, display_name)`으로 표현해 같은 ELF pipeline을 재사용한다
   (`include/nm.h:27-31`, `src/ar_parser.c:100-106`).
2. **ELF32/64 normalization** — class별 native ELF structure를 읽은 뒤 value/size를 64-bit
   공통 `t_NmSymData`로 복사하여 이후 분류/출력 경로를 하나로 만든다
   (`include/nm.h:57-67`, `src/elf_parser.c:236-264`).
3. **Separated parse/classify/present stages** — format routing, symbol extraction, type
   classification, visibility/sort/output이 source component로 나뉜다.
4. **Policy implemented after loading** — 전체 symbol을 materialize하고, 별도 visible 배열에
   복사한 뒤 custom comparator와 recursive quicksort를 적용한다
   (`src/sort_filter_print.c:202-249`).
5. **Return-code error model with incomplete aggregation** — `OK/SKIP_UNIT/FAIL_PATH/FATAL`을
   정의하지만 top-level path와 archive member failure가 최종 process status로 집계되지 않는다
   (`include/nm.h:69-74`, `src/nm.c:47-55`, `src/ar_parser.c:103-113`).

위 다섯 항목은 **VERIFIED FROM CODE**다. 성능, 안정성, GNU parity의 장단점 평가는 아직
**UNKNOWN**이다.

## Static safety boundary

확인되는 방어:

- file/format magic과 mmap 길이 확인
- top-level section-table 범위 확인
- symbol table의 `sh_link < shnum`
- string offset과 NUL terminator 검사 시도
- archive member header/payload 범위와 `ar_fmag` 확인

정적으로 확인되는 공백:

- `e_shentsize`가 실제 struct 크기인지 검증하지 않은 채 struct pointer arithmetic 수행
- 각 section의 `sh_offset + sh_size`, symbol-table payload, linked string-table 전체 범위 미검증
- `e_shstrndx` 및 일반 `st_shndx`의 section-count 범위 미검증
- ELF extended numbering (`e_shnum == 0`, `SHN_XINDEX`) 처리 없음
- `strtab_offset + str_offset` 자체의 overflow와 `strtab_size`의 mapped-file 범위 미검증
- archive fixed-width `ar_name[16]`에 `ft_strlen` 사용

근거: `src/elf_parser.c:68-103`, `src/elf_parser.c:168-193`,
`src/elf_parser.c:206-278`, `src/sym_classify.c:26-27`, `src/ar_parser.c:92-99`.
이 항목 중 선택한 malformed behavior의 runtime 결과는 `test_results.md`에 있다. 시험하지
않은 조합의 crash 가능성과 GNU `nm` parity는 여전히 **UNKNOWN**이다.
