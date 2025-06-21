#include <stdint.h>
#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

struct triple{
    uint16_t offset;
    uint8_t old, new;
};

typedef struct triple triple_t;

int main(int argc, char* argv[]) {

    if(argc != 4) errx(1, "3 params needed");

    int fd1 = open(argv[1], O_RDONLY);
    if(fd1 == -1) err(2, "Cant open to read");

    int fd2 = open(argv[2], O_RDONLY);
    if(fd2 == -1) err(3, "Cant open to read");

    struct stat s;
    if(fstat(fd1, &s) == -1 ) err(4, "Cant stat file");
    if(s.st_size % sizeof(triple_t) != 0) errx(5,"Patch file invalid");

    if(fstat(fd2, &s) == -1 ) err(6, "Cant stat file");
    if(s.st_size > UINT16_MAX ) errx(7, "File too big");

    int fd3 = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd3 == -1) err(8, "Cant open file to write");

    triple_t t;
    if(read(fd1, &t, sizeof(t)) != sizeof(t)) err(9, "Cant read triple");
    if(t.offset > s.st_size) errx(10, "Invalid offset");

    for(int i = 0; i < s.st_size; i++) {
        if(i < t.offset) {
            uint8_t c;
            if(read(fd2, &c, sizeof(c)) != sizeof(c)) err(11, "Cant read byte");
            if(write(fd3, &c, sizeof(c)) != sizeof(c)) err(12, "Cant write byte");
        } else {
            uint8_t c;
            if(read(fd2, &c, sizeof(c)) != sizeof(c)) err(13, "Cant read byte");
            if(c != t.old ) errx(14, "Chars don't match from patch file");
            if(write(fd3, &t.new, sizeof(c)) != sizeof(c)) err(14, "Cant write new byte");

            int8_t readTriple = read(fd1, &t, sizeof(t));

            if(readTriple == -1)
                err(15, "Cant read triple");
            else if(readTriple == 0) {  //last triple
                t.offset = UINT16_MAX;
                continue;
            }

            if(t.offset > s.st_size) errx(16, "Invalid offset");
        }

    }

    close(fd1);
    close(fd2);
    close(fd3);

    return 0;
}
