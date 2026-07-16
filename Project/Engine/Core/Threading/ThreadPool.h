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

/**
 * @class ThreadPool
 * @brief アプリケーション全体での並列処理を可能にする汎用スレッドプールクラス
 */
class ThreadPool {
public:
	/**
	 * @brief シングルトンインスタンスを取得します。
	 * @return ThreadPoolインスタンスの参照
	 */
	static ThreadPool& Instance();

	/**
	 * @brief スレッドプールの初期化を行い、指定された数のワーカースレッドを起動します。
	 * @param device コマンドリスト生成用のDxDeviceポインタ
	 * @param threadCount 生成するスレッド数（デフォルトはCPUコア数）
	 */
	void Initialize(DxDevice* device, size_t threadCount = std::thread::hardware_concurrency());

	/**
	 * @brief 全スレッドに終了をシグナルし、ジョブが完了するのを待機してスレッドプールをシャットダウンします。
	 */
	void Shutdown();

	/**
	 * @brief ジョブ（タスク）をキューに登録し、実行結果を待機するための future を返します。
	 * @tparam F 関数型
	 * @tparam Args 引数型群
	 * @param f 実行する関数
	 * @param args 関数の引数群
	 * @return ジョブの実行結果を追跡するための std::future オブジェクト
	 */
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

	/**
	 * @brief 起動しているワーカースレッドの数を取得します。
	 * @return ワーカースレッド数
	 */
	size_t GetThreadCount() const { return threadCount_; }

	/**
	 * @brief 現在この関数を呼び出したスレッドのワーカースレッド固有コンテキスト（WorkerContext）を取得します（スレッドローカル）。
	 * @return WorkerContextポインタ（プール外スレッドからの呼び出し時は nullptr）
	 */
	static WorkerContext* GetWorkerContext();

private:
	ThreadPool() = default;
	~ThreadPool();

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	/**
	 * @brief 各ワーカースレッドで無限ループとして動作するジョブ取得・実行ループ。
	 * @param index ワーカースレッドのインデックス
	 */
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
