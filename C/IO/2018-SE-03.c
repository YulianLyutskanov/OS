#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

bool isDigit(char ch);
bool isDigit(char ch) {
    return ch >= '1' && ch <= '9';
}

int main(int argc, char* argv[]) {

    if(argc != 3 && argc != 5) errx(1, "2 or 4 params needed");

    if(strcmp(argv[1],"-c") == 0) {
        if(argc != 3) errx(2, "2 params needed after -c");

        int len = strlen(argv[2]);
        if(len != 1 && len != 3) errx(3, "The number after -c must be num or num1-num2");

        if(len == 1) {
            if(!isDigit(argv[2][0])) errx(4, "The number is not digit");
            int num = atoi(argv[2]);

            int pos = 0;
            char c;
            int bytesRead;
            while((bytesRead = read(0, &c, sizeof(c))) == sizeof(c)) {
                if(c == '\n') {
                    pos = 0;
                    if(write(1, &c, sizeof(c)) == -1) err(6, "Cant write");
                    continue;
                }
                pos++;
                if(pos == num) {
                    if(write(1, &c, sizeof(c)) == -1) err(6, "Cant write");
                }
            }

            if(bytesRead == -1) err(5, "Cant read");
        } else {
            if(!(isDigit(argv[2][0]) && argv[2][1] == '-'  && isDigit(argv[2][2]))) {
                errx(4, "Format must be digit-digit");
            }
            int from = argv[2][0] - '0';
            int to = argv[2][2] - '0';

            if(from > to) errx(7, "from is greater than to");

            int pos = 0;
            char c;
            int bytesRead;
            while((bytesRead = read(0, &c, sizeof(c))) == sizeof(c)) {
                if(c == '\n') {
                    pos = 0;
                    if(write(1, &c, sizeof(c)) == -1) err(6, "Cant write");
                    continue;
                }
               pos++;
                if(pos >= from && pos <= to) {
                    if(write(1, &c, sizeof(c)) == -1) err(6, "Cant write");
                }
            }
            if(bytesRead == -1) err(5, "Cant read");
        }

    } else if(strcmp(argv[1], "-d") == 0) {
        if(argc != 5) errx(7, "4 params needed after -d");
        if(strcmp(argv[3], "-f") != 0) errx(8, "-f should be 3rd param");
        char delim = argv[2][0];

        int len = strlen(argv[4]);
        if(len != 1 && len != 3) errx(3, "The number after -c must be num or num1-num2");

        if(len == 1) {
            if(!isDigit(argv[4][0])) errx(4, "The number is not digit");
            int num = atoi(argv[4]);

            int field = 1;
            char c;
            int bytesRead;
            while((bytesRead = read(0, &c, sizeof(c))) == sizeof(c)) {
                if(c == '\n') {
                    field = 1;
                    if(write(1, &c, sizeof(c)) == -1) err(6, "Cant write");
                    continue;
                }
                if(c == delim) {
                    field++;
                    continue;
                }

                if(field == num) {
                    if(write(1, &c, sizeof(c)) == -1) err(6, "Cant write");
                }

            }
            if(bytesRead == -1) err(5, "Cant read");

        } else {
            if(!(isDigit(argv[4][0]) && isDigit(argv[4][2]) && argv[4][1] == '-')) errx(4, "It should be digit-digit");
            int from = argv[4][0] - '0';
            int to = argv[4][2] - '0';

            if(from > to) errx(7, "from is greater than to");

            int field = 1;
            char c;
            int bytesRead;
            while((bytesRead = read(0, &c, sizeof(c))) == sizeof(c)) {
                if(c == '\n') {
                    field = 1;
                    if(write(1, &c, sizeof(c)) == -1) err(6, "Cant write");
                    continue;
                }

                if(c == delim) {
                    if(field >= from && field < to) {
                         if(write(1, &c, sizeof(c)) == -1) err(6, "Cant write");
                    }
                    field++;
                    continue;
                }

                if(field >= from && field <= to) {
                    if(write(1, &c, sizeof(c)) == -1) err(6, "Cant write");
                }
            }
            if(bytesRead == -1) err(5, "Cant read");
        }

    }

    return 0;
}
