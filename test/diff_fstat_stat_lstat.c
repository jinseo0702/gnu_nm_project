#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void print_stat(struct stat *temp) {
    printf("struct sate {\n\
               dev_t     st_dev;     %ld         /* ID of device containing file */ \n\
               ino_t     st_ino;     %ld        /* Inode number */ \n\
               mode_t    st_mode;    %u        /* File type and mode */ \n\
               nlink_t   st_nlink;   %ld       /* Number of hard links */ \n\
               uid_t     st_uid;     %u      /* User ID of owner */ \n\
               gid_t     st_gid;     %u      /* Group ID of owner */ \n\
               dev_t     st_rdev;    %ld      /* Device ID (if special file) */ \n\
               off_t     st_size;    %ld      /* Total size, in bytes */ \n\
               blksize_t st_blksize; %ld     /* Block size for filesystem I/O */ \n\
               blkcnt_t  st_blocks;  %ld    /* Number of 512B blocks allocated */ \n\
    }\n", temp->st_dev, temp->st_ino, temp->st_mode, temp->st_nlink, temp->st_uid, temp->st_gid, temp->st_rdev, temp->st_size, temp->st_blksize, temp->st_blocks);
}


int main(int argc, char *argv[]) {
    struct stat s_state;
    struct stat l_state;
    struct stat f_state;
    int fd = open(argv[1], O_RDONLY);
    stat(argv[1], &s_state);
    lstat(argv[1], &l_state);
    fstat(fd, &f_state);
    printf("s_state \n");
    print_stat(&s_state);
    printf("\n");
    printf("l_state \n");
    print_stat(&l_state);
    printf("\n");
    printf("f_state \n");
    print_stat(&f_state);
    printf("\n");
    return 0;
}