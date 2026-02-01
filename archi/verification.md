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
8. archive format 에서 정렬이 안되는 상황이 발생.
    - a, u , w 를 제외한 주소가 0 인 type 들이 존재하기 때문.
9. archive format 에서 이름이 긴 // 곳을 표시 x
10. archive foramt 에서 N, n 을 구분하는 방법이 잘못됨. gnu 와 차이가 존재.
11. archive format 에서 sectioon header 의 이름을 출력하는 방식이 다름
12. 32비트 출력시 출력 형식 불일치
13. 32비트 obj 파일 출력시 section name 출력안됨
    - 내가 글자를 구분 못해서 32bit 자리에 64bit 내용을 넣음
14. none option 일때 정렬이 안된다. 근데 Tester에는 왜 맞다고 나올까
15. gnu format에 맞춰본다.

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
7. load_symbols 에서 archive format 에서 section 이 나오도록 조건 추가
8. compare_by_value
    - 5.2 조건에서 저렇게 비교하는게 아니라 (a,b) U, w | (a,b) !U, !w 로 변경
9. extract_ar_name, process_ar_archive
    - 위 두 함수에서 // 조건 재 정의
    - 문제는 긴 문자열을 무시.
    - 조건을 /<digits> 를 확인 할 수 있는 방향으로 재정의
10. classify_by_shndx
    - 현재 N 은 sh_flags == 0 알때만 'N' 으로 정의되지만
    - 경우에 따라서 debug 는 sh_flag 는 0이 아닐 수 있다.
    - 기준을 Occupies memory during execution 즉 메모리에 올라가지 않는다면 debug로 보았다.  
11. load_symbols
    - debug 모드를 Test 하는 조건을 걸어서 문제를 해결
12. print_symbol, write_hex
    - 위 두가지에 machine type 구분
13. human Error
    - check_section_name 에서 32/64 bit 둘다 64bit 구조체 사용
    - 상황에 맡게 수정완료
14. 내가 착각했다. 그리고 이건 15 번의 LC_ALL=C 옵션이랑 겹치게 된다.
    - 내가 LC_ALL=C 를 적용해서 강제로 정렬을 시키는데 이건 문제의 취지랑 맞지않는다.
    - 그래서 15번을 고려했다.
15. gnu 는 
    - Test 해본결과 '_', '.', '$' 는 무시하고 alpha 가 나올때까지 돈다.
    - 그리고 일단 대문자 소문자로 비교 하지 않고 비교를 하고 그래도 같은 문자라면 strcmp 로직을 돌린다.
    - 코드는 /gnu_nm_project/src/sort_filter_print.c 에 str_compare 가 변경된걸 보면된다.
