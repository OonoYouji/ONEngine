#pragma once

/// std
#include <queue>
#include <mutex>


namespace ONEngine {


template<typename T>
class ConcurrentQueue {
public:
	void Push(const T& value) {
		std::lock_guard<std::mutex> lock(mutex_);
		queue_.push(value);
	}

	bool TryPop(T& result) {
		std::lock_guard<std::mutex> lock(mutex_);
		if(queue_.empty()) return false;
		result = std::move(queue_.front());
		queue_.pop();
		return true;
	}

	bool Empty() const {
		std::lock_guard<std::mutex> lock(mutex_);
		return queue_.empty();
	}

private:
	mutable std::mutex mutex_;
	std::queue<T> queue_;
};

} /// namespeace ONEngine