#pragma once

#include <string>

static size_t GetComponentHash(const std::string& _name) {
	return std::hash<std::string>()(_name);
}

template <typename T>
static size_t GetComponentHash() {
	std::string name = typeid(T).name();
	if (name.find("class ") == 0) {
		name = name.substr(6);
	}

	return GetComponentHash(name);
}