# Improvement Candidate Comparison — No Source Change

이 문서는 후보를 비교할 뿐 source를 수정하지 않는다. 영향 수치는 baseline에서 실제로
측정된 값만 사용하며, 구현 비용과 portfolio value 평가는 **INFERENCE**다.

| Candidate | Measured problem | User rationale relation | Likely change scope | Trade-off |
|---|---|---|---|---|
| `-g` semantic filtering | GNU 15 records 중 target 12; `C` 2개와 `i` 1개 누락 | type-character filter는 의도적 단순화 | filter policy와 symbol metadata | 핵심 option 목표에 직접 연결되고 architecture 설명력이 높음 |
| 64-bit `-P` output | `100000123`이 `123`으로 출력 | rationale 미확인 | 64-bit output helper 또는 formatter | 심각한 silent value error이며 수정 범위는 비교적 국소적 |
| Error propagation | 5개 path/archive error case에서 GNU nonzero, target 0 | rationale 미확인 | main/path/archive result aggregation | CLI 신뢰성 영향이 넓지만 regression surface도 가장 넓음 |

## Recommended first candidate

`-g` semantic filtering을 먼저 추천한다.

- 핵심 option 구현을 우선했다는 **USER-PROVIDED RATIONALE**와 직접 연결된다.
- 15 대 12라는 재현 가능한 **VERIFIED FROM RUNTIME TEST**가 있다.
- presentation 문자와 semantic policy의 결합이라는 **VERIFIED FROM CODE** 원인이 있다.
- 문자 `C/i`만 whitelist에 추가하는 국소 patch와 binding-first/descriptor 설계의 차이를
  engineering decision으로 설명할 수 있다.

다만 실제 source 변경 전에 binding-first filter와 classification descriptor 중 하나를
사용자가 명시적으로 승인해야 한다.
