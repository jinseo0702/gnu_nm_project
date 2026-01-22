#include <stdio.h>

void swap(char *arr, int start, int end);
int partion(char *arr, int start, int end);

void quickSort(char *arr, int start, int end) {
	int partion2 = partion(arr, start, end);
	if (start >= end) return;
	quickSort(arr, start, partion2 - 1);
	quickSort(arr, partion2, end);
}

void swap(char *arr, int start, int end) {
	char temp;

	temp = arr[start];
	arr[start] = arr[end];
	arr[end] = temp;
}

int partion(char *arr, int start, int end) {
	static int cnt;
	int tstart = start;
	int tend = end;
	int pivot = arr[(start + end) / 2];
	while (start <= end) {
		while (arr[start] < pivot) start++;
		while (arr[end] > pivot) end--;
		if (start <= end) {
			swap(arr, start, end);
			start++;
			end--;
		}
	}
	printf("---------------------------------------\n");
	printf("cnt is %d | start is %d | end is %d \n", cnt, tstart, tend);
	printf("pivot is %c | return start is %d \n", pivot, start);
	printf("arr is %s \n", arr);
	for (; tstart <= tend; tstart++) {
		printf("%c ", arr[tstart]);
	}
	printf("\n");
	printf("---------------------------------------\n");
	cnt++;
	return start;
}

int main(void){
	char arr[11] = {'3', '7', '1', '9', '0', '5', '2', '8', '6', '4', 0};

	printf("arr is %s \n", arr);
	quickSort(arr, 0, 9);
	printf("arr is %s \n", arr);

	return 0;
}

/*
cnt0  QS[0..9]  pivot='0'  -> p=1
├─ cnt1  QS[0..0]  pivot='0' -> p=1        (끝: 구간 크기 1)
└─ cnt2  QS[1..9]  pivot='5'  -> p=6
   ├─ cnt3  QS[1..5]  pivot='2' -> p=3
   │  ├─ cnt4  QS[1..2]  pivot='2' -> p=2
   │  │  ├─ cnt5  QS[1..1]  pivot='1' -> p=2   (끝)
   │  │  └─ cnt6  QS[2..2]  pivot='2' -> p=3   (끝)
   │  └─ cnt7  QS[3..5]  pivot='3' -> p=4
   │     ├─ cnt8  QS[3..3]  pivot='3' -> p=4   (끝)
   │     └─ cnt9  QS[4..5]  pivot='4' -> p=5
   │        ├─ cnt10 QS[4..4] pivot='4' -> p=5 (끝)
   │        └─ cnt11 QS[5..5] pivot='5' -> p=6 (끝)
   └─ cnt12 QS[6..9] pivot='8' -> p=8
      ├─ cnt13 QS[6..7] pivot='7' -> p=7
      │  ├─ cnt14 QS[6..6] pivot='6' -> p=7    (끝)
      │  └─ cnt15 QS[7..7] pivot='7' -> p=8    (끝)
      └─ cnt16 QS[8..9] pivot='8' -> p=9
         ├─ cnt17 QS[8..8] pivot='8' -> p=9    (끝)
         └─ cnt18 QS[9..9] pivot='9' -> p=10   (끝)
*/

/*
arr is 3719052864 
---------------------------------------
cnt is 0 | start is 0 | end is 9 
pivot is 0 | return start is 1 
arr is 0719352864 
0 7 1 9 3 5 2 8 6 4 
---------------------------------------
---------------------------------------
cnt is 1 | start is 0 | end is 0 
pivot is 0 | return start is 1 
arr is 0719352864 
0 
---------------------------------------
---------------------------------------
cnt is 2 | start is 1 | end is 9 
pivot is 5 | return start is 6 
arr is 0412359867 
4 1 2 3 5 9 8 6 7 
---------------------------------------
---------------------------------------
cnt is 3 | start is 1 | end is 5 
pivot is 2 | return start is 3 
arr is 0214359867 
2 1 4 3 5 
---------------------------------------
---------------------------------------
cnt is 4 | start is 1 | end is 2 
pivot is 2 | return start is 2 
arr is 0124359867 
1 2 
---------------------------------------
---------------------------------------
cnt is 5 | start is 1 | end is 1 
pivot is 1 | return start is 2 
arr is 0124359867 
1 
---------------------------------------
---------------------------------------
cnt is 6 | start is 2 | end is 2 
pivot is 2 | return start is 3 
arr is 0124359867 
2 
---------------------------------------
---------------------------------------
cnt is 7 | start is 3 | end is 5 
pivot is 3 | return start is 4 
arr is 0123459867 
3 4 5 
---------------------------------------
---------------------------------------
cnt is 8 | start is 3 | end is 3 
pivot is 3 | return start is 4 
arr is 0123459867 
3 
---------------------------------------
---------------------------------------
cnt is 9 | start is 4 | end is 5 
pivot is 4 | return start is 5 
arr is 0123459867 
4 5 
---------------------------------------
---------------------------------------
cnt is 10 | start is 4 | end is 4 
pivot is 4 | return start is 5 
arr is 0123459867 
4 
---------------------------------------
---------------------------------------
cnt is 11 | start is 5 | end is 5 
pivot is 5 | return start is 6 
arr is 0123459867 
5 
---------------------------------------
---------------------------------------
cnt is 12 | start is 6 | end is 9 
pivot is 8 | return start is 8 
arr is 0123457689 
7 6 8 9 
---------------------------------------
---------------------------------------
cnt is 13 | start is 6 | end is 7 
pivot is 7 | return start is 7 
arr is 0123456789 
6 7 
---------------------------------------
---------------------------------------
cnt is 14 | start is 6 | end is 6 
pivot is 6 | return start is 7 
arr is 0123456789 
6 
---------------------------------------
---------------------------------------
cnt is 15 | start is 7 | end is 7 
pivot is 7 | return start is 8 
arr is 0123456789 
7 
---------------------------------------
---------------------------------------
cnt is 16 | start is 8 | end is 9 
pivot is 8 | return start is 9 
arr is 0123456789 
8 9 
---------------------------------------
---------------------------------------
cnt is 17 | start is 8 | end is 8 
pivot is 8 | return start is 9 
arr is 0123456789 
8 
---------------------------------------
---------------------------------------
cnt is 18 | start is 9 | end is 9 
pivot is 9 | return start is 10 
arr is 0123456789 
9 
---------------------------------------
arr is 0123456789
*/

/*
Quick Sort
### 퀵정렬 최적화(간단 요약)

- **pivot을 잘 고르기**: `맨앞/중간` 고정 말고 **랜덤** 또는 **median-of-three(앞/중간/끝의 중간값)** 쓰면 최악 케이스가 크게 줄어듦.
- **작은 구간은 삽입정렬로**: 길이가 작아지면(예: 16 이하) 재귀 대신 **insertion sort**가 더 빠른 경우가 많음.
- **중복값 많으면 3-way partition**: `< pivot`, `= pivot`, `> pivot`으로 나누면 같은 값이 많을 때 속도가 확 좋아짐.
- **스택(재귀) 줄이기**: 항상 **작은 쪽만 재귀**하고 큰 쪽은 루프로 처리하면 스택이 깊어지는 걸 막음.
- **최악 방지 장치(introsort)**: 재귀가 너무 깊어지면 **힙정렬로 전환**해서 최악도 \(O(n \log n)\) 보장.
*/

/*
내가 만든코드
        ```c
        void quickSort(t_NmSym *arr, int start, int end) {
        	int partion2 = partion(arr, start, end);
        	if (start >= end) return;
        	quickSort(arr, start, partion2 - 1);
        	quickSort(arr, partion2, end);
        }

        void swap(t_NmSym *arr, int start, int end) {
        	t_NmSym temp;

            ft_memcpy(&temp, &arr[start], sizeof(t_NmSym));
            ft_memcpy(&arr[start], &arr[end], sizeof(t_NmSym));
            ft_memcpy(&arr[end], &temp, sizeof(t_NmSym));
        }

        int partion_n(t_NmSym *arr, int start, int end) {
        	int tstart = start;
        	int tend = end;
        	ElfN_Addr pivot = arr[(start + end) / 2].st_value;
        	while (start <= end) {
        		while (arr[start].st_value < pivot) start++;
        		while (arr[end].st_value > pivot) end--;
        		if (start <= end) {
        			swap(arr, start, end);
        			start++;
        			end--;
        		}
        	}
        	return start;
        }

        int partion_none_n(t_NmSym *arr, int start, int end) {
        	int tstart = start;
        	int tend = end;
            int len = 0;
        	const char *pivot = arr[(start + end) / 2].SymName;
        	while (start <= end) {
                len = ft_strlen(arr[start].SymName) > ft_strlen(pivot) ? ft_strlen(arr[start].SymName) : ft_strlen(pivot)
        		while (ft_strncmp(arr[start].SymName, pivot) < 0){
                    start++
                };
                len = ft_strlen(arr[end].SymName) > ft_strlen(pivot) ? ft_strlen(arr[end].SymName) : ft_strlen(pivot)
        		while (ft_strncmp(arr[end].SymName, pivot) > 0){
                    end--
                };
        		if (start <= end) {
        			swap(arr, start, end);
        			start++;
        			end--;
        		}
        	}
        	return start;
        }
        ```

AI가 만든 코드
#include <string.h>

// 1. Swap 로직: 구조체 직접 대입 (memcpy보다 직관적이고 효율적)
void swap(t_NmSym *arr, int i, int j) {
    t_NmSym temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

// 이름 비교 함수 (ASCII 순)
int compare_name(t_NmSym *a, t_NmSym *b) {
    return ft_strcmp(a->SymName, b->SymName);
}

// 주소 비교 함수 (보완된 로직: 주소 같으면 이름순)
int compare_numeric(t_NmSym *a, t_NmSym *b) {
    if (a->st_value < b->st_value) return -1;
    if (a->st_value > b->st_value) return 1;
    // 주소가 같으면 이름순으로 정렬 (nm의 표준 동작)
    return compare_name(a, b);
}

// 통합 파티션 로직
int partition(t_NmSym *arr, int start, int end, int is_numeric) {
    t_NmSym pivot = arr[(start + end) / 2];
    
    while (start <= end) {
        if (is_numeric) {
            while (compare_numeric(&arr[start], &pivot) < 0) start++;
            while (compare_numeric(&arr[end], &pivot) > 0) end--;
        } else {
            while (compare_name(&arr[start], &pivot) < 0) start++;
            while (compare_name(&arr[end], &pivot) > 0) end--;
        }
        
        if (start <= end) {
            swap(arr, start, end);
            start++;
            end--;
        }
    }
    return start;
}

// 퀵 정렬 메인 함수
void quickSort(t_NmSym *arr, int start, int end, int is_numeric) {
    if (start >= end) return;

    int p = partition(arr, start, end, is_numeric);
    quickSort(arr, start, p - 1);
    quickSort(arr, p, end);
}

// 2. 내림차순(-r) 처리: 정렬 완료 후 배열을 뒤집음
void reverse_symbols(t_NmSym *arr, int size) {
    int i = 0;
    int j = size - 1;
    while (i < j) {
        swap(arr, i, j);
        i++;
        j--;
    }
}
*/