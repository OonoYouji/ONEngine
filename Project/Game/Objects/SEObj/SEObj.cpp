#include "SEObj.h"

#include "ComponentManager/AudioSource/AudioSource.h"

SEObj::SEObj() {
	CreateTag(this);
}

SEObj::~SEObj() {}

void SEObj::Initialize() {

	se_ = AddComponent<AudioSource>();


}

void SEObj::Update() {}

void SEObj::PlayOneShot(const std::string& _filePath, float _volume) {
	se_->PlayOneShot(_filePath, _volume);
}

