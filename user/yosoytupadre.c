#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char **argv)
{
  printf("=== Prueba syscalls getppid/getancestor ===\n");

  int me = getpid();
  int papa = getppid();

  printf("Mi PID: %d\n", me);
  printf("Mi PPID (getppid): %d\n", papa);

  printf("\n-- getancestor(n) --\n");
  printf("getancestor(0) = %d (debe ser %d)\n", getancestor(0), me);
  printf("getancestor(1) = %d (debe ser %d)\n", getancestor(1), papa);
  printf("getancestor(2) = %d (abuelo = 1 aquí; -1 si no existiera)\n", getancestor(2));
  printf("getancestor(10) = %d (debe ser -1)\n", getancestor(10));
  printf("getancestor(-1) = %d (debe ser -1)\n", getancestor(-1));

  int pid = fork();
  if(pid == 0){
    // ----- BLOQUE DEL HIJO -----
    printf("\n=== Desde el hijo ===\n");
    int me2 = getpid();
    int p2 = getppid();
    printf("Hijo PID: %d, su PPID: %d\n", me2, p2);
    printf("Hijo getancestor(0): %d\n", getancestor(0));
    printf("Hijo getancestor(1): %d (debe ser %d)\n", getancestor(1), p2);
    printf("Hijo getancestor(2): %d (abuelo = 2 aquí; -1 si no existiera)\n", getancestor(2));
    printf("Hijo getancestor(3): %d (bisabuelo = 1 aquí; -1 si no existiera)\n", getancestor(3));
    // ----- FIN BLOQUE DEL HIJO -----
    exit(0);
  } else {
    wait(0);
  }

  printf("\n=== Fin de pruebas ===\n");
  exit(0);
}
