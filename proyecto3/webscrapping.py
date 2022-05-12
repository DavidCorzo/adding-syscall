import bs4
import requests
import re
from typing import Tuple
from threading import Lock
from queue import Queue

# page = requests.get(URL)
class ActorWebScrapper:
    url_prefix = "https://en.wikipedia.org/"
    def __init__(self, url, buff_size):
        self.url = url
        self.page = requests.get(url)
        self.soup = bs4.BeautifulSoup(self.page.content, 'html.parser')
        self.names_urls = set()
        self.lock = Lock()
        self.buffer = Queue(maxsize=buff_size)
    
    def producer_work(self) -> Tuple[str, str]:
        for div in (self.soup.find_all("div", {"class":'div-col', "style": 'column-width: 25em;'})):
            for ul in div.find_all("ul"):
                for li in ul.find_all("li"):
                    for a in li.find_all("a"):
                        title = a.string
                        url = a.get("href")
                        if re.match(r"[a-zA-Z\(\)%]+", title):
                            actress_url = None
                            if re.match(r"/wiki/[a-zA-Z_]", url):
                                actress_url = tuple([title, url.replace("/wiki", "")])
                            elif re.match(r"/w/index\.php\?title=[a-zA-Z]&action=edit&redlink=1", url):
                                actress_url = tuple([title, None])
                            # dont return duplicates.
                            if actress_url in self.names_urls:
                                yield actress_url
                            else:
                                continue
                        else:
                            print(title, url)
    
    def producer_send_work_to_queue(self):
        pass
    
    def consumer_work(self, actress_url):
        if actress_url:
            pass
        else:
            pass
        pass
    
    def produce():

        pass

    def consume():

        pass
    

aws = ActorWebScrapper("https://en.wikipedia.org/wiki/List_of_American_film_actresses", 10)