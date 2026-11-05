"""Run after make. Exercise the public CLI using the audit's checked-in ELF fixtures."""
import struct
import subprocess
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
FIXTURES = ROOT / "portfolio_audit/bin/fixtures"


def nm(*args):
    return subprocess.run([ROOT / "ft_nm", *args], capture_output=True, text=True, timeout=4)


class AuditRegressionTests(unittest.TestCase):
    def test_global_filter_uses_elf_binding(self):
        result = nm("-g", FIXTURES / "symbol_types.o")
        self.assertEqual(result.returncode, 0)
        records = {tuple(line.split()[-2:]) for line in result.stdout.splitlines()}
        self.assertTrue({("C", "common_symbol"), ("C", "global_bss"),
                         ("i", "indirect_function")} <= records)

    def test_posix_full_width_value_and_size(self):
        with tempfile.TemporaryDirectory() as tmp:
            obj = Path(tmp) / "wide.o"
            subprocess.run(["as", "--64", "-o", obj], input=(
                ".globl wide\n.set wide, 0x100000123\n.size wide, 0x200000456\n"
            ), text=True, check=True)
            result = nm("-P", obj)
            self.assertEqual(result.returncode, 0)
            self.assertEqual(result.stdout.split(), ["wide", "A", "100000123", "200000456"])

    def test_mixed_files_preserve_failure_and_continue(self):
        result = nm(FIXTURES / "empty_file", FIXTURES / "basic64.o")
        self.assertGreater(result.returncode, 0)
        self.assertIn("global_data", result.stdout)

    def test_malformed_files_rejected_without_partial_symbols(self):
        for name in ("malformed_bad_shentsize.o", "malformed_bad_strtab_offset.o",
                     "malformed_bad_symbol_shndx.o", "malformed_truncated_archive.a",
                     "malformed_bad_fmag.a"):
            with self.subTest(name=name):
                result = nm(FIXTURES / name)
                self.assertGreater(result.returncode, 0)
                self.assertFalse(result.stdout.strip())
                self.assertTrue(result.stderr.strip())

    def test_32bit_bad_section_size(self):
        with tempfile.TemporaryDirectory() as tmp:
            obj = Path(tmp) / "bad32.o"
            data = bytearray((FIXTURES / "simple32.o").read_bytes())
            struct.pack_into("<H", data, 46, 1)  # Elf32_Ehdr.e_shentsize
            obj.write_bytes(data)
            result = nm(obj)
            self.assertGreater(result.returncode, 0)
            self.assertFalse(result.stdout.strip())

    def test_bad_archive_member_preserves_status_and_continues(self):
        with tempfile.TemporaryDirectory() as tmp:
            archive = Path(tmp) / "mixed.a"
            subprocess.run(["ar", "rcS", archive, FIXTURES / "malformed_bad_shoff.o",
                            FIXTURES / "basic64.o"], check=True)
            result = nm(archive)
            self.assertGreater(result.returncode, 0)
            self.assertIn("malformed_bad_shoff.o", result.stderr)
            self.assertIn("global_data", result.stdout)


if __name__ == "__main__":
    unittest.main()
