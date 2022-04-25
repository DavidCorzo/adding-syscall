// void inline all_sequential(uint32_t quantity_of_files) {
//     uint32_t index {quantity_of_files};
//     do {
//         file_stats fs(index);
//         sequential(fs);
//     } while (index--);
// }

// void inline parallel_files(thread_pool *tp, uint32_t quantity_of_files) {
//     /* funciones sequenciales */
//     if (!tp) {cout << "thread pool is null." << endl; exit(-1); }
//     uint32_t index {quantity_of_files};
//     do {
//         file_stats fs(index);
//         tp->submit(parallel_files, fs);
//     } while (index--);
    
// }
