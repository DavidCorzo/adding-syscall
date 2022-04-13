#include <iostream>
#include <cmath>
#include <fstream>
#include <thread>
#include <chrono>
#include <string.h>
#include <dirent.h>
#include <vector>

#define DEBUG
// #define SEQUENTIAL
// #define PARALLEL_FILES
// #define PARALLEL_FUNCTIONS
// #define PARALLEL_FILES_AND_FUNCTIONS

using std::cout;
using std::endl;
using std::vector;
using namespace std::chrono;
using namespace std;

void replacechar(char *s, char to_replace, char replacement) {
    for(int i = 0; s[i] ; i++ ) {  
		if(s[i] == to_replace) {
		   s[i] = replacement;
	    }
	} 
}

class single_col_stats {
    public:
        double mean;
        double stdev;
        double min;
        double max;
        double *arr;
        uint64_t count;
    single_col_stats() 
        // initialize
        : min{0}, max{0}, stdev{0}, mean{0} {}
    void init() {
        // calculate average. the stdev depends on the mean.
        #ifdef DEBUG
        if (this->arr == nullptr) {
            cout << "ERROR: ARRAY IS NULL." << endl;
            exit(0);
        }
        #endif
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

class file_stats {
    public:
        char *csv_filename;
        single_col_stats open;
        single_col_stats high;
        single_col_stats low;
        single_col_stats close;

    file_stats(char *csv_filename_) {
        csv_filename = csv_filename_;
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
        // uint32_t year, month, day;
        while (fscanf(csv, "%*4d-%*2d-%*2d,%lf,%lf,%lf,%lf\n", /*&year, &month, &day, */ &open.arr[index], &high.arr[index], &low.arr[index], &close.arr[index]) != EOF) {
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

        #ifdef DEBUG
            open.print();
            close.print();
            high.print();
            low.print();
        #endif
    }

    ~file_stats() {
        delete[] open.arr;
        delete[] close.arr;
        delete[] high.arr;
        delete[] low.arr;
    }
};

class file_manager {
    public:
    #define QUANTITY_OF_FILES 2
    char **data_filenames;

    file_manager(const char * const dir_s) {
        struct dirent *dir;
        DIR *dp = opendir(dir_s);
        if (dp) {
            data_filenames = (char**)malloc(QUANTITY_OF_FILES*sizeof(char*));
            uint16_t index {0};
            while ((dir=readdir(dp)) != NULL) {
                if ((strcmp(dir->d_name, ".") == 0) || (strcmp(dir->d_name, "..") == 0)) {
                    continue;
                }
                char * name { (char*)malloc(dir->d_namlen) };
                strcpy(name, dir->d_name);
                data_filenames[index++] = name;
            }
            closedir(dp);
        }
        else {
            cout << "ERROR: opendir() failed to open " << dir_s << endl;
            exit(0);
        }
    }

    ~file_manager() {
        for (size_t i {QUANTITY_OF_FILES-1};  ; i-- ) {
            delete[] data_filenames[i];
            if (!i) {break;}
        }
        delete data_filenames;
    }

    void sequential() {
        char folder[] = ".\\data\\";
        for (size_t i {QUANTITY_OF_FILES-1}; ; i-- ) {
            // path = ".\\data\\"+filename
            char path[strlen(folder)+strlen(data_filenames[i])];
            strcpy(path, folder);
            strcat(path, data_filenames[i]);
            replacechar(path, '\\', '/');
            // cout << path << " " << i << endl;
            file_stats fs(path);
            if (!i) {break;}
        }
    }
};

int main() {
    // start time duration.
    auto start = high_resolution_clock::now();
    // time taken by the program.
    
    file_manager fm(".\\data");
    fm.sequential();

    // end time duration.
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "duration=" << duration.count() << " microseconds." << endl;
    return 0;
}