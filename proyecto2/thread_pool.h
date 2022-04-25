#ifndef THREAD_POOL_H_
    #define THREAD_POOL_H_
#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <utility>
#include <future>
#include <condition_variable>
#include "safe_queue.h"

using std::queue;
using std::thread;
using std::cout;
using std::endl;

typedef double (*work_func)(stats)

class thread_pool {
    private:
        bool is_shutdown;
        safe_queue<std::function<void()>> work_queue; // modified the normal queue to lock during each operation.
        thread *threads; // will allocate the threads required.
        int16_t amount_of_threads;
        std::mutex conditional_mutex; // used to lock this resource.
        std::condition_variable conditional_lock; // 
    
        class thread_worker {
            private:
                int tid;
                thread_pool *tp;
            public:
                thread_worker(thread_pool *tp_, int tid_) : tp{tp_}, tid{tid_} {}
            
            void operator()() {
                bool dequeued;
                std::function<void()> work;
                while (!tp->is_shutdown) {
                    std::unique_lock<std::mutex> lock(tp->conditional_mutex);
                    if (tp->work_queue.empty()) {
                        tp->conditional_lock.wait(lock);
                    }
                    dequeued = tp->work_queue.dequeue(work);
                    if (dequeued) {
                        work();
                    }
                }
            }
        };
    public:
        static uint16_t instances;
        // c++ makes default constructors under the hood which we cannot use. 
        // A threadpool must only have one instance, therefore this threadpool needs to be unique so
        // we delete the default constructors.
        thread_pool(const thread_pool &) = delete; // ref constructor.
        thread_pool(thread_pool &&) = delete; // r value constructor
        thread_pool &operator=(const thread_pool &) = delete; // 
        thread_pool &operator=(thread_pool &&) = delete; // move constructor
    
    thread_pool(int16_t thread_amount) : is_shutdown{false}, amount_of_threads{thread_amount} {
        if (instances != 0) {
            // solo admitimos UNA instancia de threadpool.
            cout << "### ERROR: more than one thread pool has been spawned." << endl;
            exit(-1);
        }
        if (!thread_amount) {
            cout << "### ERROR: Thread must be non-zero" << endl;
            exit(-1);
        }
        threads = new thread[thread_amount];
        if (!threads) {
            cout << "### ERROR: threads array dynamic allocation returned null." << endl;
            exit(-1);
        }
        instances++;
    }

    ~thread_pool() {
        delete[] threads;
    }

    void init() {
        for (uint32_t i {0}; i < amount_of_threads; i++) {
            threads[i] = std::thread(thread_worker(this, i));
        }
    }
    
    void shutdown() {
        /*
        Este metodo simplemente notifica a todos los threads que es hora 
        de hacer join.
        */
        is_shutdown = true;
        conditional_lock.notify_all();
        for (uint32_t i{0}; i < amount_of_threads; i++ ) {
            if (threads[i].joinable()) {
                threads[i].join();
            }
        }
    }

    // template <typename ...Args>
    // auto submit(double , )

    // template<typename F, typename...Args>
    // auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
    //     // Create a function with bounded parameters ready to execute
    //     std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    //     // Encapsulate it into a shared ptr in order to be able to copy construct / assign 
    //     auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

    //     // Wrap packaged task into void function
    //     std::function<void()> wrapper_func = [task_ptr]() {
    //     (*task_ptr)(); 
    //     };

    //     // Enqueue generic wrapper function
    //     work_queue.enqueue(wrapper_func);

    //     // Wake up one thread if its waiting
    //     conditional_lock.notify_one();

    //     // Return future from promise
    //     return task_ptr->get_future();
    // }

    // template <typename function_t, typename...args_t>
    // auto submit_work(function_t&& f, args_t&&...args) {
    //     /*
    //     El template significa que es una función veriadica (acepta una candidad
    //     no definida de argumentos, similar a printf por ejemplo).
    //     */
    //     std::function<decltype(f(args...))()> function = std::bind(std::forward<function_t>(f), std::forward<args_t>(args)...);
    //     // lo siguiente es para hacer un espacio compartido shared_ptr.
    //     auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(function);
    //     // es para hacer una función con wrapper.
    //     std::function<void()> wrapper = [task_ptr] {
    //         (*task_ptr)();
    //     };
    //     // meter la función al queue de trabajo.
    //     work_queue.enqueue(wrapper);
    //     // despertamos un hilo.
    //     conditional_lock.notify_one();
    //     // retornamos el valor al hilo en el que se está ejecutando el trabajo.
    //     return task_ptr->get_future();
    // }
};  


#endif