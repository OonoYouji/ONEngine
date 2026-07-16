#pragma once

/// std
#include <cstdint>
#include <vector>

/// engine
#include "CPUTimeStampID.h"

namespace ONEngine {


/**
 * @class CPUTimeStamp
 * @brief CPU上での処理時間を高精度に計測・記録するタイムスタンプユーティリティ（シングルトン）クラス
 */
class CPUTimeStamp {
	/**
	 * @brief プライベートコンストラクタ。ID数に応じてバッファを確保します。
	 */
	CPUTimeStamp() {
		timeStampData_.resize(static_cast<size_t>(CPUTimeStampID::Count));
	}
	~CPUTimeStamp() = default;
	CPUTimeStamp(const CPUTimeStamp&) = delete;
	CPUTimeStamp& operator=(const CPUTimeStamp&) = delete;
	CPUTimeStamp(CPUTimeStamp&&) = delete;
	CPUTimeStamp& operator=(CPUTimeStamp&&) = delete;
public:

	/**
	 * @brief シングルトンインスタンスを取得します。
	 * @return CPUTimeStampインスタンスの参照
	 */
	static CPUTimeStamp& GetInstance() {
		static CPUTimeStamp instance;
		return instance;
	}

	/**
	 * @brief 特定の処理について高精度タイマーによる計測を開始します。
	 * @param id 計測対象のタイムスタンプID
	 */
	void BeginTimeStamp(CPUTimeStampID id);

	/**
	 * @brief 特定の処理について高精度タイマーによる計測を終了し、経過時間を記録します。
	 * @param id 計測対象のタイムスタンプID
	 */
	void EndTimeStamp(CPUTimeStampID id);

	/**
	 * @brief 指定したIDの計測結果をマイクロ秒単位で取得します。
	 * @param id 取得したい計測対象のタイムスタンプID
	 * @return 計測時間（マイクロ秒）
	 */
	double GetElapsedTimeMicroseconds(CPUTimeStampID id) const;


private:

	struct TimeStampData {
		uint64_t beginTime = 0; ///< 計測開始時のシステムクロック値
		uint64_t endTime = 0;   ///< 計測終了時のシステムクロック値
	};

	std::vector<TimeStampData> timeStampData_; ///< 計測データ群

};


} /// namespace ONEngine