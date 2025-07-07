#include "Texture.h"

Texture::Texture() {}
Texture::~Texture() {}

void Texture::CreateEmptySRVHandle() {
	srvHandle_.emplace(Handle());
}

void Texture::CreateEmptyUAVHandle() {
	uavHandle_.emplace(Handle());
}

void Texture::SetName(const std::string& _name) {
	name_ = _name;
	if (dxResource_.Get()) {
		dxResource_.Get()->SetName(std::wstring(_name.begin(), _name.end()).c_str());
	}
}
