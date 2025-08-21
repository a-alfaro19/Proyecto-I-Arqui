# Ejemplo: C + Assembly

Este ejemplo demuestra cómo un programa en C puede llamar funciones escritas en ensamblador RISC-V.

## Archivos

- `example.c`: Programa principal en C
- `math_asm.s`: Función matemática en ensamblador
- `linker.ld`: Script de enlazado
- `build.sh`: Script de compilación
- `run-qemu.sh`: Script para ejecutar QEMU

## Funcionalidad

### Programa en C (`example.c`)
- Función principal que llama a la función en ensamblador
- Funciones básicas para imprimir texto y números
- Prueba la función `sum_to_n` con diferentes valores

### Función en Assembly (`math_asm.s`)
- `sum_to_n(int n)`: Calcula la suma de números del 1 a n
- Sigue las convenciones de llamada RISC-V
- Maneja parámetros y valor de retorno correctamente

## Compilación y ejecución

```bash
# Compilar
./build.sh

# Ejecutar con QEMU (en una terminal)
./run-qemu.sh

# En otra terminal, conectar GDB
docker exec -it rvqemu /bin/bash
cd /home/rvqemu-dev/workspace/examples/c-asm
gdb-multiarch example.elf
```

## Depuración con GDB

```gdb
target remote :1234
break _start         # Punto de ruptura en main de C
break sum_to_n       # Punto de ruptura en función assembly
continue
step                 # Paso a paso
info registers       # Ver registros
```

## Convenciones de llamada RISC-V

- `a0`: Primer parámetro de entrada y valor de retorno
- `a1-a7`: Parámetros adicionales
- `s0-s11`: Registros salvados (preserved)
- `t0-t6`: Registros temporales
- `ra`: Dirección de retorno
- `sp`: Puntero de pila

La función assembly respeta estas convenciones:
1. Guarda registros que modifica en la pila
2. Recibe parámetro en `a0`
3. Devuelve resultado en `a0`
4. Restaura registros antes de retornar