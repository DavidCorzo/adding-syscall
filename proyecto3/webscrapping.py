from typing import Tuple

from numpy import insert
from sync_data_structs import safe_dict
import bs4
import requests
import re
import threading
from peewee_sql import db, insert_actress, insert_actress_filmography, insert_film, actress, actress_filmography, films

def pk_gen():
    i = 0
    while True:
        yield str(i)
        i += 1

class ActorWebScrapper:
    url_prefix = "https://en.wikipedia.org"
    def __init__(self, url:str):
        self.url = url
        self.registered_actors  = safe_dict()
        self.registered_movies  = safe_dict()
        self.movie_unmatches    = safe_dict()
        self.registered_movies  = safe_dict()
        self.producer_data      = self.producer_work()
        self.mutex              = threading.Lock()
        self.pk_mutex           = threading.Lock()
        self.cons_mutex         = threading.Lock()
        self.movie_pk_gen       = pk_gen()
        self.actor_pk_gen       = pk_gen()
        db.connect()
        db.create_tables([actress, films, actress_filmography])
    
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
                        if title:
                            actress_url = None
                            if re.match(r"/wiki/.+", url):
                                actress_url = url
                            elif re.match(r"/w/index\.php\?title=[a-zA-Z]&action=edit&redlink=1", url):
                                actress_url = None
                            # dont return duplicates.
                            if (actress_url):
                                if (not self.registered_actors.safe_check(url)):
                                    pk = self.get_actor_pk()
                                    self.registered_actors.safe_add(url, tuple((title, pk)))
                                    yield (url, title)
                                else:
                                    continue
                            else:
                                continue
                        else:
                            continue

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
    
    def consumer_work(self, actress_url):
        movies = set()
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
            return None
        soup = bs4.BeautifulSoup(content, 'html.parser')
        has_tables = bool(soup.find('table'))
        has_lists  = bool(soup.find('ul'))
        if (has_tables ^ has_lists):
            if has_tables:
                for table in soup.findAll("a"):
                    title = table.get('title')
                    href = table.get('href')
                    if href and (not self.registered_movies.safe_check(href)):
                        if title and re.match(r"/wiki/[a-zA-Z_]", href):
                            pk = None
                            if self.registered_movies.safe_check(href):
                                pk = self.registered_movies.get_val(href)
                            else:
                                pk = self.get_movie_pk()
                                self.registered_movies.safe_add(href, pk)
                            movies.add(tuple((href, title, pk)))
                        else:
                            # self.movie_unmatches.safe_add(href, tuple((title, )))
                            pass
            elif has_lists:
                for ul in soup.findAll("ul"):
                    for li in ul.findAll("li"):
                        for i in li.findAll("i"):
                            for a in i.findAll("a"):
                                title = a.get('title')
                                href  = a.get('href')
                                if href and (not self.registered_movies.safe_check(href)):
                                    if title and re.match(r"/wiki/[a-zA-Z_]", href):
                                        pk = None
                                        if self.registered_movies.safe_check(href):
                                            pk = self.registered_movies.get_val(href)
                                        else:
                                            pk = self.get_movie_pk()
                                            self.registered_movies.safe_add(href, pk)
                                        movies.add(tuple((href, title, pk)))
                                    else:
                                        # self.movie_unmatches.safe_add(href, tuple((title, )))
                                        pass
        else: # has_tables == has_lists
            for i in soup.findAll("i"):
                for a in i.findAll("a"):
                    title = a.get('title')
                    href  = a.get('href')
                    if href and (not self.registered_movies.safe_check(href)):
                        if title and re.match(r"/wiki/[a-zA-Z_]", href):
                            pk = None
                            if self.registered_movies.safe_check(href):
                                pk = self.registered_movies.get_val(href)
                            else:
                                pk = self.get_movie_pk()
                                self.registered_movies.safe_add(href, pk)
                            movies.add(tuple((href, title, pk)))
                        else:
                            pass
        return movies
    
    def consume(self, actress_url, id_consumidor, id_productor):
        if actress_url:
            movies = self.consumer_work(actress_url)
            actress_tuple = self.registered_actors.get_val(actress_url) # gets the actress id
            if movies and actress_tuple:
                actress_name, actress_id = actress_tuple
                self.cons_mutex.acquire()
                insert_actress(actress_id=actress_url, actress_name=actress_name, actress_url=actress_url, id_productor=id_productor, id_consumidor=id_consumidor)
                self.cons_mutex.release()
                for film_url, movie_name, film_id in movies:
                    self.cons_mutex.acquire()
                    insert_film(
                        actress_id=actress_url, 
                        film_id=film_url,
                        movie_name=movie_name,
                        film_url=film_url,
                        id_consumidor=id_consumidor
                    )
                    self.cons_mutex.release()
                    print('\t' + film_url, movie_name, film_id)
        else:
            # nothing to do, actress_url is null.
            pass
    
    def produce(self):
        self.mutex.acquire()
        item = next(self.producer_data, None)
        self.mutex.release()
        return item

# aws = ActorWebScrapper('https://en.wikipedia.org/wiki/List_of_American_film_actresses')
# for i in aws.producer_data:
#     print(i)
#     cons = aws.consume(i[0], 120, 100)
#     if cons:
#         for i in cons:
#             print('\t'+str(i))




