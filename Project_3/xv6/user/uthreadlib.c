#include "types.h"
#include "user.h"
#include "x86.h"

#define PGSIZE 4096


void
thread_wrapper(void *arg)
{
  struct thread_obj *obj = (struct thread_obj*)arg;
  (obj->func)(obj->arg);
  free(obj);
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
  return clone(thread_wrapper, obj, stack);
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
  // if (lock->locked) {
  //   printf(1, "lock have been acuqired!");
  //   return;
  // }
  while(xchg(&lock->locked, 1) != 0) ;
}

void
lock_release(lock_t* lock)
{
  // if (!lock->locked) {
  //   printf(1, "lock have been released!");
  //   return;
  // }
  xchg(&lock->locked, 0);
}

void 
cv_wait(cond_t* conditionVariable, lock_t* lock)
{
  while(xchg(conditionVariable, 0) == 0) {
    printf(1, "wait on 0x%x----%d, lock: %d\n", conditionVariable, *conditionVariable, lock->locked);
    lock_release(lock);
    csleep(conditionVariable);
  }
  printf(1, "end wait\n");
  lock_acquire(lock);
}

void 
cv_signal(cond_t* conditionVariable)
{
  printf(1, "wake on 0x%x----%d\n", conditionVariable, *conditionVariable);
  cwake(conditionVariable);
  xchg(conditionVariable, 1);
}
