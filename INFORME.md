# T1 — Implementación de llamadas al sistema en xv6 (RISC‑V)

**Ramo:** Sistemas Operativos  
**Rama de entrega:** `tomi-exe_t1`  
**Repositorio (fork):** `tomi-exe/xv6-riscv-grupoA`  
**Fecha:** 28/9/25

---

## 1) Introducción y objetivos

El objetivo de esta tarea es **agregar y documentar dos llamadas al sistema** en xv6-riscv: `getppid()` y `getancestor(int n)`. 
- `getppid()` devuelve el **PID del proceso padre** del proceso llamante (o `-1` si no existe).
- `getancestor(n)` devuelve el **PID del ancestro n-ésimo** del proceso llamante, siendo `n=0` el propio proceso, `n=1` el padre, `n=2` el abuelo, etc. Si no existe tal ancestro o el argumento es inválido, retorna `-1`.

Se pide además **crear un programa de prueba** (`yosoytupadre.c`) y **documentar** el diseño, la implementación, las pruebas, los problemas encontrados y sus soluciones.

---

## 2) Entorno de trabajo

- **Windows 11** con **WSL2**.
- **Distribución:** Ubuntu 22.04.5 / 24.04 (WSL).  
- **QEMU:** versión ≥ 7.2 (recomendado 8.x).  
- **Toolchain RISC‑V bare‑metal:** `riscv64-unknown-elf-gcc` y binutils asociados.  
- **Editor:** Terminal con WSL*.  
- **Repositorio base:** `mit-pdos/xv6-riscv` (fork en `tomi-exe/xv6-riscv-grupoA`).

**Comandos útiles para registrar versiones:**
```
lsb_release -d
uname -a
qemu-system-riscv64 --version
riscv64-unknown-elf-gcc --version
```

---

## 3) Camino de una syscall en xv6 (resumen técnico)

1. **Usuario (user space)**: Se invoca `getppid()` o `getancestor(n)` desde un programa C.
2. **Stubs** (`user/usys.S`, autogenerados por `user/usys.pl`): cargan en **a7** el número de syscall y ejecutan `ecall`.
3. **Trampa de usuario**: el `ecall` transfiere control al kernel mediante `usertrap` (RISC‑V).
4. **Dispatcher** (`kernel/syscall.c`): la función `syscall()` lee a7, indexa el arreglo `syscalls[]` y llama a la función `sys_*` correspondiente.
5. **Implementación** (`kernel/sysproc.c`): se ejecuta `sys_getppid()` o `sys_getancestor()` y el valor de retorno se deposita en **a0** del trapframe para volver a user.

---

## 4) Cambios realizados (archivos y líneas clave)

### 4.1 Números de syscall
En `kernel/syscall.h` se listan los IDs de cada syscall. El último existente era `SYS_close 21`, por lo que se definieron:
```c
#define SYS_getppid      22
#define SYS_getancestor  23
```

### 4.2 Dispatcher
En `kernel/syscall.c` se declaró y enrutó:
```c
extern uint64 sys_getppid(void);
extern uint64 sys_getancestor(void);

[SYS_getppid]     sys_getppid,
[SYS_getancestor] sys_getancestor,
```

### 4.3 Prototipos kernel
En `kernel/defs.h` se agregaron los prototipos:
```c
uint64 sys_getppid(void);
uint64 sys_getancestor(void);
```

### 4.4 Implementación de las syscalls
En `kernel/sysproc.c` (al final del archivo) se implementó:

```c
uint64
sys_getppid(void)
{
  struct proc *p = myproc();
  if(p->parent == 0)
    return -1;
  return p->parent->pid;
}

uint64
sys_getancestor(void)
{
  int n;

  // En esta versión de xv6, argint es 'void' (no retorna int)
  argint(0, &n);

  if(n < 0)
    return -1;

  struct proc *p = myproc();

  if(n == 0)
    return p->pid;

  for(int i = 0; i < n; i++){
    if(p->parent == 0)
      return -1;
    p = p->parent;
  }
  return p->pid;
}
```

> **Nota sobre concurrencia:** para este laboratorio es suficiente leer `p->parent` sin tomar locks explícitos; xv6 solo altera `parent` en puntos controlados (`reparent()`), y la operación es de solo lectura. Si se exigiera estricta sincronización, podría considerarse tomar `p->lock` durante la navegación.

### 4.5 Stubs de usuario y API
- `user/usys.pl`:
  ```perl
  entry("getppid");
  entry("getancestor");
  ```
- `user/user.h`:
  ```c
  int getppid(void);
  int getancestor(int);
  ```

### 4.6 Programa de prueba
`user/yosoytupadre.c`:
```c
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
  printf("getancestor(2) = %d (abuelo o -1)\n", getancestor(2));
  printf("getancestor(10) = %d (debe ser -1)\n", getancestor(10));
  printf("getancestor(-1) = %d (debe ser -1)\n", getancestor(-1));

  int pid = fork();
  if(pid == 0){
    printf("\n=== Desde el hijo ===\n");
    int me2 = getpid();
    int p2 = getppid();
    printf("Hijo PID: %d, su PPID: %d\n", me2, p2);
    printf("Hijo getancestor(0): %d\n", getancestor(0));
    printf("Hijo getancestor(1): %d (debe ser %d)\n", getancestor(1), p2);
    printf("Hijo getancestor(2): %d (abuelo o -1)\n", getancestor(2));
    exit(0);
  } else {
    wait(0);
  }

  printf("\n=== Fin de pruebas ===\n");
  exit(0);
}
```

### 4.7 Makefile (UPROGS)
Se añadió el binario a `UPROGS`:
```
    $U/_yosoytupadre
```
> Cuidado: la **última** entrada del bloque `UPROGS` no debe terminar en `\` (backslash). Un backslash extra provocó el error:  
> `Makefile:*** recipe commences before first target.`

---

## 5) Compilación y ejecución

**Compilación y arranque:**
```
make clean
make qemu
```

**Dentro de xv6:**
```
$ yosoytupadre
```
**Salida observada (ejemplo real):**
```
=== Prueba syscalls getppid/getancestor ===
Mi PID: 4
Mi PPID (getppid): 2

-- getancestor(n) --
getancestor(0) = 4 (debe ser 4)
getancestor(1) = 2 (debe ser 2)
getancestor(2) = 1 (abuelo o -1)
getancestor(10) = -1 (debe ser -1)
getancestor(-1) = -1 (debe ser -1)

=== Desde el hijo ===
Hijo PID: 5, su PPID: 4
Hijo getancestor(0): 5
Hijo getancestor(1): 4 (debe ser 4)
Hijo getancestor(2): 2 (abuelo o -1)

=== Fin de pruebas ===
```

> Nota: el mensaje `exec $ failed` se debió a teclear `"$ yosoytupadre"` (con `$` delante) por error. El comando correcto es `yosoytupadre`.

---

## 6) Problemas encontrados y soluciones

1) **QEMU < 7.2** (en Ubuntu 22.04) → *“Need qemu version >= 7.2”*  
   **Solución:** usar Ubuntu 24.04 (QEMU 8.x de los repos) o compilar QEMU reciente desde código fuente.

2) **Toolchain incorrecto** → ensamblador x86 no reconoce instrucciones RISC‑V (`la`, `li`, `csrr`, etc.)  
   **Solución:** instalar `riscv64-unknown-elf-gcc` y binutils; forzar `TOOLPREFIX=riscv64-unknown-elf-` si fuese necesario.

3) **`argint` no retorna `int`** → error *“void value not ignored as it ought to be”* al hacer `if (argint(...) < 0)`  
   **Solución:** en esta versión de xv6, `argint` es `void`. Llamar **sin** comparar y validar con lógica propia (`if (n < 0) return -1;`).

4) **Makefile con backslash de más** → *“recipe commences before first target”*  
   **Solución:** quitar `\` en la **última** entrada de `UPROGS` (`$U/_yosoytupadre`).

5) **Autenticación GitHub** (HTTPS pide contraseña)  
   **Solución:** configurar **SSH** (clave ED25519), agregar la pública en GitHub y cambiar el remoto a:  
   `git@github.com:tomi-exe/xv6-riscv-grupoA.git`.


## 7) Conclusiones

- Se añadieron correctamente las syscalls `getppid` (22) y `getancestor` (23) en xv6-riscv, con pruebas que confirman su comportamiento esperado tanto en el proceso actual como en el hijo creado por `fork()`.
- Se documentaron los puntos críticos del pipeline de syscalls en xv6 y los principales problemas prácticos de instalación/compilación en WSL.
- El reporte y el repositorio quedan listos para evaluación, con un programa de prueba reproducible y una guía de solución de problemas.

## 8) Captura de todo corriendo
<img width="707" height="619" alt="image" src="https://github.com/user-attachments/assets/e41ec838-c09b-40f0-98b2-adb68fddd3e1" />

