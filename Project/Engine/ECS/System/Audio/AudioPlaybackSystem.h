#pragma once

/// engine
#include "../Interface/ECSISystem.h"

/// ////////////////////////////////////////////////////////////
/// 音の再生を行うクラス
/// ////////////////////////////////////////////////////////////
class AudioPlaybackSystem : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AudioPlaybackSystem();
	~AudioPlaybackSystem();

	void OutsideOfRuntimeUpdate(ECSGroup* _ecs) override;
	void RuntimeUpdate(ECSGroup* _ecs) override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	float masterVolume_;

};

