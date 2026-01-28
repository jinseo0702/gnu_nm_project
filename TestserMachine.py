from ast import Try
import difflib
import itertools
from math import exp
from optparse import Option
import random
import subprocess
import os
from unittest import result

class TargetGenerator:
    def __init__(self):
        self.generator_files = []
        self.source_code = """
        #include <stdio.h>

        int global_var = 42;            /* Data Section (D) */
        int uninit_var;                 /* BSS Section (B) */
        static int static_var = 10;     /* Data Section Local (d) */
        const int read_only = 100;      /* Read-only Data (R/r) */

        void test_func(void) {          /* Text Section (T) */
            printf("Hello nm!\\\\n");
        }

        int main(void) {
            test_func();
            return 0;
        }
        """

        self.source_code_for32 = """
        int add(int a, int b) { return a + b; }

        int global = 42;
        static int sglobal = 7;

        int main(void) {
            return add(global, sglobal);
        }
        """
    
    def create_source_file(self, filename="temp_source.c"):
        """C 소스코드를 파일로 저장합니다."""
        with open(filename, "w") as f :
            f.write(self.source_code)
        self.generator_files.append(filename)
        return filename

    def create_source_file32bit(self, filename="temp_source.c"):
        """C 소스코드를 파일로 저장합니다."""
        with open(filename, "w") as f :
            f.write(self.source_code_for32)
        self.generator_files.append(filename)
        return filename
    
    def compile(self, source_file, output_name, flags=[]):
        """gcc를 실행해서 컴파일을 수행합니다"""
        cmd = ["gcc", source_file, "-o", output_name] + flags

        try:
            subprocess.check_call(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            self.generator_files.append(output_name)
            print(f"   [Build] {output_name} 생성 성공!")
            return output_name
        except subprocess.CalledProcessError:
            print(f"   [Error] {output_name} 빌드 실패 (gcc 옵션 확인 필요)")
            return None
    
    def make_all(self):
        """다양한 종류의 파일을 한 번에 생성합니다."""
        src = self.create_source_file()
        src32 = self.create_source_file32bit()
        targets = []

        print("--- 파일 생성 시작 ---")

        if res := self.compile(src, "test_exec.out", []):
            targets.append(res)
        
        if res := self.compile(src, "test_obj.o", ["-c"]):
            targets.append(res)

        if res := self.compile(src, "test_obj.so", ["-shared", "-fPIC"]):
            targets.append(res)
    
        if res := self.compile(src32, "test_32bit.o", ["-c", "-m32"]):
            targets.append(res)
        
        return targets

    def cleanup(self):
        """테스트가 끝나면 생성된 지저분하 파일을 지웁니다."""
        print("\\n--- 청소 시작 ---")
        for file in self.generator_files:
            if os.path.exists(file):
                os.remove(file)
                print(f"    [Delete] {file} 삭제됨")

class FileCorruptor:
    def create_bad_file(self, origin_file):
        """원본 파일을 읽어 랜덤한 위치를 망가뜨린 복사본을 만듭니다."""
        bad_filename = f"corruped_{origin_file}"

        with open(origin_file, "rb") as f:
            data = bytearray(f.read())
        
        # 데이터 파괴 (램덤 바이트 변경)
        if len(data) > 0:
            # 헤더 부분(앞쪽)을 망가뜨릴 확률을 높임 (치명적임)
            pos = random.randint(0, min(64, len(data) - 1))
            data[pos] = 0xFF
        
        with open(bad_filename, "wb") as f:
            f.write(data)
        
        return bad_filename

class TestRunner:
    def __init__(self, ft_nm="./ft_nm", sys_nm="nm") -> None:
        self.ft_nm = ft_nm
        self.sys_nm = sys_nm
        self.corruptor = FileCorruptor()

    def get_option_combibations(self):
        """옵션 조합 생성기 역할"""
        option = ['-a', '-P', '-u', '-r', '-g', '-n', '-z']
        combos = []
        for r in range(8):
            for c in itertools.combinations(option, r):
                combos.append(list(c))
        return combos
    
    def run_command(self, cmd):
        """명령어 실행 도우미"""

        custom_env = os.environ.copy()
        custom_env["LC_ALL"] = "C"
        try:
            return subprocess.check_output(cmd, stderr=subprocess.DEVNULL, timeout=1, env=custom_env)
        except subprocess.TimeoutExpired:
            return b"TIMEOUT"
        except subprocess.CalledProcessError as e:
            if e.returncode < 0: return f"CRASH({e.returncode})".encode()
            return b"ERROR"
    
    def run_tests(self, targets):
        print(f"테스트 시작 : 대상 파일 {len(targets)}개\\n")
        option_combs = self.get_option_combibations()

        for target in targets:
            print(f"Target: {target}")

            for flags in option_combs:
                sys_res = self.run_command([self.sys_nm] + flags + [target])
                my_sys = self.run_command([self.ft_nm] + flags + [target])

                if sys_res.decode().split() == my_sys.decode().split():
                    print(f"    [PASS] Flags: {flags}")
                else:
                    print(f"    [FAIL] Flags: {flags} -> 결과 다름")

            sys_line = sys_res.decode().splitlines()
            my_line = my_sys.decode().splitlines()
            diff = difflib.unified_diff(sys_line, my_line, fromfile='Original', tofile='Mine', lineterm='')
            for line in diff:
                print(line)

            bad_file = self.corruptor.create_bad_file(target)
            print(f"    Fuzzing: {bad_file}...", end=" ")

            result = self.run_command([self.ft_nm, bad_file])

            if "CRASH" in str(result) or "TIMEOUT" in str(result):
                print(f" {result} (Segfault 발생!!)")
            else:
                print(" 방어 성공 (Segfault 안남)")
            
            if os.path.exists(bad_file): os.remove(bad_file)
            print("-" * 40)


if __name__ == "__main__":
    gnerator = TargetGenerator()
    targets = gnerator.make_all()

    runner = TestRunner()
    runner.run_tests(targets)

    gnerator.cleanup()