from concurrent.futures import ThreadPoolExecutor, thread
import bs4
import requests
import re
from typing import Tuple
from actor_struct import actor
import threading

def pk_gen():
    i = 0
    while True:
        yield str(i)
        i += 1

class safe_dict(dict):
    def __init__(self):
        super(safe_dict, self).__init__()
        self.mutex = threading.Lock()
    
    def safe_add(self, key, value):
        self.mutex.acquire()
        if key not in self:
            self[key] = value
        self.mutex.release()

    def safe_check(self, key):
        self.mutex.acquire()
        exists = key in self
        self.mutex.release()
        return exists

class safe_set(set):
    def __init__(self):
        super(safe_set, self).__init__()
        self.mutex = threading.Lock()
    
    def safe_add(self, item):
        self.mutex.acquire()
        self.add(item)
        self.mutex.release()
    
    def safe_pop(self, item):
        self.mutex.acquire()
        p = self.pop()
        self.mutex.release()
        return p
    
    def safe_check(self, item):
        self.mutex.acquire()
        exists = item in self
        self.mutex.release()
        return exists

class ActorWebScrapper:
    url_prefix = "https://en.wikipedia.org"
    def __init__(self, url:str):
        self.url = url
        # self.registered_actors = safe_set()
        # self.registered_movies = safe_set()
        # self.movie_unmatches = safe_set()
        # self.registered_movies = safe_set()
        self.registered_actors  = safe_dict()
        self.registered_movies  = safe_dict()
        self.movie_unmatches    = safe_dict()
        self.registered_movies  = safe_dict()
        self.producer_data = self.producer_work()
        self.mutex = threading.Lock()
        self.pk_mutex = threading.Lock()
        self.actor_pk_gen = pk_gen()
        self.movie_pk_gen = pk_gen()
    
    def get_actor_pk(self):
        self.pk_mutex.acquire()
        item = next(self.actor_pk_gen)
        self.pk_mutex.release()
        return item
    
    def get_movie_pk(self):
        self.pk_mutex.acquire()
        item = next(self.movie_pk_gen)
        self.pk_mutex.release()
        return item

    def producer_work(self) -> Tuple[str, str]:
        self.page = requests.get(self.url)
        self.soup = bs4.BeautifulSoup(self.page.content, 'html.parser')
        for div in (self.soup.find_all("div", {"class":'div-col', "style": 'column-width: 25em;'})):
            for ul in div.find_all("ul"):
                for li in ul.find_all("li"):
                    for a in li.find_all("a"):
                        title = a.get('title')
                        url = a.get("href")
                        if title and re.match(r"[a-zA-Z\(\)%]+", title):
                            actress_url = None
                            if re.match(r"/wiki/[a-zA-Z_]", url):
                                actress_url = url
                            elif re.match(r"/w/index\.php\?title=[a-zA-Z]&action=edit&redlink=1", url):
                                actress_url = None
                            # dont return duplicates.
                            if (actress_url):
                                if (not self.registered_actors.safe_check(actress_url)):
                                    actor_tuple = (title, url)
                                    self.registered_actors.safe_add(actor_tuple)
                                    yield actor_tuple
                                else:
                                    continue
                            else:
                                continue
                        else:
                            print(f"Title and url not found.")

    def cut_html_filmography(self, content, url) -> Tuple[bool, str]:
        # cuts html content
        id_film = f"id=\"[a-eg-zA-EG-Z_-]*(((F|f)ilmography)|(Film_and_television_credits))[a-zA-Z_-]*\""
        class_film = f"class=\"mw-headline\""
        order_film = ((class_film, id_film), (id_film, class_film))
        start_filmography = None
        for o in order_film:
            start_filmography = re.search(f'<h2>[ \t\n]*<span\s+{o[0]}\s+{o[1]}>', content, re.DOTALL)
            if start_filmography: break
        if start_filmography is None: return None
        id_ref = f"id=\"[a-qs-zA-QS-Z_-]*(R|r)eference[a-zA-Z_-]*\""
        class_ref = f"class=\"mw-headline\""
        order_ref = ((id_ref, class_ref), (class_ref, id_ref))
        end_filmography = None
        sf = int(start_filmography.start())
        for o in order_ref:
            end_filmography = re.search(f'<h2>[ \t]*<span {class_ref} {id_ref}>', content[sf:], re.DOTALL)
            if end_filmography: break
        if not end_filmography:
            return None
        ef = sf + int(end_filmography.start())
        # # start string.
        return content[sf:ef]
    
    def consume(self, actress_url):
        if actress_url:
            if (self.url_prefix.endswith('/')) ^ (actress_url.startswith('/')):
                url = f"{self.url_prefix}{actress_url}"
            elif (not self.url_prefix.endswith('/')) and (not actress_url.startswith('/')):
                url = f"{self.url_prefix}/{actress_url}"
            elif (self.url_prefix.endswith('/')) and (actress_url.startswith('/')):
                url = f"{self.url_prefix[:-1]}{actress_url}"
            else:
                print("ERROR: url not valid.")
                return
            a_req = requests.get(url)
            content = self.cut_html_filmography(a_req.text, url=actress_url)
            if content is None:
                return False
            soup = bs4.BeautifulSoup(content, 'html.parser')
            has_tables = bool(soup.find('table'))
            has_lists = bool(soup.find('ul'))
            if (has_tables ^ has_lists):
                if has_tables:
                    for table in soup.findAll("a"):
                        title = table.get('title')
                        href = table.get('href')
                        if (title and href) and (re.match(r"[a-zA-Z-_()]+", title) and re.match(r"/wiki/[a-zA-Z_]", href)):
                            self.registered_movies.add_actor((title, href))
                        else:
                            self.movie_unmatches.add_actor((title, href))
                elif has_lists:
                    for ul in soup.findAll("ul"):
                        for li in ul.findAll("li"):
                            for i in li.findAll("i"):
                                for a in i.findAll("a"):
                                    title = a.get('title')
                                    href  = a.get('href')
                                    if (title and href) and (re.match(r"[a-zA-Z-_()]+", title) and re.match(r"/wiki/[a-zA-Z_]", href)):
                                        self.registered_movies.add_actor((title, href))
                                    else:
                                        self.movie_unmatches.add_actor((title, href))
            else: # has_tables == has_lists
                for i in soup.findAll("i"):
                    for a in i.findAll("a"):
                        title = a.get('title')
                        href  = a.get('href')
                        if (title and href) and (re.match(r"[a-zA-Z-_()]+", title) and re.match(r"/wiki/[a-zA-Z_]", href)):
                            self.registered_movies.add_actor((title, href))
                        else:
                            self.movie_unmatches.add_actor((title, href))
        return True
    
    def produce(self):
        self.mutex.acquire()
        item = next(self.producer_data, None)
        self.mutex.release()
        return item
    
    






