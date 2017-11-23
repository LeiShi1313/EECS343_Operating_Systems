#include "types.h"
#include "stat.h"
#include "user.h"

#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200

void testtag() {
    printf(1, "test tagFile...");
    int fd, i;
    fd = open("tag", O_CREATE | O_RDWR);

    if (tagFile(fd, "testkey", "testval", 7) != 1) {
        printf(1, "tag file failed!\n");
        return;
    }
    if (tagFile(fd+1, "testkey", "testval", 7) != -1) {
        printf(1, "invalid fd test failed!\n");
        return;
    }
    if (tagFile(fd, "1234567890", "testval", 7) != -1) {
        printf(1, "tag key more than 10 test failed!\n");
        return;
    }
    if (tagFile(fd, "test", "123456789012345678", 17) != 1) {
        printf(1, "tag val 17 test failed!\n");
        return;
    }
    if (tagFile(fd, "test", "1234567890123456789", 18) != 1) {
        printf(1, "tag val 18 test failed!\n");
        return;
    }
    if (tagFile(fd, "test", "1234567890123456789", 19) != -1) {
        printf(1, "tag val 19 test failed!\n");
        return;
    }
    for (i = 0; i < 100; i++) {
        if (tagFile(fd, "testkey", "testval", 7) != 1) {
            printf(1, "%d retag file failed!\n", i);
            return;
        }
    }

    for (i = 0; i < 100; i++) {
        char val = i;
        if (tagFile(fd, "testkey", &val, 1) != 1) {
            printf(1, "%d retag with different val failed!\n", i);
            return;
        }
    }

    printf(1, "PASSED!\n");
}

void testremovetag() {
    printf(1, "test removeTag...");
    int fd, i;
    char *key = "key", *val = "val", *invalidkey = "key!";
    fd = open("tag", O_CREATE | O_RDWR);

    if (tagFile(fd, key, val, 3) != 1) {
        printf(1, "tag file failed\n");
        return;
    }
    if (removeFileTag(fd, key) != 1) {
        printf(1, "remove tag failed!\n");
        return;
    }
    if (tagFile(fd, key, val, 3) != 1) {
        printf(1, "tag file failed\n");
        return;
    }
    if (removeFileTag(fd, invalidkey) != -1) {
        printf(1, "remove invalid key failed!\n");
        return;
    }
    if (removeFileTag(fd+1, key) != -1) {
        printf(1, "remove invalid fd failed!\n");
        return;
    }

    for (i = 0; i < 30; i++) {
        if (tagFile(fd, key, val, 3) != 1) {
            printf(1, "%d time tag file failed\n", i);
            return;
        }
        if (removeFileTag(fd, key) != 1) {
            printf(1, "%d time remove tag failed\n", i);
            return;
        }

    }

    printf(1, "PASSED!\n");
}

void testgettag() {
    printf(1, "test getTag...");
    int fd;
    char *key="testkey", val[18];
    fd = open("tag", O_CREATE | O_RDWR);

    if (tagFile(fd, "testkey", "testval", 7) != 1) {
        printf(1, "tag file failed!\n");
        return;
    }
    if (getFileTag(fd, key, val, 18) != 7) {
        printf(1, "get tag file failed!\n");
        return;
    }

    printf(1, "PASSED!\n");

}


int
main(int argc, char *argv[])
{
    int pid;

    pid = fork();
    if (pid == 0) {
        unlink("tag");
        testtag();
        unlink("tag");
        exit();
    }
    wait();

    pid = fork();
    if (pid == 0) {
        unlink("tag");
        testremovetag();
        unlink("tag");
        exit();
    }
    wait();

    pid = fork();
    if (pid == 0) {
        unlink("tag");
        testgettag();
        unlink("tag");
        exit();
    }
    wait();
    exit();
}

