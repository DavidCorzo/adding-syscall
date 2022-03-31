#include <iostream>
#include "queue.h"

int func() {
    cout << "executed";
    return 0;
}

int main() {
    _queue<int (*)()> q;
    q.enqueue(func);
    q.q->at(0)();
}