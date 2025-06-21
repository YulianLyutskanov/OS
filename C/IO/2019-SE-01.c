#include <err.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
struct record{
    uint32_t uid;
    uint16_t u1, u2;
    uint32_t start, end;
};

struct user{
    uint32_t uid;
    uint32_t duration;
};

typedef struct user user_t;
typedef struct record record_t;

int32_t findUser(user_t* users, uint16_t len, uint32_t id);

int32_t findUser(user_t* users, uint16_t len, uint32_t id){
    for(uint16_t j = 0; j < len; j++) {
        if(users[j].uid == id)
            return j;
    }

    return -1;
}

int main(int argc, char* argv[]) {

    if(argc != 2) errx(1, "1 param needed");

    int fd = open(argv[1], O_RDONLY);
    if(fd == -1) err(2, "Cant open file");

    struct stat s;
    if(fstat(fd, &s) == -1) err(3, "Cant stat");
    uint32_t size = s.st_size;
    if(size > 16384 * sizeof(record_t)) errx(7, "File too big");
    if(size % sizeof(record_t) != 0) errx(4, "File is broken");

    uint16_t numRecs = size /  sizeof(record_t);
    uint64_t sum = 0;
    uint32_t duration;
    record_t rec;

    user_t users[2048];
    uint16_t numUsers = 0;

    for(uint16_t i = 0; i < numRecs; i++) {
        if(read(fd, &rec, sizeof(rec)) != sizeof(rec)) err(5, "Cant read");
        if(rec.start > rec.end ) errx(6, "File is broken");
        duration = rec.end - rec.start;
        sum += duration;

        int32_t userIndex = findUser(users, numUsers, rec.uid);
        if(userIndex == -1) {
            users[numUsers] = (user_t){rec.uid, duration};
            numUsers++;
            if(numUsers >= 2048) errx(8, "Too many users");
        } else {
            if(users[userIndex].duration < duration)
                users[userIndex].duration = duration;
        }
    }

    double avg = (sum * 1.0) / numRecs;

    if(lseek(fd, 0, SEEK_SET) == -1) err(9, "Cant seek");
    double sumSquares = 0;

    for(uint16_t i = 0; i < numRecs; i++) {
        if(read(fd, &rec, sizeof(rec)) != sizeof(rec)) err(5, "Cant read");
        duration = rec.end - rec.start;
        double square = (duration - avg);
        sumSquares += (square * square);
    }

    double dispersion = sumSquares / numRecs;
    printf("avg: %f , dispersion: %f , num users : %i, num recs: %d",avg,dispersion,numUsers,numRecs);

    for(uint16_t j = 0; j < numUsers; j++) {
        if(users[j].duration * users[j].duration > dispersion) {
            printf("ID: %d , Longest session: %d \n", users[j].uid, users[j].duration);
        }
    }

    close(fd);
    return 0;
}
