# ft_nm Feature Inventory — Static Audit

## 상태 정의

- **IMPLEMENTED**: 좁게 서술한 동작의 source path가 존재한다.
- **PARTIAL**: source path는 있으나 중요한 형식, 검증, 조합 또는 오류 경계가 빠져 있다.
- **NOT IMPLEMENTED**: 해당 동작으로 가는 source path가 없다.
- **UNKNOWN**: 정적 분석만으로 결과를 판단할 수 없다.

`IMPLEMENTED`는 GNU `nm` parity 또는 runtime PASS를 의미하지 않는다.

Evidence class: line 근거가 있는 구현/부재 판단은 **VERIFIED FROM CODE**다. 아래 첫
inventory는 CHECKPOINT A static snapshot이고, 뒤의 CHECKPOINT B delta는 **VERIFIED FROM
RUNTIME TEST**다. **USER-PROVIDED RATIONALE**는 아직 없다.

## Inventory

| Feature | Status | Evidence and boundary |
|---|---|---|
| 인자 없음 -> `a.out` | IMPLEMENTED | `src/nm.c:39-46` |
| 여러 input path 순회 | IMPLEMENTED | `src/nm.c:47-52` |
| short option `a/g/u/r/P/n` | IMPLEMENTED | 6개 문자와 bit, `src/arg.c:3-35` |
| option을 path 앞/뒤에서 수집 | PARTIAL | 전체 argv를 순회하지만 `--` 종결자가 없고 `-`로 시작하는 filename을 표현할 수 없음, `src/arg.c:70-102` |
| invalid option 즉시 실패 | IMPLEMENTED | `FATAL` -> main return 1, `src/arg.c:15-21`, `src/nm.c:36-38` |
| read-only file mapping | IMPLEMENTED | `O_RDONLY`, `PROT_READ|MAP_PRIVATE`, `src/io_unit.c:12-38` |
| regular ELF magic routing | IMPLEMENTED | `src/format_router.c:12-16` |
| regular GNU/System V archive routing | IMPLEMENTED | `ARMAG`, `src/format_router.c:7-11` |
| ELF32 parsing path | PARTIAL | 공통 64-bit 최소 header 크기를 먼저 요구하며 field/section 세부 bounds가 불완전, `src/elf_parser.c:68-103` |
| ELF64 parsing path | PARTIAL | class path는 있으나 section/symbol/string payload bounds가 불완전, `src/elf_parser.c:79-90`, `src/elf_parser.c:168-180` |
| little-endian restriction | IMPLEMENTED | `ELFDATA2LSB` 외에는 거부, `src/elf_parser.c:74-75` |
| big-endian ELF | NOT IMPLEMENTED | byte-swap 또는 big-endian path 없음 |
| x86/i386 machine restriction | IMPLEMENTED | `EM_386`, `EM_X86_64`, `src/elf_parser.c:84-85`, `src/elf_parser.c:97-98` |
| `ET_REL`/`ET_EXEC`/`ET_DYN` | IMPLEMENTED | `src/elf_parser.c:82-83`, `src/elf_parser.c:95-96` |
| top-level section-table range check | PARTIAL | 전체 span만 확인; entry size와 개별 section 범위는 확인하지 않음, `src/elf_parser.c:86-101` |
| static symbol table (`SHT_SYMTAB`) | IMPLEMENTED | 첫 table 선택, `src/elf_parser.c:107-194` |
| dynamic symbol table fallback (`SHT_DYNSYM`) | NOT IMPLEMENTED | 관련 branch가 주석 처리됨, `src/elf_parser.c:114-116`, `src/elf_parser.c:152-162` |
| symbol/string table bounds validation | PARTIAL | `sh_link` index와 string terminator는 확인하지만 table payload 전체 범위/entry size는 미확인, `src/elf_parser.c:168-193`, `src/elf_parser.c:5-18` |
| ELF32/64 공통 symbol 모델 | IMPLEMENTED | value/size를 64-bit 공통 구조로 load, `src/elf_parser.c:236-264` |
| `STT_SECTION` name 복원 | PARTIAL | section-name table을 사용하지만 `e_shstrndx`/`st_shndx` bounds가 없음, `src/elf_parser.c:21-61`, `src/elf_parser.c:266-277` |
| symbol type classification | PARTIAL | 22개 반환 문자가 있으나 reserved/extended section index bounds와 GNU parity는 미검증, `src/sym_classify.c:3-82` |
| default symbol visibility | IMPLEMENTED | FILE/SECTION/empty name 제외, `src/sort_filter_print.c:3-22` |
| `-a` all-symbol visibility | PARTIAL | 기본 숨김을 해제하지만 unnamed `U`는 별도 제거; GNU 결과는 미검증, `src/sort_filter_print.c:9-18`, `src/sort_filter_print.c:37-40` |
| `-g` external-only filter | PARTIAL | type 문자 whitelist 방식이며 `C`, `u`, `i`, `v` 등이 제외됨, `src/sort_filter_print.c:29-35` |
| `-u` undefined-only filter | PARTIAL | `U`, `w`만 포함하고 undefined weak object `v`를 제외, `src/sort_filter_print.c:23-28` |
| filter option 조합 | PARTIAL | source 우선순위는 `u` -> `g` -> `a`; 조합의 GNU semantics는 미검증, `src/sort_filter_print.c:19-40` |
| name sort | PARTIAL | `_.$` prefix와 case를 자체 규칙으로 비교; locale/GNU tie semantics 미검증, `src/sort_filter_print.c:44-93` |
| numeric sort `-n` | IMPLEMENTED | value 후 name comparator, `src/sort_filter_print.c:95-109`, `src/sort_filter_print.c:225-230` |
| reverse sort `-r` | IMPLEMENTED | 정렬 뒤 array reverse, `src/sort_filter_print.c:232-241` |
| default 32/64-bit width output | IMPLEMENTED | 8/16자리 hex writer, `src/sort_filter_print.c:154-169`, `src/sort_filter_print.c:186-199` |
| POSIX output `-P` | PARTIAL | format path는 있으나 64-bit value/size를 `ft_printf("%x")`에 전달하고 formatter는 `unsigned int`만 읽음, `src/sort_filter_print.c:171-185`, `ft_printf/ft_printf.c:32-33` |
| GNU archive short member names | IMPLEMENTED | fixed 16-byte field trim, `src/ar_parser.c:49-62` |
| GNU archive long-name table (`//`, `/<offset>`) | PARTIAL | lookup path는 있으나 invalid offset fallback과 fixed buffer 제한, `src/ar_parser.c:18-47`, `src/ar_parser.c:92-107` |
| archive symbol-index member skip | PARTIAL | 의도한 `/` 판별에 fixed-width field 대상으로 `ft_strlen` 사용, `src/ar_parser.c:98` |
| archive non-ELF member diagnostic/status | NOT IMPLEMENTED | member payload가 ELF가 아니면 별도 diagnostic/status 없이 건너뜀, `src/ar_parser.c:98-108` |
| odd-size archive member alignment | IMPLEMENTED | align2, `src/ar_parser.c:109-111` |
| archive member ELF 오류 전파 | NOT IMPLEMENTED | `process_elf_unit` 반환값을 버리고 archive parser는 `OK` 반환, `src/ar_parser.c:103-113` |
| BSD extended archive names | NOT IMPLEMENTED | `#1/<length>` 처리 없음 |
| thin archives | NOT IMPLEMENTED | thin archive magic/router/member reference 처리 없음 |
| malformed ELF/archive rejection | PARTIAL | 일부 header/member range 검사는 있으나 위 bounds 공백과 truncated archive의 silent break가 존재, `src/elf_parser.c:63-103`, `src/ar_parser.c:81-91` |
| path/format 오류의 nonzero 최종 status | NOT IMPLEMENTED | `process_path` 반환값을 무시하고 main은 0 반환, `src/nm.c:47-55` |
| fd/mapping 회수 | IMPLEMENTED | 성공한 mmap 뒤 모든 dispatch 결과에서 `munmap`/`close`, `src/io_unit.c:39-51` |
| default build automation | IMPLEMENTED | root, libft, ft_printf targets, `Makefile:21-57` |
| 기존 자동 비교 스크립트 | PARTIAL | target 생성/option 비교/random byte mutation은 있으나 stderr를 버리고 token 비교하며 deterministic manifest/result artifact가 없음, `TestserMachine.py:11-230` |
| default build 성공 | IMPLEMENTED | CHECKPOINT B clean `make re` exit 0, `raw/setup/build_default.json` |
| GNU `nm` semantic parity | UNKNOWN | CHECKPOINT A에서 비교 실행하지 않음 |
| malformed corpus crash-free | UNKNOWN | CHECKPOINT A에서 실행하지 않음 |
| 성능/메모리 효율 | UNKNOWN | benchmark나 계측 없음 |

## CHECKPOINT B runtime verification delta

아래는 **VERIFIED FROM RUNTIME TEST**이며 전체 matrix는 `test_results.md`에 있다.

| Feature | Runtime status | Evidence |
|---|---|---|
| default build | PASS | clean `make re`, `raw/setup/build_default.*` |
| ELF64 relocatable | PASS | `t01` semantic records/order 일치 |
| ELF32 relocatable | PASS | `t02` semantic records/order 일치 |
| ET_EXEC / ET_DYN extraction | PASS for symbol set | `t03`, `t04`; order는 별도 `t11` FAIL |
| regular archive short/long names | PASS | `t06`, `t07` |
| `-a` inclusion | PASS | `t08`; order는 별도 `t11` |
| `-g` external-only | FAIL | `t09`, GNU 15 records 중 3개 누락 |
| `-u`, `-n`, `-r` | PASS on selected fixtures | `t10`, `t12`, `t13`, `t16` |
| `-P` | PARTIAL overall | `t14` basic PASS, `t15` >32-bit value FAIL |
| missing/invalid path final status | FAIL | `t20`-`t22` |
| malformed ELF/archive behavior | mixed | `t23`-`t30`; 3 PARTIAL, 5 FAIL |
| GNU semantic parity outside these cases | UNKNOWN | 30-case baseline 이상으로 일반화하지 않음 |
