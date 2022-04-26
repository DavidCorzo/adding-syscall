# class stats(Structure):
#     _fields_ = [
#         ("count", c_int64),
#         ("index_number", c_int64),
#         ("open_arr", POINTER(c_double)*1),
#         ("close_arr", POINTER(c_double)*1),
#         ("high_arr", POINTER(c_double)*1),
#         ("low_arr", POINTER(c_double)*1),
#         ("open_mean" , c_double), ("open_stddev" , c_double) , ("open_min" , c_double) , ("open_max" , c_double),
#         ("close_mean", c_double), ("close_stddev", c_double) , ("close_min", c_double) , ("close_max", c_double),
#         ("high_mean" , c_double), ("high_stddev" , c_double) , ("high_min" , c_double) , ("high_max" , c_double),
#         ("low_mean"  , c_double), ("low_stddev"  , c_double) , ("low_min"  , c_double) , ("low_max"  , c_double)
#         ("data_is_in_memory", c_bool)
#     ]

from concurrent.futures import ThreadPoolExecutor
from ctypes import *


c_code = cdll.LoadLibrary('C:/Users/david/Desktop/4_1/operating_system/operating-system-projects/proyecto2/stats.so')

thread_amount = 8
with ThreadPoolExecutor(max_workers=thread_amount) as exe:
    pass
for i in range(0, 998):
    c_code.stats_init(i)
    c_code.stats_destruct(i)
