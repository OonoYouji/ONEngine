#pragma once

/// std
#include <future>

namespace ONEngine {

/**
 * @class JobHandle
 * @brief 非同期タスク（ジョブ）の実行状況の確認および実行結果の受け取りを行うためのハンドルテンプレートクラス
 * @tparam T ジョブの返り値の型
 */
template<typename T>
class JobHandle {
public:
	/**
	 * @brief std::futureを移動してハンドルを初期化するコンストラクタ。
	 * @param future 追跡対象のstd::futureインスタンスの右辺値参照
	 */
	JobHandle(std::future<T>&& future)
		: future_(std::move(future)) {
	}

	/**
	 * @brief 非同期処理が完了し、結果が取得可能になったかどうかを即時判定します。
	 * @return 完了していれば true、実行中または未完了であれば false
	 */
	bool IsReady() const {
		return future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}

	/**
	 * @brief 非同期処理の完了を待機し、結果値を取得します（呼び出し元スレッドをブロックします）。
	 * @return 非同期タスクの実行結果
	 */
	T Get() {
		return future_.get();
	}

private:
	std::future<T> future_; ///< 内部管理するstd::future
};


} /// namespeace ONEngine