# ft_nm

ELF symbol table과 Unix archive 구조를 직접 해석해 심볼을 출력하는 `nm` 구현입니다.
GNU `nm`의 핵심 동작을 따라가며 ELF32/64와 regular archive를 하나의 심볼 모델로 처리합니다.

## 만든 이유

실행 파일 안의 심볼이 어떤 section과 binding 정보를 가지며, archive member가 어떻게
다시 ELF로 해석되는지 이해하기 위해 만들었습니다. GNU `nm`과의 호환성도 목표였지만,
처음부터 모든 동작을 복제하기보다 자주 쓰는 옵션과 핵심 파싱 경로를 구현하는 데 우선순위를 뒀습니다.

## 핵심 기능

- little-endian `ELF32`/`ELF64`의 `ET_REL`, `ET_EXEC`, `ET_DYN` 해석
- i386 및 x86-64 ELF의 `SHT_SYMTAB` 심볼 수집과 타입 문자 분류
- regular `ar` archive의 short name과 GNU long-name table member 순회
- ELF32/64 심볼을 64-bit value/size를 가진 공통 `t_NmSymData`로 정규화
- `-a`, `-g`, `-u`, `-r`, `-P`, `-n` 옵션과 여러 입력 파일 처리
- 인자가 없을 때 `a.out`을 기본 입력으로 사용

현재 파서는 `SHT_SYMTAB`을 대상으로 하며, `SHT_DYNSYM` fallback과 big-endian ELF,
thin/BSD archive는 구현 범위에 포함하지 않습니다.

## 동작 구조

```text
CLI option/path parsing
  -> open + fstat + read-only mmap
  -> magic router
       -> ELF32/ELF64 parser
       -> ar member iterator -> ELF parser
  -> common symbol model
  -> classify -> filter -> sort -> print
  -> munmap + close
```

일반 파일의 mapping과 archive member를 `t_unit(base, limit, display_name)`으로 표현해
같은 ELF pipeline을 재사용합니다. format routing, ELF 추출, 타입 분류, 정렬·출력은
각각 `src/format_router.c`, `src/elf_parser.c`, `src/sym_classify.c`,
`src/sort_filter_print.c`로 나뉩니다.

## 설계하면서 고민한 점

- ELF32/64의 구조체 크기는 다르지만, 이후 정책은 하나로 유지하기 위해 공통 심볼 모델로 복사했습니다.
- 이름 정렬은 `_`, `.`, `$`를 건너뛰고 대소문자를 무시해 비교하는 정책을 직접 정했습니다.
  당시 기준이 명확하지 않아 선택한 의도적 정책이며, GNU의 기본 정렬과 항상 같지는 않습니다.
- `-g`는 ELF binding이 아니라 출력 타입 문자 목록으로 필터링했습니다. 구현을 단순화한 선택이지만,
  일부 global/common 심볼이 제외되는 trade-off가 확인됐습니다.
- bounds check를 일부 두었지만 malformed ELF/archive 전체를 안전하게 거부한다고 일반화하지 않습니다.

## Build

```bash
make
```

생성 파일은 `./ft_nm`입니다. AddressSanitizer를 포함한 debug build는 다음 target을 사용합니다.

```bash
make debug
```

## Run

```bash
./ft_nm [options] [file ...]
./ft_nm -n example.o
./ft_nm -g libexample.a
./ft_nm -P executable
```

- `-a`: 기본적으로 숨기는 심볼도 포함
- `-g`: 외부 심볼로 분류한 타입만 출력
- `-u`: undefined 심볼만 출력
- `-n`: 주소값 기준 정렬
- `-r`: 정렬 결과 역순 출력
- `-P`: POSIX 형식으로 출력

## 검증 결과

| 범위 | 결과 |
|---|---|
| 비교 기준 | GNU `nm` 2.46 |
| 실행 case | 30 |
| 분류 | PASS 13 · PARTIAL 5 · FAIL 12 · CRASH 0 |
| 대표 관찰 | ELF32/64 relocatable, short/long-name archive, 선택한 `-u`/`-n`/`-r` case 일치 |

전체 case의 stdout, stderr, exit status와 판정은
[`portfolio_audit/test_results.md`](portfolio_audit/test_results.md)에 보존했습니다.

## 확인된 한계

- `-g`는 GNU가 출력한 15개 중 common symbol 2개와 indirect function 1개를 누락했습니다.
- punctuation과 대소문자가 섞인 fixture에서 symbol set은 같았지만 기본 출력 순서가 달랐습니다.
- 32-bit 범위를 넘는 `-P` value는 잘리며, path/archive 오류가 최종 nonzero status로
  집계되지 않는 경로가 있습니다.

이 문제들의 수정 방향은 audit에 후보로만 기록되어 있으며 아직 적용되지 않았습니다.
작성자도 다음 개선 우선순위를 아직 선택하지 않았습니다.

## 상세 문서

- [Audit 개요](portfolio_audit/README.md)
- [Architecture](portfolio_audit/architecture.md)
- [구현 범위](portfolio_audit/implementation_scope.md)
- [설계 판단](portfolio_audit/design_rationale.md)
- [실패 분석](portfolio_audit/failures.md)
