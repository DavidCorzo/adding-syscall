#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

struct single_col_stats {
    double mean;
    double stdev;
    double min;
    double max;
    double *arr;
    uint64_t count;
    
    void init() {
        min = max = stdev = mean = 0;
        // calculate average. the stdev depends on the mean.
        uint64_t i {this->count-1}; // register
        { // min and max code.
            this->min = this->max = this->arr[i];
        }
        for (; ; i--) { // mean code.
            this->mean += this->arr[i];
            if (!i) {break;}
        }
        this->mean = this->mean / this->count; // mean code.
        // first iteration outside loop. because the min and max func requires to start from 2.
        { // stddev code.
            this->stdev += pow(((this->arr[this->count-1]) - (this->mean)), 2);
        }
        i = this->count-2;
        for (; ; i--) {
            { // stddev code.
                this->stdev += pow(((this->arr[i]) - (this->mean)), 2);
            }
            { // min and max code. 
                if (this->arr[i] < min) min = this->arr[i]; // min.
                if (this->arr[i] > max) max = this->arr[i]; // max.
            }
            if (!i) {break;}
        }
        this->stdev = sqrt(this->stdev/(this->count-1)); // std dev code.
    }
    void print() {
        cout << "mean(" << mean << ")";
        cout << ",stdev(" << stdev << ")";
        cout << ",min(" << min << ")";
        cout << ",max(" << max << ")";
        cout << ",arr(" << arr << ")";
        cout << ",count(" << count << ")" << endl;
    }
};

struct file_stats {
    char *csv_filename;
    single_col_stats open;
    single_col_stats high;
    single_col_stats low;
    single_col_stats close;

    void init(char *csv_filename_) {
        csv_filename = csv_filename_;
        FILE *csv = fopen(csv_filename, "r");
        uint64_t total_number_of_lines {0};
        _off64_t start_from {0};
        char c; 
        while ((c = fgetc(csv)) != EOF) {
            if (c == '\n') {
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
        open.arr = new double[total_number_of_lines];
        close.arr = new double[total_number_of_lines];
        high.arr = new double[total_number_of_lines];
        low.arr = new double[total_number_of_lines];
        open.count = close.count = high.count = low.count = total_number_of_lines;
        uint64_t index {0};
        uint32_t year, month, day;
        while (fscanf(csv, "%4d-%2d-%2d,%lf,%lf,%lf,%lf\n", &year, &month, &day, &open.arr[index], &high.arr[index], &low.arr[index], &close.arr[index]) != EOF) {
            index++;
        }
        if (index != total_number_of_lines) {
            cout << "some error occured the index (" << index << ") does not match the total number of lines (" << total_number_of_lines << ") of the file." << endl;
        }
        // stats.
        open.init();
        close.init();
        high.init();
        low.init();

        open.print();
        close.print();
        high.print();
        low.print();
    }

    void destruct() {
        delete[] open.arr;
        delete[] close.arr;
        delete[] high.arr;
        delete[] low.arr;
    }
};



int main() {
    // double arr[] = {1.2, 3.5, 6.7, 10.9};
    // single_col_stats s(arr, (sizeof(arr)/sizeof(double)));
    file_stats fs;
    fs.init((char*)"index_data_1.csv");
    fs.destruct();
    return 0;
}