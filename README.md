### Agregar SYSCALLS al kernel de LINUX

### Determinar la versión de linux y la arquitectura:
En la terminal tendremos que escribir el comando: 
´´´ uname -m ´´´
Dicho comando imprimirá la arquitectura en la que está montada el sistema operativo actual. En nuestro caso el sistema operativo está montado en un sistema _86x64. Esta información es importante por que la tabla de syscalls que se usa depende de esto. Pues hay una tabla de syscalls para arquitecturas de 32 bits, 64 bits, etc. 

Media vez ya sabemos nuestra arquitectura podemos proceder al siguiente paso.

### Ubicar y abrir archivos de interés:
El kernel de linux es un proyecto construido en C de múltiples carpetas y multiples archivos. Agregar un system call requiere conocimiento de los siguientes archivos (dentro del directorio de el código fuente del kernel linux):
- /arch/x86/entry/syscalls/syscall_64.tbl
- /include/linux/syscalls.h
- /Makefile

Adicional a ubicar estos archivos, debemos crear un folder dentro de la carpeta donde se extrajo el código fuente del kernel, este folder se puede llamar cualquier cosa, nosotros le pusimos 'expr' de nombre. Dentro de 'expr' creamos un archivo expr.c donde se encuentra la implementación del código del system call.
- /expr/expr.c

Media vez se haya creado el archivo y se haya 