### Agregar SYSCALLS al kernel de LINUX : INSTRUCCIONES:

Problema:
1. Desarrollar una función que determine si una expresión aritmética se encuentra correctamente cerrada. La función deberá retornar verdadero si la expresión está correctamente cerrada y falso de lo contrario. Entiéndase por cerrada al hecho de que cada símbolo de paréntesis y/o corchete abierto debe tener su correspondiente cierre. Ejemplo (5+5), [(1+1)*(2+2)], (((([1]))))
2. Dicha función debe de estar implementada en un system call que deberá recibir como parámetro un puntero a la cadena que contiene la expresión
3. El estudiante debe utilizar C++ y el kernel de Linux para agregar el system call
4. Deberá compilar el kernel de sistema operativo para agregar la system call personalizada
5. Deberá de crear un programa que reciba como entrada en línea de comando la cadena a utilizar y hacer uso de la system call implementada en el sistema operativo. Se espera como entregable:
    - URL al repositorio donde implementó la función por medio de un system call y el programa que hace uso de la función
    - Documentación con el diagrama conceptual que explique todos los pasos necesarios que el sistema operativo tiene que realizar para que el proceso de usuario pueda utilizar la system call.
Condición:
- Proyecto puede realizarse con no más de 3 integrantes
- Tiene una ponderación 10 puntos netos
- Si el estudiante retorna el valor de la operación en un puntero tendrá 5 puntos extras netos
Ejemplo bool isvalid=my_func(&value,&exp)

### Dentro de una máquina virtual corriendo ubuntu 16.7.4:
Arrancar la máquina virtual y abrir la terminal.
A continuación descargaremos de kernel.org el código del kernel de linux, usaremos el kernel versión 4.7.1, lo descargaremos y lo extraeremos:
´´´
wget https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/snapshot/linux-95f15f5ed6e68b479c73da842012108c567c6999.tar.gz
tar xf linux-95f15f5ed6e68b479c73da842012108c567c6999
´´´
Por conveniencia renombraremos linux-95f15f5ed6e68b479c73da842012108c567c6999 a linux-kernel-v4.7.1:
´´´
mv linux-95f15f5ed6e68b479c73da842012108c567c6999 linux-kernel-v4.7.1
´´´

Despues crearemos un directorio para nuestro system call y crearemos un archivo para nuestro system call:
´´´
cd linux-kernel-v4.7.1
mkdir expr
cd expr
touch expr.c
´´´
Copie y pegue el codigo de lo que quiere que haga el syscall dentro de expr/expr.c.

La función como la tiene probablemente no será aceptada, recuerde que todos los sycalls deben de devolver un 'long', ningun otro tipo de dato es aceptado. Pero esto lo detallaremos algunos pasos, por que para cambiar el código debemos tomar en cuenta los macros de syscalls.h. Por ahora solo meta el código dentro expr/expr.c

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

Adicional a ubicar estos archivos, debemos crear un folder dentro de la carpeta donde se extrajo el código fuente del kernel, este folder se puede llamar lo que sea, nosotros le pusimos 'expr' de nombre. Dentro de 'expr' creamos un archivo expr.c donde se encuentra la implementación del código del system call.
- /expr/expr.c

Media vez se haya creado el archivo y se hayan abierto los archivos estamos listos para agregar nuestro sistem call.

### Determinar parámetros
En un system call los parametros que se tienen se deben de declarar con macros. Es decir, si se tiene 1 parametro entonces se usará el macro SYSCALL_DEFINE1, si se tienen dos entonces SYSCALL_DEFINE2. Estos macros son para expandir el código y están definidos en syscalls.h y se pueden buscar con CTRL-F "SYSCALL_DEFINE".

En nuestro caso estamos usando 2 parametros por lo que tendremos que usar SYSCALL_DEFINE2. Nuestro código está implementado de la siguiente forma (dentro de expr.c):
´´´
long expr_checker(long* value, char* expr) {
  // is correctly closed_
  long is_correctly_closed = bc_checkExpr(expr);
  if (!is_correctly_closed) {
    *value = -9999999;
  }
  else {
    // allocate for postfix string
    char postfix[str_length(expr)];
    // get postfix notation
    itp_infix_to_postfix(expr, postfix);
    // evaluate expression
    *value = peval_evalPostfix(postfix);
  }
    printk(KERN_INFO "value=%ld, is_correct=%d", *value, is_correctly_closed);
  return is_correctly_closed;
}
´´´
Esta es la función principal, sin embargo no podemos usarla así puesto a que para meter un system call al kernel debe de estar declarada como 'asmlinkage'. Una cosa que se debe recalcar es que todas las system calls devuelven un long, no se puede devolver otra cosa. Entonces a nuestro código de arriba debemos agregar y modificar algunas cosas:
- Puesto a que estamos usando 2 parámetros usaremos SYSCALL_DEFINE2.

IMPORTANTE: si se usa un macro como SYSCALL_DEFINEx, no se debe de agregar 'sys_' antes del nombre de la función, si no se usa el macro entonces se debe de poner solo el nombre de la función. Hay que recordar que en syscalls.h el macro se expande y automáticamente renombra la función con 'sys_' antes como podemos ver a continuación:
´´´
#define SYSCALL_DEFINE2(name, ...) SYSCALL_DEFINEx(2, _##name, __VA_ARGS__)
#define SYSCALL_DEFINEx(x, sname, ...)				\
	SYSCALL_METADATA(sname, x, __VA_ARGS__)			\
	__SYSCALL_DEFINEx(x, sname, __VA_ARGS__)
#define __SYSCALL_DEFINEx(x, name, ...)					\
	asmlinkage long sys##name(__MAP(x,__SC_DECL,__VA_ARGS__))	\
		__attribute__((alias(__stringify(SyS##name))));		\
	static inline long SYSC##name(__MAP(x,__SC_DECL,__VA_ARGS__));	\
	asmlinkage long SyS##name(__MAP(x,__SC_LONG,__VA_ARGS__));	\
	asmlinkage long SyS##name(__MAP(x,__SC_LONG,__VA_ARGS__))	\
	{								\
		long ret = SYSC##name(__MAP(x,__SC_CAST,__VA_ARGS__));	\
		__MAP(x,__SC_TEST,__VA_ARGS__);				\
		__PROTECT(x, ret,__MAP(x,__SC_ARGS,__VA_ARGS__));	\
		return ret;						\
	}								\
	static inline long SYSC##name(__MAP(x,__SC_DECL,__VA_ARGS__))
´´´
Notese que el macro SYSCALL_DEFINE2 llama a SYSCALL_DEFINEx que a su vez llama a SUSCALL_METADATA y posteriormente __SYSCALL_DEFINEx. Dentro de __SYSCALL_DEFINEx vemos que se declara el prototipo como "asmlinkage long sys" entonces debemos de tener en cuenta que si usamos el macro hay que quitarle el prefijo 'sys_'. Es siempre recomendable usar el macro puesto a que agrega muchas cosas más a la función y es posible que si no se usa el macro el system call no funciones en lo absoluto.

Cambiaremos nuestro código a lo siguiente:
- En lugar de "long expr_checker(long* value, char* expr)" a "SYSCALL_DEFINE2(expr_checker, long*, value, char*, expr)" (con las comas entre los tipos de datos y todo). Recordar de NO AGREGAR EL 'sys_' al nombre de la función si se está usando el macro.

Nos quedará así la función:
´´´
SYSCALL_DEFINE2(expr_checker, long*, value, char*, expr) {...}
´´´

Estamos listos para agregar el prototipo de nuestra función en los demás archivos correspondientes.

### Agregar función a syscalls.h.
En syscalls.h ir hasta abajo antes del #endif y agregar el prototipo de la siguiente manera:
´´´
asmlinkage long sys_<nombre-de-función>(<tipo> <param1>, <tipo> <param2>);
´´´
Aquí no usamos los macros, solo en expr.c usamos macros. Aquí debemos agregar el prototipo tal y como esperaríamos que se expandiera después de expandir el macro.

### Agregar función a syscalls_64.tbl
Estamos usando syscalls_64.tbl pero si su arquitectura es diferente se debe de usar syscalls_32.tbl o la que sea correspondiente. Dentro del archivo buscar el número de system call más alto, en nuestro caso hay un total de 545 system calls, entonces usaremos la 546. El system call se agrega con el siguiente formato:
´´´<number> <abi> <name> <entry point>´´´
El formato significa lo siguiente:
- number: el número de system call, en nuestro caso 546 por que es la siguiente system call.
- abi: segun nuestra arquitectura seleccionamos ya sea 'common', '64', o 'x32'. Common significa que la system call funciona para x64-64 y para x32. 64 significa que es para una arquitetura de 64 bits, y 32 significa que es para una arquitectura de 32 bits.
- name: es el nombre de nuestra función SIN EL 'sys_'.
- entry point: es el nombre de nuestra función CON EL 'sys_'.
En nuestro caso es: 
´´´´
546	    64      expr_checker        sys_expr_checker
´´´

Guardar y cerrar el archivo.

### Agregar el directorio en el cual se encuentra implementado el system call a el Makefile del kernel

Abrir /Makefile dentro del kernel. Este es un archivo gigantezco lleno de comandos, debemos buscar donde dice 'core-y', nos apareceran varias instancias, iremos a la segunda instancia donde dice:
´´´´
core-y += kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ block/ 
´´´
Agregaremos nuestro directorio, en nuestro caso se llama 'expr/'. Recordar poner un espacio. Quedará así:
´´´
core-y += kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ block/ expr/
´´´

### Makefile del systemcall de nuestro proyecto:
Dentro de /expr, en el mismo directorio donde se encuentra expr.c. Al principio creamos un archivo vacío que se llamaba Makefile, este Makefile no es el mismo que el que hicimos en el paso anterior, es otro nuevo que se encuentra dentro del folder que contiene el código de nuestro system call. Abrir este archivo.
En este archivo agregar el nombre del archivo en este formato:
´´´
obj-y := <nombre-del-archivo-que-tiene-la-implementacion-del-syscall>.o
´´´
En nuestro caso:
´´´
obj-y := expr.o
´´´
Esto agrega el archivo del código objeto de nuestro código al kernel.

### Compilar el kernel
Por ultimo debemos compilar todo el kernel, este proceso se tardará aproximadamente 20 minutos si el sistema operativo está montado sobre un disco duro de estado sólido, y aproximadamente 2 horas si está montado sobre un disco duro magnético.

Primero debemos instalar las siguientes dependencias:
´´´
sudo apt-get install gcc
sudo apt-get install libncurses5-dev
sudo apt-get install bison
sudo apt-get install flex
sudo apt-get install libssl-dev
sudo apt-get install libelf-dev
sudo apt-get update
sudo apt-get upgrade
´´´

Después debemos ejecutar el siguiente comando:
´´´
sudo make menuconfig
´´´
Nos abrirá un menú, en este debemos seleccionar donde dice 'file systems' y verificar que el 'ext4' esté seleccionado, de esta manera nos aseguramos que el file system que estémos usando sea compatible con la compilación.

Después estamos listos para compilar el kernel. Si es la primera vez que se compila el kernel no ejecute el siguiente paso, pero si intentó compilar y falló, debe de limpiar los archivos objeto que fallaron puesto a que sino se incluirán en el ejecutable. Si compilo previamente y fallo ejecute:
´´´
sudo make clean
´´´
Esto eliminará todos los archivos .o que fallaron y los generará de nuevo correctamente, de lo contrario la compilación puede fallar puesto a que los .o que están malos se usarán sin generarlos de nuevo.

Ahora estamos listos para compilar, si usted sabe cuantos procesadores tiene su computadora disponibles entonces puede poner un flag para usar los procesadores disponibles y compilar más rápido, si no sabe entonces simplemente no ponga el flag. El flag se pone con el siguiente formato:
´´´
sudo make -j<numero-de-procesadores>
´´´
En nuestro caso tenemos 6 procesadores disponibles para compilarlo entonces ejecutaremos:
´´´´
sudo make -j6
´´´
Si usted no sabe cuantos procesadores están disponibles simplemente no ponga el '-j<numero-de-procesadores>'. Pero es recomendable puesto a que si se usan, por ejemplo, 6 procesadores, la compilación estará lista 6 veces más rápido que si se usa solo un procesador. Vale la pena puesto a que la compilación tarda un monton de tiempo.

Cuando se termine de compilar hay que ver que no hayan ocurrido errores, si hay errores hay que poner ´´´sudo make clean´´´ y repetir compilación hasta que no hayan errores. Si hay warnings no importa, solo los errores son los que tienen problemas.

### Reboot y sustituir el kernel actual por el kernel modificado con el syscall agregado
Por último, debemos sustituir el kernel actual por el que acabos de compilar. Para hacer esto ejecutar:
´´´
sudo make modules_install install
´´´
Esto se tardará aproximadamente 5 minutos.
Al terminar ejecutar:
´´´
shutdown -r now
´´´
Este comando le dará reboot al sistema y hara efectivo el cambio de kernel. La máquina se reiniciará, cuando se reinicie el kernel nuevo estará corriendo.

### Probar el system call:
Ya con nuestro kernel compilado con la nueva system call vamos a crear un archivo llamado 'userspace.c' en cualquier lugar de la computadora:
´´´
touch userspace.c
´´´
Abrir userspace.c y agregar lo siguiente:
´´´
#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
int main() {
    long value = 0;
    char expr[] = "(2+1)*8";
    long int is_correct = syscall(546, &value, expr);
    printf("value=%ld, expr=%s", value, (expr?"true":"false"));
    return 0;
}
´´´
El código imprime:
´´´
value=24, expr=true
´´´
Indicando que se evaluó la expresión, y los parentesis están bien cerrados. El system call funcionó correctamente.
