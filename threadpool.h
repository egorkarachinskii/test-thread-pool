#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

class ThreadPool {
public:
    ThreadPool(unsigned int size);
    template<class F, class... Args> auto add(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    void start();
    void stop();

private:
    void process();
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool bStopped;
};

ThreadPool::ThreadPool(unsigned int threads):bStopped(true)
{
    for(size_t i = 0;i<threads;++i)
        workers.emplace_back([this] { process();} );
}
void ThreadPool::process()
{
    for(;;)
    {
        if(bStopped)
            continue;
        std::function<void()> task;
        std::unique_lock<std::mutex> lock(queueMutex);
        condition.wait(lock, [this]{ return bStopped || !tasks.empty(); });
        task = std::move(tasks.front());
        tasks.pop();
        task();
    }

}
void ThreadPool::start()
{
    std::cout<< "pool started;"<<std::endl;
    bStopped=false;
}

void ThreadPool::stop()
{
    std::cout<< "pool stopped;"<<std::endl;
    bStopped=true;
}


template<class F, class... Args> auto ThreadPool::add(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared< std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> res = task->get_future();

    std::unique_lock<std::mutex> lock(queueMutex);

    tasks.emplace([task](){ (*task)(); });

    condition.notify_one();
    return res;
}
