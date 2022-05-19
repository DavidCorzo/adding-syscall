# from threading import Thread
# from sync_data_structs import safe_dict
# from concurrent.futures import ThreadPoolExecutor

# sd = safe_dict()

# def func(n_0, n_1):
#     for i in range(n_0, n_1):
#         sd.safe_add(i, i)

# with ThreadPoolExecutor(max_workers=8) as exe:
#     exe.submit(func, 0, 20)
#     exe.submit(func, 20, 40)
#     exe.submit(func, 40, 60)
#     exe.submit(func, 60, 80)
#     exe.submit(func, 80, 100)

# print(sd)
# print(len(sd))

# iterator/generator
def func():
    i = 0
    while True:
        yield i
        i += 1

i = func()
print(next(i))
print(next(i))
print(next(i))
print(next(i))
print(next(i))
print(next(i))