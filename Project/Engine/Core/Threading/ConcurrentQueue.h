#pragma once

/// std
#include <queue>
#include <mutex>


namespace ONEngine {


/**
 * @class ConcurrentQueue
 * @brief スレッドセーフなキュー（デキュー/エンキュー）を提供するテンプレートクラス
 * @tparam T 格納するオブジェクトの型
 */
template<typename T>
class ConcurrentQueue {
public:
	/**
	 * @brief 要素をキューの末尾に追加します（スレッドセーフ）。
	 * @param value 追加する値
	 */
	void Push(const T& value) {
		std::lock_guard<std::mutex> lock(mutex_);
		queue_.push(value);
	}

	/**
	 * @brief キューの先頭から要素を取り出そうと試みます。空の場合は即時復帰します（スレッドセーフ）。
	 * @param result 取り出した要素を格納する参照
	 * @return 要素を取り出すのに成功すれば true、キューが空であれば false
	 */
	bool TryPop(T& result) {
		std::lock_guard<std::mutex> lock(mutex_);
		if(queue_.empty()) return false;
		result = std::move(queue_.front());
		queue_.pop();
		return true;
	}

	/**
	 * @brief キューが現在空かどうかを判定します（スレッドセーフ）。
	 * @return 空であれば true、要素があれば false
	 */
	bool Empty() const {
		std::lock_guard<std::mutex> lock(mutex_);
		return queue_.empty();
	}

private:
	mutable std::mutex mutex_; ///< 同期用ミューテックス
	std::queue<T> queue_;      ///< 内部コンテナ
};

} /// namespeace ONEngine