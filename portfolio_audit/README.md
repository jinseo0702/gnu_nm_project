# ft_nm CHECKPOINT B Evidence Index

이 디렉터리는 기존 `ft_nm` source를 수정하지 않고 commit
`064ca553813e93c7d1870cdeab440f7c1ece56f9`의 정적 구현 범위와 GNU `nm` 2.46
baseline을 분석한 CHECKPOINT B 산출물이다.

## 이번 단계의 범위

- 완료: 환경 기록, architecture/inventory/scope 분석, clean build, 30-case deterministic
  comparison, 5-case harness mutation validation, 대표 failure 5개 분석
- 생성: 21개 fixture/artifact와 case별 raw stdout/stderr/exit metadata
- 수행하지 않음: source 수정, improvement cycle, benchmark, 사용자 요청으로 제외한 보안/ASan 평가

따라서 이 문서의 `IMPLEMENTED`는 해당 source 경로가 존재한다는 뜻이며 GNU `nm`과의
동작 일치나 malformed input 안전성을 뜻하지 않는다. 런타임으로 확인하지 않은 항목은
`확인 불가`로 유지한다.

## Evidence classification

- **VERIFIED FROM CODE**: source line 또는 Makefile/README line을 근거로 한 정적 사실
- **VERIFIED FROM RUNTIME TEST**: 환경/도구 probe, clean build, 30-case comparison,
  5-case harness validation, source hash 재검증
- **USER-PROVIDED RATIONALE**: 없음
- **INFERENCE**: 정적 공백에서 예상되는 위험과 향후 test case 선정
- **UNKNOWN**: 30개 case 밖의 GNU `nm` parity, 미시험 입력, 성능처럼 아직 측정하지 않은 결과

각 문서에서 별도 표기가 없는 source-evidence 문장은 **VERIFIED FROM CODE**다. 위험이
"발생할 수 있다"는 해석은 **INFERENCE**이며, 실제 발생 여부는 **UNKNOWN**으로 유지한다.

## Source-counted summary

| 항목 | 정적 확인 값 | 의미 |
|---|---:|---|
| CLI option letters | 6 | `-a`, `-g`, `-u`, `-r`, `-P`, `-n` |
| format/parser routes | 3 | ELF32, ELF64, regular `ar` |
| accepted ELF object types | 3 | `ET_REL`, `ET_EXEC`, `ET_DYN` |
| accepted machine IDs | 2 | `EM_386`, `EM_X86_64` |
| emitted symbol type characters | 22 | 분류 함수의 서로 다른 반환 문자 수 |
| selected symbol-table kinds | 1 | `SHT_SYMTAB`; `SHT_DYNSYM` 경로는 주석 처리됨 |
| deterministic runtime cases | 30 | 13 PASS, 5 PARTIAL, 12 FAIL, 0 CRASH |
| harness mutation checks | 5 | 5/5 injected errors detected |
| generated fixture/artifact files | 21 | SHA-256와 byte size를 manifest에 보존 |

## 문서

| 질문 | 문서 |
|---|---|
| 어떤 환경과 commit을 기준으로 했는가? | [`baseline_environment.md`](baseline_environment.md) |
| 프로그램은 어떤 흐름으로 동작하는가? | [`architecture.md`](architecture.md) |
| 무엇이 구현/부분 구현/미구현 상태인가? | [`feature_inventory.md`](feature_inventory.md) |
| ELF, archive, option, symbol 분류 범위는 어디까지인가? | [`implementation_scope.md`](implementation_scope.md) |
| 승인 후 무엇을 어떻게 비교할 것인가? | [`test_plan.md`](test_plan.md) |
| baseline 숫자와 재현 환경은 무엇인가? | [`baseline.md`](baseline.md) |
| case별 결과는 무엇인가? | [`test_results.md`](test_results.md), [`test_results.json`](test_results.json) |
| 대표 failure의 원인과 후보 설계는 무엇인가? | [`failures.md`](failures.md) |
| 사용자가 설명한 당시 설계 의도는 무엇인가? | [`design_rationale.md`](design_rationale.md) |
| 어떤 문제를 먼저 고칠 후보로 볼 것인가? | [`improvement.md`](improvement.md) |

## Stop condition

CHECKPOINT B에서 중단했다. 기존 source는 변경하지 않았고 improvement 후보는 아직 적용하지
않았다. source 수정은 별도 명시적 승인 이후에만 수행한다.
