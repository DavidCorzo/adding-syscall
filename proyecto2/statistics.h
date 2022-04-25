#include <iostream>

using std::cout;
using std::endl;


struct stats {
    uint64_t count;
    uint32_t index_number;
    double *open_arr, *close_arr, *high_arr, *low_arr;
    double open_mean, open_stddev, open_min, open_max;
    double close_mean, close_stddev, close_min, close_max;
    double high_mean, high_stddev, high_min, high_max;
    double low_mean, low_stddev, low_min, low_max;
};

double stats_init(stats &s, uint32_t const f_index) {
    /* 
    This function gets the index name of the file and then
    opens this file and gets all the data in it. It does not perform
    any computations, it only gets the data from the file into RAM.
    */
    s.index_number = f_index;
    char csv_filename[40];
    sprintf(csv_filename, ".\\data\\index_data_%d.csv", s.index_number);
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
    s.open_arr = new double[total_number_of_lines];
    s.close_arr = new double[total_number_of_lines];
    s.high_arr = new double[total_number_of_lines];
    s.low_arr = new double[total_number_of_lines];
    uint64_t index {0};
    while (fscanf(csv, "%*4d-%*2d-%*2d,%lf,%lf,%lf,%lf\n", (s.open_arr+index), (s.high_arr+index), (s.low_arr+index), (s.close_arr+index)) != EOF) { index++; }
    fclose(csv);
    if (index != total_number_of_lines) {
        cout << "some error occured the index (" << index << ") does not match the total number of lines (" << total_number_of_lines << ") of the file." << endl; 
    }
    s.count = total_number_of_lines;
    return 0;
}

double inline stats_destruct(stats &s, const double const *arr, uint64_t const count, double const mean) {
    delete[] s.open_arr;
    delete[] s.close_arr;
    delete[] s.high_arr;
    delete[] s.low_arr;
    return 0;
}

double inline mean(stats &s, const double const *arr, uint64_t const count, double const mean) {
    double mean {0};
    uint64_t i {count - 1};
    do {
        mean += arr[i];
    } while (i--);
    return (mean / count);
}

double inline max(stats &s, const double const *arr, uint64_t const count, double const mean) {
    uint64_t i { count - 1 };
    double max { arr[i] };
    do {
        if (arr[i] > max) max = arr[i]; // max
    } while(i--);
    return max;
}

double inline min(stats &s, const double const *arr, uint64_t const count, double const mean) {
    uint64_t i { count - 1 };
    double min { arr[i] };
    do {
        if (arr[i] < min) min = arr[i]; // min
    } while(i--);
    return min;
}

double inline stddev(stats &s, const double const *arr, uint64_t const count, double const mean) {
    uint64_t i {count - 1};
    double standard_deviation {0};
    do {
        standard_deviation += pow(arr[i] - mean, 2);
    } while (i--);
    standard_deviation = sqrt(standard_deviation / (count - 1));
    return standard_deviation;
}