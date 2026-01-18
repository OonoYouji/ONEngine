#pragma once

#include <d3d12.h>

/// engine
#include "../ComPtr/ComPtr.h"
#include "../Resource/DxResource.h"
#include <Windows.h>
#include <cstdint>

namespace ONEngine {

class DxDevice;
class DxCommand;

/// @brief GPU 上での処理時間をタイムスタンプクエリで計測するユーティリティクラス
///
/// ID ごとに Begin / End の 2 点のタイムスタンプを発行し、
/// CPU 側で GPU 実行時間（ms）として取得する。
/// Singleton として使用される。
class GPUTimeStamp {

	GPUTimeStamp() = default;
	~GPUTimeStamp() = default;
	GPUTimeStamp(const GPUTimeStamp&) = delete;
	GPUTimeStamp& operator=(const GPUTimeStamp&) = delete;
	GPUTimeStamp(const GPUTimeStamp&&) = delete;
	GPUTimeStamp& operator=(const GPUTimeStamp&&) = delete;

public:

	/// @brief GPUTimeStamp のシングルトンインスタンスを取得する
	/// @return GPUTimeStamp の参照
	static GPUTimeStamp& GetInstance() {
		static GPUTimeStamp instance;
		return instance;
	}

	/// @brief GPU タイムスタンプ計測用リソースを初期化する
	/// @param dxDevice D3D12 デバイスラッパー
	/// @param dxCommand コマンドリスト／キューを管理するクラス
	/// @param maxTimerCount 同時に使用可能なタイマー ID の最大数
	void Initialize(DxDevice* dxDevice, DxCommand* dxCommand, UINT maxTimerCount);

	/// @brief 指定 ID の GPU タイムスタンプ計測を開始する
	///
	/// この関数は内部的にタイムスタンプクエリを発行する。
	/// 同じ ID に対して EndTimeStamp を必ず呼ぶ必要がある。
	///
	/// @param id タイマー ID（0 ～ maxTimerCount - 1）
	void BeginTimeStamp(uint32_t id);

	/// @brief 指定 ID の GPU タイムスタンプ計測を終了する
	///
	/// BeginTimeStamp と対になるタイムスタンプを発行し、
	/// クエリ結果を ReadBack バッファへ Resolve する。
	///
	/// @param id タイマー ID（0 ～ maxTimerCount - 1）
	void EndTimeStamp(uint32_t id);

	/// @brief 指定 ID の GPU 実行時間をミリ秒単位で取得する
	///
	/// Begin / End の両方が正しく実行されていない場合や、
	/// GPU 側でまだ結果が書き込まれていない場合は負の値を返す。
	///
	/// @param id タイマー ID（0 ～ maxTimerCount - 1）
	/// @return GPU 実行時間（ミリ秒）。取得できない場合は負の値。
	double GetTimeStampMSec(uint32_t id);

private:

	/// @brief タイマー ID からクエリヒープ上のインデックスを計算する
	/// @param id タイマー ID
	/// @return クエリヒープ上の開始インデックス
	uint32_t GetIndex(uint32_t id);

private:
	/// 1 タイマーあたりに使用するタイムスタンプ数（開始・終了）
	static constexpr size_t kTimestampPerTimer = 2;

	DxCommand* pDxCommand_ = nullptr;

	ComPtr<ID3D12QueryHeap> queryHeap_;
	DxResource readBackResource_;

	uint64_t gpuTimestampFrequency_ = 0;
	uint32_t maxTimerCount_ = 10;
};

} /// namespace ONEngine
