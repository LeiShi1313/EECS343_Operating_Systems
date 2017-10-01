#include "types.h"
#include "stat.h"
#include "user.h"
#include "ProcessInfo.h"
#include "param.h"

// copy from kernel/proc.h
enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };
static char *statesStr[] = {
  [UNUSED]    "UNUSED",
  [EMBRYO]    "EMBRYO",
  [SLEEPING]  "SLEEP",
  [RUNNABLE]  "RUNNABLE",
  [RUNNING]   "RUNNING",
  [ZOMBIE]    "ZOMBIE"
};

int
main(void)
{
  struct ProcessInfo* processInfoTable = (struct ProcessInfo*) malloc((uint)(NPROC * sizeof(struct ProcessInfo)));
  int num = getprocs(processInfoTable);
  int i;
  for (i = 0; i < num; i++)
    printf(1, "%d  %d  %s  %d  %s\n", processInfoTable[i].pid, processInfoTable[i].ppid, statesStr[processInfoTable[i].state], processInfoTable[i].sz, processInfoTable[i].name);
  free((void *)processInfoTable);
  exit();
}
