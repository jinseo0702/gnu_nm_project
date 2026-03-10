# ft_nm

GNU `nm`을 기준으로 만든 ELF/AR 심볼 분석기입니다.  
Linux `x86_32` / `x86_64` 대상 `ELF32`, `ELF64`, `ar archive`를 안전하게 해석하고 심볼을 정렬/필터링해 출력합니다.

## Project Description

이 프로젝트는 단순히 심볼을 찍는 프로그램이 아니라, 아래 흐름을 직접 구현한 `ft_nm`입니다.

- 입력 파일이 `ELF`인지 `ar archive`인지 판별
- ELF 헤더, 섹션 헤더, 심볼 테이블, 문자열 테이블 검증
- archive 내부 member를 순회하며 각 ELF를 다시 해석
- 심볼 타입 분류 후 옵션에 따라 필터링/정렬/출력

지원 범위:

- 포맷: `ELF32`, `ELF64`, `ar`
- 옵션: `-a` `-g` `-u` `-r` `-P` `-n`
- 기본 입력: 인자가 없으면 `a.out`

## 옵션

- `-a`: 숨겨지는 심볼까지 포함해 전체 심볼을 출력합니다.
- `-g`: 외부 심볼 위주로 출력합니다.
- `-u`: undefined 심볼만 출력합니다.
- `-n`: 이름 기준이 아니라 주소값 기준으로 정렬합니다.
- `-r`: 정렬 결과를 역순으로 출력합니다.
- `-P`: POSIX 형식으로 출력합니다.

## 프로젝트 구조

- `src/arg.c`: 옵션/경로 파싱
- `src/format_router.c`: ELF / AR 판별
- `src/ar_parser.c`: archive member 순회
- `src/elf_parser.c`: ELF 헤더/섹션/심볼 로드
- `src/sym_classify.c`: 심볼 타입 분류
- `src/sort_filter_print.c`: 정렬/필터/출력

## Build

```bash
make
make debug
```

생성 파일: `./ft_nm`

## Run

```bash
./ft_nm <file>
./ft_nm -n <file>
./ft_nm -g <file>
./ft_nm -u <file>
./ft_nm -P <file>
./ft_nm <archive.a>
```

## Test

간단 검증:

```bash
make
gcc -c test/testNm.c -o testNm.o
ar rcs libtestNm.a testNm.o
./ft_nm testNm.o
./ft_nm libtestNm.a
nm testNm.o
```

자동 비교/퍼징:

```bash
python3 TestserMachine.py
```

`TestserMachine.py`는 테스트 바이너리를 만들고, 시스템 `nm` 결과와 비교하며, 손상된 파일 입력도 함께 확인합니다.

32bit 확인:

```bash
gcc -m32 -c test/Test32.c -o test32.o
./ft_nm test32.o
```

## 문서

- `archi/architecture.md`
- `archi/Implementation_diagram.md`
- `archi/verification.md`
