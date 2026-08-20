# ft_nm Baseline Failure Analysis

Runtime 출력과 exit status는 **VERIFIED FROM RUNTIME TEST**, code path와 root cause는
**VERIFIED FROM CODE**, possible fix direction은 **INFERENCE**다. 보안성 평가는 사용자
요청으로 제외하며 아래 malformed-input 사례는 format correctness/robustness로만 다룬다.

## F1 — `-g`가 external symbol 3개를 누락

Classification: GNU compatibility correctness gap caused by intentional simplification / symbol
filtering. Severity: HIGH within `-g` behavior.

- Expected: GNU `nm -g`는 15개 record를 출력하며 `common_symbol C`, `global_bss C`,
  `indirect_function i`를 포함한다.
- Actual: `ft_nm -g`는 12개 record만 출력하며 위 3개가 없다.
- Relevant code path: `src/sort_filter_print.c:29-35`, `src/sym_classify.c:14-15`,
  `src/sym_classify.c:65-68`.
- Root cause: external 여부를 ELF binding으로 판단하지 않고 출력 type 문자 whitelist
  `A/B/D/R/T/W/w/U/V`로 판단한다. 따라서 global인 `C`와 `i`가 구조적으로 누락된다.
- User rationale: binding 대신 출력 문자를 사용한 것은 **USER-PROVIDED RATIONALE**에 따른
  의도적 단순화다. 따라서 원인 선택은 의도적이지만 GNU parity 결과의 누락도 사실이다.
- Evidence: `t09_option_global`, `raw/cases/t09_option_global/comparison.json`.
- Possible fix directions: `st_info_bind != STB_LOCAL` 중심으로 filter하거나, classification
  결과에 `is_external/is_undefined` semantic flag를 함께 저장한다.

## F2 — `-P`에서 64-bit symbol value가 32-bit로 잘림

Classification: correctness bug / output encoding. Severity: HIGH within ELF64 `-P`.

- Expected: `high_absolute A 100000123` (hexadecimal).
- Actual: `high_absolute A 123`.
- Relevant code path: `src/sort_filter_print.c:171-185`, `ft_printf/ft_printf.c:32-33`.
- Root cause: `uint64_t st_value/st_size`를 `%x`로 전달하지만 formatter는 vararg를
  `unsigned int`로 읽는다. 32-bit를 넘는 상위 값이 출력 record에 반영되지 않는다.
- Evidence: `t15_posix_high64` raw stdout과 comparison JSON.
- Possible fix directions: 64-bit hex writer를 공통화하거나 formatter에 명시적 64-bit
  conversion을 추가한다.

## F3 — default name ordering이 GNU `nm`과 다름

Classification: intentional policy deviation / GNU ordering compatibility gap. Severity: MEDIUM.

- Expected: controlled fixture에서 `Alpha`, `_alpha`, `_zulu`, `alpha`, ... 순서.
- Actual: `_zulu`가 마지막으로 이동한다.
- Relevant code path: `src/sort_filter_print.c:44-79`, `src/sort_filter_print.c:81-93`.
- Root cause: comparator가 leading `_`, `.`, `$`를 모두 건너뛰고 case-insensitive 1차
  비교를 수행한다. GNU `nm` 2.46의 `LC_ALL=C` ordering과 다른 policy다.
- User rationale: 이 comparator는 **USER-PROVIDED RATIONALE**에 따른 의도적 정책이다.
  당시 정확한 기준이 불명확해 직접 결정했으므로 accidental bug로 표현하지 않는다.
- Evidence: `t11_sort_name`; symbol multiset은 같고 order만 다르다.
- Possible fix directions: oracle-compatible comparator를 명시적으로 구현하고 punctuation,
  case, equal-key tie case를 table-driven regression으로 고정한다.

## F4 — path/archive 오류가 최종 nonzero status로 집계되지 않음

Classification: correctness/robustness bug / error propagation. Severity: HIGH for CLI automation.

- Expected: missing file, empty/non-object file, truncated archive, bad member trailer에서 GNU
  `nm`은 nonzero status를 반환한다.
- Actual: `ft_nm`은 해당 5개 case에서 status 0을 반환하며 empty/truncated archive 일부는
  diagnostic도 없다.
- Relevant code path: `src/nm.c:47-55`, `src/io_unit.c:24-28`,
  `src/ar_parser.c:81-91`, `src/ar_parser.c:113`.
- Root cause: `main`이 `process_path` 반환값을 버리고 항상 0을 반환한다. archive parser는
  truncated tail에서 `break` 후 `OK`를 반환한다.
- Evidence: `t20`, `t21`, `t22`, `t28`, `t29`.
- Possible fix directions: path/member result를 accumulator에 병합하고 `SKIP_UNIT`, malformed,
  fatal을 구분한 뒤 최종 process status와 diagnostic을 한 곳에서 결정한다.

## F5 — 잘못된 `e_shentsize` ELF를 정상 symbol table처럼 출력

Classification: robustness bug / ELF structural validation. Severity: HIGH for format correctness.

- Expected: GNU `nm`은 `e_shentsize=1` fixture를 invalid header로 판단하고 symbol을 출력하지
  않는다.
- Actual: `ft_nm`은 6개 정상처럼 보이는 symbol record를 출력하고 status 0을 반환한다.
- Relevant code path: `src/elf_parser.c:86-101`, `src/elf_parser.c:123-150`.
- Root cause: section-table aggregate range는 declared `e_shentsize`로 계산하지만 실제 access는
  `Elf32_Shdr *`/`Elf64_Shdr *` pointer arithmetic을 사용한다. declared entry size가 native
  structure size와 같은지 검증하지 않는다.
- Evidence: `t25_bad_shentsize` raw stdout/stderr와 comparison JSON.
- Possible fix directions: class별 header invariant를 preflight에서 검증하거나 모든 section
  access를 checked view/accessor를 통해 수행한다.

## Correctness bugs versus scope limitations

| Kind | Measured examples |
|---|---|
| Correctness bug | 64-bit `-P` truncation, nonzero status 미전파 |
| Intentional GNU compatibility gap | `-g` type-character 단순화의 symbol 누락, 독자 name-ordering policy |
| Robustness bug | invalid `e_shentsize`를 받아들이고 symbol 출력 |
| Scope limitation | `SHT_DYNSYM`/`-D` 없음, `--` 미지원, big-endian/extended numbering/thin/BSD archive 미구현 |
| Formatting-only difference | invalid option에서 GNU usage detail이 더 많음 (`t18` PARTIAL) |

## Highest-value portfolio candidate

F1 `-g` filtering을 우선 후보로 추천한다. GNU semantic comparison으로 15개 중 3개 누락을
정확히 수치화할 수 있고, 원인이 "presentation character whitelist로 semantic policy를
결정"한 설계에 있어 단순 문자 추가보다 모델 분리 문제로 설명할 수 있다.

### Possible solution architectures

1. **Binding-first filter (recommended)** — `st_info_bind`, `st_shndx`, symbol type을 사용해
   external/undefined 여부를 먼저 결정하고 출력 문자는 presentation 단계에서만 사용한다.
2. **Classification descriptor** — classifier가 문자 하나 대신 `{display_type, is_external,
   is_undefined, default_visible}`을 반환해 모든 option이 같은 semantic metadata를 공유한다.
3. **Table-driven policy** — bind/type/index 조합을 descriptor table로 정의하고 option별 predicate를
   별도 적용한다. 확장은 쉽지만 현재 규모에는 구현량이 가장 크다.

아직 어떤 방향도 source에 적용하지 않았다.
