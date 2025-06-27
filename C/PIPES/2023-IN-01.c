#include <err.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

bool isDigit(char c);
bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

uint8_t myAtoi(char c);
uint8_t myAtoi(char c) {
    return c -'0';
}

char str[3][5] = { "tic ", "tac ", "toe\n" };

void handle(uint8_t wc, int from, int to);
void handle(uint8_t wc, int from, int to) {
    uint8_t cur;
    int8_t bytesRead;
    bool sent = false;

    while((bytesRead = read(from, &cur, sizeof(cur))) == sizeof(cur)) {
        //sleep(1);
        if(cur >= wc) {
            if(!sent)
                if(write(to, &cur, sizeof(cur)) != sizeof(cur)) err(13, "canr write");
            break;
        }else {
            if(write(1, str[cur % 3], 4) != 4) err(16, "cant write");
            //printf("wrote when idx: %d\n", cur);
            cur++;
            if(write(to, &cur, sizeof(cur)) != sizeof(cur)) err(13, "canttt write");
        }

        if(cur >= wc) sent = true;

    }

    if(bytesRead == -1) err(15, "cant read");
}

int main(int argc, char* argv[]) {

    if(argc != 3) errx(1, "2 params needed");
    if(strlen(argv[1]) != 1 || (strlen(argv[2]) != 1 && strlen(argv[2]) != 2)) errx(2, "invalid args");
    if(!isDigit(argv[1][0])) errx(3, "first is not digit");
    uint8_t nc = myAtoi(argv[1][0]);
    if(nc > 7 || nc == 0) errx(4, "first must be [1,7]");

    uint8_t wc;
    if(strlen(argv[2]) == 1) {
        if(!isDigit(argv[1][0])) errx(3, "second is not digit");
        wc = myAtoi(argv[2][0]);
    }
    if(strlen(argv[2]) == 2) {
        if(!isDigit(argv[2][0]) || !isDigit(argv[2][1])) errx(3, "second is not digit");
        wc = myAtoi(argv[2][0]) * 10 + myAtoi(argv[2][1]);
    }

    if(wc > 35) errx(5, "second must be [1,35]");

   int p[8][2];
   uint8_t pipes = nc + 1;

    for(uint8_t i = 0; i < pipes; i++) {
        if(pipe(p[i]) == -1) err(8, "cant pipe");
    }

    for(uint8_t i = 1; i <= nc; i++){
        pid_t pid = fork();
        if(pid == -1) err(8, "cant fork");

        if(pid == 0) { //child
            int from = p[i - 1][0];
            int to = p[i][1];

            for(uint8_t j = 0; j < pipes; j++) {
                if(p[j][0] != from) close(p[j][0]);
                if(p[j][1] != to) close(p[j][1]);

            }

            handle(wc, from, to);
            close(from);
            close(to);

            return 0;
        }

    }

    //parent
    int from = p[nc][0];
    int to = p[0][1];

    for(uint8_t j = 0; j < pipes; j++) {
        if(p[j][0] != from) close(p[j][0]);
        if(p[j][1] != to) close(p[j][1]);
    }

    uint8_t index = 0;
    if(write(1, str[0], strlen(str[0])) == -1) err(11, "cant write");
    index++;
    if(write(to, &index, sizeof(index)) == -1) err(11, "canr write");

    handle(wc, from, to);

    close(from);
    close(to);

    for(uint8_t i = 1; i <= nc; i++) {
        int status;
        if(wait(&status) == -1) err(12, "cant wait");

        if(!WIFEXITED(status)) errx(13, "child was temrminated");
        else if(WEXITSTATUS(status) != 0) errx(14, "es !=0");
        //else dprintf(2, "child %d has exited\n", i);
    }


    return 0;
}
