#include "Guid.h"

/// std
#include <iomanip>
#include <sstream>

/// engine
#include "Engine/Core/Utility/Utility.h"

Guid::Guid() : high(0), low(0) {}
Guid::Guid(uint64_t _high, uint64_t _low) : high(_high), low(_low) {}

std::string Guid::ToString() const {
	std::ostringstream oss;
	oss << std::hex << std::setfill('0')
		<< std::setw(16) << high
		<< std::setw(16) << low;
	return oss.str();
}

std::string Guid::ToString(const Guid& _guid) {
	return _guid.ToString();
}

Guid Guid::FromString(const std::string& _str) {
	/// _str が32文字でない場合は無効なGuidを返す
	if (_str.size() != 32) {
		return Guid{};
	}

	uint64_t hi = std::stoull(_str.substr(0, 16), nullptr, 16);
	uint64_t lo = std::stoull(_str.substr(16, 16), nullptr, 16);
	return Guid(hi, lo);
}

bool operator==(const Guid& a, const Guid& b) {
	return (a.high == b.high) && (a.low == b.low);
}

bool operator!=(const Guid& a, const Guid& b) {
	return !(a == b);
}

Guid GenerateGuid() {
	return Guid(Random::UInt64(), Random::UInt64());
}
