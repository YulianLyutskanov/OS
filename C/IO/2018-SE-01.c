#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

    if(argc != 3) errx(1, "2 params needed");
    
    if(strcmp(argv[1], "-d") == 0) {
        uint8_t hash[256] = {0};
        uint16_t len = strlen(argv[2]);

        for(int i = 0; i < len; i++){
            hash[(uint8_t)argv[2][i]] = 1;
        }

        int8_t bytesRead;
        uint8_t c;
        while((bytesRead = read(0, &c, sizeof(c))) == sizeof(c)) {
            if(hash[c] != 1) {
                if(write(1, &c, sizeof(c)) != sizeof(c)) err(3, "cant write");
            }
        }

        if(bytesRead == -1) err(2, "Cant read");
    } else if(strcmp(argv[1], "-s") == 0) {
        uint8_t hash[256] = {0};
        uint16_t len = strlen(argv[2]);

        for(int i = 0; i < len; i++){
            hash[(uint8_t)argv[2][i]] = 1;
        }

        int8_t bytesRead;
        uint8_t cur;
        uint8_t prev = 0;
        while((bytesRead = read(0, &cur, sizeof(cur))) == sizeof(cur)) {
            if(hash[cur] == 1 && prev == cur) continue;

            if(write(1, &cur, sizeof(cur)) != sizeof(cur)) err(4, "Cant write");
            prev = cur;
        }

        if(bytesRead == -1) err(2, "Cant read");
    } else {
        uint16_t len = strlen(argv[1]);
        if(len != strlen(argv[2])) errx(5,"Strings not equal");

        uint8_t from_to[256] = {0};
        for(int i = 0; i < 256; i++) {
            from_to[i] = (uint8_t)i;
        }

        for(int i = 0; i < len; i++) {
            from_to[(uint8_t)argv[1][i]] = argv[2][i];
        }

        int8_t bytesRead;
        uint8_t c;
        while((bytesRead = read(0, &c, sizeof(c))) == sizeof(c)) {
             if(write(1, &from_to[c], sizeof(uint8_t)) != sizeof(uint8_t)) err(6, "Cant write");
        }

        if(bytesRead == -1) err(2, "Cant read");

    }

    return 0;
}
