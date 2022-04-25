#ifndef THREAD_POOL_H
    #define THREAD_POOL_H
#include <iostream>
#include <cmath>
#include <fstream>
#include <thread>
#include <chrono>
#include <string.h>
#include <dirent.h>
#include <vector>

// #define DEBUG

using std::cout;
using std::cin;
using std::endl;

#define NOT_NULL(arr) if (this->arr == nullptr) { cout << "ERROR: ARRAY IS NULL." << endl; exit(0); }

struct single_col_stats {
    double mean, stdev, min, max;
    double *arr;
    uint64_t count, i;
}
struct file_stats {
    single_col_stats open;
    single_col_stats high;
    single_col_stats low;
    single_col_stats close;
    uint32_t index_number;
};


void inline single_col_stats_init(single_col_stats &scs, uint64_t count_, double *arr_) {
    scs.count = count_;
    scs.arr = arr_;
}

void inline single_col_stats_min_func(single_col_stats &scs) {
    scs.i = scs.count-1;
    scs.min = scs.arr[scs.count-1];
    do {
        if (scs.arr[scs.i] < scs.min) scs.min = scs.arr[scs.i]; // min
    } while (scs.i--);
}

void inline single_col_stats_max_func(single_col_stats &scs) {
    scs.i = scs.count-1;
    scs.max = scs.arr[scs.count-1];
    do {
        if (scs.arr[scs.i] > scs.max) scs.max = scs.arr[scs.i]; // max
    } while(scs.i--);
}

void inline single_col_stats_min_max_func(single_col_stats &scs) {
    // faster than the individual.
    scs.i = scs.count-1;
    scs.min = scs.max = scs.arr[scs.count-1];
    do {
        if (scs.arr[scs.i] > scs.max) scs.max = scs.arr[scs.i]; // max
        if (scs.arr[scs.i] < scs.min) scs.min = scs.arr[scs.i]; // min
    } while(scs.i--);
}

void inline single_col_stats_min_max_stddev_func(single_col_stats &scs) {
    scs.i = scs.count-1;
    scs.min = scs.max = scs.arr[scs.count-1];
    do {
        if (scs.arr[scs.i] > scs.max) scs.max = scs.arr[scs.i]; // max
        if (scs.arr[scs.i] < scs.min) scs.min = scs.arr[scs.i]; // min
        scs.stdev += pow(scs.arr[scs.i] - scs.mean, 2); // stddev
    } while (scs.i--);
    scs.stdev = sqrt(scs.stdev/(scs.count-1));
}

void inline single_col_stats_mean_func(single_col_stats &scs) {
    scs.i = scs.count-1;
    scs.mean = 0;
    do {
        scs.mean += scs.arr[scs.i];
    } while (scs.i--);
    scs.mean = scs.mean / scs.count; // mean code.
}

void inline single_col_stats_stddev_func(single_col_stats &scs) {
    // calculate average. the stdev depends on the mean.
    scs.i = scs.count-1;
    do {
        scs.stdev += pow(scs.arr[scs.i] - scs.mean, 2);
    } while (scs.i--);
    scs.stdev = sqrt(scs.stdev/(scs.count-1)); // std dev code.
}

void inline single_col_stats_calculate_all_func(single_col_stats &scs) {
    sing_col_stats_mean_func(scs); // the stddev depends on the mean.
    sing_col_stats_stddev_func(scs); // stddev function
    sing_col_stats_min_func(scs); // min function
    sing_col_stats_max_func(scs); // max function
    // min_max_stddev_func(); // calculates min, max, and stddev, at the same time.
}

#ifdef DEBUG
void single_col_stats_print() {
    cout << "mean(" << mean << ")";
    cout << ",stdev(" << stdev << ")";
    cout << ",min(" << min << ")";
    cout << ",max(" << max << ")";
    cout << ",arr(" << arr << ")";
    cout << ",count(" << count << ")" << endl;
}
#endif


void file_stats_destruct(file_stats &fs) {
    delete[] fs.open.arr;
    delete[] fs.close.arr;
    delete[] fs.high.arr;
    delete[] fs.low.arr;
}

void file_stats_init(file_stats &fs, uint32_t index_number_) {
    /* 
    This function gets the index name of the file and then
    opens this file and gets all the data in it. It does not perform
    any computations, it only gets the data from the file into RAM.
    */
    fs.index_number = index_number_;
    char csv_filename[40];
    sprintf(csv_filename, ".\\data\\index_data_%d.csv", fs.index_number);
    FILE *csv = fopen(csv_filename, "r");
    if (!csv) {
        cout << "### CSV " << csv_filename << " gave null. " << endl;
        exit(0);
    }
    uint64_t total_number_of_lines {0};
    _off64_t start_from {0};
    char c; 
    while ((c = fgetc(csv)) != EOF) {
        if (c == '\n') {
            // waits for the first line to be detected and then starts counting.
            if (!start_from) {
                start_from = ftello64(csv);
                start_from++;
                continue;
            }
            else {
                total_number_of_lines++;
            }
        } 
    }
    fseek(csv, start_from, SEEK_SET); // rewind file cursor to first line (not zeroth).
    // populating the arrays.
    double *open_arr = new double[total_number_of_lines];
    double *close_arr = new double[total_number_of_lines];
    double *high_arr = new double[total_number_of_lines];
    double *low_arr = new double[total_number_of_lines];
    uint64_t index {0};
    while (fscanf(csv, "%*4d-%*2d-%*2d,%lf,%lf,%lf,%lf\n", (open_arr+index), (high_arr+index), (low_arr+index), (close_arr+index)) != EOF) { index++; }
    fclose(csv);
    if (index != total_number_of_lines) { cout << "some error occured the index (" << index << ") does not match the total number of lines (" << total_number_of_lines << ") of the file." << endl; }
    fs.open.init(total_number_of_lines, open_arr);
    fs.close.init(total_number_of_lines, close_arr);
    fs.high.init(total_number_of_lines, high_arr);
    fs.low.init(total_number_of_lines, low_arr);
}

void file_stats_sequential(file_stats &fs) {
    fs.open.calculate_all_func();
    fs.close.calculate_all_func();
    fs.high.calculate_all_func();
    fs.low.calculate_all_func();
}


#endif