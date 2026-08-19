# ft_nm Baseline — CHECKPOINT B

Evidence class: environment, build, fixture, comparison, harness, source hash 결과는
**VERIFIED FROM RUNTIME TEST**다.

## Reproducibility

```text
Recorded at: 2026-08-19T19:21:33+09:00
Commit: 064ca553813e93c7d1870cdeab440f7c1ece56f9
Branch: ver1.2
OS/Kernel/Arch: Linux-7.0.0-29-generic-x86_64-with-glibc2.43
Compiler: gcc (Ubuntu 15.2.0-16ubuntu1) 15.2.0
Oracle: GNU nm (GNU Binutils for Ubuntu) 2.46
Build: make re
Harness: python3 portfolio_audit/tools/run_baseline.py
Locale: LC_ALL=C
```

## Result

| PASS | PARTIAL | FAIL | CRASH | Total cases | Generated fixtures |
|---:|---:|---:|---:|---:|---:|
| 13 | 5 | 12 | 0 | 30 | 21 |

Harness mutation validation: 5/5 detected.

Security/ASan assessment: **EXCLUDED BY USER**. 보안 진단은 실행·해석·집계하지 않았다.

Tracked source integrity: UNCHANGED.
Before/after tracked hash maps and Git diff are stored in `raw/source_integrity.json`.

## Interpretation boundary

PASS 수는 이 30개 fixture/case에서 정의한 동작만 설명한다. 미시험 architecture, 모든 ELF,
모든 archive producer, 모든 GNU `nm` option에 대한 일반적 호환성을 의미하지 않는다.
