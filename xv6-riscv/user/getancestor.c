#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc != 2) {
    printf("Uso: getancestor <numero>\n");
    printf("Ejemplo: getancestor 1\n");
    exit(1);
  }
  
  int n = atoi(argv[1]);
  int result = getancestor(n);
  
  printf("getancestor(%d) = %d\n", n, result);
  exit(0);
}