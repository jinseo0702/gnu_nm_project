ELF 바이너리 아키텍처 및 데이터 추출 메커니즘에 관한 포괄적 분석 보고서

1. 서론: 시스템의 기저를 지탱하는 ELF의 철학

현대 컴퓨팅 생태계, 특히 유닉스(Unix) 및 리눅스(Linux) 계열의 운영체제에서 실행 가능한 바이너리의 표준으로 자리 잡은 ELF(Executable and Linkable Format)는 단순한 파일 저장 형식을 넘어, 운영체제의 커널(Kernel)이 사용자 공간(User Space)의 프로세스를 로드하고 관리하는 인터페이스 그 자체로 기능한다. 1999년 86open 프로젝트에 의해 x86 프로세서의 표준 바이너리 포맷으로 채택된 이래 1, ELF는 임베디드 시스템부터 슈퍼컴퓨터에 이르기까지 광범위한 아키텍처를 지원하며 그 유연성과 확장성을 증명해왔다.

본 보고서는 ELF 파일의 정적 분석(Static Analysis)과 데이터 추출(Data Extraction)을 수행하고자 하는 보안 연구원, 시스템 엔지니어, 그리고 리버스 엔지니어링 전문가를 위해 작성되었다. 특히 사용자의 질의에 따라, ELF 파일 내부의 핵심 구조체인 섹션 헤더(Section Header), 심볼 테이블(Symbol Table), 그리고 섹션 본문(Section Body) 간의 유기적인 상호작용 관계를 심층적으로 규명하는 데 초점을 맞춘다.

ELF 파일은 설계상 두 가지의 상이한 뷰(View)를 제공한다.2

컴파일러와 링커가 바라보는 **'링킹 뷰(Linking View)'**는 파일을 논리적인 섹션(Section)들의 집합으로 이해하며, 이는 코드 재배치(Relocation)와 심볼 해석(Symbol Resolution)에 최적화되어 있다. 반면, 운영체제의 로더(Loader)가 바라보는 **'실행 뷰(Execution View)'**는 메모리 접근 권한과 페이징(Paging) 효율성을 고려하여 섹션들을 세그먼트(Segment)라는 더 큰 단위로 묶어 처리한다.1

데이터 추출과 바이너리 분석은 주로 파일이 디스크에 저장된 상태를 다루므로, 본 보고서는 링킹 뷰의 핵심인 섹션 기반의 구조 분석에 집중한다.

이 보고서는 단순한 구조체의 필드 정의를 나열하는 것을 넘어, 각 필드가 왜 그러한 형태를 띠게 되었는지, 그리고 필드 간의 참조 관계(Cross-Reference)가 어떻게 거대한 소프트웨어 로직을 형성하는지에 대한 2차, 3차적 통찰을 제공한다. 또한, 이론적 논의에 그치지 않고 가상의 로그 데이터를 기반으로 한 구체적인 분석 시나리오를 통해, 16진수(Hexadecimal) 데이터가 어떻게 의미 있는 정보로 변환되는지 그 전 과정을 상세히 기술한다.

2. ELF 헤더: 바이너리 해석의 나침반

모든 데이터 추출 알고리즘의 시작점은 파일의 오프셋 0에 위치한 ELF 헤더(ELF Header)이다. 이 헤더는 파일 전체의 '로드맵' 역할을 수행하며, 분석가가 파일의 나머지 부분을 어떻게 해석해야 할지에 대한 메타데이터를 제공한다.2

2.1 e_ident 배열과 플랫폼 독립성

ELF 파일의 첫 16바이트는 e_ident 배열로 할당되어 있으며, 이는 ELF 포맷이 특정 하드웨어에 종속되지 않고 다양한 아키텍처를 포용할 수 있게 하는 핵심 기제이다.

매직 넘버 (Magic Number): 첫 4바이트(0x7F, 'E', 'L', 'F')는 파일의 서명이다. 분석 도구는 이 바이트를 확인하여 해당 파일이 분석 가능한 대상인지를 1차적으로 판별한다.1

클래스(Class)와 엔디안(Data Encoding): 5번째 바이트(EI_CLASS)는 파일이 32비트(ELFCLASS32)인지 64비트(ELFCLASS64)인지를 결정한다. 이는 이후 파싱할 구조체(Elf32_Sym vs Elf64_Sym)의 크기와 필드 배치를 결정짓는 가장 중요한 분기점이다.3

6번째 바이트(EI_DATA)는 데이터가 리틀 엔디안(Little-Endian)인지 빅 엔디안(Big-Endian)인지를 나타낸다. 분석 도구는 자신의 호스트 시스템과 대상 파일의 엔디안이 다를 경우, 바이트 스와핑(Byte Swapping)을 수행하여 정수 값을 올바르게 해석해야 한다.

버전 및 ABI: 이후 바이트들은 ELF 버전과 OS ABI(Application Binary Interface) 정보를 담고 있어, 특정 운영체제만의 확장 기능이 사용되었는지 파악할 수 있게 한다.4

2.2 테이블 위치의 결정과 구조적 통찰

ELF 헤더에서 데이터 추출을 위해 반드시 확보해야 할 정보는 섹션 헤더 테이블의 위치와 크기 정보이다.

```text
필드명 (64비트 기준)타입의미 및 분석적 함의
e_shoffElf64_Off섹션 헤더 테이블의 파일 오프셋. 분석가는 이 주소로 파일 포인터를 이동시켜 섹션 정보 파싱을 시작한다.4
e_shentsizeElf64_Half섹션 헤더 항목 하나(Elf64_Shdr)의 크기. 일반적으로 64비트에서 64바이트이다.
e_shnumElf64_Half섹션 헤더의 개수. e_shentsize와 곱하여 전체 테이블의 크기를 계산할 수 있다.
e_shstrndxElf64_Half섹션 이름 문자열 테이블의 인덱스. 이 필드가 가리키는 섹션을 먼저 파싱하지 않으면, 다른 모든 섹션의 이름(예: .text, .data)을 알 수 없게 된다.3
```

심화 통찰: e_shstrndx가 SHN_XINDEX(0xffff) 값을 가지는 경우가 있다. 이는 섹션의 개수가 SHN_LORESERVE(0xff00) 이상으로 많아 e_shnum 필드(16비트)로 표현할 수 없을 때 발생한다. 이 경우 실제 섹션 이름 문자열 테이블의 인덱스는 섹션 헤더 테이블의 첫 번째 엔트리(인덱스 0)의 sh_link 필드에 저장된다.3 이러한 예외 처리는 대규모 애플리케이션 분석 시 파서의 견고함을 결정하는 요소이다.

3. 섹션 헤더 (Section Header): 데이터의 문맥적 정의

섹션 헤더 테이블은 바이너리를 구성하는 논리적 블록들에 대한 정의서이다. Elf64_Shdr 구조체는 각 섹션의 이름, 유형, 메모리 로드 주소, 파일 내 위치, 크기 등을 정의하며, 특히 다른 섹션과의 관계를 명시함으로써 바이너리의 유기적인 연결을 가능하게 한다.2

3.1 Elf64_Shdr 구조체의 해부 및 데이터 추출 전략

각 섹션 헤더는 64바이트(64비트 기준)로 구성되며, 다음과 같은 핵심 필드를 포함한다.4

```text
Ctypedef struct {
    Elf64_Word  sh_name;      /* Section name (string tbl index) */
    Elf64_Word  sh_type;      /* Section type */
    Elf64_Xword sh_flags;     /* Section flags */
    Elf64_Addr  sh_addr;      /* Section virtual addr at execution */
    Elf64_Off   sh_offset;    /* Section file offset */
    Elf64_Xword sh_size;      /* Section size in bytes */
    Elf64_Word  sh_link;      /* Link to another section */
    Elf64_Word  sh_info;      /* Additional section information */
    Elf64_Xword sh_addralign; /* Section alignment */
    Elf64_Xword sh_entsize;   /* Entry size if section holds table */
} Elf64_Shdr;
```

sh_name (이름 식별): 이 값은 문자열 자체가 아니라, e_shstrndx가 가리키는 문자열 테이블(.shstrtab) 내의 바이트 오프셋이다. 데이터 추출 시 가장 먼저 .shstrtab 섹션의 데이터를 메모리에 로드한 후, BaseAddress + sh_name 위치의 문자열을 읽어와야 비로소 섹션의 정체(예: .symtab)를 파악할 수 있다.

sh_type (데이터 해석 방법): 섹션의 성격을 규정한다. SHT_PROGBITS는 코드나 데이터를, SHT_SYMTAB은 심볼 정보를, SHT_NOBITS는 초기화되지 않은 데이터(BSS)를 의미한다.5 데이터 추출 도구는 이 타입에 따라 파싱 로직을 분기(Switch)해야 한다.

sh_offset & sh_size (물리적 위치): 파일 내에서 해당 섹션의 데이터가 어디에 위치하고 얼마나 큰지를 나타낸다. fseek과 fread를 수행하기 위한 직접적인 인자이다.

3.2 관계성의 핵심: sh_link와 sh_info의 다형성 해석

사용자 쿼리의 핵심인 '섹션 간의 관계'는 sh_link와 sh_info 필드에 의해 정의된다. 이 두 필드는 섹션의 타입(sh_type)에 따라 그 의미가 달라지는 다형성(Polymorphism)을 띤다. 이를 정확히 해석하지 못하면 섹션 간의 연결 고리가 끊겨 심볼의 이름이나 재배치 대상을 찾을 수 없게 된다.5

아래 표는 주요 섹션 타입에 따른 두 필드의 의미를 정리한 것이다.

```text
sh_type (섹션 타입)sh_link (연결된 섹션 인덱스)sh_info (추가 정보)관계의 의미 및 데이터 추출 통찰
SHT_SYMTAB(정적 심볼 테이블)연결된 문자열 테이블(.strtab)의 섹션 인덱스.심볼 테이블 내의 마지막 로컬 심볼(Local Symbol)의 인덱스 + 1.심볼 구조체의 st_name 값을 해석하려면 sh_link가 가리키는 섹션을 참조해야 한다. sh_info는 심볼 정렬 최적화에 사용된다.
SHT_DYNSYM(동적 심볼 테이블)연결된 동적 문자열 테이블(.dynstr)의 섹션 인덱스.심볼 테이블 내의 마지막 로컬 심볼의 인덱스 + 1.실행 파일이나 공유 라이브러리에서 런타임 링킹을 위해 사용된다.
SHT_RELA / SHT_REL(재배치 정보)참조하는 심볼 테이블(.symtab/.dynsym)의 섹션 인덱스.재배치를 적용할 대상 섹션(예:.text)의 인덱스.가장 복잡한 관계이다. "어떤 심볼(sh_link)"을 참조하여 "어떤 섹션(sh_info)"을 수정해야 하는지를 정의한다.7
SHT_DYNAMIC(동적 링킹 정보)이 섹션의 항목들이 사용하는 문자열 테이블의 인덱스.0 (사용 안 함)동적 링커(ld.so)가 사용하는 정보들의 집합이다.
SHT_HASH(심볼 해시 테이블)해싱이 적용되는 심볼 테이블의 섹션 인덱스.0 (사용 안 함)심볼 검색 속도를 높이기 위한 해시 테이블과 원본 심볼 테이블을 연결한다.
```

분석적 통찰: sh_link 필드는 데이터베이스의 외래 키(Foreign Key)와 유사한 역할을 한다. 예를 들어, SHT_RELA 타입의 섹션을 분석할 때 sh_link를 따라가면 심볼 테이블을 얻을 수 있고, sh_info를 따라가면 패치해야 할 코드가 있는 .text 섹션을 찾을 수 있다. 이러한 '삼각 관계'를 이해하는 것이 데이터 추출의 핵심이다.

4. 심볼 테이블 (Symbol Table): 바이너리의 주소록

심볼 테이블은 함수, 전역 변수 등 프로그램의 구성 요소들에 '이름'과 '속성'을 부여하는 데이터베이스이다. 바이너리 레벨에서 컴퓨터는 오직 주소(Address)만을 이해하지만, 심볼 테이블은 이 주소에 인간이 이해할 수 있는 의미를 매핑한다.8

4.1 Elf64_Sym 구조체의 심층 분석

심볼 테이블 섹션(SHT_SYMTAB)의 본문은 Elf64_Sym 구조체의 배열로 이루어져 있다.

```text
Ctypedef struct {
    Elf64_Word    st_name;  /* Symbol name (string tbl index) */
    unsigned char st_info;  /* Symbol type and binding */
    unsigned char st_other; /* Symbol visibility */
    Elf64_Half    st_shndx; /* Section index */
    Elf64_Addr    st_value; /* Symbol value */
    Elf64_Xword   st_size;  /* Symbol size */
} Elf64_Sym;
```

4.1.1 st_name: 간접 참조 방식

st_name은 문자열이 아닌 정수형 인덱스이다. 이 값은 해당 심볼 테이블 섹션의 헤더(sh_link)가 가리키는 문자열 테이블 섹션의 시작점으로부터의 오프셋이다.

추출 로직: StringTableBase + st_name 위치에서 NULL(\0)을 만날 때까지 읽는다.

최적화: ELF는 문자열 테이블의 크기를 줄이기 위해 접미사가 같은 문자열들이 공간을 공유하도록 허용한다. 예를 들어 "main"과 "domain"이 있다면, "domain"만 저장하고 "main"은 "domain"의 중간을 가리키게 할 수 있다.

4.1.2 st_info: 타입과 바인딩의 압축

이 1바이트 필드는 상위 4비트(Binding)와 하위 4비트(Type)로 나뉘어 정보를 저장한다.8

추출 매크로: ELF64_ST_BIND(i)는 i >> 4, ELF64_ST_TYPE(i)는 i & 0xf 연산을 수행한다.

Binding: 심볼의 가시 범위를 결정한다.
STB_LOCAL (0): 파일 내부에서만 유효.
STB_GLOBAL (1): 모든 파일에서 참조 가능.
STB_WEAK (2): 글로벌과 유사하나, 링킹 시 동일 이름의 글로벌 심볼이 있으면 덮어씌워짐.

Type: 심볼의 성격을 결정한다.
STT_OBJECT (1): 변수, 배열 등의 데이터.
STT_FUNC (2): 함수 또는 실행 코드.
STT_SECTION (3): 섹션 자체를 가리키는 심볼. 재배치 시 주로 사용됨.

4.1.3 st_shndx와 st_value: 위치 결정의 메커니즘

이 두 필드는 심볼이 실제로 '어디에' 존재하는지를 알려준다.

st_shndx (섹션 인덱스): 심볼이 정의된 섹션의 인덱스이다.
일반적인 정수: 해당 섹션에 데이터가 존재함.
SHN_UNDEF (0): 현재 파일에 정의되지 않음(외부 참조).
SHN_ABS (0xfff1): 절대 주소. 재배치에 의해 변경되지 않음.
SHN_COMMON (0xfff2): 할당되지 않은 공용 블록 (초기화되지 않은 전역 변수 등).

st_value (값/주소): 파일 종류에 따라 해석이 달라진다.8
재배치 가능 파일(.o): st_shndx 섹션의 시작점으로부터의 오프셋(Offset).
실행 파일/공유 객체: 가상 메모리 상의 절대 주소(Virtual Address).

통찰: 데이터 추출 시 가장 흔한 실수는 st_value를 무조건 파일 오프셋으로 착각하는 것이다. 실행 파일 분석 시에는 st_value에서 해당 섹션의 가상 주소(sh_addr)를 빼고, 섹션의 파일 오프셋(sh_offset)을 더하는 변환 과정(Address Translation)이 필수적이다.

5. 섹션 본문 (Section Body): 실제 데이터의 저장소

섹션 헤더와 심볼 테이블이 메타데이터라면, 섹션 본문은 실제 콘텐츠이다.

.text: 실행 가능한 기계어 코드가 저장된다. sh_flags에 SHF_EXECINSTR가 설정된다.
.data: 초기화된 전역 변수 및 정적 변수가 저장된다. SHF_WRITE가 설정된다.
.rodata: 문자열 리터럴, const 변수 등 읽기 전용 데이터가 저장된다.
.bss: 초기화되지 않은 변수가 차지할 공간. 파일 내에서는 공간을 차지하지 않고(SHT_NOBITS), 메모리 로드 시 0으로 초기화된 공간이 할당된다. 따라서 데이터 추출 시 .bss 섹션의 파일 오프셋을 읽으려 하면 엉뚱한 데이터를 읽게 된다.

6. 데이터 추출 및 분석 방법론: 단계별 알고리즘

앞서 논의한 구조적 지식을 종합하여, ELF 파일에서 특정 전역 변수의 값을 추출하는 일반화된 알고리즘을 제시한다.

초기화: 파일을 바이너리 모드로 열고, ELF 헤더(Elf64_Ehdr)를 읽어 유효성을 검증한다.
섹션 헤더 로드: e_shoff로 이동하여 e_shnum 개수만큼 Elf64_Shdr을 읽어 배열에 저장한다.
문자열 테이블 식별: e_shstrndx 인덱스의 섹션 헤더를 찾아, 해당 섹션의 본문(.shstrtab)을 메모리에 로드한다.
심볼 테이블 탐색: 섹션 헤더들을 순회하며 sh_type == SHT_SYMTAB인 섹션(.symtab)을 찾는다.
심볼 이름 해석 준비: .symtab 섹션 헤더의 sh_link를 확인하여, 연결된 문자열 테이블(.strtab)의 본문을 로드한다.
타겟 심볼 검색:
.symtab의 내용을 Elf64_Sym 단위로 순회한다.
각 심볼의 st_name을 이용해 .strtab에서 이름을 가져와 타겟 이름(예: "my_variable")과 비교한다.
위치 계산 및 추출:
일치하는 심볼을 찾으면 st_shndx를 확인한다.
해당 인덱스의 섹션 헤더(TargetShdr)를 참조한다.
파일 오프셋 계산:
Object File: FileOffset = TargetShdr.sh_offset + Sym.st_value
Executable: FileOffset = TargetShdr.sh_offset + (Sym.st_value - TargetShdr.sh_addr)
계산된 오프셋으로 이동하여 st_size만큼 데이터를 읽는다.

7. 구체적 분석 예시: 가상의 로그 데이터를 기반으로

사용자의 요청에 따라, 분석 도구가 출력한 가상의 로그 데이터를 상정하고 이를 바탕으로 한글 분석 예시를 제시한다. 이 시나리오는 "보안 분석가가 firmware.elf 파일 내에서 g_auth_key라는 전역 변수의 초기값과, 이 변수가 참조하는 문자열을 추출하는 과정"을 가정한다.

7.1 제공된 로그 데이터 (Exhibit A: Section & Symbol Dump)

다음은 분석 도구가 firmware.elf 파일을 파싱하여 출력한 로그의 일부이다.

```text
[LOG-01] ELF Header Parsed. Machine: Advanced Micro Devices X86-64
[LOG-02] Section Header Table (@offset 0x1200):
[Idx] Name      Type      Addr        Offset      Size      Link Info
[ 0]           NULL      00000000    00000000    00000000  0    0
[ 1].text     PROGBITS  00401000    00001000    00000200  0    0
[ 2].rodata   PROGBITS  00402000    00002000    00000100  0    0
[ 3].data     PROGBITS  00403000    00003000    00000020  0    0
[ 4].symtab   SYMTAB    00000000    00003020    000000A8  5    3
[ 5].strtab   STRTAB    00000000    000030C8    00000080  0    0
[ 6].shstrtab STRTAB    00000000    00003148    00000050  0    0
[LOG-03] Symbol Table Scan (.symtab):
[Idx] Value              Size Type    Bind    Vis      Ndx Name
[ 8] 0000000000403010   8    OBJECT  GLOBAL  DEFAULT  3   g_auth_key
[ 9] 0000000000402040   0    NOTYPE  LOCAL   DEFAULT  2  .LC07
```

2 분석 과정 및 데이터 추출 상세

위 로그를 바탕으로 g_auth_key의 데이터를 추출하는 과정을 단계별로 분석한다.

1단계: 타겟 심볼의 정보 파악
로그 [LOG-03]의 인덱스 8번 라인을 분석한다.
Name: g_auth_key
Type: OBJECT (데이터 변수임)
Ndx (Section Index): 3
Value: 0x00403010
Size: 8 바이트 (64비트 포인터 또는 long long 정수로 추정됨)
해석: 이 변수는 섹션 인덱스 3번(.data)에 위치하며, 메모리 로드 시 가상 주소 0x00403010에 배치된다.

2단계: 파일 내 물리적 위치 계산
로그 [LOG-02]에서 섹션 인덱스 3번(.data)의 정보를 확인한다.
Section Name: .data
Addr (Virtual Address): 0x00403000
Offset (File Offset): 0x00003000
이 파일은 실행 파일(가상 주소가 할당되어 있음)이므로, 가상 주소를 파일 오프셋으로 변환해야 한다.
공식: Target_File_Offset = Symbol_Value - Section_Addr + Section_Offset
계산: 0x00403010 - 0x00403000 + 0x00003000
Offset Delta = 0x10 (섹션 시작점으로부터 16바이트 뒤)
Final Offset = 0x3010

3단계: 1차 데이터 추출 (변수 값 확인)
파일 오프셋 0x3010 위치에서 8바이트를 읽었더니 다음과 같은 헥사(Hex) 값이 나왔다고 가정한다 (가상의 Hex Dump).
: 40 20 40 00 00 00 00 00
값 해석: 리틀 엔디안 시스템(x86-64)이므로, 이 바이트들은 역순으로 조합되어 0x0000000000402040이 된다.
의미: g_auth_key 변수의 초기값은 0x402040이다. 이 값은 포인터(메모리 주소)로 보인다.

4단계: 2차 데이터 추출 (포인터 추적)
추출된 값 0x402040이 가리키는 실제 데이터가 무엇인지 확인하기 위해 다시 섹션 헤더를 참조한다.
0x402040 주소는 어느 섹션에 속하는가?
로그 [LOG-02]를 보면 .rodata 섹션의 범위가 0x402000 ~ 0x402100 (Size 0x100)이다.
따라서 0x402040은 .rodata 섹션(인덱스 2)에 속한다.
다시 파일 오프셋을 계산한다.
Target Section: .rodata (인덱스 2)
계산: 0x402040 (Target Addr) - 0x402000 (Section Base) + 0x2000 (File Offset)
Offset Delta = 0x40
Final Offset = 0x2040

5단계: 최종 데이터 확인
파일 오프셋 0x2040 위치의 데이터를 읽는다.
: 53 45 43 52 45 54 5F 50 41 53 53 00 ("SECRET_PASS" + NULL)

최종 결론:
분석 결과, 전역 변수 g_auth_key는 .data 섹션에 존재하며, 이 변수는 .rodata 섹션에 저장된 문자열 상수 **"SECRET_PASS"**를 가리키는 포인터임이 밝혀졌다. 이 과정은 섹션 헤더의 주소 매핑 정보와 심볼 테이블의 값 정보를 결합하여 포인터 체인을 따라가는 전형적인 정적 분석 기법이다.

8. 심화 분석: 링킹, 재배치, 그리고 보안 이슈

8.1 재배치(Relocation)와 심볼의 결합

앞선 예시는 이미 주소가 확정된 실행 파일을 다루었다. 하지만 .o (오브젝트) 파일 단계에서는 주소가 0으로 설정되어 있다. 이때 sh_link와 sh_info의 관계가 결정적인 역할을 한다.

링커는 .rela.text 섹션을 읽는다.
sh_link를 통해 심볼 테이블을 찾아, 어떤 심볼(예: printf)을 참조해야 하는지 파악한다.
sh_info를 통해 .text 섹션 내의 정확히 어느 오프셋을 수정해야 하는지 파악한다.
최종적으로 심볼의 실제 주소가 결정되면, 그 값을 .text 섹션의 해당 위치에 덮어쓴다(Patch).

8.2 보안 및 포렌식 관점의 시사점

섹션 스트리핑(Stripping): 공격자나 상용 소프트웨어 배포자는 strip 명령어로 .symtab을 제거한다. 이 경우 로컬 변수 이름은 사라지지만, 앞서 언급했듯이 .dynsym(동적 심볼)은 실행을 위해 남겨져야 한다. 따라서 분석가는 .symtab이 없을 때 자동으로 .dynsym을 대체재로 사용하여 분석을 시도해야 한다.

안티 디버깅 트릭: 악성코드는 섹션 헤더 테이블을 고의로 손상시키거나(e_shoff를 엉뚱한 곳으로 설정), sh_link 값을 조작하여 분석 도구(GDB, IDA Pro 등)의 크래시를 유발할 수 있다. 그러나 운영체제 로더는 섹션 헤더가 아닌 프로그램 헤더(Program Header)를 사용하여 메모리를 구성하므로, 프로그램은 정상적으로 실행된다. 이는 **"섹션 헤더는 실행에 필수적이지 않다"**는 맹점을 이용한 것이다. 따라서 고급 분석가는 섹션 헤더에 의존하지 않고, 프로그램 헤더와 원시 바이너리 스캐닝을 통해 데이터를 복구하는 기술을 갖추어야 한다.

9. 결론

ELF 파일 형식은 단순한 데이터 컨테이너가 아니라, 컴파일러의 의도와 운영체제의 실행 환경을 연결하는 정교한 인터페이스이다. 섹션 헤더는 데이터의 위치와 속성을 정의하는 **'지도(Map)'**이며, 심볼 테이블은 그 지도 위에서 의미 있는 지점을 가리키는 **'색인(Index)'**이다. 그리고 이 둘은 sh_link, sh_info, st_shndx라는 연결 고리를 통해 유기적으로 결합되어 있다.

본 보고서를 통해 제시된 데이터 추출 방법론과 로그 분석 예시는 이러한 구조적 관계가 실제 바이너리 해석에 어떻게 적용되는지를 명확히 보여준다. 특히 파일 오프셋과 가상 메모리 주소 간의 변환, 그리고 심볼 값(st_value)의 문맥적 해석은 정확한 분석을 위한 필수 조건이다. 분석가는 이러한 메커니즘을 깊이 이해함으로써, 단순한 정보 추출을 넘어 시스템의 동작 원리를 꿰뚫어 보는 통찰력을 확보할 수 있다.
