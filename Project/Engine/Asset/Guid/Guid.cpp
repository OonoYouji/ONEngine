#include "Guid.h"

/// std
#include <iomanip>
#include <sstream>

/// engine
#include "Engine/Core/Utility/Utility.h"

using namespace ONEngine;

namespace {

/// @brief stirngに変換したときにGuidの長さ
constexpr size_t kGuidStringLength = 32;

/// @brief high/lowそれぞれの16進数表記の長さ
constexpr size_t kGuidHexSegmentLength = 16;

} /// namespace


void ONEngine::from_json(const nlohmann::json& _j, Guid& _guid) {
	_guid = Guid::FromString(_j.value("guid", ""));
}

void ONEngine::to_json(nlohmann::json& _j, const Guid& _guid) {
	_j = nlohmann::json{
		{ "guid", _guid.ToString() },
	};
}


/// @brief 無効値のGuid定義
const Guid Guid::kInvalid = Guid{ 0, 0 };


Guid::Guid() : high(0), low(0) {}
Guid::Guid(uint64_t _high, uint64_t _low) : high(_high), low(_low) {}

std::string Guid::ToString() const {
	/// ----- GuidをStringに変換する ----- ///

	std::ostringstream oss;
	oss << std::hex << std::setfill('0')
		<< std::setw(kGuidHexSegmentLength) << high
		<< std::setw(kGuidHexSegmentLength) << low;
	return oss.str();
}

bool Guid::CheckValid() const {
	return (high != 0) || (low != 0);
}

std::string Guid::ToString(const Guid& _guid) {
	return _guid.ToString();
}

Guid Guid::FromString(const std::string& _str) {
	/// ----- StringをGuidに変換して返す ----- ///

	/// _str が32文字でない場合は無効なGuidを返す
	if (_str.size() != kGuidStringLength) {
		return Guid{};
	}

	uint64_t hi = std::stoull(_str.substr(0, kGuidHexSegmentLength), nullptr, kGuidHexSegmentLength);
	uint64_t lo = std::stoull(_str.substr(kGuidHexSegmentLength, kGuidHexSegmentLength), nullptr, kGuidHexSegmentLength);
	return Guid(hi, lo);
}

bool ONEngine::operator==(const Guid& a, const Guid& b) {
	return (a.high == b.high) && (a.low == b.low);
}

bool ONEngine::operator!=(const Guid& a, const Guid& b) {
	return !(a == b);
}

Guid ONEngine::GenerateGuid() {
	return Guid(Random::UInt64(), Random::UInt64());
}
