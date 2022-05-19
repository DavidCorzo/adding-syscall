from concurrent.futures import ThreadPoolExecutor, thread
from typing import Tuple, Iterable
import threading
import time
import logging
import queue
import webscrapping
import sys
import signal

logging.basicConfig(filename='logging_output.txt', level=logging.DEBUG, format='%(thread)d:%(message)s')

BUF_SIZE = 10
q:Iterable[Tuple[str, str]] = queue.Queue(BUF_SIZE)
aws = webscrapping.ActorWebScrapper(
    url="https://en.wikipedia.org/wiki/List_of_American_film_actresses"
)

done = False
done_mutex = threading.Lock()

ITEM_TO_PROCESS, PRODUCER_ID = 0, 1

class ProducerThread(threading.Thread):
    def __init__(self, target=None, name=None):
        super(ProducerThread,self).__init__()
        self.target = target
        self.name = name

    def run(self):
        global done
        while not done:
            if not q.full(): # Producer
                producer_id = self.ident
                producer_name = self.name
                item:tuple = (aws.produce(), producer_id, producer_name)
                if item[0] is None:
                    done_mutex.acquire()
                    done = True
                    done_mutex.release()
                q.put(item)
                # print(f'{item}')
                logging.debug(f'link({item}), q_size({q.qsize()}), thread({item[PRODUCER_ID]})')
        return

class ConsumerThread(threading.Thread):
    def __init__(self, target=None, name=None):
        super(ConsumerThread,self).__init__()
        self.target = target
        self.name = name
        return

    def run(self):
        global done
        while True:
            if done and q.empty():
                break
            if not q.empty(): # Consumer
                item = q.get()
                if item[0] is None: 
                    continue
                items_to_consume, producer_id, producer_name = item
                consumer_id = self.ident
                consumer_name = self.name
                url, title = items_to_consume
                try:
                    aws.consume(actress_url=url, id_consumidor=consumer_id, id_productor=producer_id)
                except Exception as e:
                    print(f'{items_to_consume} gave errors')
                    print(e)
                    continue
                logging.debug(f'consumer={consumer_name}, title={title}, url={url}')
        return

if __name__ == '__main__':
    # consumer_count, producer_count = sys.argv[0], sys.argv[1]
    # start = time.time()
    pr_thread_count, co_thread_count = 4, 4
    producers, consumers = [], []
    for i in range(pr_thread_count):
        producers.append(ProducerThread(name=f'producer_{i}'))
    for i in range(co_thread_count):
        consumers.append(ConsumerThread(name=f'consumer_{i}'))

    for prod in producers: prod.start()
    for cons in consumers: cons.start()

    for prod in producers: prod.join()
    for cons in consumers: cons.join()

    # print(f'took {time.time() - start} seconds')

    # with open("file.txt", mode="w+", encoding='utf8') as file:
    #     file.writelines([str(x) for x in aws.registered_actors])
    