#include "types.h"
#include "user.h"

#define PGSIZE (4096)

volatile int global = 1;

void DoThreadWork(void* arg_ptr); // function signature for our DoThreadFunction
void DoThreadWork2(void* arg_ptr); // function signature for our DoThreadFunction

int
main(int argc, char* argv[])
{
 void* stack = malloc(PGSIZE*2); // allocate 2 pages of space
 if((uint)stack % PGSIZE) {
 // make sure that stack is page-aligned
 stack = stack + (PGSIZE - (uint)stack % PGSIZE);
 }

 int* arg = NULL; // our DoThreadWork function is simple and doesn't need an arg
 int clone_pid = clone(DoThreadWork, arg, stack);
 clone_pid += 0;
 // the main thread of execution (aka parent process) continues executing here
 while(global != 5) {
 ; // wait for child thread to finish
 }
 printf(1, "global: %d\n", global); // prints "global: 5"

 arg = malloc(sizeof(int) * 2);
 arg[0] = 1;
 arg[1] = 2;
 clone_pid = clone(DoThreadWork2, arg, stack);
 clone_pid += 0;
 while(global != 3) {
 ; // wait for child thread to finish
 }
 printf(1, "global: %d\n", global); // prints "global: 5"
 exit();
}

void
DoThreadWork(void* arg_ptr) {
 // clone creates a new thread, which begins execution here
 
 global = 5;
 exit();
}
void
DoThreadWork2(void* arg_ptr) {
 // clone creates a new thread, which begins execution here
 
 global = *((int*)arg_ptr) + *((int*)arg_ptr+1);
 exit();
}