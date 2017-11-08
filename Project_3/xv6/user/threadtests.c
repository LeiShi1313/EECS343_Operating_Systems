#include "types.h"
#include "user.h"

#define PGSIZE (4096)

volatile int global = 1;

void ThreadWithoutArg(void* arg_ptr); 
void ThreadWithArg(void* arg_ptr);
void ThreadDontExit(void* arg_ptr);
void ThreadIncre(void* arg_ptr);
void ThreadSpawnThread(void* arg_ptr);

void 
getStack(void **stack) {
    *stack = malloc(PGSIZE * 2);
    if ((uint)*stack % PGSIZE) {
        *stack = *stack + (PGSIZE - (uint)*stack % PGSIZE);
    }
}


int
main(int argc, char* argv[])
{
    void* stack = NULL; // allocate 2 pages of space
    int* arg = NULL; 
    int clone_pid;
    int ret_pid;
   


    printf(1, "Test 1 start************************\n"); 
    getStack(&stack);
    clone_pid = clone(ThreadWithoutArg, arg, stack);
    clone_pid += 0;
    // the main thread of execution (aka parent process) continues executing here
    while(global != 5) {
        ; // wait for child thread to finish
    }
    free(stack);
    printf(1, "Test 1 clone without arg, global: %d\n", global); // prints "global: 5"
   


    printf(1, "Test 2 start************************\n"); 
    getStack(&stack);
    arg = malloc(sizeof(int) * 2);
    arg[0] = 1; arg[1] = 2;
    clone_pid = clone(ThreadWithArg, arg, stack);
    clone_pid += 0;
    while(global != 3) {
        ; 
    }
    free(stack);
    free(arg);
    printf(1, "Test 2 clone with arg, global: %d\n", global);
   
    

    printf(1, "Test 3 start************************\n"); 
    getStack(&stack);
    arg = malloc(sizeof(int) * 2);
    arg[0] = 1; arg[1] = 2;
    clone_pid = clone(ThreadWithArg, arg, stack);
    ret_pid = join(clone_pid);
    free(stack);
    free(arg);
    printf(1, "Test 3 join with arg, global: %d, pid: %d, cloned pid: %d\n", global, ret_pid, clone_pid);



    printf(1, "Test 4 start************************\n"); 
    getStack(&stack);
    arg = NULL;
    clone_pid = clone(ThreadDontExit, arg, stack);
    printf(1, "clone_pid: %d\n", clone_pid); 
    ret_pid = join(clone_pid);
    free(stack);
    printf(1, "Test 4 thread didn't explicit exit, global: %d\n", global); 
   



    printf(1, "Test 5 start************************\n"); 
    getStack(&stack);
    arg = NULL;
    int clone_pid0 = clone(ThreadIncre, arg, stack);
    printf(1, "clone_pid: %d\t", clone_pid0); 
    int clone_pid1 = clone(ThreadIncre, arg, stack);
    printf(1, "clone_pid: %d\t", clone_pid1); 
    int clone_pid2 = clone(ThreadIncre, arg, stack);
    printf(1, "clone_pid: %d\n", clone_pid2); 
    ret_pid = join(clone_pid0);
    ret_pid = join(clone_pid1);
    ret_pid = join(clone_pid2);
    free(stack);
    printf(1, "Test 5 3 threads, global: %d\n", global); 




    printf(1, "Test 6 start************************\n"); 
    global = 10;
    sbrk(PGSIZE*global*2);
    stack = malloc(PGSIZE*2);
    if ((uint)stack % PGSIZE) {
        stack = stack + (PGSIZE - (uint)stack % PGSIZE);
    }
    arg = malloc(sizeof(int));
    *arg = 0;
    clone_pid = clone(ThreadSpawnThread, arg, stack);
    ret_pid = join(clone_pid);
    free(stack);
    free(arg);
    printf(1, "Test 6 thread spawn thread, global: %d, arg: %d\n", global, *arg); 



    exit();
}

void
ThreadWithoutArg(void* arg_ptr) {
    global = 5;
    exit();
}
void
ThreadWithArg(void* arg_ptr) {
    global = *((int*)arg_ptr) + *((int*)arg_ptr+1);
    exit();
}
void
ThreadDontExit(void* arg_ptr) {
    global = 5;
}
void
ThreadIncre(void* arg_ptr) {
    global++;
    exit();
}
void
ThreadSpawnThread(void* arg_ptr) {
    global--;
    if (global) {
        void *stack = malloc(PGSIZE*2);
        printf(1, "Stack: %x\n", (uint)stack);
        if (stack == 0) exit();
        if ((uint)stack % PGSIZE) {
            stack = stack + (PGSIZE - (uint)stack % PGSIZE);
        }
        printf(1, "Stack: %x\n", (uint)stack);
        int clone_pid = clone(ThreadSpawnThread, arg_ptr, stack);
        printf(1, "Spawned pid: %d\n", clone_pid);
        *((int*) arg_ptr) += clone_pid;
        join(clone_pid);
        free(stack);
    }
    exit();
}
