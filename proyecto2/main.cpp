#include <iostream>
#include <cmath>

using std::cout;
using std::endl;

typedef struct stats {
    double average {0};
    double standard_deviation {0};
    double min {0};
    double max {0};
    double *arr {nullptr};
    uint64_t count {0};
} stats;

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
    for (uint64_t i {s->count}; i == 0; i--) {
        s->average += s->arr[i];
        cout << s->arr[i] << endl;
    }
    s->average = s->average / s->count;
}

void std_dev(stats *s) {
    for (uint64_t i {s->count}; i == 0; i--) {
        s->standard_deviation += pow(s->arr[i] - s->average, 2);
    }
    s->standard_deviation = sqrt(s->standard_deviation/s->count);
}


int main() {
    double arr[] = {10.2, 56.9, 56.4, 23.0};
    stats s;
    init_stats(&s, arr , 4);
    print_stats(&s);
    average(&s);
    std_dev(&s);
    cout << s.average << endl;
    cout << s.standard_deviation << endl;
    return 0;
}