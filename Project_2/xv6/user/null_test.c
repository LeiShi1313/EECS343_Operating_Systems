#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
    printf(1, "Read data from address 0x0: %p\n", *((int*)0));
    exit();
}
