/* thread user library functions */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

int ppid;

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

void worker(void *arg_ptr);

int
main(int argc, char *argv[])
{
   ppid = getpid();

   int arg = 35;
   printf(1, "arg addr: %x\n", &arg);
   int thread_pid = thread_create(worker, &arg);
   assert(thread_pid > 0);

   int join_pid = thread_join(thread_pid);
   assert(join_pid == thread_pid);

   printf(1, "TEST PASSED\n");
   exit();
}

void
worker(void *arg_ptr) {
   int arg;
   printf(1, "arg addr: %x\n", &arg);
   arg = 2;
}

