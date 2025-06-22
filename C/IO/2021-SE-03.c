#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

void myWrite(int to, const char* str);
void myWrite(int to, const char* str) {
    int len = strlen(str);

    if(write(to, str, len) != len) err(3, "Cant write to file");
}

int main(int argc, char* argv[]) {

    if(argc != 3) errx(1, "2 params needed");

    int fd1 = open(argv[1], O_RDONLY);
    if(fd1 == -1) err(2, "Cant open file");

    int fd2 = open(argv[2], O_TRUNC | O_CREAT | O_WRONLY, 0666);
    if(fd2 == -1) err(2, "Cant open file");

    struct stat s;
    if(fstat(fd1, &s) == -1) err(4, "Cant stat");
    uint32_t size = s.st_size;
    if(size % 2 != 0) errx(5, "Broken file");
    uint32_t els = s.st_size / 2;
    if(els > 524288) errx(5, "Broken file");

    myWrite(fd2, "#include <stdint.h>\n\n");
    myWrite(fd2, "uint32_t arrN = ");
    char n[10];
    snprintf(n, 10, "%d", els);
    myWrite(fd2, n);
    myWrite(fd2, ";\n\n");
    myWrite(fd2, "uint16_t arr[] = { ");

    uint16_t num;
    for(uint32_t i = 0; i < els - 1; i++) {
        if(read(fd1, &num, sizeof(num)) != sizeof(num)) err(7, "Cant read");
        snprintf(n, 10, "%d", num);

        myWrite(fd2, n);
        myWrite(fd2, ", ");
    }

    if(read(fd1, &num, sizeof(num)) != sizeof(num)) err(7,"cant read");
    snprintf(n, 10, "%d", num);
    myWrite(fd2, n);
    myWrite(fd2, "};\n");

    close(fd1);
    close(fd2);
    return 0;
}
