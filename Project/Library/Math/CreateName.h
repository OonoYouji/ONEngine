#pragma once

#include <string>

template <class T>
inline std::string CreateName(T* t) {
	std::string name = typeid(*t).name();
	name = name.substr(std::string("class ").length());
	return name;
}