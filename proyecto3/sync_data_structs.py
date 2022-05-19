import threading

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
    
    def get_val(self, key):
        self.mutex.acquire()
        item = self.get(key)
        self.mutex.release()
        return item

class safe_set(set): # not used
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