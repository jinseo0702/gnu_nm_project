ARCHITECTURE

[GOAL] 
ft_nm : Linux x_86_32 /x_86_64 대상 ELF(32/64) 및 archive(ar) 포멧을 안전하게 해석하여 심볼을 출력한다.
안전성 최우선 : 모든 메모리 점근은 범위 검사 후 수행한다.
구현 옵션 : -a -g -u -r -P -n

[P0:MUST] : If not implemented, it is considered a failure.
1. 포맷 검출 게이트
    - 입력 path 는 ELF 또는 ar 일 수 있다.
    - ELF magic(ELFMAG)과 ar magic(ARMAG) 모두 반드시 검사한다.
    - ar은 armagic 이 있는 SARMAG 의 크기 다음이 member payload 의 시작이다.
2. base/limit 정의
    - base는 "현재 해석 단위의 시작 주소"
    - 일반 ELF: mmap 시작 주소
    - ar member : member payload 시작 주소
    - limit 은 해당 단위의 유효 크기 (일반 파일 크기 또는 member payload 크기)
3. 범위검사 강제
    - 모든 접근은 CHECK_RANGE(offset, size, limit)로만 허용
    - 검사 형태 : offset <= limit 먼저 , 다음 size + offset <= limit
    - MOVE_ADDRESS(base, offset)는 범위검사 없이 주소만 이동 (호출 전 검사 필수)
4. ar 처리 규칙
    - ar <ar.h> 헤더의 foramt을 따른다. magic 검사는 필수
    - member payload 를 독립적인 limit으로 보고 , payload에서 ELF magic 재검사
    - member offset 계산시 짝수 패딩 (align2) 반영
    - ar_name[16]만 파싱, '\0' 종결 가정 금지
    - ar_name trim : 끝에 '/' 또는 공백 패딩 제거
    - "/<digits>"일 때만 string table에서 복원, 그 외는 ar_name[16]에서 직접 이름 추출
    - ar_fmag[2] 는 ft_memcmp(ar_fmag, ARFMAG, 2)로 ARFMAG를 반드시 확인, 불일치 시 심각한 오류로 판단하고 ar 관련 자원 반납 후 다음 path로
5. ELF 필수 검증
    - EI_DATA : ELFDATA2LSB 만
    - EI_CALSS : ELFCLASS32 , ELFCLASS64 만
    - e_type : ET_REL, ET_EXEC, ET_DYN 
    - e_machine : EM_386, EM_X86_64
    - section table : e_shoff != 0 , e_dhnum != 0 , e_shentsize 검증, 그리고 e_shoff + e_shnum*e_shentsize 범위검사
6. 심볼 테이블 선택
    - SHT_SYMTAB 우선, 없으면 SHT_SYMTAB
    - 둘다 없으면 메세지 출력 후 다음 해석 단위로 이동
7. 문자열 테이블 안전 접근
    - 문자열은 심볼테이블의 sh_link(범위검사 포함)에 st_name를 통해 함수로만 접근
    - ft_memchr(strtab+st_name, '\0', remaining)로 널 종결 존재 확인
8. 옵션 정책
    - 우선순위 (a|g|u) > n > P, r 은 정렬 뒤집기
    - 정렬: n 없으면 name 기준, 있으면 value 기준(동일 value면 name)
    - P : POSIX.2 출력 포맷
9. 프로그램 종료 정책
    - 시스템 오류(malloc 등)이 아니면 프로그램을 종료하지 않는다.
    - 알 수 없는 옵션은 fatal(즉시 실패 처리)
10. 리소스 회수 정책
    - 분기 / 에러 로 다음 단계로 넘어갈 때, 그 분기에서 얻은 자원은 반드시 반납:
        - malloc/free , open/close, mmap/munmap
    - free 후 NULL guard로 안정성 강화(가능한 범위에서)
11. bit Type 캐스팅 정책
    - 32 bit 의 아키텍쳐에서는 데이터 추출 후 64 bit 의 format 와 같이 upscale casted 를 사용해 보관

[IMPLEMENTATION:CONSTRAINTS]
    - 사용 라이브러리 : libft, ft_printf 해당 디렉토리 수정 금지
    - 허용함수 :  open / close / mmap / munmap / write / fstat / malloc / free / exit / perror / strerror / getpagesize
    - 작업 디렉토리 : ./src , ./include
    - 주석 작성 금지
    - 실행 검증(런타임 테스트)은 하지 않음(정적 검증/명세 준수로 확인)
