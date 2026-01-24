#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <ar.h>

#define MOVE_POINTER(base, offset) (void *)((unsigned char *)(base) + (offset))

typedef struct s_ar_hdr{
    char ar_name[17];		/* Member file name, sometimes / terminated. */
    char ar_date[13];		/* File date, decimal seconds since Epoch.  */
    char ar_uid[7];
    char ar_gid[7];	/* User and group IDs, in ASCII decimal.  */
    char ar_mode[9];		/* File mode, in ASCII octal.  */
    char ar_size[11];		/* File size, in ASCII decimal.  */
    char ar_fmag[3];		/* Always contains ARFMAG.  */

} t_ar_hdr;

typedef struct s_Special{
    unsigned char *ptr;
    size_t size;
    char name[17];
}t_Special;

void print_mem(const char *str, const char *str2, int size){
    printf("%s : ", str2);
    for (int i = 0; i < size; i++) {
        printf("%d ", str[i]);
    }
    printf("\n");
}

void print_magic(const char *str, const char *str2, int size){
    printf("%s : ", str2);
    for (int i = 0; i < size; i++) {
        if (!isprint(str[i]) || isspace(str[i])) printf(" 0x%X ", str[i]);
        else printf("%c", str[i]);
    }
    printf("\n");
}

void print_data(const unsigned char *str, int idx, int size, const char *name){
    printf("print_data index is %d : \n", idx);
    printf("print_data name is %s : \n", name);
    for (int i = 0; i < size; i++) {
        if (!isgraph(str[i])) printf(" 0x%X ", str[i]);
        else printf("%c", str[i]);
    }
    printf("\n");
}

void cpy_ar_hdr(t_ar_hdr *temp_ar_hdr, struct ar_hdr *ar_hdr){
    strncpy(temp_ar_hdr->ar_name, ar_hdr->ar_name, 16);
    strncpy(temp_ar_hdr->ar_date, ar_hdr->ar_date, 12);
    strncpy(temp_ar_hdr->ar_uid, ar_hdr->ar_uid, 6);
    strncpy(temp_ar_hdr->ar_gid, ar_hdr->ar_gid, 6);
    strncpy(temp_ar_hdr->ar_mode, ar_hdr->ar_mode, 8);
    strncpy(temp_ar_hdr->ar_size, ar_hdr->ar_size, 10);
    strncpy(temp_ar_hdr->ar_fmag, ar_hdr->ar_fmag, 2);
}

void print_all(t_ar_hdr *temp_ar_hdr) {
print_magic(temp_ar_hdr->ar_name, "ar_name", 16);
print_magic(temp_ar_hdr->ar_date, "ar_date", 12);
print_magic(temp_ar_hdr->ar_uid, "ar_uid ", 6);
print_magic(temp_ar_hdr->ar_gid, "ar_gid ", 6);
print_magic(temp_ar_hdr->ar_mode, "ar_mode", 8);
print_magic(temp_ar_hdr->ar_size, "ar_size", 10);
print_magic(temp_ar_hdr->ar_fmag, "ar_fmag", 2);

print_mem(temp_ar_hdr->ar_name, "ar_name", 16);
print_mem(temp_ar_hdr->ar_date, "ar_date", 12);
print_mem(temp_ar_hdr->ar_uid, "ar_uid ", 6);
print_mem(temp_ar_hdr->ar_gid, "ar_gid ", 6);
print_mem(temp_ar_hdr->ar_mode, "ar_mode", 8);
print_mem(temp_ar_hdr->ar_size, "ar_size", 10);
print_mem(temp_ar_hdr->ar_fmag, "ar_fmag", 2);
}

//print simple archive format file
int main(int argc, char *argv[]) {
    int fd = open(argv[1], O_RDONLY);
    struct stat buf;
    t_Special temp[100];
    size_t speCnt = 0;
    memset(temp, 0, sizeof(t_ar_hdr));

    fstat(fd, &buf);
    unsigned char *ptr = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (memcmp((const char *)ptr, ARMAG, SARMAG) == 0) {
        printf("Yes\n");
    } else {
        printf("No\n");
    }

    print_magic((char *)ptr, "magigIs", 8);
    print_mem((char *)ptr, "magigIs", 8);
    int cnt = 1;
    size_t amountSize = 8;
    const size_t size = sizeof(struct ar_hdr);

    for (int i = 0; amountSize < buf.st_size; i++) {
        printf("Cnt is %d\n", cnt);
        struct ar_hdr *temp_ar = MOVE_POINTER(ptr, amountSize);
        t_ar_hdr temp_ar_hdr = {0,};
        if (i == 0) {
            cpy_ar_hdr(&temp_ar_hdr, temp_ar);
        } else {
            cpy_ar_hdr(&temp_ar_hdr, temp_ar);
        }
        if (temp_ar_hdr.ar_name[0] == '/') {
            strncpy(temp[speCnt].name, temp_ar->ar_name, 16);
            char *tp = strchr(temp[speCnt].name, 0x20);
            *tp = '\0';
            temp[speCnt].ptr = (unsigned char *)temp_ar;
            temp[speCnt].size = atoi(temp_ar_hdr.ar_size);
            speCnt++;
        }
        amountSize += atoi(temp_ar_hdr.ar_size);
        print_all(&temp_ar_hdr);
        amountSize += size;
        cnt++;
        printf("\n");
    }

    for (size_t i = 0; i < speCnt; i++) {
        const unsigned char *spePlusSsize = temp[i].ptr + size;
        
        print_data(spePlusSsize, i, temp[i].size, temp[i].name);
    }

    close(fd);
    munmap(ptr, buf.st_size);
    return 0;
}