#include <iostream>
#include <cmath>
#include <fstream>
#include <thread>
#include <chrono>
#include <string.h>
#include <dirent.h>
#include <vector>
#include <fstream>
#include "statistics.h"

using std::cout;
using std::endl;
using std::vector;
using std::string;
using namespace std::chrono;
using namespace std;
using std::ofstream;

std::chrono::microseconds timer(void (*func)()) {
    auto start = high_resolution_clock::now();
    func();
    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stop - start);
    return duration;
}

void sequential() {
    file_stats fs_arr[999];
    for (uint64_t i {0}; i < 1'000; i++ ) {
        fs_arr[i].init(i);
    }
}


int main() {
    
    sequential();

    return 0;
}