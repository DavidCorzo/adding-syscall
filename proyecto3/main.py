from concurrent.futures import ThreadPoolExecutor, thread
import threading
import time
import logging
import random
import queue
import webscrapping
from typing import Tuple, Iterable

logging.basicConfig(level=logging.DEBUG, format='%(thread)d:%(message)s',)

BUF_SIZE = 10
q:Iterable[Tuple[str, str]] = queue.Queue(BUF_SIZE)
aws = webscrapping.ActorWebScrapper(
    url="https://en.wikipedia.org/wiki/List_of_American_film_actresses"
)

done = False
done_mutex = threading.Lock()

class ProducerThread(threading.Thread):
    def __init__(self, target=None, name=None):
        super(ProducerThread,self).__init__()
        self.target = target
        self.name = name

    def run(self):
        global done
        while not done:
            if not q.full(): # Producer
                item:tuple = aws.produce()
                if item is None:
                    done_mutex.acquire()
                    done = True
                    done_mutex.release
                q.put(item)
                q_ = q
                logging.debug(f'link({item}), q_size({q.qsize()}), thread({self.ident})')
                # time.sleep(random.random())
        return

class ConsumerThread(threading.Thread):
    def __init__(self, target=None, name=None):
        super(ConsumerThread,self).__init__()
        self.target = target
        self.name = f"name({name}), tid({self.ident}): "
        return

    def run(self):
        global done
        while True:
            if done and q.empty():
                break
            if not q.empty(): # Consumer
                item = q.get()
                if item is None: break
                title, url = item
                c = aws.consume(url)
                logging.debug(f'link({(title, url)}), q_size({q.qsize()}), thread({self.ident})')
                # time.sleep(random.random())
        return

if __name__ == '__main__':
    pr_thread_count, co_thread_count = 4, 4
    producers, consumers = [], []
    for i in range(pr_thread_count):
        producers.append(ProducerThread(name='producer'))
    for i in range(co_thread_count):
        consumers.append(ConsumerThread(name='consumer'))
    
    for prod in producers: prod.start()
    for cons in consumers: cons.start()
    
    for prod in producers: prod.join()
    for cons in consumers: cons.join()

    with open("file.txt", mode="w+", encoding='utf8') as file:
        file.writelines([str(x) for x in aws.registered_actors])
    