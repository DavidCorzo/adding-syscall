#include <iostream>
#include <cmath>
#include <fstream>
#include <thread>
#include <chrono>
#include <string.h>
#include <fstream>
#include <unistd.h>
#include "statistics.h"

using std::cout;
using std::endl;
using std::string;
using namespace std::chrono;
using namespace std;
using std::ofstream;
using microsec = std::chrono::microseconds;

// #define DEBUG
#define CLOCK_START() auto start = high_resolution_clock::now();
#define CLOCK_STOP() auto stop = high_resolution_clock::now(); auto duration = duration_cast<microseconds>(stop - start);


void sequential_files_and_functions(uint32_t q_files) {
    /* all in the main thread. */
    uint32_t i {q_files-1};
    stats s;
    do {
        stats_init(s, i); // serial
        // mean
        mean_func(s);
        // min
        min_func(s);
        // max
        max_func(s);
        // stddev
        stddev_func(s);
        // dealloc
        stats_destruct(s);
    } while (i--);
}

void single_file_parallel_functions(thread_pool &tp, uint32_t file_index) {
    /*
    Toma como argumento un solo indice a un archivo.

    */
    stats s;
    stats_init(s, file_index);
    // mean
    auto mean_future = tp.submit(mean_func, s);
    // min
    auto min_future = tp.submit(min_func, s);
    // max
    auto max_future = tp.submit(max_func, s);
    // mean join: stddev depends on the mean so we join all the mean workers.
    mean_future.get();
    // stddev
    auto stddev_future = tp.submit(stddev_func, s);
    // min: join
    min_future.get();
    // max: join 
    max_future.get();
    // stddev: join
    stddev_future.get();
    #if DEBUG
    // print
    stats_print(s);
    // sleep
    usleep(100'000);
    #endif
    stats_destruct(s);
}

void parallel_functions_sequential_files(thread_pool &tp, uint64_t q_files) {
    uint64_t i {q_files-1};
    do {
        single_file_parallel_functions(tp, i);
    } while (i--);
}


void parallel_files_parallel_functions(thread_pool &tp, uint64_t q_files) {
    /* paralelizar archivos y funciones dentro de cada archivo. */
    uint64_t i {q_files-1};
    stats s[q_files];
    std::future<void> futures[q_files];
    do {
        stats_init(s[i], i);
        mean_func(s[i]);
    } while (i--);
    i = q_files-1;
    do {
        futures[i] = tp.submit(min_max_stddev, s[i]);
    } while (i--);
    i = q_files-1;
    do {
        futures[i].get();
        stats_destruct(s[i]);
    } while (i--);
}

void single_file_sequential_functions(uint32_t index) {
    stats s;
    // serial
    stats_init(s, index); 
    // mean
    mean_func(s);
    // min
    min_func(s);
    // max
    max_func(s);
    // stddev
    stddev_func(s);
    // dealloc
    stats_destruct(s);
}

void parallel_files_sequential_functions(thread_pool &tp, uint64_t q_files) {
    uint64_t i {q_files - 1};
    do {
        tp.submit(single_file_sequential_functions, i);
    } while (i--);
}

uint16_t thread_pool::instances = 0; // static variable initialization.

int main() {
    uint32_t amount_of_threads {8};
    uint32_t t[] = {1,2,4,8};
    for (uint64_t i{0}; i < 10; i++ ) {
        for (uint64_t j{0}; j < 4; j++ ) {
            {
                cout << "parallel functions sequential files: ";
                thread_pool tp(t[j]);
                tp.init();
                CLOCK_START();
                parallel_functions_sequential_files(tp, 10);
                CLOCK_STOP();
                tp.shutdown();
                cout << duration.count() << " with " << t[j] << " threads." << endl;
            }
        }
        // tp.init();
        // CLOCK_START();
        // parallel_files_parallel_functions(tp, 998);
        // tp.shutdown();
        // CLOCK_STOP();
        // cout << "parallel files with parallel functions: " << endl
        //      << "\tAmount of threads: " << amount_of_threads << endl 
        //      << "\ttime taken: " << duration.count() << " microseconds." << endl;
        // cout << "parallel functions with sequential files: " << endl
        //     << "\tAmount of threads: " << amount_of_threads << endl 
        //     << "\ttime taken: " << duration.count() << " microseconds." << endl;
    }
    // int threads[] = {1, 2, 4, 8};
    // for (int i = 0; i < (sizeof(threads)/sizeof(int)); i++ ) {
    //     for (int j = 0; j < 10; j++ ) {
    //         uint32_t amount_of_threads = threads[i];
    //         cout << endl << "...amount of threads = " << amount_of_threads << " iteration = " << j << " ..." << endl;
    //         {
        // CLOCK_START();
        // sequential_files_and_functions(998);
        // CLOCK_STOP();
        // cout << "sequential files with sequential functions: " << endl
        //     << "\tAmount of threads: 1" << endl 
        //     << "\ttime taken: " << duration.count() << " microseconds." << endl;
                
    //         }
    //         {    
                
    //         }
    //         {
    //             thread_pool tp(amount_of_threads);
    //             tp.init();
    //             CLOCK_START();
    //             parallel_files_parallel_functions(tp, 998);
    //             tp.shutdown();
    //             CLOCK_STOP();
    //             cout << "parallel files with parallel functions: " << endl
    //                 << "\tAmount of threads: " << amount_of_threads << endl 
    //                 << "\ttime taken: " << duration.count() << " microseconds." << endl;
    //         }
    //         {
    //             thread_pool tp(amount_of_threads);
    //             tp.init();
    //             CLOCK_START();
    //             parallel_files_sequential_functions(tp, 998);
    //             tp.shutdown();
    //             CLOCK_STOP();
    //             cout << "parallel files with sequential functions: " << endl
    //                 << "\tAmount of threads: " << amount_of_threads << endl 
    //                 << "\ttime taken: " << duration.count() << " microseconds." << endl;
    //         }               
    //     }   
    // }
    return 0;
}