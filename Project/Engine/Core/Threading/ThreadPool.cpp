#include "ThreadPool.h"

namespace ONEngine {

ThreadPool& ThreadPool::Instance() {
    static ThreadPool instance;
    return instance;
}

ThreadPool::~ThreadPool() {
    Shutdown();
}

void ThreadPool::Initialize(size_t threadCount) {
    if(running_) return;

    running_ = true;
    threadCount_ = threadCount;

    for(size_t i = 0; i < threadCount_; ++i) {
        workers_.emplace_back(&ThreadPool::WorkerLoop, this);
    }
}

void ThreadPool::Shutdown() {
    if(!running_) return;

    running_ = false;

    // 空ジョブを入れてスレッドを起こす
    for(size_t i = 0; i < workers_.size(); ++i) {
        jobQueue_.Push([] {});
    }

    for(auto& t : workers_) {
        if(t.joinable()) t.join();
    }

    workers_.clear();
}

void ThreadPool::WorkerLoop() {
    while(running_) {
        std::function<void()> job;
        if(jobQueue_.TryPop(job)) {
            job();
        } else {
            std::this_thread::yield();
        }
    }
}

} /// namespeace ONEngine