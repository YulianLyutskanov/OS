#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <unistd.h>

struct pair {
    uint32_t first;
    uint32_t second;
};
typedef struct pair pair_t;

int main(int argc, char* argv[]) {

    if(argc != 4) { errx(1,"Params must be 3!"); }

    int fd1 = open(argv[1], O_RDONLY);
    if(fd1 < 0) { err(2, "Can't open first file"); }
    int fd2 = open(argv[2], O_RDONLY);
    if(fd2 < 0) { err(3, "Can't open second file"); }
    int fd3 = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd3 < 0) { err(4, "Can't open third file"); }

    int8_t bytesRead;
    pair_t p;
    while((bytesRead = read(fd1, &p, sizeof(p))) == sizeof(p)) {
        if (lseek(fd2, p.first * sizeof(uint32_t), SEEK_SET) < 0) { err(5, "Can't lseek"); }

        for(uint32_t i = 0; i < p.second ; i++) {
            uint32_t num;
            if(read(fd2, &num, sizeof(num)) < 0) { err(6, "Can't read"); }
            if(write(fd3, &num, sizeof(num)) < 0) { err(7, "Can't write"); }
        }
    }

    if(bytesRead < 0) { err(5, "Can't read nums correctly"); }

    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}
