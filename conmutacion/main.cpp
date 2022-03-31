#include <iostream>
#include <vector>
#include <time.h>
#include <unistd.h>
#include <cstdlib>
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

int k_context_switch() {
    // solo se tarda una iteración para simular el context switch.
    cout << "context switch() 1 unit of time." << endl;
    UNIT_OF_TIME
    return 0;
}

int k_starting() {
    cout << "kernel starting..." << endl;
    UNIT_OF_TIME
    return 0;
}

int k_printer() {
    for (uint32_t i {0}; i < QUANTITY_OF_PROCESSES; i++ ) {
        cout << "using counter=" << i << " currently at value=" << counter[i] << endl;
    }
    UNIT_OF_TIME
    return 0;
}

uint32_t quantum_time() {
    // seed random function with current time.
    srand(time(NULL));
    // calculating iteration number from 1 to 10, excluding 0.
    return ((rand() % 9) + 1);
}

void cpu(uint32_t quantum, uint32_t process_id) {
    cout << "cpu" << endl;
    process_t running_process {admitted_processes.q->at(process_id)};
    for (uint32_t q {0}; q < quantum; q++ ) {
        running_process();
    }
}

int main() {
    // reset all members of the array to 0.
    memset(counter, 0, sizeof(counter));
    // these lines creates the new process, admits it but does not yet put it in ready.
    admitted_processes.enqueue(u_process_1); // UNIT_OF_TIME
    admitted_processes.enqueue(u_process_2); // UNIT_OF_TIME
    admitted_processes.enqueue(u_process_3); // UNIT_OF_TIME
    admitted_processes.enqueue(u_process_4); // UNIT_OF_TIME
    admitted_processes.enqueue(u_process_5); // UNIT_OF_TIME
    process_t currently_running {&k_starting};
    uint64_t index {0};
    currently_running();
    while (1) {
        index = (index + 1) % admitted_processes.size();
        cpu(quantum_time(), index);
        k_context_switch();
        UNIT_OF_TIME
    }
    return 0;
}