# ft_nm Baseline Test Results

Evidence class: 아래 분류와 수치는 **VERIFIED FROM RUNTIME TEST**다. GNU `nm` 2.46에
동일 input을 전달했고 raw stdout/stderr/exit status를 보존했다.

## Counts

| PASS | PARTIAL | FAIL | CRASH | Total |
|---:|---:|---:|---:|---:|
| 13 | 5 | 12 | 0 | 30 |

## Matrix

| ID | Category | Classification | Compared behavior | Result | Raw |
|---|---|---|---|---|---|
| `t01_elf64_rel_default` | ELF64 | **PASS** | ELF64 relocatable basic name/type/value/order | semantic symbol records, order, status, and diagnostics match | [`raw/cases/t01_elf64_rel_default`](raw/cases/t01_elf64_rel_default/comparison.json) |
| `t02_elf32_rel_default` | ELF32 | **PASS** | ELF32 relocatable parsing and 32-bit values | semantic symbol records, order, status, and diagnostics match | [`raw/cases/t02_elf32_rel_default`](raw/cases/t02_elf32_rel_default/comparison.json) |
| `t03_elf64_exec_default` | ELF type | **PASS** | ET_EXEC static symbol extraction | semantic symbol record set and status match; order is tested separately | [`raw/cases/t03_elf64_exec_default`](raw/cases/t03_elf64_exec_default/comparison.json) |
| `t04_elf64_shared_default` | ELF type | **PASS** | ET_DYN shared object static symbol extraction | semantic symbol record set and status match; order is tested separately | [`raw/cases/t04_elf64_shared_default`](raw/cases/t04_elf64_shared_default/comparison.json) |
| `t05_dynsym_only` | Symbol table | **PARTIAL** | dynsym-only/no-SHT_SYMTAB behavior | primary diagnostic matches but extra/missing diagnostic categories differ | [`raw/cases/t05_dynsym_only`](raw/cases/t05_dynsym_only/comparison.json) |
| `t06_archive_short` | Archive | **PASS** | regular archive short member traversal | semantic symbol records, order, status, and diagnostics match | [`raw/cases/t06_archive_short`](raw/cases/t06_archive_short/comparison.json) |
| `t07_archive_long` | Archive | **PASS** | GNU // and /offset long member names | semantic symbol records, order, status, and diagnostics match | [`raw/cases/t07_archive_long`](raw/cases/t07_archive_long/comparison.json) |
| `t08_option_all` | Option -a | **PASS** | all-symbol inclusion | semantic symbol record set and status match; order is tested separately | [`raw/cases/t08_option_all`](raw/cases/t08_option_all/comparison.json) |
| `t09_option_global` | Option -g | **FAIL** | external symbol inclusion/exclusion | missing or changed symbol record: (None, 'common_symbol', 'C', 4, None) | [`raw/cases/t09_option_global`](raw/cases/t09_option_global/comparison.json) |
| `t10_option_undefined` | Option -u | **PASS** | undefined and weak-undefined inclusion | semantic symbol records, order, status, and diagnostics match | [`raw/cases/t10_option_undefined`](raw/cases/t10_option_undefined/comparison.json) |
| `t11_sort_name` | Ordering | **FAIL** | default name collation with punctuation/case | symbol set matches but ordering differs | [`raw/cases/t11_sort_name`](raw/cases/t11_sort_name/comparison.json) |
| `t12_sort_numeric` | Ordering -n | **PASS** | numeric value ordering | semantic symbol records, order, status, and diagnostics match | [`raw/cases/t12_sort_numeric`](raw/cases/t12_sort_numeric/comparison.json) |
| `t13_sort_reverse` | Ordering -r | **PASS** | reverse of default ordering | semantic symbol records, order, status, and diagnostics match | [`raw/cases/t13_sort_reverse`](raw/cases/t13_sort_reverse/comparison.json) |
| `t14_posix_basic` | Option -P | **PASS** | POSIX record name/type/value/size | semantic symbol records, order, status, and diagnostics match | [`raw/cases/t14_posix_basic`](raw/cases/t14_posix_basic/comparison.json) |
| `t15_posix_high64` | Option -P | **FAIL** | POSIX output above 32-bit value range | missing or changed symbol record: (None, 'high_absolute', 'A', 4294967587, None) | [`raw/cases/t15_posix_high64`](raw/cases/t15_posix_high64/comparison.json) |
| `t16_option_u_n` | Option combination | **PASS** | undefined filter combined with numeric sort | semantic symbol records, order, status, and diagnostics match | [`raw/cases/t16_option_u_n`](raw/cases/t16_option_u_n/comparison.json) |
| `t17_multiple_files` | CLI | **PASS** | multiple-file headings and symbols | semantic symbol records, order, status, and diagnostics match | [`raw/cases/t17_multiple_files`](raw/cases/t17_multiple_files/comparison.json) |
| `t18_invalid_option` | CLI error | **PARTIAL** | invalid option status and diagnostic | diagnostic category matches but exact exit code or diagnostic detail differs | [`raw/cases/t18_invalid_option`](raw/cases/t18_invalid_option/comparison.json) |
| `t19_double_dash` | CLI | **FAIL** | end-of-options marker | success/error status differs | [`raw/cases/t19_double_dash`](raw/cases/t19_double_dash/comparison.json) |
| `t20_missing_file` | Path error | **FAIL** | missing path status and diagnostic | error status or diagnostic category differs | [`raw/cases/t20_missing_file`](raw/cases/t20_missing_file/comparison.json) |
| `t21_empty_file` | Format error | **FAIL** | empty file rejection | error status or diagnostic category differs | [`raw/cases/t21_empty_file`](raw/cases/t21_empty_file/comparison.json) |
| `t22_text_file` | Format error | **FAIL** | non-ELF/non-archive rejection | error status or diagnostic category differs | [`raw/cases/t22_text_file`](raw/cases/t22_text_file/comparison.json) |
| `t23_truncated_elf` | Malformed ELF | **PARTIAL** | truncated ELF header rejection | input is rejected without symbols, but diagnostic category differs | [`raw/cases/t23_truncated_elf`](raw/cases/t23_truncated_elf/comparison.json) |
| `t24_bad_shoff` | Malformed ELF | **PARTIAL** | out-of-file section table offset | input is rejected without symbols, but diagnostic category differs | [`raw/cases/t24_bad_shoff`](raw/cases/t24_bad_shoff/comparison.json) |
| `t25_bad_shentsize` | Malformed ELF | **FAIL** | invalid section header entry size | unexpected symbol output differs while handling invalid input | [`raw/cases/t25_bad_shentsize`](raw/cases/t25_bad_shentsize/comparison.json) |
| `t26_bad_strtab` | Malformed ELF | **FAIL** | out-of-file linked string table | unexpected symbol output differs while handling invalid input | [`raw/cases/t26_bad_strtab`](raw/cases/t26_bad_strtab/comparison.json) |
| `t27_bad_symbol_shndx` | Malformed ELF | **FAIL** | symbol section index beyond section count | missing or changed symbol record: (None, 'global_data', 'A', 4, None) | [`raw/cases/t27_bad_symbol_shndx`](raw/cases/t27_bad_symbol_shndx/comparison.json) |
| `t28_truncated_archive` | Malformed archive | **FAIL** | truncated archive member header | error status or diagnostic category differs | [`raw/cases/t28_truncated_archive`](raw/cases/t28_truncated_archive/comparison.json) |
| `t29_bad_archive_fmag` | Malformed archive | **FAIL** | invalid archive member trailer magic | error status or diagnostic category differs | [`raw/cases/t29_bad_archive_fmag`](raw/cases/t29_bad_archive_fmag/comparison.json) |
| `t30_bad_elf_archive_member` | Archive error | **PARTIAL** | malformed ELF member error propagation | input is rejected without symbols, but diagnostic category differs | [`raw/cases/t30_bad_elf_archive_member`](raw/cases/t30_bad_elf_archive_member/comparison.json) |

## Classification boundary

- PASS: parsed semantic records, required order, success/error status, diagnostic category가 일치
- PARTIAL: 핵심 symbol/error category는 일치하지만 meaningful diagnostic 또는 exact status 차이 존재
- FAIL: symbol record/set/order, success-vs-error status, diagnostic category 중 핵심 동작이 불일치
- CRASH: signal, timeout, 또는 usable result 부재

Raw output은 정규화하지 않았고 `raw/cases/<id>/`에 그대로 보존했다.
