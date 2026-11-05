# ft_nm Design Rationale

이 문서는 CHECKPOINT B 이후 사용자 답변을 코드/런타임 사실과 분리해 기록한다.

## Verified facts

- **VERIFIED FROM CODE**: `-a/-g/-u/-r/-P/-n` 여섯 option path가 구현되어 있다.
- **VERIFIED FROM CODE**: default comparator는 leading `_`, `.`, `$`를 건너뛰고
  case-insensitive 1차 비교를 한다 (`src/sort_filter_print.c:44-79`).
- **VERIFIED FROM CODE**: `-g`는 binding이 아니라 type 문자 whitelist로 filter한다
  (`src/sort_filter_print.c:29-35`).
- **VERIFIED FROM RUNTIME TEST**: GNU `nm -g`의 15개 record 중 `ft_nm`은 12개를 출력해
  `C` 두 개와 `i` 한 개를 누락했다 (`t09_option_global`).
- **VERIFIED FROM RUNTIME TEST**: 선택한 ordering fixture에서 symbol set은 같지만 GNU
  `nm` 2.46과 순서가 달랐다 (`t11_sort_name`).

## User-provided rationale

- **USER-PROVIDED RATIONALE**: 목표는 GNU `nm`의 정확한 호환과 핵심 option 구현 모두였지만,
  핵심 option 구현에 더 중점을 두었다.
- **USER-PROVIDED RATIONALE**: `_`, `.`, `$`와 대소문자를 무시하는 정렬은 의도한 정책이다.
  구현 당시 해당 정책의 정확한 기준이 불명확해 직접 정책을 결정했다.
- **USER-PROVIDED RATIONALE**: `-g`를 ELF binding 대신 출력 type 문자로 filter한 것은
  의도적인 단순화다.

## Inference

- **INFERENCE**: 정렬의 GNU 대비 FAIL은 우연한 구현 결함이라기보다, 불명확한 요구에서
  독자 정책을 선택한 결과인 compatibility gap으로 설명하는 편이 정확하다.
- **INFERENCE**: `-g`의 measured 누락은 의도적 단순화가 만든 trade-off다. 단순화 자체와
  GNU parity를 만족하지 못한 runtime 결과를 동시에 기록해야 한다.
- **INFERENCE**: 핵심 option 구현을 우선했다는 목표에 비추면 첫 improvement 후보로
  `-g` semantic filtering을 선택하는 것이 프로젝트 의도와 가장 잘 맞는다.

## Still unknown

- 당시 참고한 GNU/Binutils 문서나 과제 specification의 정확한 범위
- malformed input과 process exit status에 부여했던 원래 우선순위
- `-P`의 64-bit value path가 설계 당시 인지된 제한이었는지 여부
