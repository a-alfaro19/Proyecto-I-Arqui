# Ejemplo mínimo de aplicación en RISC-V con ensamblador y depuración en QEMU

Este documento describe el procedimiento completo para construir un entorno de ejecución y depuración de programas bare-metal en arquitectura RISC-V de 32 bits, utilizando ensamblador, QEMU y GDB, dentro de un contenedor Docker.

---

## 1. Estructura de directorios

Se asume la siguiente estructura base en el sistema de archivos:

```
.
├── Dockerfile
├── run.sh
└── workspace/
    ├── test.s
    └── linker.ld
```

- `workspace/` contiene el código fuente en ensamblador (`test.s`) y el script de enlace (`linker.ld`).
- `Dockerfile` define la imagen que incluye el emulador QEMU y el toolchain RISC-V.
- `run.sh` automatiza la construcción de la imagen y la ejecución del contenedor.

---

## 2. Construcción y ejecución inicial del contenedor

El script `run.sh` debe ejecutarse con permisos de ejecución para construir la imagen `rvqemu` y crear un contenedor interactivo que monte el directorio `workspace` en el entorno de desarrollo.

```bash
chmod +x run.sh
./run.sh
```

**Notas**:
- La construcción se realiza solo si la imagen no existe.
- El contenedor generado utiliza como directorio de trabajo `/home/rvqemu-dev/workspace`.

---

## 3. Compilación del programa

Con el contenedor en ejecución, se debe acceder al directorio de trabajo y compilar el archivo `test.s` con el siguiente comando:

```bash
cd /home/rvqemu-dev/workspace
riscv64-unknown-elf-gcc -march=rv32im -mabi=ilp32     -nostdlib -ffreestanding -g test.s -T linker.ld -o test.elf
```

**Descripción de las opciones utilizadas**:
- `-march=rv32im`: define la arquitectura de destino (RISC-V 32 bits con extensiones I y M).
- `-mabi=ilp32`: especifica el ABI ILP32.
- `-nostdlib`: evita la inclusión de librerías estándar (entorno bare-metal).
- `-ffreestanding`: indica que el programa es independiente del entorno estándar de C.
- `-g`: incluye información de depuración para GDB.
- `-T linker.ld`: utiliza un script de enlace personalizado para definir el mapa de memoria.

El resultado es un binario ejecutable en formato ELF denominado `test.elf`.

---

## 4. Ejecución en QEMU con servidor GDB

Para iniciar la emulación y habilitar la depuración remota, se debe ejecutar:

```bash
qemu-system-riscv32 -machine virt -nographic -bios none     -kernel test.elf -S -gdb tcp::1234
```

**Parámetros relevantes**:
- `-machine virt`: emula una máquina virtual genérica compatible con RISC-V.
- `-nographic`: desactiva la salida gráfica y redirige la I/O a la terminal.
- `-bios none`: desactiva el firmware y arranca directamente desde el binario.
- `-kernel test.elf`: define el binario a ejecutar.
- `-S`: detiene la CPU en el arranque, permitiendo conexión previa de GDB.
- `-gdb tcp::1234`: inicia un servidor GDB escuchando en el puerto TCP 1234.

---

## 5. Conexión de GDB

Para conectarse al servidor GDB de QEMU, se debe abrir una segunda terminal y ejecutar:

```bash
docker exec -it rvqemu /bin/bash
cd /home/rvqemu-dev/workspace
gdb-multiarch test.elf
```

Dentro de GDB se utilizan los siguientes comandos:

```gdb
target remote :1234    # Conexión al servidor GDB en QEMU
layout asm             # Visualización del código ensamblador (opcional)
layout regs            # Visualización de registros (opcional)
break _start           # Colocación de un punto de ruptura en la etiqueta _start
continue               # Ejecución hasta el punto de ruptura
n                      # Ejecución instrucción por instrucción
```

---

## 6. Finalización de la sesión

La finalización puede realizarse desde GDB con:

```gdb
monitor quit
```

