import threading
import time
import logging
import random
import queue
import webscrapping

logging.basicConfig(level=logging.DEBUG, format='(%(threadName)-9s) %(message)s',)

BUF_SIZE = 10
q = queue.Queue(BUF_SIZE)
aws = webscrapping.ActorWebScrapper(
    url="https://en.wikipedia.org/wiki/List_of_American_film_actresses"
)

class ProducerThread(threading.Thread):
    def __init__(self, target=None, name=None):
        super(ProducerThread,self).__init__()
        self.target = target
        self.name = name

    def run(self):
        while True:
            if not q.full():
                item = next(aws.producer_data)
                q.put(item)
                logging.debug(f'Putting {item} : {q.qsize()} items in queue')
                time.sleep(random.random())
        return

class ConsumerThread(threading.Thread):
    def __init__(self, target=None, name=None):
        super(ConsumerThread,self).__init__()
        self.target = target
        self.name = name
        return

    def run(self):
        while True:
            if not q.empty():
                title, url = q.get()
                logging.debug(f'Getting {(title, url)} from queue')
                # logging.debug('Getting ' + str(item) + ' : ' + str(q.qsize()) + ' items in queue')
                time.sleep(random.random())
        return

if __name__ == '__main__':
    p = ProducerThread(name='producer')
    c = ConsumerThread(name='consumer')

    p.start()
    time.sleep(2)
    c.start()
    time.sleep(2)