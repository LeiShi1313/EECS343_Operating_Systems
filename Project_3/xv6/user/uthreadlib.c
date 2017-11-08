#include "types.h"
#include "user.h"
#include "x86.h"

#define PGSIZE 4096


void
thread_wrapper(void *arg)
{
  printf(1, "wrapper---arg addr: %x\n", arg);
  struct thread_obj *obj = (struct thread_obj*)arg;
  printf(1, "wrapper---arg addr: %x\n", obj->arg);
  printf(1, "wrapper---func addr: %x\n", obj->func);
  printf(1, "wrapper---func size: %d\n", sizeof(obj->func));
  printf(1, "wrapper---a addr: %x\n", obj->a);
  printf(1, "wrapper---b addr: %x\n", obj->b);
  printf(1, "wrapper---c addr: %x\n", obj->c);
  printf(1, "wrapper---d addr: %x\n", obj->d);
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
  printf(1, "create---arg addr: %x\n",arg);
  struct thread_obj obj;
  printf(1, "create---obj addr: %x\n", &obj);
  obj.func = start_routine;
  obj.arg = arg;
  obj.c = arg;
  obj.d = arg;
  printf(1, "create---obj.arg addr: %x\n", obj.arg);
  printf(1, "create---obj.func addr: %x\n", obj.func);
  printf(1, "create---a addr: %x\n", obj.a);
  printf(1, "create---b addr: %x\n", obj.b);
  printf(1, "create---c addr: %x\n", obj.c);
  printf(1, "create---d addr: %x\n", obj.d);
  return clone(thread_wrapper, &obj, stack);
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
