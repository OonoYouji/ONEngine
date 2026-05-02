#include "CPUTimeStamp.h"

#include <chrono>


namespace ONEngine {

void CPUTimeStamp::BeginTimeStamp(CPUTimeStampID id) {
	auto now = std::chrono::high_resolution_clock::now();
	uint64_t nowTime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
	timeStampData_[static_cast<size_t>(id)].beginTime = nowTime;
}

void CPUTimeStamp::EndTimeStamp(CPUTimeStampID id) {
	auto now = std::chrono::high_resolution_clock::now();
	uint64_t nowTime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
	timeStampData_[static_cast<size_t>(id)].endTime = nowTime;
}

double CPUTimeStamp::GetElapsedTimeMicroseconds(CPUTimeStampID id) const {
	uint32_t idx = static_cast<uint32_t>(id);
	uint64_t time = timeStampData_[idx].endTime - timeStampData_[idx].beginTime;
	return static_cast<double>(time) / 1000.0;
}


}