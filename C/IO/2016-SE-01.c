#include <stdint.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]){

    if(argc != 2)
        err(1, "Programm must accept exactly 1 parameter");

    int fd = open(argv[1], O_RDWR);
    if(fd == -1) { err(2, "Couldn't open file"); }

    uint32_t byte[256] = {0};
    uint8_t cur;
    ssize_t bytesRead = 0;

    while((bytesRead = read(fd,&cur,1)) == 1){
        byte[cur] += 1;
    }

    if(bytesRead < 0) { err(3, "Error while reading bytes"); }

    if(lseek(fd, 0, SEEK_SET) == -1) { err(4,"Could not move to begin to file"); }

    for(uint16_t i = 0; i < 256; i++){
        for(uint32_t j = 0; j < byte[i]; j++){
            if(write(fd, &i, 1) != 1)
                err(5,"Could not write to file");
        }
    }

    close(fd);

    return 0;
}
