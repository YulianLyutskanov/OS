#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

    if(argc != 3) errx(1, "2 params needed");

    int fd1 = open(argv[1], O_RDONLY);
    if(fd1 == -1) err(2, "cant open");

    int fd2 = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd2 == -1) err(3, "cant open");

    int8_t bytesRead;
    uint8_t byte;

    while((bytesRead = read(fd1, &byte, sizeof(byte))) > 0) {
        if(byte == 0x55) {
            uint8_t size;
            if((bytesRead = read(fd1, &size, sizeof(size))) == -1) err(5, "cant read");
            else if(bytesRead == 0) break;
            uint8_t checksum = 0x55 ^ size;

            for(uint8_t i = 3; i <= size - 1; i++) {
                if((bytesRead = read(fd1, &byte, sizeof(byte))) == -1) err(6,"cant read");
                else if(bytesRead == 0) break;
                checksum ^= byte;
            }

            if((bytesRead = read(fd1, &byte, sizeof(byte))) == -1) err(7, "cant read");
            else if(bytesRead == 0) break;
            if(checksum == byte) {
                if(lseek(fd1, -size , SEEK_CUR) == -1) err(7, "cant seek");
                for(uint8_t i = 1; i <= size; i++) {
                    if((bytesRead = read(fd1, &byte, sizeof(byte))) == -1) err(5, "cant read");
                    if(write(fd2, &byte, sizeof(byte)) != sizeof(byte)) err(8, "cant write");

                }
            }
        }
    }


    if(bytesRead == -1) err(4, "cant read");
    close(fd1);
    close(fd2);
    return 0;

}
