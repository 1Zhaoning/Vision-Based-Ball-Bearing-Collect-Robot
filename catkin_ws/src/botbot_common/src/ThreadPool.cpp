#include "../include/botbot_common/ThreadPool.hpp"
#include <functional>
#include <thread>

using namespace mms;

ThreadPool::ThreadPool(uint num_threads)
{
    _thread_runners.reserve(num_threads);
    for (uint i = 0; i < num_threads; ++i)
    {
        _thread_runners.emplace_back(std::bind(&ThreadPool::worker_loop, this));
    }
}

ThreadPool::~ThreadPool()
{
    _is_shutdown = true;
    {
        std::unique_lock<std::mutex> lock(_queue_mtx);
        _job_queue.clear();
    }
    _job_queue_cond_var.notify_all();
    _active_cond_var.notify_all();
    for (std::thread &worker : _thread_runners)
    {
        worker.join();
    }
}

void ThreadPool::worker_loop()
{
    while (!_is_shutdown)
    {
        std::function<void()> Job;
        {
            std::unique_lock<std::mutex> lock(_queue_mtx);

            _job_queue_cond_var.wait(lock, [this] { return !_job_queue.empty() || _is_shutdown; });
            if (_is_shutdown) { return; }

            Job = _job_queue.front();
            _job_queue.pop_front();
        }

        std::invoke(Job);
        _jobs_active--;

        _active_cond_var.notify_all();
    }
}

void ThreadPool::wait_for_completion()
{
    {
        std::unique_lock<std::mutex> lock(_active_mtx);
        _active_cond_var.wait(lock, [this]() { return _jobs_active == 0; });
    }
}