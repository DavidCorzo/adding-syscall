#include <iostream>
#include <cmath>
#include <fstream>
#include <thread>
#include <chrono>
#include <string.h>
#include <dirent.h>
#include <vector>
#include "statistics.h"

// start time duration.
#define CLOCK_START() auto start = high_resolution_clock::now();
// end time duration.
#define CLOCK_END() auto stop = high_resolution_clock::now(); auto duration = duration_cast<microseconds>(stop - start);

using std::cout;
using std::endl;
using std::vector;
using std::string;
using namespace std::chrono;
using namespace std;


int main() {
    CLOCK_START();
    
    file_stats fs_arr[999];
    for (uint64_t i {500}; i < 1'000; i++ ) {
        fs_arr[i].init(i);
    }
    

    CLOCK_END();
    return 0;
}