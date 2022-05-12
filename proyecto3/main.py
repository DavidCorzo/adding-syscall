from concurrent.futures import ThreadPoolExecutor, thread
from dataclasses import dataclass
import threading
import sqlalchemy as db
from queue import Queue
from webscrapping import WebScrapper
from random import randint
import time

# docker exec -it mysql_docker bash

# dbconfig = {
#     'host':'localhost',
#     'port':3306,
#     'user':'admin',
#     'password':'123',
#     'database':'test'
# }
# connection_url = f"mysql+pymysql://{dbconfig['user']}:{dbconfig['password']}@{dbconfig['host']}:{dbconfig['port']}/{dbconfig['database']}"
# engine = db.create_engine(connection_url)
# connection = engine.connect()

# exit()


@dataclass
class actor:
    name:str
    url:str

class ConsumerProducer:
    def __init__(self, buffer_size, url, consumer_threads=4, producer_threads=4):
        self.q:Queue        = Queue(buffer_size)
        self.buff_size      = buffer_size
        self.producer_lock  = threading.Lock()
        self.consumer_lock  = threading.Lock()
        self.buff_index     = 0 # shared resource
        self.consumer_tp    = ThreadPoolExecutor(max_workers=consumer_threads)
        self.producer_tp    = ThreadPoolExecutor(max_workers=producer_threads)
    
    def producer_work(self):
        
        pass
    
    def consumer_work(self):
        
        pass
    
    def produce(self):
        while True:
            if not self.q.full():
                self.producer_tp.submit()
                # begin: area critica
                self.producer_lock.acquire()
                self.buff_index += 1 
                self.consumer_lock.release()
                # end: area critica

    def consume(self):
        while True:
            if not self.q.empty():
                item = self.q.get()
                print(item)
                time.sleep(1)
                # begin: area critica
                self.consumer_lock.acquire()
                self.buff_index = (self.buff_index + 1) % self.buff_size
                self.producer_lock.release()
                # end: area critica


NUM_THREADS = 4

if __name__ == '__main__':
    cp = ConsumerProducer(buffer_size=10, url="https://en.wikipedia.org/wiki/List_of_American_film_actresses")
    
    pass