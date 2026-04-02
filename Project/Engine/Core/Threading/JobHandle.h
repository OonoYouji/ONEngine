#pragma once

/// std
#include <future>

namespace ONEngine {

template<typename T>
class JobHandle {
public:
	JobHandle(std::future<T>&& future)
		: future_(std::move(future)) {
	}

	bool IsReady() const {
		return future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}

	T Get() {
		return future_.get();
	}

private:
	std::future<T> future_;
};


} /// namespeace ONEngine