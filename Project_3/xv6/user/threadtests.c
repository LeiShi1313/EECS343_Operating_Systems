#include "types.h"
#include "user.h"

#define PGSIZE (4096)

volatile int global = 1;

void ThreadWithoutArg(void* arg_ptr); 
void ThreadWithArg(void* arg_ptr);
void ThreadDontExit(void* arg_ptr);

int
main(int argc, char* argv[])
{
    void* stack = malloc(PGSIZE*2); // allocate 2 pages of space
    if((uint)stack % PGSIZE) {
        // make sure that stack is page-aligned
        stack = stack + (PGSIZE - (uint)stack % PGSIZE);
    }
    int* arg = NULL; 
    int clone_pid;
    int ret_pid;
   


    printf(1, "Test 1 start************************\n"); 
    clone_pid = clone(ThreadWithoutArg, arg, stack);
    clone_pid += 0;
    // the main thread of execution (aka parent process) continues executing here
    while(global != 5) {
        ; // wait for child thread to finish
    }
    printf(1, "Test 1 clone without arg, global: %d\n", global); // prints "global: 5"
   


    printf(1, "Test 2 start************************\n"); 
    arg = malloc(sizeof(int) * 2);
    arg[0] = 1; arg[1] = 2;
    clone_pid = clone(ThreadWithArg, arg, stack);
    clone_pid += 0;
    while(global != 3) {
        ; 
    }
    printf(1, "Test 2 clone with arg, global: %d\n", global);
    free(arg);
   
    

    printf(1, "Test 3 start************************\n"); 
    arg = malloc(sizeof(int) * 2);
    arg[0] = 1; arg[1] = 2;
    clone_pid = clone(ThreadWithArg, arg, stack);
    ret_pid = join(clone_pid);
    printf(1, "Test 3 join with arg, global: %d, pid: %d, cloned pid: %d\n", global, ret_pid, clone_pid);
    free(arg);



    printf(1, "Test 4 start************************\n"); 
    arg = NULL;
    clone_pid = clone(ThreadDontExit, arg, stack);
    printf(1, "clone_pid: %d\n", clone_pid); 
    ret_pid = join(clone_pid);
    printf(1, "Test 4 thread didn't explicit exit, global: %d\n", global); 
   



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
