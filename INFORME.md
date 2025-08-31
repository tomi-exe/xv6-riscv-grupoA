# INFORME T0 – Instalación de xv6

## Pasos seguidos
1. Instalé WSL2 en Windows 11.
2. Instalé Ubuntu (22.04 inicialmente, luego 24.04).
3. Instalé dependencias: `build-essential`, `gdb-multiarch`, `qemu-system-misc`.
4. Instalé toolchain RISC-V (`gcc-riscv64-unknown-elf`).
5. Cloné el repositorio xv6-riscv.
6. Compilé y ejecuté con `make qemu`.

## Problemas encontrados y soluciones
- **Problema:** Versión de QEMU (6.2) demasiado baja.  
  **Solución:** Migré a Ubuntu 24.04 que trae QEMU 8.x.
- **Problema:** `gcc` compilaba en x86.  
  **Solución:** Instalé `riscv64-unknown-elf-gcc` y forcé `TOOLPREFIX`.

## Confirmación de funcionamiento
- xv6 arranca correctamente en QEMU.
- Probados comandos: `ls`, `echo "Hola xv6"`, `cat README`.
  
<img width="739" height="1020" alt="Captura de pantalla 2025-08-31 164939" src="https://github.com/user-attachments/assets/cc4a9ecd-bac6-4f0d-9f38-a0eff3679389" />
