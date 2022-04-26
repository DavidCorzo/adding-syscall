#ifndef SAFE_QUEUE_H
    #define SAFE_QUEUE_H
#include <iostream>
#include <mutex>
#include <queue>

/*
Esta basicamente
*/

#define LOCK_QUEUE(m) std::unique_lock<std::mutex> lock(m);

template <typename T>
class safe_queue {
    private:
        std::queue<T> q;
        std::mutex  m;
    public:
        safe_queue() {}
        ~safe_queue() {}
    
    bool empty() {
        // lock the queue before accesing
        LOCK_QUEUE(m);
        return q.empty();
    }

    int size() {
        LOCK_QUEUE(m);
        return q.size();
    }

    void enqueue(T& t) {
        LOCK_QUEUE(m);
        q.push(t);
    }

    bool dequeue(T &t) {
        LOCK_QUEUE(m);
        if (q.empty()) {
            return false;
        }
        t = std::move(q.front());
        q.pop();
        return true;
    }
};

#endif