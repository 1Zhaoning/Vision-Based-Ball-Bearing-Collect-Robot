#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace mms{
class ThreadPool
/**
    @brief A very simple threadpooling class with a work queue and a set number of threads to use
*/
{
public:
    /**
        @brief el-constructos
        @param[in] num_threads The number of thread workers to use (defaults to number of hardware threads).
        Note you will need to manually tune this number for performance, using more threads is not always better.
    */
    ThreadPool(uint num_threads = std::thread::hardware_concurrency());

    /**
        @brief Destructor will empty the work queue, but will block the calling thread
        until all workers have finished their current job.
    */
    ~ThreadPool();

    /**
        @brief Method to add 'jobs' for the thread workers to do.
        @param[in] new_job A functor and params list that can be bound to a std::function<void()> type.

        @details
        The return value of the function will be discarded, so if you require outputs pass them by non-const reference.
        If you want a function (not a method) use:
        @code{.cpp}
            my_threadpool.add_job(&function_name, arg1, arg2 ...);
        @endcode

        If you want to use a method you must use:
        @code{.cpp}
            my_threadpool.add_job(&class_name::method_name, &object, arg1, arg2 ...);
            // OR
            my_threadpool.add_job(&class_name::method_name, this, arg1, arg2 ...);
        @endcode
        Note the number of arguements does not matter and can be zero.
    */
    template<typename F, typename ... Param>
    void add_job(F &&f, Param && ... param){
        {
            std::unique_lock<std::mutex> lock(_queue_mtx);
            _job_queue.push_back(std::bind(std::forward<F>(f), std::forward<decltype(param)>(param)...));
        }
        _jobs_active++;
        _job_queue_cond_var.notify_one();
    }

    /**
        @brief Blocks the calling thread until all jobs in the queue have been completed.
        If you do not call this function, your program may continue running before the workers have completed any jobs,
        likely introducing bugs.
    */
    void wait_for_completion();

private:
    std::vector<std::thread> _thread_runners;
    std::deque<std::function<void()>> _job_queue;
    std::mutex _queue_mtx;
    std::mutex _active_mtx;

    std::condition_variable _active_cond_var;
    std::condition_variable _job_queue_cond_var;

    std::atomic_bool _is_shutdown{false};
    std::atomic_int _jobs_active{0};

    void worker_loop();
};
} // namespace mms