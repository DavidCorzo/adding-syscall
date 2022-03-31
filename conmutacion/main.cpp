#include <iostream>
#include <vector>
#include <time.h>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include <fstream>
#include "user_processes.h"
#include "queue.h"
#include "string.h"

using std::cout;
using std::cin;
using std::endl;
using std::vector;

#define QUANTITY_OF_PROCESSES 5
#define UNIT_OF_TIME sleep(1);
int counter[QUANTITY_OF_PROCESSES];
#define CONTEXTSW_INDEX 0
#define PRINTER_INDEX 1
int k_counter[2];

typedef int (*process_t)();
// process admition queue.
_queue<process_t> admitted_processes;


// processes.
int u_process_1() {
    #define PROCESS_1_INDEX 0
    cout << "u_process_1()" << endl;
    counter[PROCESS_1_INDEX]++;
    return 0;
}
int u_process_2() {
    #define PROCESS_2_INDEX 1
    cout << "u_process_2()" << endl;
    counter[PROCESS_2_INDEX]++;
    return 0;
}
int u_process_3() {
    #define PROCESS_3_INDEX 2
    cout << "u_process_3()" << endl;
    counter[PROCESS_3_INDEX]++;
    return 0;
}
int u_process_4() {
    #define PROCESS_4_INDEX 3
    cout << "u_process_4()" << endl;
    counter[PROCESS_4_INDEX]++;
    return 0;
}
int u_process_5() {
    #define PROCESS_5_INDEX 4
    cout << "u_process_5()" << endl;
    counter[PROCESS_5_INDEX]++;
    return 0;
}

int k_starting() {
    cout << "kernel starting..." << endl;
    return 0;
}

int k_printer() {
    for (uint32_t i {0}; i < QUANTITY_OF_PROCESSES; i++ ) {
        cout << "using counter=" << i << " currently at value=" << counter[i] << endl;
    }
    k_counter[PRINTER_INDEX]++;
    return 0;
}

uint32_t quantum_time() {
    // seed random function with current time.
    srand(time(NULL));
    // calculating iteration number from 1 to 10, excluding 0.
    return ((rand() % 9) + 1);
}

process_t k_context_switch(uint32_t *index) {
    // solo se tarda una iteración para simular el context switch.
    cout << "context switch() 1 unit of time." << endl;
    *index = (*index + 1) % admitted_processes.size();
    k_counter[CONTEXTSW_INDEX]++;
    return admitted_processes.q->at(*index);
}

void my_handler(int s){
    std::ofstream file;
    file.open("stats.csv");
    file << "proceso;tipo;identificador;quantum_asignado;valor_variable;cantidad_conmutada\n";
    file << "context_switch;kernel;" << &k_context_switch    << "1;"    << k_counter[CONTEXTSW_INDEX] << k_counter[CONTEXTSW_INDEX] << "\n";
    file << "admition;kernel;"      << &admitted_processes  << "1;"    <<  "1"                        <<  "1"                       << "\n";
    file << "printer;kernel;"       << &k_printer           << "1;"    <<  k_counter[PRINTER_INDEX]   <<  k_counter[PRINTER_INDEX]  << "\n";
    file << "u_process1;user;"      << &u_process_1         << "1-10;" <<  counter[0]                 <<  counter[0]                << "\n";
    file << "u_process2;user;"      << &u_process_2         << "1-10;" <<  counter[1]                 <<  counter[1]                << "\n";
    file << "u_process3;user;"      << &u_process_3         << "1-10;" <<  counter[2]                 <<  counter[2]                << "\n";
    file << "u_process4;user;"      << &u_process_4         << "1-10;" <<  counter[3]                 <<  counter[3]                << "\n";
    file << "u_process5;user;"      << &u_process_5         << "1-10;" <<  counter[4]                 <<  counter[4]                << "\n";
    file.close();
    printf("Caught signal %d\n",s);
    exit(1);
}

int main() {
    // reset all members of the array to 0.
    memset(counter, 0, sizeof(counter));
    signal(SIGINT, my_handler);
    // these lines creates the new process, admits it but does not yet put it in ready.
    admitted_processes.enqueue(u_process_1); UNIT_OF_TIME
    admitted_processes.enqueue(u_process_2); UNIT_OF_TIME
    admitted_processes.enqueue(u_process_3); UNIT_OF_TIME
    admitted_processes.enqueue(u_process_4); UNIT_OF_TIME
    admitted_processes.enqueue(u_process_5); UNIT_OF_TIME
    admitted_processes.enqueue(k_printer);
    process_t fn {&k_starting};
    uint32_t index {0};
    uint32_t quantum { 0 };
    while (1) {
        // context switch
        fn();
        if (quantum == 0) {
            quantum = quantum_time();
            fn = k_context_switch(&index);
        }
        quantum--;
    }
    return 0;
}