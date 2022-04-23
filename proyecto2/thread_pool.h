#include <iostream>
#include <thread>
#include <queue>

using std::queue;
using std::thread;
using std::cout;
using std::endl;

typedef void (*work_unit_ptr_t)();

class thread_pool {
    public:
        queue<work_unit_ptr_t> work_queue;
        thread *threads;
        bool is_shutdown;
        // c++ makes default constructors under the hood which we cannot use. 
        // A threadpool must only have one instance, therefore this threadpool needs to be unique so
        // we delete the default construtors.
        thread_pool(const thread_pool &) = delete; // ref constructor.
        thread_pool(thread_pool &&) = delete; // r value constructor
        thread_pool &operator=(const thread_pool &) = delete; // 
        thread_pool &operator=(thread_pool &&) = delete; // move constructor
    
    thread_pool(int16_t thread_amount) : is_shutdown{false} {
        if (!thread_amount) {
            cout << "Thread must be non-zero" << endl;
            exit(-1);
        }
        threads = new thread[thread_amount];
        
    }

    ~thread_pool() {

    }
            
};  