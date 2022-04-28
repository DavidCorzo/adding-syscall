from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
from time import perf_counter
from ctypes import *

from main import sequential_model

FILE_COUNT:int = 999


c_code = cdll.LoadLibrary('./stats.so') # C:/Users/david/Desktop/4_1/operating_system/operating-system-projects/proyecto2/
# c_code.start_up_config()

def todo_sequencial(c_code):
    t0 = perf_counter()
    c_code.sequential_model(FILE_COUNT-1)
    t1 = perf_counter()
    return (t1-t0)

def parallel_files_model(c_code, thread_amount):
    t0 = t1 = None
    t0 = perf_counter()
    if __name__ == '__main__':
        with ThreadPoolExecutor(max_workers=thread_amount) as exe:
            exe.map(c_code.single_paralel_file_model, [x for x in range(0, FILE_COUNT)])
    t1 = perf_counter()
    return (t1-t0)


def parallel_funcs_model(c_code, thread_num):
    t0 = perf_counter()
    if __name__ == '__main__':
        file_list = [x for x in range(0, FILE_COUNT)]
        map(c_code.stats_init, file_list) # secuencial
        with ProcessPoolExecutor(max_workers=thread_num) as exe:
            for i in range(0, FILE_COUNT):
                exe.submit(c_code.mean_func, i)
                exe.submit(c_code.stddev_func, i)
                exe.submit(c_code.min_func, i)
                exe.submit(c_code.max_func, i)
        map(c_code.stats_destruct, file_list)
    t1 = perf_counter()
    return (t1-t0)

def parallel_files_and_functions(c_code, thread_num):
    t0 = perf_counter()
    if __name__ == '__main__':
        file_list = [x for x in range(0, FILE_COUNT)]
        for i in file_list:
            c_code.stats_init(i)
        with ThreadPoolExecutor(max_workers=thread_num) as exe:
            exe.map(c_code.mean_func, file_list)
            exe.map(c_code.stddev_func, file_list)
            exe.map(c_code.min_func, file_list)
            exe.map(c_code.max_func, file_list)
        for i in file_list:
            c_code.stats_destruct(i)
    t1 = perf_counter()
    return (t1-t0)


print(sequential_model(c_code=c_code))
# for i in range(0, 10):
#     todo_sequencial(c_code=c_code)