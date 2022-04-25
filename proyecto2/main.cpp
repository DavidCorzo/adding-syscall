#include <iostream>
#include <cmath>
#include <fstream>
#include <thread>
#include <chrono>
#include <string.h>
#include <fstream>
#include "statistics.h"
#include "thread_pool.h"

using std::cout;
using std::endl;
using std::string;
using namespace std::chrono;
using namespace std;
using std::ofstream;
using microsec = std::chrono::microseconds;

// microsec timer(void (*func)()) {
//     auto start = high_resolution_clock::now();
//     func();
//     auto stop = high_resolution_clock::now(); 
//     auto duration = duration_cast<microseconds>(stop - start);
//     return duration;
// }


void sequential(uint32_t q_files) {
    /* all in the main thread. */
    uint32_t i {q_files};
    stats s;
    do {
        stats_init(s, i);
        // mean
        s.close_mean = mean(s.close_arr, s.count);  // close_arr
        s.open_mean = mean(s.open_arr, s.count);   // open_arr
        s.high_mean = mean(s.high_arr, s.count);   // high_arr
        s.low_mean = mean(s.low_arr, s.count);    // low_arr
        // min
        s.close_min = min(s.close_arr, s.count);  // close_arr
        s.open_min = min(s.open_arr, s.count);   // open_arr
        s.high_min = min(s.high_arr, s.count);   // high_arr
        s.low_min = min(s.low_arr, s.count);    // low_arr
        // max
        s.close_max = max(s.close_arr, s.count);  // close_arr
        s.open_max = max(s.open_arr, s.count);   // open_arr
        s.high_max = max(s.high_arr, s.count);   // high_arr
        s.low_max = max(s.low_arr, s.count);    // low_arr
        // stddev
        s.close_stddev = stddev(s.close_arr, s.count, s.close_mean);  // close_arr
        s.open_stddev = stddev(s.open_arr, s.count, s.open_mean);   // open_arr
        s.high_stddev = stddev(s.high_arr, s.count, s.high_mean);   // high_arr
        s.low_stddev = stddev(s.low_arr, s.count, s.low_mean);    // low_arr
        stats_destruct(s);
    } while (i--);
}

void parallel_functions(thread_pool &tp, uint32_t q_files) {
    /* archivos sequenciales */
    uint32_t i {q_files};
    stats s;
    do {
        stats_init(s, i);
        // mean
        s.close_mean = mean(s.close_arr, s.count);  // close_arr
        s.open_mean = mean(s.open_arr, s.count);   // open_arr
        s.high_mean = mean(s.high_arr, s.count);   // high_arr
        s.low_mean = mean(s.low_arr, s.count);    // low_arr
        // min
        auto close_min_future = tp.submit(min, s.close_arr, s.count);
        // s.close_min = min(s.close_arr, s.count);  // close_arr
        // s.open_min = min(s.open_arr, s.count);   // open_arr
        // s.high_min = min(s.high_arr, s.count);   // high_arr
        // s.low_min = min(s.low_arr, s.count);    // low_arr
        // // max
        // s.close_max = max(s.close_arr, s.count);  // close_arr
        // s.open_max = max(s.open_arr, s.count);   // open_arr
        // s.high_max = max(s.high_arr, s.count);   // high_arr
        // s.low_max = max(s.low_arr, s.count);    // low_arr
        // // stddev
        // s.close_stddev = stddev(s.close_arr, s.count, s.close_mean);  // close_arr
        // s.open_stddev = stddev(s.open_arr, s.count, s.open_mean);   // open_arr
        // s.high_stddev = stddev(s.high_arr, s.count, s.high_mean);   // high_arr
        // s.low_stddev = stddev(s.low_arr, s.count, s.low_mean);    // low_arr
        stats_destruct(s);
    } while (i--);
}

void parallel_files_and_functions(thread_pool &tp) {
    /* paralelizar archivos y funciones dentro de cada archivo. */

}

uint16_t thread_pool::instances = 0; // static variable initialization.

uint64_t func1() {
    uint64_t num {0};
    for (uint64_t i {0}; i < 500'000; i++) {
        num += i;
    }
    return num;
}

uint64_t func2() {
    uint64_t num {0};
    for (uint64_t i {0}; i < 500'000; i++) {
        num += i;
    }
    return num;
}

int main() {
    thread_pool tp(3);
    tp.init();
    
    auto future1 = tp.submit(func1);
    auto future2 = tp.submit(func2);

    auto res1 = future1.get();
    auto res2 = future2.get();

    tp.shutdown();
    return 0;
}