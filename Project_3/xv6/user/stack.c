/* set up stack correctly (and without extra items) */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

int ppid;
#define PGSIZE (4096)

volatile int global = 1;

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
   void *stack;
   int i;
   for (i = 0; i < 1000; i++) {
     stack = malloc(PGSIZE*2);
     assert(stack != NULL);
     if((uint)stack % PGSIZE)
       stack = stack + (4096 - (uint)stack % PGSIZE);

     int clone_pid = clone(worker, stack, stack);
     assert(clone_pid > 0);
     int ret_pid = join(clone_pid);
     assert(ret_pid == clone_pid);
     printf(1, "TEST PASSED\n");
     free(stack);
   }
   exit();
}

void
worker(void *arg_ptr) {
   assert((uint)&arg_ptr == ((uint)arg_ptr + PGSIZE - 4));
   assert(*((uint*) (arg_ptr + PGSIZE - 8)) == 0xffffffff);
   global = 5;
   exit();
}
