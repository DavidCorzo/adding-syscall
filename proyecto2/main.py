from concurrent.futures import ThreadPoolExecutor
from time import perf_counter
from ctypes import *
QUANTITY_OF_FILES:int = 999


c_code = cdll.LoadLibrary('C:/Users/david/Desktop/4_1/operating_system/operating-system-projects/proyecto2/stats.so')

def todo_sequencial(c_code):
    for i in range(0, QUANTITY_OF_FILES):
        c_code.sequential_model(i)
        print(i)

def parallel_files(c_code, thread_amount):
    t0 = t1 = None
    with ThreadPoolExecutor(max_workers=thread_amount) as exe:
        t0 = perf_counter()
        exe.submit(c_code.paralel_file_model, QUANTITY_OF_FILES-1)
        t1 = perf_counter()
    # duration = t1 - t0
    print("duration: ", t0, t1)


def parallel_functions(c_code, thread_amount):
    with ThreadPoolExecutor(max_workers=thread_amount) as exe:
        pass

def parallel_files_and_functions(c_code, thread_amount):
    with ThreadPoolExecutor(max_workers=thread_amount) as exe:
        pass


parallel_files(c_code=c_code, thread_amount=2)
# todo_sequencial(c_code=c_code)