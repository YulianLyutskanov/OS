#include <stdint.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void myReadFromWriteTo(int from, int to);
void myReadFromWriteTo(int from, int to) {
    int8_t bytesRead;
    uint8_t c;
    while((bytesRead = read(from, &c, sizeof(c))) == sizeof(c)) {
        if(write(to, &c, sizeof(c)) == -1) err(1,"Cant write");
    }
    if(bytesRead == -1) err(2, "Cant read");
}

int main(int argc, char* argv[]) {

    if(argc == 1) {
        myReadFromWriteTo(0, 1);
    }


    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-") == 0) {
             myReadFromWriteTo(0, 1);
        } else {
            int fd = open(argv[i], O_RDONLY);
            if(fd == -1) err(3, "Cant open file");
            myReadFromWriteTo(fd, 1);
            close(fd);
        }

    }

    return 0;
}
