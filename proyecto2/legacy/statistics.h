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

void stats_init(stats &s, uint32_t const f_index) {
    /* 
    This function gets the index name of the file and then
    opens this file and gets all the data in it. It does not perform
    any computations, it only gets the data from the file into RAM.
    */
    s.index_number = f_index;
    char csv_filename[40];
    sprintf(csv_filename, "./data/index_data_%d.csv", s.index_number);
    FILE *csv = fopen(csv_filename, "r");
    if (!csv) {
        cout << "### CSV " << csv_filename << " gave null. " << endl;
        exit(0);
    }
    uint64_t total_number_of_lines {0};
    off64_t start_from {0};
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
}

void inline stats_destruct(stats &s) {
    delete[] s.open_arr;
    delete[] s.close_arr;
    delete[] s.high_arr;
    delete[] s.low_arr;
}

void inline mean_func(stats &s) {
    uint64_t i {s.count - 1};
    s.close_mean = s.open_mean = s.high_mean = s.low_mean = 0;
    do {
        s.close_mean += s.close_arr[i];
        s.open_mean  += s.open_arr[i] ;
        s.high_mean  += s.high_arr[i] ;
        s.low_mean   += s.low_arr[i]  ;
    } while (i--);
    s.close_mean = (s.close_mean / s.count);
    s.open_mean  = (s.open_mean / s.count);
    s.high_mean  = (s.high_mean / s.count);
    s.low_mean   = (s.low_mean / s.count);
}

void inline max_func(stats &s) {
    uint64_t i { s.count - 1 };
    s.open_max = s.open_arr[i];
    s.close_max = s.close_arr[i];
    s.low_max = s.low_arr[i];
    s.high_max = s.high_arr[i];
    do {
        if (s.open_arr[i] > s.open_max) s.open_max = s.open_arr[i];
        if (s.close_arr[i] > s.close_max) s.close_max = s.close_arr[i];
        if (s.low_arr[i] > s.low_max) s.low_max = s.low_arr[i];
        if (s.high_arr[i] > s.high_max) s.high_max = s.high_arr[i];
    } while(i--);
}

void inline min_func(stats &s) {
    uint64_t i { s.count - 1 };
    s.open_min = s.open_arr[i];
    s.close_min = s.close_arr[i];
    s.low_min = s.low_arr[i];
    s.high_min = s.high_arr[i];
    do {
        if (s.open_arr[i] < s.open_min) s.open_min = s.open_arr[i];
        if (s.close_arr[i] < s.close_min) s.close_min = s.close_arr[i];
        if (s.low_arr[i] < s.low_min) s.low_min = s.low_arr[i];
        if (s.high_arr[i] < s.high_min) s.high_min = s.high_arr[i];
    } while(i--);
}

void inline stddev_func(stats &s) {
    uint64_t i {s.count - 1};
    s.open_stddev = s.close_stddev = s.low_stddev = s.high_stddev = 0;
    do {
        s.open_stddev += pow(s.open_arr[i] - s.open_mean, 2);
        s.close_stddev += pow(s.close_arr[i] - s.close_mean, 2);
        s.low_stddev += pow(s.low_arr[i] - s.low_mean, 2);
        s.high_stddev += pow(s.high_arr[i] - s.high_mean, 2);
    } while (i--);
    s.open_stddev = sqrt(s.open_stddev / (s.count - 1));
    s.close_stddev = sqrt(s.close_stddev / (s.count - 1));
    s.low_stddev = sqrt(s.low_stddev / (s.count - 1));
    s.high_stddev = sqrt(s.high_stddev / (s.count - 1));
}

void stats_print(stats &s) {
    cout << "s.count(" << s.count << "), ";
    cout << "s.index_number(" << s.index_number << "), ";
    cout << "s.open_mean(" << s.open_mean << "), ";
    cout << "s.open_stddev(" << s.open_stddev << "), ";
    cout << "s.open_min(" << s.open_min << "), ";
    cout << "s.open_max(" << s.open_max << "), ";
    cout << "s.close_mean(" << s.close_mean << "), ";
    cout << "s.close_stddev(" << s.close_stddev << "), ";
    cout << "s.close_min(" << s.close_min << "), ";
    cout << "s.close_max(" << s.close_max << "), ";
    cout << "s.high_mean(" << s.high_mean << "), ";
    cout << "s.high_stddev(" << s.high_stddev << "), ";
    cout << "s.high_min(" << s.high_min << "), ";
    cout << "s.high_max(" << s.high_max << "), ";
    cout << "s.low_mean(" << s.low_mean << "), ";
    cout << "s.low_stddev(" << s.low_stddev << "), ";
    cout << "s.low_min(" << s.low_min << "), ";
    cout << "s.low_max(" << s.low_max << ")";
    cout << endl << endl;
}

void min_max_stddev(stats &s) {
    uint64_t i {s.count - 1};
    // stddev
    s.open_stddev = s.close_stddev = s.low_stddev = s.high_stddev = 0;
    // min
    s.open_min = s.open_arr[i];
    s.close_min = s.close_arr[i];
    s.low_min = s.low_arr[i];
    s.high_min = s.high_arr[i];
    // max
    s.open_max = s.open_arr[i];
    s.close_max = s.close_arr[i];
    s.low_max = s.low_arr[i];
    s.high_max = s.high_arr[i];
    do {
        // stddev
        s.open_stddev += pow(s.open_arr[i] - s.open_mean, 2);
        s.close_stddev += pow(s.close_arr[i] - s.close_mean, 2);
        s.low_stddev += pow(s.low_arr[i] - s.low_mean, 2);
        s.high_stddev += pow(s.high_arr[i] - s.high_mean, 2);
        // min
        if (s.open_arr[i] < s.open_min) s.open_min = s.open_arr[i];
        if (s.close_arr[i] < s.close_min) s.close_min = s.close_arr[i];
        if (s.low_arr[i] < s.low_min) s.low_min = s.low_arr[i];
        if (s.high_arr[i] < s.high_min) s.high_min = s.high_arr[i];
        // max
        if (s.open_arr[i] > s.open_max) s.open_max = s.open_arr[i];
        if (s.close_arr[i] > s.close_max) s.close_max = s.close_arr[i];
        if (s.low_arr[i] > s.low_max) s.low_max = s.low_arr[i];
        if (s.high_arr[i] > s.high_max) s.high_max = s.high_arr[i];
    } while (i--);
    s.open_stddev = sqrt(s.open_stddev / (s.count - 1));
    s.close_stddev = sqrt(s.close_stddev / (s.count - 1));
    s.low_stddev = sqrt(s.low_stddev / (s.count - 1));
    s.high_stddev = sqrt(s.high_stddev / (s.count - 1));
}
