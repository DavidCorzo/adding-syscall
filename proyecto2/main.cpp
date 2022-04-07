#include <iostream>
#include <cmath>
#include <fstream>

using std::cout;
using std::endl;

typedef struct stats {
    double average;
    double standard_deviation;
    double min;
    double max;
    double *arr;
    uint64_t count;
} stats;

typedef struct file_stats {
    stats open;
    stats high;
    stats low;
    stats close;
} file_stats;

#define QUANTITY_OF_FILES 1
file_stats fs[QUANTITY_OF_FILES];
uint64_t fs_index {0};

void init_stats(stats *s, double *arr, uint64_t size) {
    s->average = 0.0;
    s->standard_deviation = 0.0;
    s->min = 0.0;
    s->max = 0.0;
    s->arr = {arr};
    s->count = size;
}

void print_stats(stats *s) {
    cout << "average:" << s->average << endl;
    cout << "standard_deviation:" << s->standard_deviation << endl;
    cout << "min:" << s->min << endl;
    cout << "max:" << s->max << endl;
    cout << "arr:" << s->arr << endl;
    cout << "count:" << s->count << endl;
}

void average(stats *s) {
    // calculate average.
    for (uint64_t i {s->count-1}; ; i--) {
        s->average += s->arr[i];
        if (!i) {break;}
    }
    s->average = s->average / s->count;
}

void std_dev(stats *s) {
    for (uint64_t i {s->count-1}; ; i--) {
        s->standard_deviation += pow((s->arr[i] - s->average), 2);
        if (!i) {break;}
    }
    s->standard_deviation = sqrt(s->standard_deviation/(s->count-1));
}

void min_max(stats *s) {
    double min, max;
    min = max = s->arr[s->count-1];
    
    for (uint64_t i {s->count - 2}/*penultimo elemento*/; /*no end condition*/ ; i-- ) {
        double element {s->arr[i]};
        if (s->arr[i] < min) {
            min = s->arr[i];
        }
        if (s->arr[i] > max) {
            max = s->arr[i];
        }
        if (!i) {break;}
    }
    s->min = min;
    s->max = max;
}

void stats_of_csv(const char * const filename) {
    FILE *csv = fopen(filename, "r");
    uint32_t year, month, day;
    double open, high, low, close;
    char c;
    while ((c = fgetc(csv)) != EOF) {if (c == '\n') break; }
    while (fscanf(csv, "%4d-%2d-%2d,%lf,%lf,%lf,%lf\n", &year, &month, &day, &open, &high, &low, &close) != EOF) {
        cout << year << "-" << month << "-" << day << " " << open << " " << high << " " << low << " " << close << endl;
    }
}

int main() {
    stats_of_csv("index_data_1.csv");
    return 0;
}