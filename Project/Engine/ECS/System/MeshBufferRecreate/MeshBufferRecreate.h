#pragma once

/// engine
#include "../Interface/ECSISystem.h"

/// /////////////////////////////////////////////////
/// カスタムメッシュのバッファを再作成するシステム
/// /////////////////////////////////////////////////
namespace ONEngine {

class MeshBufferRecreate final : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	MeshBufferRecreate(class DxDevice* _dxDevice);
	~MeshBufferRecreate() = default;

	void RuntimeUpdate(class ECSGroup* _ecs) override;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxDevice* pDxDevice_ = nullptr;
};


} /// ONEngine
