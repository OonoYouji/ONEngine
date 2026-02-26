#include "ThreadPool.h"

namespace ONEngine {

thread_local WorkerContext* ThreadPool::tlsContext_ = nullptr;

ThreadPool& ThreadPool::Instance() {
	static ThreadPool instance;
	return instance;
}

WorkerContext* ThreadPool::GetWorkerContext() {
	return tlsContext_;
}

ThreadPool::~ThreadPool() {
	Shutdown();
}

void ThreadPool::Initialize(DxDevice* device, size_t threadCount) {
	if(running_) return;

	device_ = device;
	running_ = true;
	threadCount_ = threadCount == 0 ? 1 : threadCount;

	contexts_.resize(threadCount_);

	for(size_t i = 0; i < threadCount_; ++i) {
		contexts_[i].index = i;
		workers_.emplace_back([this, i]() { WorkerLoop(i); });
	}
}

void ThreadPool::Shutdown() {
	if(!running_) return;

	running_ = false;

	// スレッドを起こす
	for(size_t i = 0; i < workers_.size(); ++i) {
		jobQueue_.Push([] {});
	}

	for(auto& t : workers_) {
		if(t.joinable()) t.join();
	}

	workers_.clear();
	contexts_.clear();
}

void ThreadPool::WorkerLoop(size_t index) {
	tlsContext_ = &contexts_[index];

	// スレッド専用 UploadCommand
	tlsContext_->uploadCmd = std::make_unique<DxUploadCommand>();
	tlsContext_->uploadCmd->Initialize(device_);

	while(running_ || !jobQueue_.Empty()) {
		std::function<void()> job;
		if(jobQueue_.TryPop(job)) {
			job();
		} else {
			std::this_thread::yield();
		}
	}

	tlsContext_ = nullptr;
}

} // namespace ONEngine
