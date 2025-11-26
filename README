## 1. Resumen / Objetivo

Impleméntense dos llamadas al sistema que permitan **quitar** y **restaurar** el permiso de **lectura** sobre un rango de páginas de usuario:

- `int mrdprotect(void *addr, int len)` — quita el permiso de lectura (limpia el bit `PTE_R`) en `len` páginas empezando en `addr`.  
- `int munrdprotect(void *addr, int len)` — restaura el permiso de lectura (pone el bit `PTE_R`) en esas páginas.

Condiciones y reglas:
- `addr` debe estar **alineada** a `PGSIZE`.  
- `len > 0`.  
- Todas las páginas del rango deben existir y ser de usuario (`PTE_V` y `PTE_U`).  
- No tocar memoria kernel (verificar `MAXVA`).  
- En caso de error devolver `-1`. En caso de éxito devolver `0`.

---

## 2. Archivos añadidos / modificados

- `kernel/rdprotect.c` — **nuevo**: implementa `mrdprotect()` y `munrdprotect()`.  
- `kernel/syscall.h` — añadidos `SYS_mrdprotect` y `SYS_munrdprotect`.  
- `kernel/syscall.c` — (se deben añadir) `extern uint64 sys_mrdprotect(void)`, `extern uint64 sys_munrdprotect(void)` y entradas en la tabla `syscalls[]`.  
- `kernel/sysproc.c` — wrappers `sys_mrdprotect` y `sys_munrdprotect`.  
- `user/usys.pl` — añadidos `mrdprotect` y `munrdprotect`.  
- `user/user.h` — prototipos `int mrdprotect(void*, int); int munrdprotect(void*, int);`.  
- `user/rdprotect_test.c` — programa de prueba (creado).

---

## 3. Código (para reproducir / revisar)

### 3.1 `kernel/rdprotect.c` (archivo completo)
```c
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"
#include "vm.h"

// Protege paginas para que no puedan leerse (quita bit PTE_R).
// addr debe estar alineado a PGSIZE.
// len es número de páginas.
int
mrdprotect(void *addr, int len)
{
  if (((uint64)addr % PGSIZE) != 0)
    return -1;
  if (len <= 0)
    return -1;

  struct proc *p = myproc();
  if (p == 0)
    return -1;

  pagetable_t pagetable = p->pagetable;
  uint64 start = (uint64)addr;
  if (start >= MAXVA)
    return -1;

  if (len > ((MAXVA - start) / PGSIZE))
    return -1;
  uint64 end = start + (uint64)len * PGSIZE;
  if (end > MAXVA)
    return -1;

  for (uint64 va = start; va < end; va += PGSIZE) {
    pte_t *pte = walk(pagetable, va, 0);
    if (pte == 0)
      return -1;
    if (!(*pte & PTE_V))
      return -1;
    if (!(*pte & PTE_U))
      return -1;
    // Sólo limpiar el bit de lectura, dejando los demás intactos
    *pte &= ~PTE_R;
  }

  // invalidar TLB para que cambios sean efectivos
  asm volatile("sfence.vma" ::: "memory");

  return 0;
}

int
munrdprotect(void *addr, int len)
{
  if (((uint64)addr % PGSIZE) != 0)
    return -1;
  if (len <= 0)
    return -1;

  struct proc *p = myproc();
  if (p == 0)
    return -1;

  pagetable_t pagetable = p->pagetable;
  uint64 start = (uint64)addr;
  if (start >= MAXVA)
    return -1;

  if (len > ((MAXVA - start) / PGSIZE))
    return -1;
  uint64 end = start + (uint64)len * PGSIZE;
  if (end > MAXVA)
    return -1;

  for (uint64 va = start; va < end; va += PGSIZE) {
    pte_t *pte = walk(pagetable, va, 0);
    if (pte == 0)
      return -1;
    if (!(*pte & PTE_V))
      return -1;
    if (!(*pte & PTE_U))
      return -1;
    // activar bit de lectura
    *pte |= PTE_R;
  }

  asm volatile("sfence.vma" ::: "memory");

  return 0;
}
```

### 3.2 Wrappers en `kernel/sysproc.c` (añadir)
```c
uint64
sys_mrdprotect(void)
{
  uint64 addr;
  int len;
  if (argaddr(0, &addr) < 0)
    return (uint64)-1;
  if (argint(1, &len) < 0)
    return (uint64)-1;
  return (uint64)mrdprotect((void*)addr, len);
}

uint64
sys_munrdprotect(void)
{
  uint64 addr;
  int len;
  if (argaddr(0, &addr) < 0)
    return (uint64)-1;
  if (argint(1, &len) < 0)
    return (uint64)-1;
  return (uint64)munrdprotect((void*)addr, len);
}
```

### 3.3 `user/rdprotect_test.c` (programa de prueba)
```c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  char *addr = sbrk(0); // current heap top
  if (sbrk(4096) < 0) {
    printf("sbrk fallo\n");
    exit(1);
  }

  addr[0] = 'Z';

  if (mrdprotect(addr, 1) < 0) {
    printf("mrdprotect falló\n");
    exit(1);
  }

  // Escritura aun permitida
  addr[0] = 'A';

  // Intento de lectura — en xv6 la lectura debería provocar una excepción y matar el proceso.
  char c = addr[0];
  printf("Valor leído: %c (esto NO debería imprimirse si la protección funciona)\n", c);

  // Revertir
  if (munrdprotect(addr, 1) < 0) {
    printf("munrdprotect falló\n");
    exit(1);
  }

  printf("Protección revertida correctamente.\n");
  exit(0);
}
```

### 3.4 Línea añadida en `kernel/syscall.h`
```c
/* T3: syscalls mrdprotect / munrdprotect */
#define SYS_mrdprotect 24
#define SYS_munrdprotect 25
```
> **Atención:** los números pueden variar si tu lista previa es distinta. Verifica `kernel/syscall.h` antes de aceptar estos números.

---

## 4. Compilación y ejecución (comandos)

```bash
# entrar al repo
cd ~/xv6-riscv

# crear rama de trabajo (opcional)
git checkout -b tomi_t3_rdprotect

# regenerar stubs userland (si aplica)
make

# compilar
make

# arrancar qemu
make qemu
# en el shell de xv6 que aparece:
rdprotect_test
```

---

## 5. Resultado observado (salida registrada)

Ejemplo real observado en consola al ejecutar `rdprotect_test`:

```
xv6 kernel is booting
hart 1 starting
hart 2 starting
init: starting sh
$ rdprotect_test
usertrap(): unexpected scause 0xf pid=3
 sepc=0x30 stval=0x4000
```

- `stval=0x4000` corresponde a la dirección de la página protegida (coherente con el test).  
- `scause` 0xf indica una excepción generada por el acceso — es necesario decodificar `scause` con `usertrap()` para confirmar si es *load access fault* o si hay otro problema.

---

## 6. Diagnóstico y análisis

Posibles causas del `usertrap(): unexpected scause`:

1. **Máscara/PTE mal aplicada:** se borraron bits distintos a `PTE_R` (por ejemplo `PTE_V` o `PTE_U`) dejando el PTE inválido o inaccesible.  
2. **Falta de invalidación de TLB:** sin `sfence.vma`, harts pueden usar entradas antiguas/incorrectas.  
3. **Modificación sobre la pagetable equivocada.**  
4. **`usertrap()` no decodifica correctamente `scause`** y lo marca como `unexpected`.  
5. **Test usuario hace otra operación (store vs load),** provocando otro tipo de fault.

---

## 7. Depuración recomendada (pasos detallados)

1. **Instrumentar `kernel/rdprotect.c`** con `cprintf` para imprimir PTE antes y después de cada cambio:
```c
cprintf("mrdprotect: va %p pte before 0x%p\n", (void*)va, (void*)*pte);
*pte &= ~PTE_R;
cprintf("mrdprotect: va %p pte after  0x%p\n", (void*)va, (void*)*pte);
```

2. **Decodificar `scause` en `usertrap()`** añadiendo:
```c
cprintf("usertrap(): scause=%lx stval=%lx sepc=%lx pid=%d\n", r_scause(), r_stval(), r_sepc(), myproc()->pid);
```

3. **Asegurar `sfence.vma`** tras modif. (ya incluido con `asm volatile("sfence.vma" ::: "memory");`).  
4. **Probar lectura aislada** en el test (no escribir después del `mrdprotect`) para identificar la instrucción que falla.  
5. **Corregir** la manipulación de `*pte` si se observa corrupción.

---

## 8. Casos borde cubiertos

- `addr` no alineada → devuelve `-1`.  
- `len <= 0` → devuelve `-1`.  
- Rango que excede `MAXVA` → devuelve `-1`.  
- Si cualquier página en el rango no está mapeada (`walk()` devuelve NULL) o no es de usuario → devuelve `-1`.

---

## 9. Tests realizados y su interpretación

- `rdprotect_test`: reserva una página, escribe, protege, intenta leer. Resultado: proceso genera un trap que indica que la protección tuvo efecto pero requiere depuración para confirmar que sólo `PTE_R` fue alterado.

---

## 13. Anexo: salida observada
