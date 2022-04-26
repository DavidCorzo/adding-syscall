#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

typedef struct stats {
    long long int count, index_number;
    double *open_arr  , *close_arr   , *high_arr , *low_arr;
    double open_mean  , open_stddev  , open_min  , open_max;
    double close_mean , close_stddev , close_min , close_max;
    double high_mean  , high_stddev  , high_min  , high_max;
    double low_mean   , low_stddev   , low_min   , low_max;
    bool data_is_in_memory;
} stats;

#define QUANTITY_OF_FILES 998
stats s_arr[QUANTITY_OF_FILES];

void stats_init(long long int const f_index) {
    /* 
    This function gets the index name of the file and then
    opens this file and gets all the data in it. It does not perform
    any computations, it only gets the data from the file into RAM.
    */
    stats *s = (s_arr + f_index);
    s->data_is_in_memory = false;
    s->index_number = f_index;
    char csv_filename[40];
    sprintf(csv_filename, "./data/index_data_%d.csv", s->index_number);
    FILE *csv = fopen(csv_filename, "r");
    if (!csv) {
        printf("### CSV %s gave null.", csv_filename);
        exit(0);
    }
    long long int total_number_of_lines = 0;
    off64_t start_from = 0;
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
    s->open_arr  = (double*)malloc(total_number_of_lines*sizeof(double));
    s->close_arr = (double*)malloc(total_number_of_lines*sizeof(double));
    s->high_arr  = (double*)malloc(total_number_of_lines*sizeof(double));
    s->low_arr   = (double*)malloc(total_number_of_lines*sizeof(double));
    unsigned long long int index = 0;
    while (fscanf(csv, "%*4d-%*2d-%*2d,%lf,%lf,%lf,%lf\n", (s->open_arr+index), (s->high_arr+index), (s->low_arr+index), (s->close_arr+index)) != EOF) { index++; }
    fclose(csv);
    if (index != total_number_of_lines) {
        printf("some error occured the index (%d) does not match the total number of lines (%d) of the file.", index, total_number_of_lines);
    }
    s->count = total_number_of_lines;
    s->data_is_in_memory = true;
    printf("initialized %lld\n", f_index);
}

void stats_destruct(long long int const index) {
    stats *s = (s_arr + index);
    free(s->open_arr);
    free(s->close_arr);
    free(s->high_arr);
    free(s->low_arr);
    printf("destroyed %lld\n", index);
}

void mean_func(long long int const index) {
    stats *s = (s_arr + index);
    long long int i = s->count - 1;
    s->close_mean = s->open_mean = s->high_mean = s->low_mean = 0;
    do {
        s->close_mean += s->close_arr[i];
        s->open_mean  += s->open_arr[i] ;
        s->high_mean  += s->high_arr[i] ;
        s->low_mean   += s->low_arr[i]  ;
    } while (i--);
    s->close_mean = (s->close_mean / s->count);
    s->open_mean  = (s->open_mean / s->count);
    s->high_mean  = (s->high_mean / s->count);
    s->low_mean   = (s->low_mean / s->count);
}

void max_func(long long int const index) {
    stats *s = (s_arr + index);
    long long int i = s->count - 1;
    s->open_max = s->open_arr[i];
    s->close_max = s->close_arr[i];
    s->low_max = s->low_arr[i];
    s->high_max = s->high_arr[i];
    do {
        if (s->open_arr[i] > s->open_max) s->open_max = s->open_arr[i];
        if (s->close_arr[i] > s->close_max) s->close_max = s->close_arr[i];
        if (s->low_arr[i] > s->low_max) s->low_max = s->low_arr[i];
        if (s->high_arr[i] > s->high_max) s->high_max = s->high_arr[i];
    } while(i--);
}

void min_func(long long int const index) {
    stats *s = (s_arr + index);
    long long int i = s->count - 1;
    s->open_min = s->open_arr[i];
    s->close_min = s->close_arr[i];
    s->low_min = s->low_arr[i];
    s->high_min = s->high_arr[i];
    do {
        if (s->open_arr[i] < s->open_min) s->open_min = s->open_arr[i];
        if (s->close_arr[i] < s->close_min) s->close_min = s->close_arr[i];
        if (s->low_arr[i] < s->low_min) s->low_min = s->low_arr[i];
        if (s->high_arr[i] < s->high_min) s->high_min = s->high_arr[i];
    } while(i--);
}

void mean_stddev_func(long long int const index) {
    stats *s = (s_arr + index);
    mean_func(index);
   long long int i = s->count - 1;
    s->open_stddev = s->close_stddev = s->low_stddev = s->high_stddev = 0;
    do {
        s->open_stddev += pow(s->open_arr[i] - s->open_mean, 2);
        s->close_stddev += pow(s->close_arr[i] - s->close_mean, 2);
        s->low_stddev += pow(s->low_arr[i] - s->low_mean, 2);
        s->high_stddev += pow(s->high_arr[i] - s->high_mean, 2);
    } while (i--);
    s->open_stddev = sqrt(s->open_stddev / (s->count - 1));
    s->close_stddev = sqrt(s->close_stddev / (s->count - 1));
    s->low_stddev = sqrt(s->low_stddev / (s->count - 1));
    s->high_stddev = sqrt(s->high_stddev / (s->count - 1));
}

void stats_print(long long int const index) {
    stats *s = (s_arr + index);
    printf("s.count(%d), ", s->count);
    printf("s.index_number(%lf), ", s->index_number);
    printf("s.open_mean(%lf), ", s->open_mean);
    printf("s.open_stddev(%lf), ", s->open_stddev);
    printf("s.open_min(%lf), ", s->open_min);
    printf("s.open_max(%lf), ", s->open_max);
    printf("s.close_mean(%lf), ", s->close_mean);
    printf("s.close_stddev(%lf), ", s->close_stddev);
    printf("s.close_min(%lf), ", s->close_min);
    printf("s.close_max(%lf), ", s->close_max);
    printf("s.high_mean(%lf), ", s->high_mean);
    printf("s.high_stddev(%lf), ", s->high_stddev);
    printf("s.high_min(%lf), ", s->high_min);
    printf("s.high_max(%lf), ", s->high_max);
    printf("s.low_mean(%lf), ", s->low_mean);
    printf("s.low_stddev(%lf), ", s->low_stddev);
    printf("s.low_min(%lf), ", s->low_min);
    printf("s.low_max(%lf), ", s->low_max);
    printf("s.data_is_in_memory(%d)\n\n", s->data_is_in_memory);
}

// void example() {
//     stats_init(s+0, 0);
//     mean_stddev_func(s+0);
//     printf("quant: %d, %p", QUANTITY_OF_FILES, s);
//     stats_print(s+0);
//     stats_destruct(s+0);
// }

// gcc -std=c99 stats.c -shared -o stats.so

// void paralel_file_model(long long int index) {
//     /*funciones estadisticas secuenciales, archivos paralelos.*/
//     stats_init      ((s+index), index);
//     mean_stddev_func((s+index));
//     min_func        ((s+index));
//     max_func        ((s+index));
//     stats_destruct  ((s+index));
// }

// void sequential_model() {
//     /* todo secuencial. */
// }

// void paralel_file_paralel_func_model() {
//     /* archivos paralelos & funciones paralelas */
// }