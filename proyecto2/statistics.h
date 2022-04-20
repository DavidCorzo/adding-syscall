#include <iostream>
#include <cmath>
#include <fstream>
#include <thread>
#include <chrono>
#include <string.h>
#include <dirent.h>
#include <vector>

#define DEBUG

using std::cout;
using std::cin;
using std::endl;

#define NOT_NULL(arr) if (this->arr == nullptr) { cout << "ERROR: ARRAY IS NULL." << endl; exit(0); }

void replacechar(char *s, char to_replace, char replacement) {
    for(int i = 0; s[i] ; i++ ) {  
		if(s[i] == to_replace) {
		   s[i] = replacement;
	    }
	} 
}

struct single_col_stats {
    public:
        double mean;
        double stdev;
        double min;
        double max;
        double *arr;
        uint64_t count;
        uint64_t i; // register

    single_col_stats(): stdev{0}, mean{0}, count{0}, arr{nullptr},min{0}, max{0}, i{0} {
        NOT_NULL(arr);
    }
    
    void inline init(uint64_t count_, double *arr_) {
        count = count_;
        arr = arr_;
        min = max = arr[count - 1];
    }

    void inline min_func() {
        i = count-1;
        do {
            if (arr[i] < min) min = arr[i]; // min
            i--;
        } while (!i);
    }

    void inline max_func() {
        i = count-1;
        do {
            if (arr[i] > max) max = arr[i]; // max
            i--;
        } while(!i);
    }

    void inline min_max_func() {
        // faster than the individual.
        i = count-1;
        do {
            if (arr[i] > max) max = arr[i]; // max
            if (arr[i] < min) min = arr[i]; // min
            i--;
        } while(!i);
    }

    void inline min_max_stddev_func() {
        i = count-1;
        do {
            if (arr[i] > max) max = arr[i]; // max
            if (arr[i] < min) min = arr[i]; // min
            stdev += pow(arr[i] - mean, 2); // stddev
            i--;
        } while (!i);
        stdev = sqrt(stdev/(count-1));
    }

    void inline mean_func() {
        i = count-1;
        do {
            this->mean += this->arr[i];
            i--;
        } while (!i);
        this->mean = this->mean / this->count; // mean code.
    }

    void inline stddev_func() {
        // calculate average. the stdev depends on the mean.
        i = count-1;
        do {
            stdev += pow(arr[i] - mean, 2);
            i--;
        } while (!i);
        stdev = sqrt(stdev/(count-1)); // std dev code.
    }

    void inline calculate_all_func() {
        mean_func(); // the stddev depends on the mean.
        min_max_stddev_func(); // calculates min, max, and stddev, at the same time.
    }

    #ifdef DEBUG
        void print() {
            cout << "mean(" << mean << ")";
            cout << ",stdev(" << stdev << ")";
            cout << ",min(" << min << ")";
            cout << ",max(" << max << ")";
            cout << ",arr(" << arr << ")";
            cout << ",count(" << count << ")" << endl;
        }
    #endif
};

struct file_stats {
    single_col_stats open;
    single_col_stats high;
    single_col_stats low;
    single_col_stats close;

    void init(uint32_t index_number) {
        char csv_filename[30] = {[0]='\0'};
        strcat(csv_filename, ".\\data\\index_data_");
        strcat(csv_filename, std::to_string(index_number).c_str());
        strcat(csv_filename, ".csv");
        cout << csv_filename << endl;
        exit(0);
        FILE *csv = fopen(csv_filename, "r");
        if (!csv) {
            cout << "### CSV " << csv_filename << " gave null." << endl;
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
        open.init(total_number_of_lines, new double[total_number_of_lines]);
        close.init(total_number_of_lines, new double[total_number_of_lines]);
        high.init(total_number_of_lines, new double[total_number_of_lines]);
        low.init(total_number_of_lines, new double[total_number_of_lines]);
        uint64_t index {0};
        // uint32_t year, month, day;
        while (fscanf(csv, "%*4d-%*2d-%*2d,%lf,%lf,%lf,%lf\n", /*&year, &month, &day, */ &open.arr[index], &high.arr[index], &low.arr[index], &close.arr[index]) != EOF) {
            index++;
        }
        if (index != total_number_of_lines) {
            cout << "some error occured the index (" << index << ") does not match the total number of lines (" << total_number_of_lines << ") of the file." << endl;
        }
        // stats.
        open.calculate_all_func();
        close.calculate_all_func();
        high.calculate_all_func();
        low.calculate_all_func();

        #ifdef DEBUG
            open.print();
            close.print();
            high.print();
            low.print();
        #endif
    }

    ~file_stats() {}
};

class file_manager {
    public:
    uint32_t static quantity_of_files {1'000};
    char **data_filenames;

    file_manager(const char * const dir_s) {
        
    }

    ~file_manager() {
        
    }
};