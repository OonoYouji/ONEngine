#pragma once

/// std
#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <atomic>
#include <memory>

/// engine
#include "ConcurrentQueue.h"


namespace ONEngine {

class ThreadPool {
public:
	static ThreadPool& Instance();

	void Initialize(size_t threadCount = std::thread::hardware_concurrency());
	void Shutdown();

	// 任意の関数を非同期実行
	template<class F, class... Args>
	auto Enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
		using ReturnType = std::invoke_result_t<F, Args...>;

		auto task = std::make_shared<std::packaged_task<ReturnType()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		std::future<ReturnType> result = task->get_future();

		jobQueue_.Push([task]() { (*task)(); });

		return result;
	}

	size_t GetThreadCount() const { return threadCount_; }

private:
	ThreadPool() = default;
	~ThreadPool();

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	void WorkerLoop();

private:
	std::vector<std::thread> workers_;
	ConcurrentQueue<std::function<void()>> jobQueue_;

	std::atomic<bool> running_ = false;
	size_t threadCount_ = 0;
};


} /// namespeace ONEngine