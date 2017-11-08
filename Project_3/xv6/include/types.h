#ifndef _TYPES_H_
#define _TYPES_H_

// Type definitions

typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;

typedef struct lock_t {
  uint locked;
}lock_t;

typedef struct cond_t {
  uint locked;
}cond_t;

struct thread_obj {
  void *a;
  void *b;
  void (*func)(void*);
  void *c;
  void *d;
  void *arg;
};

#ifndef NULL
#define NULL (0)
#endif

#endif //_TYPES_H_
