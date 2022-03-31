#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

template <class _queue_type>
class _queue {
    public:
        vector<_queue_type> *q;
    _queue() {
        q = new vector<_queue_type>();
    }
    ~_queue() {
        delete q;
    }
    bool enqueue(_queue_type element) {
        q->insert(q->begin(), element);
        return true;
    }
    _queue_type dequeue() {
        if (q->size() == 0) {
            cout << "### WARNING: popping from an empty queue.";
            return -1;
        }
        _queue_type back_value {q->back()};
        q->pop_back();
        return back_value;
    }
    void print() {
        for (uint32_t i {0}; i < q->size(); i++) {
            cout << " " << q->at(i);
        }
        cout << endl;
    }

    uint32_t size() {
        return q->size();
    }
};