#pragma once

/// std
#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <atomic>
#include <memory>

/// engine
#include "Engine/Core/DirectX12/Command/DxUploadCommand.h"
#include "ConcurrentQueue.h"

namespace ONEngine {

class DxDevice;
class DxUploadCommand;

/// 各Workerスレッドが保持するコンテキスト
struct WorkerContext {
	size_t index;
	std::unique_ptr<DxUploadCommand> uploadCmd;
};

class ThreadPool {
public:
	static ThreadPool& Instance();

	void Initialize(DxDevice* device, size_t threadCount = std::thread::hardware_concurrency());
	void Shutdown();

	template<class F, class... Args>
	auto Enqueue(F&& f, Args&&... args)
		-> std::future<std::invoke_result_t<F, Args...>> {
		using ReturnType = std::invoke_result_t<F, Args...>;

		auto task = std::make_shared<std::packaged_task<ReturnType()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		std::future<ReturnType> result = task->get_future();

		jobQueue_.Push([task]() { (*task)(); });

		return result;
	}

	size_t GetThreadCount() const { return threadCount_; }

	/// 現在のWorkerContext取得（Workerスレッド内のみ）
	static WorkerContext* GetWorkerContext();

private:
	ThreadPool() = default;
	~ThreadPool();

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	void WorkerLoop(size_t index);

private:
	std::vector<std::thread> workers_;
	ConcurrentQueue<std::function<void()>> jobQueue_;

	std::atomic<bool> running_ = false;
	size_t threadCount_ = 0;

	DxDevice* device_ = nullptr;

	std::vector<WorkerContext> contexts_;

	static thread_local WorkerContext* tlsContext_;
};

} // namespace ONEngine
