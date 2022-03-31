#include <iostream>
#include <vector>
#include <time.h>
#include "user_processes.h"
#include "queue.h"
#include "string.h"

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::vector;

#define QUANTITY_OF_PROCESSES 5
int counter[5];

// processes.
int process_1() {
    #define PROCESS_1_INDEX 1
    counter[PROCESS_1_INDEX]++;
    return 0;
}
int process_2() {
    #define PROCESS_2_INDEX 2
    counter[PROCESS_2_INDEX]++;
    return 0;
}
int process_3() {
    #define PROCESS_3_INDEX 3
    counter[PROCESS_3_INDEX]++;
    return 0;
}
int process_4() {
    #define PROCESS_4_INDEX 4
    counter[PROCESS_4_INDEX]++;
    return 0;
}
int process_5() {
    #define PROCESS_5_INDEX 5
    counter[PROCESS_5_INDEX]++;
    return 0;
}

// the array containing the pointers to all the functions.


// the 1 core, 1 nucleus CPU that generates the quantum iterations and executes the function.
void k_cpu(int quantum_time, int (*function)()) {
    
}

void k_context_switch() {
    // 
    cout << "context switch() 1 unit of time." << endl;
}

void proceso_kernel_construct(int (*function)()) {
    // Proceso kernel de admisión, su propósito es crear y 
    // admitir un proceso de usuario (uno a la vez), el 
    // quantum de proceso de este kernel será de 1 ciclo 
    // (Un ciclo es una llamada a la función) 
    
}

int main() {
    memset(counter, 0, sizeof(counter)*QUANTITY_OF_PROCESSES);
    
}