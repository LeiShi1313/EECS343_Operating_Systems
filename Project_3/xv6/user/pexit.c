/* race condition in CV? (must atomically release lock and sleep) */
#include "types.h"
#include "user.h"
#include "x86.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

#define N 1
int ppid;
int pid1, pid2;
volatile int bufsize;
int loops = 1000;
int result = 0;
lock_t lock;
cond_t nonfull, nonempty;

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

void infi(void *arg);

int
main(int argc, char *argv[])
{
  ppid = getpid();

  lock_init(&lock);
  bufsize = 0;

  assert((pid1 = thread_create(infi, NULL)) > 0);
  assert((pid2 = thread_create(infi, NULL)) > 0);
  printf(1, "child: %d, child %d\n", pid1, pid2);

  sleep(10);

  exit();
}

void
infi(void *arg) {
  while (1) ;
}
