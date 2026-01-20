#include "../include/debug.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>
#include "../ft_printf/libftprintf.h"



int main(int argc, char *argv[]) {
    // 아직 다중은 안만들고 Test 하는중
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        ft_fprintf(2, "Error \n");
    }
    struct stat stat_temp;
    if (fstat(fd, &stat_temp) < 0) {
        ft_fprintf(2, "fstat Error \n");
    }
    DBG_STAT(&stat_temp);
    Elf64_Ehdr *Elf64 = mmap(NULL, stat_temp.st_size, PROT_READ , MAP_PRIVATE, fd, 0);
    if (Elf64 == MAP_FAILED) {
        ft_fprintf(2, "mmap faild \n");
    }
    unsigned char *e_ident = Elf64->e_ident;
    DBG_E_IDENT(e_ident);
    DBG_ELF64(Elf64);
    DBG_ELF64_SHDR(Elf64);
    DBG_ELF64_SYM(Elf64);

    return (0);
}
