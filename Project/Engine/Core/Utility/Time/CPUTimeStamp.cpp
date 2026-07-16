#include "CPUTimeStamp.h"

#include <chrono>


namespace ONEngine {

/**
 * @brief 特定の処理について高精度タイマーによる計測を開始します。
 * @param id 計測対象のタイムスタンプID
 */
void CPUTimeStamp::BeginTimeStamp(CPUTimeStampID id) {
	auto now = std::chrono::high_resolution_clock::now();
	uint64_t nowTime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
	timeStampData_[static_cast<size_t>(id)].beginTime = nowTime;
}

/**
 * @brief 特定の処理について高精度タイマーによる計測を終了し、経過時間を記録します。
 * @param id 計測対象 of タイムスタンプID
 */
void CPUTimeStamp::EndTimeStamp(CPUTimeStampID id) {
	auto now = std::chrono::high_resolution_clock::now();
	uint64_t nowTime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
	timeStampData_[static_cast<size_t>(id)].endTime = nowTime;
}

/**
 * @brief 指定したIDの計測結果をミリ秒単位で取得します（関数名と異なり実際はミリ秒に変換している）。
 * @param id 取得したい計測対象のタイムスタンプID
 * @return 計測時間（ミリ秒）
 */
double CPUTimeStamp::GetElapsedTimeMicroseconds(CPUTimeStampID id) const {
	uint32_t idx = static_cast<uint32_t>(id);
	uint64_t time = timeStampData_[idx].endTime - timeStampData_[idx].beginTime;
	return static_cast<double>(time) / 1000.0;
}


}