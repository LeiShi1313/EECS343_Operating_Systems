#include "types.h"
#include "stat.h"
#include "user.h"

#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   return; \
}

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
    if (tagFile(-1, "testkey", "testval", 7) != -1) {
        printf(1, "invalid fd -1 test failed!\n");
        return;
    }
    if (tagFile(1000, "testkey", "testval", 7) != -1) {
        printf(1, "invalid fd 1000 test failed!\n");
        return;
    }
    if (tagFile(fd, "1234567890", "testval", 7) != -1) {
        printf(1, "tag key more than 10 test failed!\n");
        return;
    }
    if (tagFile(fd, "testkey", "123456789012345678", 17) != 1) {
        printf(1, "tag val 17 test failed!\n");
        return;
    }
    if (tagFile(fd, "testkey", "1234567890123456789", 18) != 1) {
        printf(1, "tag val 18 test failed!\n");
        return;
    }
    if (tagFile(fd, "testkey", "1234567890123456789", 19) != -1) {
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
        char val = i+1;
        if (tagFile(fd, "testkey", &val, 1) != 1) {
            printf(1, "%d retag with different val failed!\n", i);
            return;
        }
    }
    for (i = 0; i < 15; i++) {
        char _key[] = {i+1, 0};
        if (tagFile(fd, _key, "val", 3) != 1) {
            printf(1, "tag with different key %d failed!\n", i);
            return;
        }
    }
    close(fd);
    if (tagFile(fd, "testkey", "testval", 7) != -1) {
        printf(1, "closed fd test failed!\n");
        return;
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
    int fd, res, i;
    char *key="testkey", val[18];
    fd = open("tag", O_CREATE | O_RDWR);

    if (tagFile(fd, "testkey", "testval", 7) != 1) {
        printf(1, "tag file failed!\n");
        return;
    }
    res = getFileTag(fd, key, val, 18);
    assert(res == 7);
    char *_val = "testval";
    for (i = 0; i < 7; i++) {
        assert(val[i] == _val[i]);
    }

    res = tagFile(fd, key, "12345678901234567890", 18);
    assert(res == 1);
    res = getFileTag(fd, key, val, 18);
    assert(res == 18);
    _val = "123456789012345678";
    for (i = 0; i < 18; i++) {
       assert(val[i] == _val[i]);
    } 

    res = tagFile(fd, key, "123", 3);
    assert(res == 1);
    res = removeFileTag(fd, key);
    assert(res == 1);
    res = getFileTag(fd, key, val, 18);
    assert(res == -1);

    res = tagFile(fd, key, "123", 3);
    assert(res == 1);
    res = getFileTag(fd, key, val, 18);
    assert(res == 3);
    char *_val1 = "123";
    for (i = 0; i < 3; i++) {
        assert(val[i] == _val1[i]);
    }
    res = tagFile(fd, key, "1234", 4);
    assert(res == 1);
    res = getFileTag(fd, key, val, 18);
    assert(res == 4);
    char *_val2 = "1234";
    for (i = 0; i < 4; i++) {
        assert(val[i] == _val2[i]);
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

