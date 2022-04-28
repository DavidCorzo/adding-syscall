from copy import copy
from threading import Thread
import pandas as pd
from concurrent.futures import ThreadPoolExecutor
from concurrent.futures import ProcessPoolExecutor
import time

# para = archivos y funciones, para1 = solo funciones
OPEN_STR, CLOSE_STR, HIGH_STR, LOW_STR = "open", "close", "high", "low"
FILE_COUNT = 999

def count_func(index):
    df = pd.read_csv(f"./data/index_data_{index}.csv")
    columns = {col_name:0 for col_name in df.columns if (col_name != 'dates')}

    for col_name in columns.keys():
        columns[col_name] = df[col_name].count()

    with open(f"./out/archivo_out_{index}.csv", encoding='utf8', mode='a+') as file:
        for col_name, values in columns.items():
            file.write(f"count({col_name}, {values}) ")

def stddev_func(index):
    df = pd.read_csv(f"./data/index_data_{index}.csv")
    columns = {col_name:0 for col_name in df.columns if (col_name != 'dates')}

    for col_name in columns.keys():
        columns[col_name] = df[col_name].std()

    with open(f"./out/archivo_out_{index}.csv", encoding='utf8', mode='a+') as file:
        for col_name, values in columns.items():
            file.write(f"stddev({col_name}, {values}) ")

    
def mean_func(index):
    df = pd.read_csv(f"./data/index_data_{index}.csv")
    columns = {col_name:0 for col_name in df.columns if (col_name != 'dates')}

    for col_name in columns.keys():
        columns[col_name] = df[col_name].mean()

    with open(f"./out/archivo_out_{index}.csv", encoding='utf8', mode='a+') as file:
        for col_name, values in columns.items():
            file.write(f"mean({col_name}, {values}) ")

def min_func(index):
    df = pd.read_csv(f"./data/index_data_{index}.csv")
    columns = {col_name:0 for col_name in df.columns if (col_name != 'dates')}

    for col_name in columns.keys():
        columns[col_name] = df[col_name].min()

    with open(f"./out/archivo_out_{index}.csv", encoding='utf8', mode='a+') as file:
        for col_name, values in columns.items():
            file.write(f"min({col_name}, {values}) ")

def max_func(index):
    df = pd.read_csv(f"./data/index_data_{index}.csv")
    columns = {col_name:0 for col_name in df.columns if (col_name != 'dates')}

    for col_name in columns.keys():
        columns[col_name] = df[col_name].max()

    with open(f"./out/archivo_out_{index}.csv", encoding='utf8', mode='a+') as file:
        for col_name, values in columns.items():
            file.write(f"stddev({col_name}, {values}) ")

def single_file_seq_funcs(index):
    count_func(index)
    mean_func(index)
    stddev_func(index)
    min_func(index)
    max_func(index)


def sequential_model(): # todo secuencial
    t0 = t1 = None
    t0 = time.perf_counter()
    for i in range(0, FILE_COUNT):
        count_func(i)
        mean_func(i)
        stddev_func(i)
        min_func(i)
        max_func(i)
    t1 = time.perf_counter()
    return (t1 - t0)

def parallel_files_model(thread_num): # funciones estadisticas secuenciales
    t0 = t1 = None
    t0 = time.perf_counter()
    if __name__ == '__main__':
        with ThreadPoolExecutor(max_workers=thread_num) as exe:
            exe.map(single_file_seq_funcs, [x for x in range(0, FILE_COUNT)])
    t1 = time.perf_counter()
    return (t1 - t0)

def parallel_funcs_model(thread_num): # archivos secuenciales
    t0 = t1 = None
    t0 = time.perf_counter()
    if __name__ == '__main__':
        with ThreadPoolExecutor(max_workers=thread_num) as exe:
            for i in range(0, FILE_COUNT):
                exe.submit(count_func, i)
                exe.submit(mean_func, i)
                exe.submit(stddev_func, i)
                exe.submit(min_func, i)
                exe.submit(max_func, i)
    t1 = time.perf_counter()
    return (t1 - t0)

def parallel_files_funcs_model(thread_num): # archivos y funciones en paralelo.
    t0 = t1 = None
    t0 = time.perf_counter()
    if __name__ == '__main__':
        file_list = [x for x in range(0, FILE_COUNT)]
        with ThreadPoolExecutor(max_workers=thread_num) as exe:
            exe.map(count_func, file_list)
            exe.map(mean_func, file_list)
            exe.map(stddev_func, file_list)
            exe.map(min_func, file_list)
            exe.map(max_func, file_list)
    t1 = time.perf_counter()
    return (t1 - t0)


# print(f"sequential_model({sequential_model()})")
print(f"parallel_funcs_model({parallel_funcs_model(8)})")
print(f"parallel_files_funcs_model({parallel_files_funcs_model(8)})")
print(f"parallel_files_model({parallel_files_model(8)})")