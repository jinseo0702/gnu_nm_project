# ft_nm Baseline Test Plan and Executed Coverage

## Objective

CHECKPOINT A 승인 후 untouched implementation을 GNU `nm` 2.46과 비교해 정적 판단을
runtime evidence로 전환했다. 실행 결과는 `test_results.md/json`, 원본은 `raw/`에 있다.

Evidence class: 설치 도구 버전은 **VERIFIED FROM RUNTIME TEST**(version probe), source에서
도출한 coverage 대상은 **VERIFIED FROM CODE**, case 선택과 예상 failure는 **INFERENCE**다.
case 선택은 **INFERENCE**, 실행 결과는 **VERIFIED FROM RUNTIME TEST**다.

## Reference / oracle

- Primary semantic oracle: 현재 설치된 GNU `nm` (GNU Binutils 2.46)
- Fixture 구조 검증 보조 도구: GNU `readelf` 2.46과 필요 시 ELF specification
- Archive fixture 생성/구조 확인: GNU `ar` 2.46

GNU `readelf`와 ELF specification은 fixture setup이 의도한 구조인지 확인하는 보조
oracle이며, `ft_nm` 출력 parity의 primary oracle은 같은 input을 받은 GNU `nm`이다.

## Isolation policy

생성한 모든 audit 파일은 다음 아래에 격리했다.

```text
portfolio_audit/
  tests/
  tools/
  bin/
    fixtures/
  raw/
```

기존 `src/`, `include/`, `libft/`, `ft_printf/`, `test/`, `diff/`, `Makefile`, `README.md`는
수정하지 않는다. build가 source 수정 없이는 실패하면 실패 자체를 기록한다.

## Proposed deterministic cases

| ID | Category | Target behavior |
|---|---|---|
| t01_elf64_rel | ELF64 | local/global text, data, BSS, rodata, undefined symbol의 name/type/value/order |
| t02_elf32_rel | ELF32 | 동일 의미의 32-bit symbol과 8자리 address formatting |
| t03_exec_dyn | ELF type | `ET_EXEC`, non-stripped `ET_DYN`/PIE의 static symbol extraction |
| t04_no_symtab | table policy | stripped ELF 또는 dynsym-only ELF에서 `SHT_DYNSYM` fallback 부재와 diagnostic/status |
| t05_archive_names | archive | short name, odd-sized member, GNU `//`/`/<offset>` long name과 member headers |
| t06_symbol_types | classify/filter | weak func/object, common, GNU unique, ifunc, absolute, section/file symbol과 `-a/-g/-u` |
| t07_sorting | sort | `_`, `.`, `$`, case 차이, 같은 value/name tie를 default/`-n`/`-r`로 비교 |
| t08_posix_64 | output | 32-bit를 넘는 ELF64 value/size를 `-P`로 출력해 truncation 여부 확인 |
| t09_cli_multi | CLI/error | 기본 `a.out`, multiple paths, option 위치/조합, invalid option, `--`, dash-leading path |
| t10_malformed_elf | validation | header/section/symtab/strtab offset과 size를 한 필드씩 deterministic하게 손상 |
| t11_extended_index | ELF edge | reserved `st_shndx`, `SHN_XINDEX`, extended section numbering 처리와 crash 여부 |
| t12_malformed_ar | archive validation | truncated header/payload, bad `ar_fmag`, invalid size/name offsets, malformed ELF member의 오류 전파, BSD/thin inputs |

총 제안 case group은 12개다. 각 group은 하나의 핵심 판단만 갖도록 작은 fixture로
나누고, compiler/linker가 생성하기 어려운 malformed/extended case는 byte-exact fixture
generator와 SHA-256을 함께 보존한다.

## Build and execution protocol

1. audit 시작 시 commit, working tree, 도구 버전, 기존 source hash를 다시 기록한다.
2. repository의 default `make`를 실행하고 command/stdout/stderr/exit/duration을 원본 보존한다.
3. fixture compile/link/archive 명령과 결과를 각각 보존한다.
4. 동일 fixture와 option argv를 `./ft_nm`과 GNU `nm`에 전달한다.
5. 각 실행에 timeout을 적용하고 stdout, stderr, exit status, signal을 분리 보존한다.
6. 종료 후 source hash와 Git diff를 재검증한다.

보안/ASan 평가는 사용자 요청으로 제외했다. default binary의 observable output과 exit
behavior만 GNU `nm`과 비교했다.

## Semantic comparison

Raw output은 수정하지 않는다. derived comparison에서만 아래 normalization을 허용한다.

- 모든 실행에서 `LC_ALL=C`를 고정해 locale 차이를 사전에 통제
- hexadecimal의 leading zero와 A-F letter case만 canonical numeric value로 변환
- multi-file/archive header의 audit 임시 root만 제거하되 file basename과 member name은 보존
- semantic-record case에서 whitespace를 field delimiter로 해석하되, 별도 formatting case에서는
  raw spacing을 그대로 비교
- diagnostic 비교에서 executable prefix(`nm`/`ft_nm`)와 quote style은 제거할 수 있으나,
  diagnostic category, 대상 file/member, 발생 횟수는 보존

다음은 정상화하지 않는다.

- 같은 fixture에서 나온 symbol value/address 자체
- symbol name/type/value/size의 누락 또는 변경
- symbol 순서가 test target인 case의 order 차이
- archive member 누락/오식별
- diagnostic 종류 또는 exit status 차이
- timeout 또는 default 실행 crash
- filter option이 포함/제외한 symbol set 차이

각 symbol output은 가능한 경우 `(member, name, type, value, size)` record로 parse하여
multiset, ordering, formatting을 별도 비교한다.

## Result classification

- **PASS**: case의 핵심 semantic record, order/status가 GNU `nm`과 일치한다.
- **PARTIAL**: 핵심 symbol은 맞지만 non-core format 또는 일부 metadata/diagnostic이 빠진다.
- **FAIL**: 핵심 symbol set/type/value/order, format policy, archive traversal, status가 틀리거나 없다.
- **CRASH**: default `ft_nm`이 signal 종료 또는 timeout으로 usable result를 만들지 못한다.

최종 숫자는 machine-readable `test_results.json`에서 계산한다. 실제 30개 case 결과는
PASS 13, PARTIAL 5, FAIL 12, CRASH 0이며 `test_results.md` matrix와 연결된다.

## Harness validation result

정상 oracle record를 기준으로 symbol 제거, type 변경, value 변경, order swap, exit status
변경을 각각 주입했다. 5개 mutation 모두 FAIL로 검출되었으며 원본 결과는
`raw/harness_validation.json`에 보존했다.
