#pragma once

/// engine
#include "../Interface/ECSISystem.h"

/// /////////////////////////////////////////////////
/// カスタムメッシュのバッファを再作成するシステム
/// /////////////////////////////////////////////////
class MeshBufferRecreate final : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	MeshBufferRecreate(class DxDevice* _pDxDevice);
	~MeshBufferRecreate() = default;

	void Update(EntityComponentSystem* _pEntityComponentSystem) override;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxDevice* pDxDevice_ = nullptr;
};

