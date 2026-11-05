# ft_nm Baseline Environment — CHECKPOINT A

## Environment

Evidence class: **VERIFIED FROM RUNTIME TEST** (environment/tool probes only; target binary는
실행하지 않음).

```text
Recorded at: 2026-08-19T18:40:05+09:00
Commit: 064ca553813e93c7d1870cdeab440f7c1ece56f9
Branch: ver1.2
Dirty before audit: No (`git status --short` produced no output)
Upstream divergence: 0 ahead / 0 behind
OS: Ubuntu 26.04 LTS
Kernel: Linux 7.0.0-29-generic
Arch: x86_64
Compiler: gcc 15.2.0
GNU nm: GNU Binutils 2.46
Python: 3.14.4
Build command: make
Debug build command: make debug
Run command: ./ft_nm [options] [file ...]
Default input: a.out
```

버전 명령과 Git 조회만 수행했다. CHECKPOINT A 범위에서는 `make`, `make debug`,
`./ft_nm`, `nm`, `TestserMachine.py`를 실행하지 않았다. 그러므로 build 성공 여부와
런타임 결과는 아직 `확인 불가`다.

## Build/run source evidence

- default target와 산출물 `ft_nm`: `Makefile:4`, `Makefile:39-45`
- debug/ASan target: `Makefile:13-19`, `Makefile:47-48`
- documented build/run commands: `README.md:39-57`
- 인자가 없을 때 `a.out`: `src/nm.c:39-46`

## Initial source integrity hashes

아래 SHA-256은 audit 문서를 만들기 전에 기록했다.

| Existing file | SHA-256 |
|---|---|
| `README.md` | `b32defd063165951ec6325e7ce8ce822a9d38435d7f7ab9576f6173808aea713` |
| `Makefile` | `2aee774376f343ceaa09ce74d8e25921c1f37ab8c9795764ddd2600cd92e29ba` |
| `include/nm.h` | `b44352916330fa143126db5fd546f49229e79fb34f9799b4ec0b742eb7a25834` |
| `src/nm.c` | `db0af7efd6e801b83285e15c0e6657e0e013179867f1fa0b89157380fb1a2609` |
| `src/arg.c` | `f787bf5deeab524fe44593e13a567f446073c54e8a0018556c4a3ff830300928` |
| `src/io_unit.c` | `3bafec6ecb5ee040313721743a0b8abd491959073b2dc75e1fc9d21a8c77f96a` |
| `src/format_router.c` | `2d484f34f5909e6fc66ea7d4826a26c5a7de23393b76a91e7d599f3b2e237a87` |
| `src/ar_parser.c` | `92694bebaa10f5fa441bf2ba6b424e7bf69e6a2502b5803da089b73c2e4669f2` |
| `src/elf_parser.c` | `528df346a3594a5c9818d0845e5ca41f1dd4ce0ddcbfdbe66b59388e4a50a7ec` |
| `src/sym_classify.c` | `878738b72a93f680b4388e3db1a46fbc8ddf84fe99123b2a77ae9aa6c9bd9603` |
| `src/sort_filter_print.c` | `12ba8a20cd63f1396710d2cc8dbe023378acf1f0d939fc28b14a997f36f8779f` |

최종 검증에서는 같은 파일의 hash와 Git diff를 다시 확인한다.

감사 문서 생성 후의 working tree는 `portfolio_audit/`만 untracked인 dirty 상태다. tracked
source diff는 없으며 위 hash는 재확인되었다. 이 상태 설명은 **VERIFIED FROM RUNTIME
TEST**다.
