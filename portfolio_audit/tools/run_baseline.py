#!/usr/bin/env python3
"""Deterministic GNU nm vs ft_nm baseline runner.

All generated fixtures and evidence stay below portfolio_audit/. Existing tracked
project files are hashed before and after the run and are never edited here.
"""

from __future__ import annotations

import copy
import hashlib
import json
import os
import platform
import re
import shutil
import struct
import subprocess
import sys
import time
from collections import Counter
from datetime import datetime
from pathlib import Path
from typing import Any


AUDIT = Path(__file__).resolve().parents[1]
REPO = AUDIT.parent
TESTS = AUDIT / "tests"
BIN = AUDIT / "bin"
FIXTURES = BIN / "fixtures"
RAW = AUDIT / "raw"
SETUP_RAW = RAW / "setup"
CASE_RAW = RAW / "cases"
TARGET_BUILD = REPO / "ft_nm"
TARGET_BASELINE = BIN / "ft_nm_baseline"
ORACLE = Path(shutil.which("nm") or "nm")
ENV = {**os.environ, "LC_ALL": "C", "LANG": "C"}
TIMEOUT_SECONDS = 4


def write_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def run_process(
    command: list[str],
    *,
    cwd: Path = REPO,
    env: dict[str, str] | None = None,
    timeout: int = TIMEOUT_SECONDS,
) -> dict[str, Any]:
    started = time.monotonic()
    try:
        completed = subprocess.run(
            command,
            cwd=cwd,
            env=env or ENV,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=timeout,
            check=False,
        )
        return {
            "command": command,
            "cwd": str(cwd),
            "exit_code": completed.returncode,
            "timed_out": False,
            "duration_seconds": round(time.monotonic() - started, 6),
            "stdout": completed.stdout.decode("utf-8", errors="replace"),
            "stderr": completed.stderr.decode("utf-8", errors="replace"),
        }
    except subprocess.TimeoutExpired as error:
        stdout = error.stdout or b""
        stderr = error.stderr or b""
        return {
            "command": command,
            "cwd": str(cwd),
            "exit_code": None,
            "timed_out": True,
            "duration_seconds": round(time.monotonic() - started, 6),
            "stdout": stdout.decode("utf-8", errors="replace"),
            "stderr": stderr.decode("utf-8", errors="replace"),
        }


def save_run(stem: Path, run: dict[str, Any]) -> None:
    stem.parent.mkdir(parents=True, exist_ok=True)
    stem.with_suffix(".stdout").write_text(run["stdout"], encoding="utf-8")
    stem.with_suffix(".stderr").write_text(run["stderr"], encoding="utf-8")
    metadata = {key: value for key, value in run.items() if key not in {"stdout", "stderr"}}
    write_json(stem.with_suffix(".json"), metadata)


def setup_step(name: str, command: list[str], *, cwd: Path = REPO) -> dict[str, Any]:
    run = run_process(command, cwd=cwd, timeout=60)
    save_run(SETUP_RAW / name, run)
    if run["timed_out"] or run["exit_code"] != 0:
        raise RuntimeError(f"setup step failed: {name}")
    return run


def git_output(*args: str) -> str:
    run = run_process(["git", *args], timeout=30)
    if run["exit_code"] != 0:
        raise RuntimeError(f"git {' '.join(args)} failed: {run['stderr']}")
    return run["stdout"].strip()


def tracked_hashes() -> dict[str, str]:
    run = subprocess.run(
        ["git", "ls-files", "-z"],
        cwd=REPO,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=True,
    )
    result: dict[str, str] = {}
    for raw_name in run.stdout.split(b"\0"):
        if not raw_name:
            continue
        relative = raw_name.decode("utf-8", errors="strict")
        path = REPO / relative
        if path.is_file():
            result[relative] = sha256(path)
    return result


def generate_malformed_elf(source: Path) -> dict[str, Path]:
    original = bytearray(source.read_bytes())
    if original[:4] != b"\x7fELF" or original[4] != 2 or original[5] != 1:
        raise RuntimeError("malformed fixture generator requires little-endian ELF64")

    shoff = struct.unpack_from("<Q", original, 40)[0]
    shentsize = struct.unpack_from("<H", original, 58)[0]
    shnum = struct.unpack_from("<H", original, 60)[0]
    if shentsize != 64 or not shoff or not shnum:
        raise RuntimeError("unexpected ELF64 section table layout")

    outputs: dict[str, Path] = {}

    truncated = FIXTURES / "malformed_truncated_elf"
    truncated.write_bytes(original[:32])
    outputs["truncated"] = truncated

    bad_shoff = bytearray(original)
    struct.pack_into("<Q", bad_shoff, 40, len(original) + 4096)
    bad_shoff_path = FIXTURES / "malformed_bad_shoff.o"
    bad_shoff_path.write_bytes(bad_shoff)
    outputs["bad_shoff"] = bad_shoff_path

    bad_shentsize = bytearray(original)
    struct.pack_into("<H", bad_shentsize, 58, 1)
    bad_shentsize_path = FIXTURES / "malformed_bad_shentsize.o"
    bad_shentsize_path.write_bytes(bad_shentsize)
    outputs["bad_shentsize"] = bad_shentsize_path

    symtab_index = None
    strtab_index = None
    symtab_offset = None
    symtab_size = None
    symtab_entsize = None
    for index in range(shnum):
        base = shoff + index * shentsize
        sh_type = struct.unpack_from("<I", original, base + 4)[0]
        if sh_type == 2:
            symtab_index = index
            symtab_offset = struct.unpack_from("<Q", original, base + 24)[0]
            symtab_size = struct.unpack_from("<Q", original, base + 32)[0]
            strtab_index = struct.unpack_from("<I", original, base + 40)[0]
            symtab_entsize = struct.unpack_from("<Q", original, base + 56)[0] or 24
            break
    if None in {symtab_index, strtab_index, symtab_offset, symtab_size, symtab_entsize}:
        raise RuntimeError("SHT_SYMTAB not found in basic fixture")

    strtab_header = shoff + int(strtab_index) * shentsize
    strtab_offset = struct.unpack_from("<Q", original, strtab_header + 24)[0]
    strtab_size = struct.unpack_from("<Q", original, strtab_header + 32)[0]
    strtab = original[strtab_offset : strtab_offset + strtab_size]

    bad_strtab = bytearray(original)
    struct.pack_into("<Q", bad_strtab, strtab_header + 24, len(original) + 8192)
    bad_strtab_path = FIXTURES / "malformed_bad_strtab_offset.o"
    bad_strtab_path.write_bytes(bad_strtab)
    outputs["bad_strtab"] = bad_strtab_path

    bad_shndx = bytearray(original)
    mutated = False
    count = int(symtab_size) // int(symtab_entsize)
    for index in range(count):
        entry = int(symtab_offset) + index * int(symtab_entsize)
        name_offset = struct.unpack_from("<I", original, entry)[0]
        if name_offset >= len(strtab):
            continue
        name = strtab[name_offset:].split(b"\0", 1)[0]
        if name == b"global_data":
            struct.pack_into("<H", bad_shndx, entry + 6, shnum + 32)
            mutated = True
            break
    if not mutated:
        raise RuntimeError("global_data symbol not found")
    bad_shndx_path = FIXTURES / "malformed_bad_symbol_shndx.o"
    bad_shndx_path.write_bytes(bad_shndx)
    outputs["bad_shndx"] = bad_shndx_path

    return outputs


def build_fixtures() -> dict[str, Path]:
    fixture_commands = [
        ("fixture_basic64", ["gcc", "-c", "-g", "-o", str(FIXTURES / "basic64.o"), str(TESTS / "basic64.s")]),
        ("fixture_simple32", ["gcc", "-m32", "-c", "-g", "-o", str(FIXTURES / "simple32.o"), str(TESTS / "simple32.s")]),
        ("fixture_order64", ["gcc", "-c", "-g", "-o", str(FIXTURES / "order64.o"), str(TESTS / "order64.s")]),
        ("fixture_high64", ["gcc", "-c", "-g", "-o", str(FIXTURES / "high64.o"), str(TESTS / "high64.s")]),
        (
            "fixture_symbol_types",
            [
                "gcc", "-fcommon", "-fno-stack-protector", "-O0", "-g", "-c",
                "-o", str(FIXTURES / "symbol_types.o"), str(TESTS / "symbol_types.c"),
            ],
        ),
        (
            "fixture_executable",
            ["gcc", "-no-pie", "-O0", "-g", "-o", str(FIXTURES / "executable64"), str(TESTS / "executable.c")],
        ),
        (
            "fixture_shared",
            ["gcc", "-shared", "-fPIC", "-O0", "-g", "-o", str(FIXTURES / "shared64.so"), str(TESTS / "shared.c")],
        ),
    ]
    for name, command in fixture_commands:
        setup_step(name, command)

    stripped = FIXTURES / "shared64_dynsym_only.so"
    shutil.copy2(FIXTURES / "shared64.so", stripped)
    setup_step("fixture_strip_dynsym_only", ["strip", "--strip-all", str(stripped)])

    long_member = FIXTURES / "member_with_a_very_long_filename_for_archive.o"
    shutil.copy2(FIXTURES / "basic64.o", long_member)
    setup_step(
        "fixture_archive_short",
        ["ar", "rcs", str(FIXTURES / "archive_short.a"), str(FIXTURES / "basic64.o"), str(FIXTURES / "simple32.o")],
    )
    setup_step(
        "fixture_archive_long",
        ["ar", "rcs", str(FIXTURES / "archive_long.a"), str(long_member)],
    )

    (FIXTURES / "empty_file").write_bytes(b"")
    (FIXTURES / "plain_text.txt").write_text("deterministic non-ELF input\n", encoding="ascii")
    malformed = generate_malformed_elf(FIXTURES / "basic64.o")

    (FIXTURES / "malformed_truncated_archive.a").write_bytes(b"!<arch>\n" + b"partial")
    bad_fmag = bytearray((FIXTURES / "archive_short.a").read_bytes())
    if len(bad_fmag) < 68:
        raise RuntimeError("archive fixture unexpectedly short")
    bad_fmag[8 + 58 : 8 + 60] = b"??"
    (FIXTURES / "malformed_bad_fmag.a").write_bytes(bad_fmag)
    setup_step(
        "fixture_archive_bad_elf_member",
        ["ar", "rcs", str(FIXTURES / "archive_bad_elf_member.a"), str(malformed["bad_shoff"])],
    )

    setup_step("verify_basic64_header", ["readelf", "-h", str(FIXTURES / "basic64.o")])
    setup_step("verify_simple32_header", ["readelf", "-h", str(FIXTURES / "simple32.o")])
    setup_step("verify_symbol_table", ["readelf", "-Ws", str(FIXTURES / "symbol_types.o")])
    setup_step("verify_archive_short", ["ar", "t", str(FIXTURES / "archive_short.a")])
    setup_step("verify_archive_long", ["ar", "t", str(FIXTURES / "archive_long.a")])

    return {
        "basic64": FIXTURES / "basic64.o",
        "simple32": FIXTURES / "simple32.o",
        "order64": FIXTURES / "order64.o",
        "high64": FIXTURES / "high64.o",
        "symbol_types": FIXTURES / "symbol_types.o",
        "executable": FIXTURES / "executable64",
        "shared": FIXTURES / "shared64.so",
        "dynsym_only": stripped,
        "archive_short": FIXTURES / "archive_short.a",
        "archive_long": FIXTURES / "archive_long.a",
        "empty": FIXTURES / "empty_file",
        "text": FIXTURES / "plain_text.txt",
        "truncated_elf": malformed["truncated"],
        "bad_shoff": malformed["bad_shoff"],
        "bad_shentsize": malformed["bad_shentsize"],
        "bad_strtab": malformed["bad_strtab"],
        "bad_shndx": malformed["bad_shndx"],
        "truncated_archive": FIXTURES / "malformed_truncated_archive.a",
        "bad_fmag": FIXTURES / "malformed_bad_fmag.a",
        "bad_elf_archive": FIXTURES / "archive_bad_elf_member.a",
        "missing": FIXTURES / "does_not_exist.o",
    }


def normalize_context(text: str) -> str:
    context = text.strip()
    fixture_prefix = str(FIXTURES) + os.sep
    context = context.replace(fixture_prefix, "")
    if "/" in context and "(" not in context and "[" not in context:
        context = Path(context).name
    return context


DEFAULT_SYMBOL = re.compile(
    r"^(?:(?P<value>[0-9A-Fa-f]+)\s+)?(?P<type>[A-Za-z?])\s+(?P<name>.+?)\s*$"
)
POSIX_SYMBOL = re.compile(
    r"^(?P<name>\S+)\s+(?P<type>[A-Za-z?])(?:\s+(?P<value>[0-9A-Fa-f]+))?(?:\s+(?P<size>[0-9A-Fa-f]+))?\s*$"
)


def parse_symbol_records(stdout: str, mode: str) -> tuple[list[dict[str, Any]], list[str]]:
    records: list[dict[str, Any]] = []
    unparsed: list[str] = []
    context: str | None = None
    pattern = POSIX_SYMBOL if mode == "posix" else DEFAULT_SYMBOL
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if not line:
            continue
        if line.endswith(":"):
            context = normalize_context(line[:-1])
            continue
        match = pattern.match(line)
        if not match:
            unparsed.append(raw_line)
            continue
        groups = match.groupdict()
        value_text = groups.get("value")
        size_text = groups.get("size")
        records.append(
            {
                "context": context,
                "name": groups["name"],
                "type": groups["type"],
                "value": int(value_text, 16) if value_text is not None else None,
                "size": int(size_text, 16) if size_text is not None else None,
            }
        )
    return records, unparsed


def diagnostic_classes(stderr: str) -> list[str]:
    classes: set[str] = set()
    lowered = stderr.lower()
    lines = [line.strip().lower() for line in stderr.splitlines() if line.strip()]
    if "invalid option" in lowered or "unrecognized option" in lowered:
        classes.add("invalid_option")
    if "no such file" in lowered or "cannot open" in lowered:
        classes.add("missing_file")
    if any(re.search(r":\s*no symbols\s*$", line) for line in lines):
        classes.add("no_symbols")
    malformed_markers = (
        "file format not recognized",
        "format not recognized",
        "file truncated",
        "malformed archive",
        "section header size",
    )
    if any(marker in lowered for marker in malformed_markers):
        classes.add("invalid_format")
    if "addresssanitizer" in lowered:
        classes.add("asan_error")
    if stderr.strip() and not classes:
        classes.add("other_diagnostic")
    return sorted(classes)


def status_kind(run: dict[str, Any]) -> str:
    if run["timed_out"]:
        return "timeout"
    code = run["exit_code"]
    if code is not None and code < 0:
        return "crash"
    if code == 0:
        return "ok"
    return "error"


def normalize_run(run: dict[str, Any], mode: str) -> dict[str, Any]:
    records: list[dict[str, Any]] = []
    unparsed: list[str] = []
    if mode in {"default", "posix", "error"}:
        parse_mode = "default" if mode == "error" else mode
        records, unparsed = parse_symbol_records(run["stdout"], parse_mode)
    return {
        "exit_code": run["exit_code"],
        "status_kind": status_kind(run),
        "diagnostic_classes": diagnostic_classes(run["stderr"]),
        "diagnostic_line_count": len([line for line in run["stderr"].splitlines() if line.strip()]),
        "records": records,
        "unparsed_stdout": unparsed,
    }


def record_key(record: dict[str, Any]) -> tuple[Any, ...]:
    return (record["context"], record["name"], record["type"], record["value"], record["size"])


def record_counter(records: list[dict[str, Any]]) -> Counter[tuple[Any, ...]]:
    return Counter(record_key(record) for record in records)


def compare_normalized(
    case: dict[str, Any], oracle: dict[str, Any], target: dict[str, Any]
) -> tuple[str, str, dict[str, Any]]:
    dimensions = {
        "status_kind_equal": oracle["status_kind"] == target["status_kind"],
        "exit_code_equal": oracle["exit_code"] == target["exit_code"],
        "diagnostic_classes_equal": oracle["diagnostic_classes"] == target["diagnostic_classes"],
    }
    if target["status_kind"] in {"timeout", "crash"}:
        return "CRASH", f"target {target['status_kind']}", dimensions

    if case["mode"] in {"default", "posix"}:
        dimensions["records_equal"] = oracle["records"] == target["records"]
        dimensions["record_multiset_equal"] = record_counter(oracle["records"]) == record_counter(target["records"])
        dimensions["unparsed_stdout_equal"] = oracle["unparsed_stdout"] == target["unparsed_stdout"]
        if not dimensions["status_kind_equal"]:
            return "FAIL", "success/error status differs", dimensions
        if dimensions["records_equal"] and dimensions["unparsed_stdout_equal"]:
            if dimensions["diagnostic_classes_equal"]:
                return "PASS", "semantic symbol records, order, status, and diagnostics match", dimensions
            return "PARTIAL", "symbol records match but diagnostic behavior differs", dimensions
        if dimensions["record_multiset_equal"]:
            if not case.get("order_sensitive", True):
                if dimensions["diagnostic_classes_equal"]:
                    return "PASS", "semantic symbol record set and status match; order is tested separately", dimensions
                return "PARTIAL", "symbol record set matches but diagnostic behavior differs", dimensions
            return "FAIL", "symbol set matches but ordering differs", dimensions
        oracle_counter = record_counter(oracle["records"])
        target_counter = record_counter(target["records"])
        missing = list((oracle_counter - target_counter).elements())
        extra = list((target_counter - oracle_counter).elements())
        if missing:
            return "FAIL", f"missing or changed symbol record: {missing[0]}", dimensions
        if extra:
            return "FAIL", f"extra or changed symbol record: {extra[0]}", dimensions
        return "FAIL", "stdout could not be parsed equivalently", dimensions

    status_semantics_equal = (
        (oracle["exit_code"] == 0 and target["exit_code"] == 0)
        or (oracle["exit_code"] not in {None, 0} and target["exit_code"] not in {None, 0})
    )
    dimensions["status_semantics_equal"] = status_semantics_equal
    dimensions["records_equal"] = oracle["records"] == target["records"]
    dimensions["record_multiset_equal"] = record_counter(oracle["records"]) == record_counter(target["records"])
    oracle_diag = set(oracle["diagnostic_classes"])
    target_diag = set(target["diagnostic_classes"])
    dimensions["diagnostic_overlap"] = bool(oracle_diag & target_diag)
    if not dimensions["record_multiset_equal"]:
        return "FAIL", "unexpected symbol output differs while handling invalid input", dimensions
    if status_semantics_equal and oracle_diag == target_diag:
        dimensions["diagnostic_line_count_equal"] = (
            oracle["diagnostic_line_count"] == target["diagnostic_line_count"]
        )
        if dimensions["exit_code_equal"] and dimensions["diagnostic_line_count_equal"]:
            return "PASS", "error/success status and diagnostic category match", dimensions
        return "PARTIAL", "diagnostic category matches but exact exit code or diagnostic detail differs", dimensions
    if status_semantics_equal and dimensions["diagnostic_overlap"]:
        return "PARTIAL", "primary diagnostic matches but extra/missing diagnostic categories differ", dimensions
    if status_semantics_equal and oracle_diag and target_diag:
        return "PARTIAL", "input is rejected without symbols, but diagnostic category differs", dimensions
    return "FAIL", "error status or diagnostic category differs", dimensions


def build_cases(fixtures: dict[str, Path]) -> list[dict[str, Any]]:
    def path(name: str) -> str:
        return str(fixtures[name])

    return [
        {"id": "t01_elf64_rel_default", "category": "ELF64", "mode": "default", "argv": [path("basic64")], "purpose": "ELF64 relocatable basic name/type/value/order"},
        {"id": "t02_elf32_rel_default", "category": "ELF32", "mode": "default", "argv": [path("simple32")], "purpose": "ELF32 relocatable parsing and 32-bit values"},
        {"id": "t03_elf64_exec_default", "category": "ELF type", "mode": "default", "order_sensitive": False, "argv": [path("executable")], "purpose": "ET_EXEC static symbol extraction"},
        {"id": "t04_elf64_shared_default", "category": "ELF type", "mode": "default", "order_sensitive": False, "argv": [path("shared")], "purpose": "ET_DYN shared object static symbol extraction"},
        {"id": "t05_dynsym_only", "category": "Symbol table", "mode": "error", "argv": [path("dynsym_only")], "purpose": "dynsym-only/no-SHT_SYMTAB behavior"},
        {"id": "t06_archive_short", "category": "Archive", "mode": "default", "argv": [path("archive_short")], "purpose": "regular archive short member traversal"},
        {"id": "t07_archive_long", "category": "Archive", "mode": "default", "argv": [path("archive_long")], "purpose": "GNU // and /offset long member names"},
        {"id": "t08_option_all", "category": "Option -a", "mode": "default", "order_sensitive": False, "argv": ["-a", path("symbol_types")], "purpose": "all-symbol inclusion"},
        {"id": "t09_option_global", "category": "Option -g", "mode": "default", "argv": ["-g", path("symbol_types")], "purpose": "external symbol inclusion/exclusion"},
        {"id": "t10_option_undefined", "category": "Option -u", "mode": "default", "argv": ["-u", path("symbol_types")], "purpose": "undefined and weak-undefined inclusion"},
        {"id": "t11_sort_name", "category": "Ordering", "mode": "default", "argv": [path("order64")], "purpose": "default name collation with punctuation/case"},
        {"id": "t12_sort_numeric", "category": "Ordering -n", "mode": "default", "argv": ["-n", path("order64")], "purpose": "numeric value ordering"},
        {"id": "t13_sort_reverse", "category": "Ordering -r", "mode": "default", "argv": ["-r", path("basic64")], "purpose": "reverse of default ordering"},
        {"id": "t14_posix_basic", "category": "Option -P", "mode": "posix", "argv": ["-P", path("basic64")], "purpose": "POSIX record name/type/value/size"},
        {"id": "t15_posix_high64", "category": "Option -P", "mode": "posix", "argv": ["-P", path("high64")], "purpose": "POSIX output above 32-bit value range"},
        {"id": "t16_option_u_n", "category": "Option combination", "mode": "default", "argv": ["-u", "-n", path("basic64")], "purpose": "undefined filter combined with numeric sort"},
        {"id": "t17_multiple_files", "category": "CLI", "mode": "default", "argv": [path("basic64"), path("simple32")], "purpose": "multiple-file headings and symbols"},
        {"id": "t18_invalid_option", "category": "CLI error", "mode": "error", "argv": ["-z", path("basic64")], "purpose": "invalid option status and diagnostic"},
        {"id": "t19_double_dash", "category": "CLI", "mode": "default", "argv": ["--", path("basic64")], "purpose": "end-of-options marker"},
        {"id": "t20_missing_file", "category": "Path error", "mode": "error", "argv": [path("missing")], "purpose": "missing path status and diagnostic"},
        {"id": "t21_empty_file", "category": "Format error", "mode": "error", "argv": [path("empty")], "purpose": "empty file rejection"},
        {"id": "t22_text_file", "category": "Format error", "mode": "error", "argv": [path("text")], "purpose": "non-ELF/non-archive rejection"},
        {"id": "t23_truncated_elf", "category": "Malformed ELF", "mode": "error", "argv": [path("truncated_elf")], "purpose": "truncated ELF header rejection"},
        {"id": "t24_bad_shoff", "category": "Malformed ELF", "mode": "error", "argv": [path("bad_shoff")], "purpose": "out-of-file section table offset"},
        {"id": "t25_bad_shentsize", "category": "Malformed ELF", "mode": "error", "argv": [path("bad_shentsize")], "purpose": "invalid section header entry size"},
        {"id": "t26_bad_strtab", "category": "Malformed ELF", "mode": "error", "argv": [path("bad_strtab")], "purpose": "out-of-file linked string table"},
        {"id": "t27_bad_symbol_shndx", "category": "Malformed ELF", "mode": "default", "argv": [path("bad_shndx")], "purpose": "symbol section index beyond section count"},
        {"id": "t28_truncated_archive", "category": "Malformed archive", "mode": "error", "argv": [path("truncated_archive")], "purpose": "truncated archive member header"},
        {"id": "t29_bad_archive_fmag", "category": "Malformed archive", "mode": "error", "argv": [path("bad_fmag")], "purpose": "invalid archive member trailer magic"},
        {"id": "t30_bad_elf_archive_member", "category": "Archive error", "mode": "error", "argv": [path("bad_elf_archive")], "purpose": "malformed ELF member error propagation"},
    ]


def run_case(case: dict[str, Any]) -> dict[str, Any]:
    case_dir = CASE_RAW / case["id"]
    oracle_run = run_process([str(ORACLE), *case["argv"]])
    target_run = run_process([str(TARGET_BASELINE), *case["argv"]])
    save_run(case_dir / "oracle", oracle_run)
    save_run(case_dir / "target", target_run)
    oracle_norm = normalize_run(oracle_run, case["mode"])
    target_norm = normalize_run(target_run, case["mode"])
    classification, summary, dimensions = compare_normalized(case, oracle_norm, target_norm)
    result = {
        **case,
        "oracle_command": oracle_run["command"],
        "target_command": target_run["command"],
        "classification": classification,
        "summary": summary,
        "dimensions": dimensions,
        "oracle": oracle_norm,
        "target": target_norm,
        "raw_directory": str(case_dir.relative_to(AUDIT)),
    }
    write_json(case_dir / "comparison.json", result)
    return result


def validate_harness(first_case: dict[str, Any]) -> dict[str, Any]:
    case = {**first_case, "mode": "default"}
    oracle = copy.deepcopy(first_case["oracle"])
    validations: list[dict[str, Any]] = []

    def check(name: str, mutate) -> None:
        synthetic = copy.deepcopy(oracle)
        mutate(synthetic)
        classification, summary, dimensions = compare_normalized(case, oracle, synthetic)
        validations.append(
            {
                "mutation": name,
                "classification": classification,
                "detected": classification in {"FAIL", "CRASH"},
                "summary": summary,
                "dimensions": dimensions,
            }
        )

    check("remove_symbol_record", lambda target: target["records"].pop())
    check("change_symbol_type", lambda target: target["records"][0].__setitem__("type", "?"))

    def change_value(target: dict[str, Any]) -> None:
        for record in target["records"]:
            if record["value"] is not None:
                record["value"] += 1
                return
        raise RuntimeError("no valued record for harness mutation")

    check("change_symbol_value", change_value)
    check("swap_symbol_order", lambda target: target["records"].__setitem__(slice(0, 2), reversed(target["records"][:2])))

    def change_exit(target: dict[str, Any]) -> None:
        target["exit_code"] = 1
        target["status_kind"] = "error"

    check("change_exit_status", change_exit)
    report = {
        "validation_count": len(validations),
        "detected_count": sum(1 for item in validations if item["detected"]),
        "all_detected": all(item["detected"] for item in validations),
        "validations": validations,
    }
    write_json(RAW / "harness_validation.json", report)
    return report


def render_test_results(results: list[dict[str, Any]], counts: dict[str, int]) -> str:
    lines = [
        "# ft_nm Baseline Test Results",
        "",
        "Evidence class: 아래 분류와 수치는 **VERIFIED FROM RUNTIME TEST**다. GNU `nm` 2.46에",
        "동일 input을 전달했고 raw stdout/stderr/exit status를 보존했다.",
        "",
        "## Counts",
        "",
        "| PASS | PARTIAL | FAIL | CRASH | Total |",
        "|---:|---:|---:|---:|---:|",
        f"| {counts['PASS']} | {counts['PARTIAL']} | {counts['FAIL']} | {counts['CRASH']} | {len(results)} |",
        "",
        "## Matrix",
        "",
        "| ID | Category | Classification | Compared behavior | Result | Raw |",
        "|---|---|---|---|---|---|",
    ]
    for item in results:
        raw = item["raw_directory"]
        lines.append(
            f"| `{item['id']}` | {item['category']} | **{item['classification']}** | "
            f"{item['purpose']} | {item['summary']} | [`{raw}`]({raw}/comparison.json) |"
        )
    lines.extend(
        [
            "",
            "## Classification boundary",
            "",
            "- PASS: parsed semantic records, required order, success/error status, diagnostic category가 일치",
            "- PARTIAL: 핵심 symbol/error category는 일치하지만 meaningful diagnostic 또는 exact status 차이 존재",
            "- FAIL: symbol record/set/order, success-vs-error status, diagnostic category 중 핵심 동작이 불일치",
            "- CRASH: signal, timeout, 또는 usable result 부재",
            "",
            "Raw output은 정규화하지 않았고 `raw/cases/<id>/`에 그대로 보존했다.",
        ]
    )
    return "\n".join(lines) + "\n"


def render_baseline(
    metadata: dict[str, Any],
    counts: dict[str, int],
    total: int,
    harness: dict[str, Any],
    source_integrity: dict[str, Any],
    fixture_count: int,
) -> str:
    return f"""# ft_nm Baseline — CHECKPOINT B

Evidence class: environment, build, fixture, comparison, harness, source hash 결과는
**VERIFIED FROM RUNTIME TEST**다.

## Reproducibility

```text
Recorded at: {metadata['recorded_at']}
Commit: {metadata['commit']}
Branch: {metadata['branch']}
OS/Kernel/Arch: {metadata['platform']}
Compiler: {metadata['compiler']}
Oracle: {metadata['oracle']}
Build: make re
Harness: python3 portfolio_audit/tools/run_baseline.py
Locale: LC_ALL=C
```

## Result

| PASS | PARTIAL | FAIL | CRASH | Total cases | Generated fixtures |
|---:|---:|---:|---:|---:|---:|
| {counts['PASS']} | {counts['PARTIAL']} | {counts['FAIL']} | {counts['CRASH']} | {total} | {fixture_count} |

Harness mutation validation: {harness['detected_count']}/{harness['validation_count']} detected.

Security/ASan assessment: **EXCLUDED BY USER**. 보안 진단은 실행·해석·집계하지 않았다.

Tracked source integrity: {'UNCHANGED' if source_integrity['unchanged'] else 'CHANGED'}.
Before/after tracked hash maps and Git diff are stored in `raw/source_integrity.json`.

## Interpretation boundary

PASS 수는 이 30개 fixture/case에서 정의한 동작만 설명한다. 미시험 architecture, 모든 ELF,
모든 archive producer, 모든 GNU `nm` option에 대한 일반적 호환성을 의미하지 않는다.
"""


def main() -> int:
    for generated in (FIXTURES, SETUP_RAW, CASE_RAW):
        if generated.exists():
            shutil.rmtree(generated)
        generated.mkdir(parents=True, exist_ok=True)

    before_hashes = tracked_hashes()
    before_diff = git_output("diff", "--", ".")
    metadata = {
        "recorded_at": datetime.now().astimezone().isoformat(timespec="seconds"),
        "commit": git_output("rev-parse", "HEAD"),
        "branch": git_output("branch", "--show-current"),
        "platform": platform.platform(),
        "compiler": run_process(["gcc", "--version"])["stdout"].splitlines()[0],
        "oracle": run_process([str(ORACLE), "--version"])["stdout"].splitlines()[0],
        "python": sys.version.splitlines()[0],
    }

    setup_step("build_default", ["make", "re"])
    if not TARGET_BUILD.is_file():
        raise RuntimeError("make re succeeded but ft_nm was not produced")
    shutil.copy2(TARGET_BUILD, TARGET_BASELINE)
    fixtures = build_fixtures()
    fixture_manifest = {
        str(path.relative_to(AUDIT)): {"sha256": sha256(path), "bytes": path.stat().st_size}
        for path in sorted(FIXTURES.iterdir())
        if path.is_file()
    }
    write_json(RAW / "fixture_manifest.json", fixture_manifest)

    cases = build_cases(fixtures)
    results = [run_case(case) for case in cases]
    counts = {label: sum(1 for result in results if result["classification"] == label) for label in ("PASS", "PARTIAL", "FAIL", "CRASH")}
    harness = validate_harness(results[0])

    setup_step("cleanup_build", ["make", "fclean"])
    after_hashes = tracked_hashes()
    after_diff = git_output("diff", "--", ".")
    source_integrity = {
        "before": before_hashes,
        "after": after_hashes,
        "hashes_equal": before_hashes == after_hashes,
        "git_diff_before": before_diff,
        "git_diff_after": after_diff,
        "unchanged": before_hashes == after_hashes and before_diff == after_diff == "",
    }
    write_json(RAW / "source_integrity.json", source_integrity)

    final = {
        "schema_version": 1,
        "metadata": metadata,
        "normalization": {
            "normalized": [
                "LC_ALL=C",
                "hexadecimal leading zeros and A-F case parsed as numeric values",
                "audit fixture root removed from file headings",
                "semantic whitespace parsed as field delimiters",
                "diagnostic executable prefix and quote style reduced to categories",
            ],
            "not_normalized": [
                "symbol name/type/value/size",
                "symbol inclusion/exclusion",
                "symbol order",
                "archive member identity",
                "success versus nonzero status",
                "diagnostic category",
                "signal or timeout in default execution",
            ],
        },
        "counts": counts,
        "test_count": len(results),
        "fixture_count": len(fixture_manifest),
        "tests": results,
        "harness_validation": harness,
        "security_assessment": {
            "status": "EXCLUDED_BY_USER",
            "executed": False,
            "counted_in_baseline": False,
        },
        "source_integrity": {
            "unchanged": source_integrity["unchanged"],
            "manifest": "raw/source_integrity.json",
        },
    }
    write_json(AUDIT / "test_results.json", final)
    (AUDIT / "test_results.md").write_text(render_test_results(results, counts), encoding="utf-8")
    (AUDIT / "baseline.md").write_text(
        render_baseline(metadata, counts, len(results), harness, source_integrity, len(fixture_manifest)),
        encoding="utf-8",
    )
    print(json.dumps({"counts": counts, "harness": harness, "security_assessment": "EXCLUDED_BY_USER", "source_unchanged": source_integrity["unchanged"]}, indent=2))
    return 0 if source_integrity["unchanged"] and harness["all_detected"] else 2


if __name__ == "__main__":
    raise SystemExit(main())
