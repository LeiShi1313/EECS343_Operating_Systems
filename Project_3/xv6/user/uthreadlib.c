#include "types.h"
#include "user.h"
#include "x86.h"

#define PGSIZE 4096


void
thread_wrapper(void *arg)
{
  struct thread_obj *obj = (struct thread_obj*)arg;
  (obj->func)(obj->arg);
  exit();
}


int
thread_create(void (*start_routine)(void*), void* arg)
{
  void *stack = malloc(PGSIZE * 2);
  if (!stack) return -1;
  if ((uint)stack % PGSIZE) {
    stack += PGSIZE - (uint)stack % PGSIZE;
  }
  struct thread_obj *obj = malloc(sizeof(struct thread_obj));
  obj->func = start_routine;
  obj->arg = arg;
  int ret_pid = clone(thread_wrapper, &obj, stack);
  free(obj);
  return ret_pid;
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

void 
cv_wait(cond_t* conditionVariable, lock_t* lock)
{
    ;
}

void 
cv_signal(cond_t* conditionVariable)
{
    ;
}
