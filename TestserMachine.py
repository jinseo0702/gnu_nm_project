from ast import Try
import subprocess
import os

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
    
    def create_source_file(self, filename="temp_source.c"):
        """C 소스코드를 파일로 저장합니다."""
        with open(filename, "w") as f :
            f.write(self.source_code)
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
        targets = []

        print("--- 파일 생성 시작 ---")

        if res := self.compile(src, "test_exec.out", []):
            targets.append(res)
        
        if res := self.compile(src, "test_obj.o", ["-c"]):
            targets.append(res)

        if res := self.compile(src, "test_obj.so", ["-shared", "-fPIC"]):
            targets.append(res)
    
        if res := self.compile(src, "test_32bit.o", ["-c", "-m32"]):
            targets.append(res)
        
        return targets

    def cleanup(self):
        """테스트가 끝나면 생성된 지저분하 파일을 지웁니다."""
        print("\\n--- 청소 시작 ---")
        for file in self.generator_files:
            if os.path.exists(file):
                os.remove(file)
                print(f"    [Delete] {file} 삭제됨")

# class FileCorruptor:
#     def create_bad_file(self, origin_file):
#         """"""