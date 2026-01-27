# 코드검증
---

### 현재발생하는 문제

1. ./nm <path> <option>
    - 위와 같이 option 이 path 보다 뒤로 가게되면 인식이 안되는 문제
2. section header 가 없을때 GNU nm 과 나의 nm 의 출력 format 차이
    - GNU nm 은 무조건 symtab 이 없으면 symbol 이 없음 이라고 출력하는데 ./nm 은 SHT_DYNSYM 을 해석
    - symbol 해석 못했을 시 no symbols 미비
3. dprintf 의 사용
    - 내 명세에는 ft_printf 계열만 사용이라고 입력 했지만  코드작성자가 padding 형식을 맏추기 위해서  임의적 판단 후 사용
4. ft_fprintf(2, "", va_arg); 를 사용하는 error 출력 부분이 통일 되지않고 분산되어 있어 디버깅시 어려움 발생
    - 현재 대부분의 format이 ft_fprintf("ft_nm: %s: %s\n") 형태를 보이기 때문에 통일 작업 실행
5. Symbol Type 이 '?' 가 나오는 구간이 있음 확인 필요
    - _DYNAMIC -> d : 0 OBJECT  LOCAL  DEFAULT   23 _DYNAMIC
    - __abi_tag -> r : 32 OBJECT  LOCAL  DEFAULT    4
    - __do_global_dtors_aux_fini_array_entry -> d : 0 FUNC    LOCAL  DEFAULT   16
    - __frame_dummy_init_array_entry -> d : 0 OBJECT  LOCAL  DEFAULT   21
    - classify_by_shndx 의 조건을 명세와 비교한다.
    - 전체적인 조건설정이 이상하다. 한번확인을 해야한다.
5.1. 현재 발생한 문제는 매우 크다.
    - sym->st_shndx == SHN_ABS 인 값들이 출력이 되지않는다.
5.2. 정렬이 안되는 문제
    - 특히 'a', 'U', 'w' 의 symbol 이 정렬이 안됨
6. symbol type is 'a' , not print
7. archive STT_SECTION 이 보이지 않음
    - STT_SECTION 의 이름은 Symbol 의 name 과 다르게 움직임.

---

### 해결

1. parse_arguments 의 함수가 옵션을 처리하는 조건이 인자의 첫문자열 부분만 option 으로 조건설정
    - 모든 argv 배열을 돌면서 option 을 확인 하는 방향으로 수정
2. find_symbol_table 에서 SHT_DYNSYM 조건 재설정
    - 안전하게 처리 했지만 GNU nm 이랑 내가 만든 nm 이랑 정책의 차이 발생 하지만 GNU nm 을 채택
    - find_symbol_table 에서 found_dynsym 관련 내용제거 후 err_msg 추가
3. padding format 을 맞출 수 있도록 dprintf 제거후 논리 수정
    - print_symbol
    - sort_and_print_symbols
    - process_ar_archive
    - 위 함수에서 ft_printf를 사용
    - nm format 을 맞추기 위해 write_hex 함수도입후 16진수 추출
4. NM_LOG 적용
    - parse_option_string
    - process_path
    - print_error
5.
    - classify_by_shndx 에서 조건을 수정했다
    - GNU 의 nm 은 좀 다르게 동작해서 내가 생각하는데로 욺직이지 않는다.
5.1 
    - is_visible
5.2
    - compare_by_value 에서 'a', 'U' 'w' 비교 조건 추가
6.
    - is_visible 에 type != 'a' and 공백일때 조건 추가