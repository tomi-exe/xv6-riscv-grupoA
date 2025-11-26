#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("Mi PID: %d\n", getpid());
  printf("PID de mi padre: %d\n", getppid());
  exit(0);
}