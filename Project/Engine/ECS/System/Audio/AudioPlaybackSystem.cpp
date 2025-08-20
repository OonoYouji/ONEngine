#include "AudioPlaybackSystem.h"

/// engine
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Audio/AudioSource.h"

AudioPlaybackSystem::AudioPlaybackSystem() {}
AudioPlaybackSystem::~AudioPlaybackSystem() {}

void AudioPlaybackSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {}

void AudioPlaybackSystem::RuntimeUpdate(ECSGroup* _ecs) {
	///
	ComponentArray<AudioSource>* asArray = _ecs->GetComponentArray<AudioSource>();
	if(!asArray || asArray->GetUsedComponents().empty()) {
		return;
	}

	for(auto& as : asArray->GetUsedComponents()) {
		if (!as || !as->enable) {
			continue;
		}



	}

}
