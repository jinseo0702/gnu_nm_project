# ft_nm Implementation Scope — Static Matrix

Evidence class: 명시된 분기/문자/option 집계는 **VERIFIED FROM CODE**다. GNU 호환성과
malformed-input 결과는 **UNKNOWN**이며, missing check에서 예상한 위험은 **INFERENCE**다.

## Counting policy

숫자는 source의 명시적 분기와 서로 다른 반환값만 센다. parser branch가 존재하면
`recognized`로 기록할 수 있지만, GNU `nm`과 같은 결과를 출력한다는 뜻의 `fully
supported` 또는 `verified`로 바꾸지 않는다.

## Input format scope

| Dimension | Recognized path | Missing/limited path | Evidence |
|---|---|---|---|
| Container | standalone ELF, regular `ar` | thin archive, BSD archive naming | `src/format_router.c:3-18`, `src/ar_parser.c:18-63` |
| ELF class | ELF32, ELF64 | none beyond these two | `src/elf_parser.c:76-103` |
| Byte order | little-endian | big-endian | `src/elf_parser.c:74-75` |
| Machine IDs | i386, x86-64 | other architectures | `src/elf_parser.c:84-85`, `src/elf_parser.c:97-98` |
| ELF type | relocatable, executable, shared object/PIE | core and processor-specific types | `src/elf_parser.c:82-83`, `src/elf_parser.c:95-96` |
| Symbol table | `SHT_SYMTAB` | `SHT_DYNSYM` fallback | `src/elf_parser.c:146-167` |
| Extended ELF numbering | none | `e_shnum == 0`, `SHN_XINDEX`, extended `e_shstrndx` | `src/elf_parser.c:86-103`, `src/sym_classify.c:26-27` |

정적 집계는 format/parser route 3개(ELF32, ELF64, regular ar), ELF object type 3개,
machine ID 2개, byte order 1개다. malformed input의 완전한 성공/실패 범위는 8개 선택
case를 실행한 뒤에도 전체 입력 공간에 대해서는 **UNKNOWN**이다.

## Option scope

| Option | Source behavior | Static status |
|---|---|---|
| default | FILE/SECTION/empty-name symbol을 숨기고 name sort | IMPLEMENTED path; GNU parity 확인 불가 |
| `-a` | 기본 숨김 해제 | PARTIAL: unnamed `U`는 계속 숨김 |
| `-g` | `A B D R T W w U V` type whitelist | PARTIAL: binding 자체가 아닌 type whitelist이며 일부 global/weak type 누락 |
| `-u` | `U`, `w`만 선택 | PARTIAL: undefined weak object `v` 누락 |
| `-n` | value, then name sort | IMPLEMENTED path |
| `-r` | 최종 배열 reverse | IMPLEMENTED path |
| `-P` | `name type value [size]` | PARTIAL: 64-bit value/size가 32-bit `%x` formatter로 전달됨 |

서로 다른 option letter는 6개다. 여러 filter가 함께 있으면 코드 분기상 `-u`가
가장 먼저 결과를 결정하고 그다음 `-g`, 마지막이 `-a`다.

## Symbol classification scope

source에서 반환 가능한 서로 다른 type 문자는 22개다.

| Category | Characters | Rule source |
|---|---|---|
| Absolute/common/undefined | `a A C U` | `src/sym_classify.c:8-25` |
| Weak | `v V w W` | `src/sym_classify.c:16-23`, `src/sym_classify.c:69-79` |
| GNU-specific | `i u` | `src/sym_classify.c:65-68` |
| BSS/text/data/read-only | `b B t T d D r R` | `src/sym_classify.c:30-53` |
| Unwind/debug/non-allocated | `p N n` | `src/sym_classify.c:28-29`, `src/sym_classify.c:54-57` |
| Fallback | `?` | `src/sym_classify.c:58` |

이 집계는 반환 문자 수다. 실제 compiler/linker fixture에서 22개를 모두 올바르게
재현한다거나 GNU `nm` 분류와 일치한다는 주장은 baseline 전에는 할 수 없다.

## Validation coverage

| Structure | Present checks | Missing checks relevant to baseline |
|---|---|---|
| ELF ident/header | minimum span, magic, class, endian, type, machine | class별 `e_ehsize`, class-machine pairing, header field consistency |
| Section table | nonzero offset/count, aggregate span | `e_shentsize`, per-section payload span, extended numbering, `e_shstrndx` |
| Symbol table | first `SHT_SYMTAB`, `sh_link < shnum`, count division | symbol payload span, linked section kind/span, valid entry size, allocation overflow |
| Symbol/string | string offset, terminator search | mapped strtab span, addition overflow, general `st_shndx`, `SHN_XINDEX` |
| Archive | member header span, `ar_fmag`, payload span, align2 | strict decimal field validation, malformed-tail error, fixed-name strlen safety, member ELF failure propagation, BSD/thin variants |

## Recognized versus verified

```text
Recognized format/parser routes: 3
Recognized option letters: 6
Emittable type characters: 22
Fully GNU-compatible input variants: UNKNOWN
Fully verified option behaviors: UNKNOWN
Crash-free malformed cases: UNKNOWN
```

## Runtime-measured scope at CHECKPOINT B

```text
Deterministic cases: 30
PASS: 13
PARTIAL: 5
FAIL: 12
CRASH in default comparison: 0
Harness mutations detected: 5 / 5
Generated fixture/artifact files: 21
Security/ASan assessment: EXCLUDED BY USER
```

이 수치는 **VERIFIED FROM RUNTIME TEST**이며 `test_results.json`에서 계산되었다. CRASH 0은
default binary와 이 30개 case에만 한정하며 malformed input 전체의 crash-free 보장은 아니다.
