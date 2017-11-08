#include "types.h"
#include "user.h"
#include "x86.h"

#define PGSIZE 4096

int
thread_create(void (*start_routine)(void*), void* arg)
{
  void *stack = malloc(PGSIZE * 2);
  if (!stack) return -1;
  if ((uint)stack % PGSIZE) {
    stack += PGSIZE - (uint)stack % PGSIZE;
  }
  return clone(start_routine, arg, stack);
}

int
thread_join(int pid)
{
  int ustack = getustack(pid);
  if (ustack < 0) return -1;
  int ret_pid = join(pid);
  free((void*)ustack);
  return ret_pid;
}

void
lock_init(lock_t* lock)
{
  lock->locked = 0;
}

void
lock_acquire(lock_t* lock)
{
  while(xchg(&lock->locked, 1) != 0) ;
}

void
lock_release(lock_t* lock)
{
  xchg(&lock->locked, 0);
}
