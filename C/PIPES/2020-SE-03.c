#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>

struct data {
    char name[8];
    uint32_t offset, len;

};

typedef struct data data_t;

int main(int argc, char* argv[]) {

    if(argc != 2) errx(1, "1 param needed");

    int fd = open(argv[1], O_RDONLY);
    if(fd == -1) err(2, "cant open");
    struct stat s;
    if(fstat(fd, &s) == -1) err(3, "cant stat");
    if(s.st_size % sizeof(data_t) != 0) errx(4, "invalid file");
    if(s.st_size / sizeof(data_t) > 8 ) errx(5, "to large file");

    uint8_t childs = s.st_size / sizeof(data_t);
    data_t data[8];
    int16_t bytesRead;
    if((bytesRead = read(fd, &data, s.st_size)) == -1) err(6, "cant read");
    if(bytesRead != s.st_size) errx(7, "cant read at once");

    close(fd);

    int pfd[2];
    if(pipe(pfd) == -1) err(8, "cant pipe");
    pid_t pid;

    for(int8_t i = 0; i < childs; i++) {
        pid = fork();
        if(pid == -1) err(9, "cant fork");
        if(pid == 0) {
            close(pfd[0]);
            if(dup2(pfd[1], 1) == -1 ) err(11, "cant dup2");
            close(pfd[1]);

            int fd2 = open(data[i].name, O_RDONLY);
            if(fd2 == -1) err(10, "cant open");
            struct stat st;
            if(fstat(fd2, &st) == -1) err(12, "cant stat");

            if((data[i].offset + data[i].len) * sizeof(uint16_t) > (long unsigned int)st.st_size ) errx(14, "not such offset");

            if(lseek(fd2, data[i].offset * sizeof(uint16_t), SEEK_SET) == -1) err(15, "cant seek");

            uint16_t num;
            uint16_t res;
            for(uint32_t j = 0; j < data[i].len; j++) {
                if(read(fd2, &num, sizeof(num)) != sizeof(num)) err(16, "cant read");
                res ^= num;
            }
            if(write(1, &res, sizeof(res)) != sizeof(res)) err(17, "cant write");

            close(fd2);
            return 0;
        }
    }
    close(pfd[1]);

    int status;
    while(wait(&status) > 0) {
        if(!WIFEXITED(status)) errx(18, "child was terminated");
        if(WEXITSTATUS(status) != 0) errx(19, "child exited not 0");
    }

    uint16_t res = 0;
    uint16_t num;

    for(uint8_t i = 0; i < childs; i++) {
        if(read(pfd[0], &num, sizeof(num)) != sizeof(num)) err(20, "cant rwad from pipe");
        res ^= num;
    }

    char result[15];
    snprintf(result, 15, "result: %x\n", res);
    write(1, &result, strlen(result));

    close(pfd[0]);
    return 0;
}
