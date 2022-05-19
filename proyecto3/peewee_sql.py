from peewee import SqliteDatabase, AutoField, CharField, Model
from sqlalchemy import true

db = SqliteDatabase('movies.db')

class actress(Model):
    actress_id      = CharField(unique=True) # autoincrement primary key
    actress_name    = CharField(unique=True)
    actress_url     = CharField()
    id_consumidor   = CharField()
    id_productor    = CharField()
    class Meta: database = db

class films(Model):
   film_id       = CharField(unique=True) # autoincrement primary key
   movie_name    = CharField(unique=True)
   film_url      = CharField()
   id_consumidor = CharField()
   class Meta:
       database = db

class actress_filmography(Model):
    actress_id       = CharField() # url actor
    film_id         = CharField() # url film
    id_consumidor   = CharField()
    class Meta:
        database = db

def insert_actress(actress_id, actress_name, actress_url, id_consumidor, id_productor):
    trans = actress(
        actress_id = actress_id,
        actress_name = actress_name,
        actress_url = actress_url,
        id_consumidor = id_consumidor,
        id_productor = id_productor
    )
    trans.save()

def insert_actress_filmography(actress_id, film_id, id_consumidor):
    trans = actress_filmography(
        actress_id = actress_id,
        film_id = film_id,
        id_consumidor = id_consumidor
    )
    trans.save()

def insert_film(actress_id, film_id, movie_name, film_url, id_consumidor):
    # needs to exists prior to movie insertion
    insert_actress_filmography(actress_id, film_id, id_consumidor)
    try:
        chamo = films(
            film_id = film_id,
            movie_name = movie_name,
            film_url = film_url,
            id_consumidor = id_consumidor
        )
        chamo.save()
    except:
        # movie is already in database.
        print(f"ERROR: {movie_name} {id_consumidor}")
            
