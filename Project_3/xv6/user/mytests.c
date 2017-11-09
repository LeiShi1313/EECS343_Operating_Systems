/* clone and play with the argument */
#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "x86.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

int ppid;
volatile int arg = 55;
volatile int global = 1;
volatile uint newfd = 0;
unsigned int size = 0;
lock_t lock, lock2;
int num_threads = 30;
int loops = 1000;
cond_t cond, cond1, cond2;
int result = 0;
cond_t nonfull, nonempty;
int bufsize = 0;
int N = 1;

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

void worker(void *arg_ptr);
void clone_worker(void *arg_ptr);
void clone2_worker(void *arg_ptr);
void clone3_worker(void *arg_ptr);
void join_worker(void *arg_ptr);
void join2_worker(void *arg_ptr);
void join4_worker(void *arg_ptr);
void thread_worker(void *arg_ptr);
void thread2_worker(void *arg_ptr);
void thread3_worker(void *arg_ptr);
void noexit_worker(void *arg_ptr);
void stack_worker(void *arg_ptr);
void size_worker(void *arg_ptr);
void multi_worker(void *arg_ptr);
void locks_worker(void *arg_ptr);
void cond_worker(void *arg_ptr);
void cond2_worker(void *arg_ptr);
void cond3_worker(void *arg_ptr);
void cond4_worker(void *arg_ptr);
void produce(void *arg);
void consume(void *arg);




unsigned int fib(unsigned int n) {
   if (n == 0) {
      return 0;
   } else if (n == 1) {
      return 1;
   } else {
      return fib(n - 1) + fib(n - 2);
   }
}

int
main(int argc, char *argv[])
{
    ppid = getpid();
    void *stack;
    int pid, clone_pid, fd, arg, new_thread_pid, join_pid, fork_pid;
    int i, thread_pid, pid1, pid2;
       


    pid = fork();
    if (pid == 0) {
        printf(1, "test badclone ");
        stack = malloc(PGSIZE*2);
        assert(stack != NULL);
        if((uint)stack % PGSIZE == 0)
          stack += 4;

        assert(clone(worker, 0, stack) == -1);  // when stack is not page-aligned, clone should fail

        stack = sbrk(0);
        if((uint)stack % PGSIZE){
          stack = stack + (PGSIZE - (uint)stack % PGSIZE);
        }
        sbrk( ((uint)stack - (uint)sbrk(0)) + PGSIZE/2 );
        assert((uint)stack % PGSIZE == 0);
        assert((uint)sbrk(0) - (uint)stack == PGSIZE/2);
        assert(clone(worker, 0, stack) == -1);
        free(stack);
        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();



    pid = fork();
    if (pid == 0) {
        printf(1, "test clone "); 
        global = 1;
        stack = malloc(PGSIZE*2);
        assert(stack != NULL);
        if((uint)stack % PGSIZE) {
          stack = stack + (PGSIZE - (uint)stack % PGSIZE);
        }
 
        clone_pid = clone(clone_worker, 0, stack);
        assert(clone_pid > 0);
        while(global != 5) {
           ; // wait for thread worker to change global
        }
        assert(global == 5)
        free(stack);
        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();
 
 
    pid = fork();
    if (pid == 0) {
        printf(1, "test clone2 "); 
        global = 1;
        arg = 55;
        stack = malloc(PGSIZE*2);
        assert(stack != NULL);
        if((uint)stack % PGSIZE)
          stack = stack + (4096 - (uint)stack % PGSIZE);
 
        clone_pid = clone(clone2_worker, (void*)&arg, stack);
        assert(clone_pid > 0);
        while(global != 55);
        assert(arg == 1);
        free(stack);
        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();
 
 
    pid = fork();
    if (pid == 0) {
        printf(1, "test clone3 ");
        newfd = 0;
        stack = malloc(PGSIZE*2);
        assert(stack != NULL);
        if((uint)stack % PGSIZE)
          stack = stack + (4096 - (uint)stack % PGSIZE);
 
        fd = open("tmp", O_WRONLY|O_CREATE);
        assert(fd == 3);
        clone_pid = clone(clone3_worker, 0, stack);
        assert(clone_pid > 0);
        while(!newfd);
        assert(write(newfd, "goodbye\n", 8) == -1);
        free(stack);
        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();
 

    pid = fork();
    if (pid == 0) {
        printf(1, "test join ");
        global = 1;
        stack = malloc(PGSIZE*2);
        assert(stack != NULL);
        if((uint)stack % PGSIZE)
          stack = stack + (PGSIZE - (uint)stack % PGSIZE);

        arg = 42;
        new_thread_pid = clone(join_worker, &arg, stack);
        assert(new_thread_pid > 0);

        join_pid = join(new_thread_pid);
        assert(join_pid == new_thread_pid);
        assert(global == 2);
        free(stack);
        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();
 
 
    pid = fork();
    if (pid == 0) {
        printf(1, "test join2 ");
        global = 1;
        stack = malloc(PGSIZE*2);
        assert(stack != NULL);
        if((uint)stack % PGSIZE)
          stack = stack + (4096 - (uint)stack % PGSIZE);

        arg = 42;
        clone_pid = clone(join2_worker, &arg, stack);
        assert(clone_pid > 0);

        sbrk(PGSIZE);
        assert(join(clone_pid) == clone_pid);
        assert(global == 2);
        free(stack);
        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();



    pid = fork();
    if (pid == 0) {
        printf(1, "test join3 ");
        fork_pid = fork();
        if(fork_pid == 0) {
          exit();
        }
        assert(fork_pid > 0);

        join_pid = join(fork_pid);
        assert(join_pid == -1);
        wait();
        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();


    pid = fork();
    if (pid == 0) {
        printf(1, "test join4 ");
        global = 1;
        stack = malloc(PGSIZE*2);
        assert(stack != NULL);
        if((uint)stack % PGSIZE)
          stack = stack + (4096 - (uint)stack % PGSIZE);
 
        arg = 42;
        clone_pid = clone(join4_worker, &arg, stack);
        assert(clone_pid > 0);
 
        sleep(250);
        assert(wait() == -1);
 
        join_pid = join(clone_pid);
        assert(join_pid == clone_pid);
        assert(global == 2);
        free(stack); 
        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();
     


    pid = fork();
    if (pid == 0) {
        printf(1, "test thread ");
        global = 1;
        arg = 35;
        thread_pid = thread_create(thread_worker, &arg);
        assert(thread_pid > 0);

        join_pid = thread_join(thread_pid);
        assert(join_pid == thread_pid);
        assert(global == 2);

        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();


    pid = fork();
    if (pid == 0) {
        printf(1, "test thread2 ");
        for(i = 0; i < 2000; i++) {
            global = 1;
            thread_pid = thread_create(thread2_worker, 0);
            assert(thread_pid > 0);
            join_pid = thread_join(thread_pid);
            assert(join_pid == thread_pid);
            assert(global == 5);
            assert((uint)sbrk(0) < (150 * 4096) && "shouldn't even come close");
        }

        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();


    pid = fork();
    if (pid == 0) {
        printf(1, "test thread3 ");
        arg = 35;
        thread_pid = thread_create(thread3_worker, &arg);
        assert(thread_pid > 0);

        join_pid = thread_join(thread_pid);
        assert(join_pid == thread_pid);

        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();


    pid = fork();
    if (pid == 0) {
        printf(1, "test noexit ");
        arg = 42;
        thread_pid = thread_create(noexit_worker, &arg);
        assert(thread_pid > 0);

        join_pid = thread_join(thread_pid);
        assert(join_pid == thread_pid);
        assert(global == 2);

        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();

    pid = fork();
    if (pid == 0) {
        printf(1, "test stack ");
        for (i = 0; i < 1000; i++) {
          stack = malloc(PGSIZE*2);
          assert(stack != NULL);
          if((uint)stack % PGSIZE)
            stack = stack + (4096 - (uint)stack % PGSIZE);

          clone_pid = clone(stack_worker, stack, stack);
          assert(clone_pid > 0);
          thread_pid = join(clone_pid);
          assert(thread_pid == clone_pid);
          free(stack);
        }
        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();

    pid = fork();
    if (pid == 0) {
        printf(1, "test size ");
        int arg = 101;

        lock_init(&lock);
        lock_init(&lock2);
        lock_acquire(&lock);
        lock_acquire(&lock2);

        for (i = 0; i < num_threads; i++) {
          thread_pid = thread_create(size_worker, &arg);
          assert(thread_pid > 0);
        }

        size = (unsigned int)sbrk(0);

        while (global < num_threads) {
           lock_release(&lock);
           sleep(100);
           lock_acquire(&lock);
        }

        global = 0;
        sbrk(10000);
        size = (unsigned int)sbrk(0);
        lock_release(&lock);

        while (global < num_threads) {
           lock_release(&lock2);
           sleep(100);
           lock_acquire(&lock2);
        }
        lock_release(&lock2);


        join_pid = thread_join(thread_pid);
        assert(join_pid > 0);

        printf(1, "TEST PASSED\n");

        exit();
    }
    wait();

    pid = fork();
    if (pid == 0) {
        printf(1, "test multi ");
        assert(fib(28) == 317811);

        arg = 101;
        for (i = 0; i < num_threads; i++) {
           thread_pid = thread_create(multi_worker, &arg);
           assert(thread_pid > 0);
           join_pid = thread_join(thread_pid);
           assert(join_pid > 0);
        }
        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();


    pid = fork();
    if (pid == 0) {
        printf(1, "test locks ");
        global = 0;
        lock_init(&lock);

        for (i = 0; i < num_threads; i++) {
           thread_pid = thread_create(locks_worker, 0);
           assert(thread_pid > 0);
           join_pid = thread_join(thread_pid);
           assert(join_pid > 0);
        }
        assert(global == num_threads * loops);
        printf(1, "TEST PASSED\n");

        exit();
    }
    wait();


    pid = fork();
    if (pid == 0) {
        printf(1, "test cond ");
        global = 0;
        lock_init(&lock);
        thread_pid = thread_create(cond_worker, 0);
        assert(thread_pid > 0);
     
        sleep(20);
        lock_acquire(&lock);
        global = 2;
        cv_signal(&cond);
        sleep(50);
        global = 1;
        lock_release(&lock);
     
        join_pid = thread_join(thread_pid);
        assert(join_pid == thread_pid);
     
        printf(1, "TEST PASSED\n");

        exit();
    }
    wait();


    pid = fork();
    if (pid == 0) {
        printf(1, "test cond2 ");
        global = 0;
        lock_init(&lock);

        thread_pid = thread_create(cond2_worker, 0);
        assert(thread_pid > 0);

        sleep(50);
        lock_acquire(&lock);
        global = 2;
        cv_signal(&cond2);
        lock_release(&lock);

        sleep(50);
        lock_acquire(&lock);
        global = 1;
        cv_signal(&cond1);
        lock_release(&lock);

        join_pid = thread_join(thread_pid);
        assert(join_pid == thread_pid);

        printf(1, "TEST PASSED\n");
        exit();

        exit();
    }
    wait();



    pid = fork();
    if (pid == 0) {
        printf(1, "test cond3 ");
        global = 0;
        lock_init(&lock);

        for(i = 0; i < num_threads; i++) {
          thread_pid = thread_create(cond3_worker, 0);
          assert(thread_pid > 0);

          sleep(50);

          lock_acquire(&lock);
          assert(global == i);
          cv_signal(&cond);
          lock_release(&lock);

          join_pid = thread_join(thread_pid);
          assert(join_pid > 0);

          sleep(10);
          lock_acquire(&lock);
          assert(global == i+1);
          lock_release(&lock);
        }
        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();

    pid = fork();
    if (pid == 0) {
        printf(1, "test cond4 ");
        global = 0;
        lock_init(&lock);
        int thread_pids[num_threads];

        for(i = 0; i < num_threads; i++) {
          thread_pids[i] = thread_create(cond4_worker, 0);
          assert(thread_pids[i] > 0);
        }

        sleep(50);

        lock_acquire(&lock);
        assert(global == 0);
          cv_signal(&cond);
        lock_release(&lock);

        for(i = 0; i < num_threads; i++) {
          join_pid = thread_join(thread_pids[i]);
          assert(join_pid > 0);
        }

        sleep(10);
        lock_acquire(&lock);
        assert(global == num_threads);
        lock_release(&lock);
        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();

    pid = fork();
    if (pid == 0) {
        printf(1, "test race ");
        lock_init(&lock);
        bufsize = 0;
        assert((pid1 = thread_create(produce, NULL)) > 0);
        assert((pid2 = thread_create(consume, NULL)) > 0);
        thread_join(pid1);
        thread_join(pid2);
        assert(result == loops);  
        printf(1, "TEST PASSED\n");
        exit();
    }
    wait();
    exit();
}

void
worker(void *arg_ptr) {
   exit();
}
void
clone_worker(void *arg_ptr) {
   assert(global == 1);
   global = 5;
   exit();
}
void
clone2_worker(void *arg_ptr) {
   int tmp = *(int*)arg_ptr;
   *(int*)arg_ptr = 1;
   assert(global == 1);
   global = tmp;
   exit();
}
void
clone3_worker(void *arg_ptr) {
   assert(write(3, "hello\n", 6) == 6);
   xchg(&newfd, open("tmp2", O_WRONLY|O_CREATE));
   exit();
}
void
join_worker(void *arg_ptr) {
   int arg = *(int*)arg_ptr;
   assert(arg == 42);
   assert(global == 1);
   global++;
   exit();
}
void
join2_worker(void *arg_ptr) {
   int arg = *(int*)arg_ptr;
   assert(arg == 42);
   assert(global == 1);
   global++;
   exit();
}
void
join4_worker(void *arg_ptr) {
   int arg = *(int*)arg_ptr;
   assert(arg == 42);
   assert(global == 1);
   global++;
   exit();
}
void
thread_worker(void *arg_ptr) {
   int arg = *(int*)arg_ptr;
   assert(arg == 35);
   assert(global == 1);
   global++;
}
void
thread2_worker(void *arg_ptr) {
   assert(global == 1);
   global+=4;
   exit();
}
void
thread3_worker(void *arg_ptr) {
    exit();
}
void
noexit_worker(void *arg_ptr) {
   int arg = *(int*)arg_ptr;
   assert(arg == 42);
   assert(global == 1);
   global++;
   // no exit() in thread
}
void
stack_worker(void *arg_ptr) {
   assert((uint)&arg_ptr == ((uint)arg_ptr + PGSIZE - 4));
   assert(*((uint*) (arg_ptr + PGSIZE - 8)) == 0xffffffff);
   global = 5;
   exit();
}
void
size_worker(void *arg_ptr) {
   lock_acquire(&lock);
   assert((unsigned int)sbrk(0) == size);
   global++;
   lock_release(&lock);

   lock_acquire(&lock2);
   assert((unsigned int)sbrk(0) == size);
   global++;
   lock_release(&lock2);

   exit();
}

void
multi_worker(void *arg_ptr) {
   int arg = *(int*)arg_ptr;
   assert(arg == 101);
   assert(global == 1);
   assert(fib(2) == 1);
   assert(fib(3) == 2);
   assert(fib(9) == 34);
   assert(fib(15) == 610);
   exit();
}

void
locks_worker(void *arg_ptr) {
   int i, j, tmp;
   for (i = 0; i < loops; i++) {
      lock_acquire(&lock);
      tmp = global;
      for(j = 0; j < 50; j++); // take some time
      global = tmp + 1;
      lock_release(&lock);
   }
   exit();
}
void
cond_worker(void *arg_ptr) {
  lock_acquire(&lock);
  assert(global == 0);
  cv_wait(&cond, &lock);
  assert(global == 1);
  lock_release(&lock);
  exit();
}
void
cond2_worker(void *arg_ptr) {
  lock_acquire(&lock);
  assert(global == 0);
  cv_wait(&cond1, &lock);
  assert(global == 1);
  lock_release(&lock);
  exit();
}
void
cond3_worker(void *arg_ptr) {
  lock_acquire(&lock);
  cv_wait(&cond, &lock);
  global++;
  lock_release(&lock);
  exit();
}
void
cond4_worker(void *arg_ptr) {
  lock_acquire(&lock);
  cv_wait(&cond, &lock);
  global++;
  lock_release(&lock);
  exit();
}

void
produce(void *arg) {
  while(result < loops) {
    lock_acquire(&lock);
    while(bufsize == N) {
      cv_wait(&nonfull, &lock);
    }

    bufsize++;
    cv_signal(&nonempty);
    lock_release(&lock);
  }
  exit();
}

void
consume(void *arg) {
  while(1) {
    lock_acquire(&lock);
    while(bufsize == 0) {
      cv_wait(&nonempty, &lock);
    }
    result++;
    bufsize--;
    cv_signal(&nonfull);
    lock_release(&lock);
    if (result == loops)
        exit();
  }
}


