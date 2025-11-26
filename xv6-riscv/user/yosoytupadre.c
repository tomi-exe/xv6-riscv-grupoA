#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("=== Prueba de llamadas al sistema ===\n");
  
  // Obtener PID actual
  int mypid = getpid();
  printf("Mi PID: %d\n", mypid);
  
  // Parte I: Probar getppid()
  int parent_pid = getppid();
  printf("PID de mi padre: %d\n", parent_pid);
  
  // Parte II: Probar getancestor()
  printf("\n=== Prueba de getancestor() ===\n");
  
  int ancestor0 = getancestor(0);
  printf("getancestor(0): %d (debería ser mi PID: %d)\n", ancestor0, mypid);
  
  int ancestor1 = getancestor(1);
  printf("getancestor(1): %d (debería ser mi padre: %d)\n", ancestor1, parent_pid);
  
  int ancestor2 = getancestor(2);
  printf("getancestor(2): %d (abuelo o -1 si no existe)\n", ancestor2);
  
  // Crear un proceso hijo para probar jerarquía
  int pid = fork();
  if(pid == 0) {
    // Proceso hijo
    printf("\n=== Desde proceso hijo ===\n");
    printf("Hijo - Mi PID: %d\n", getpid());
    printf("Hijo - Mi padre: %d\n", getppid());
    printf("Hijo - getancestor(0): %d\n", getancestor(0));
    printf("Hijo - getancestor(1): %d\n", getancestor(1));
    printf("Hijo - getancestor(2): %d\n", getancestor(2));
    exit(0);
  } else {
    // Proceso padre
    wait(0);  // Esperar al hijo
  }
  
  printf("\n=== Fin de las pruebas ===\n");
  exit(0);
}