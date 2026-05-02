#pragma once

/// std
#include <cstdint>
#include <vector>

/// engine
#include "CPUTimeStampID.h"

namespace ONEngine {


/// ///////////////////////////////////////////////////
/// CPU 上での処理時間を高精度に計測するタイムスタンプユーティリティクラス
/// ///////////////////////////////////////////////////
class CPUTimeStamp {
	CPUTimeStamp() {
		timeStampData_.resize(static_cast<size_t>(CPUTimeStampID::Count));
	}
	~CPUTimeStamp() = default;
	CPUTimeStamp(const CPUTimeStamp&) = delete;
	CPUTimeStamp& operator=(const CPUTimeStamp&) = delete;
	CPUTimeStamp(CPUTimeStamp&&) = delete;
	CPUTimeStamp& operator=(CPUTimeStamp&&) = delete;
public:

	static CPUTimeStamp& GetInstance() {
		static CPUTimeStamp instance;
		return instance;
	}

	/// @brief 計測開始
	/// @param id 計測用ID
	void BeginTimeStamp(CPUTimeStampID id);

	/// @brief 計測終了
	/// @param id 計測用ID
	void EndTimeStamp(CPUTimeStampID id);

	/// @brief 計測結果をマイクロ秒単位で取得する
	/// @param id 取得したい計測用ID
	/// @return 計測時間（マイクロ秒）
	double GetElapsedTimeMicroseconds(CPUTimeStampID id) const;


private:

	struct TimeStampData {
		uint64_t beginTime = 0;
		uint64_t endTime = 0;
	};

	std::vector<TimeStampData> timeStampData_;

};


} /// namespace ONEngine