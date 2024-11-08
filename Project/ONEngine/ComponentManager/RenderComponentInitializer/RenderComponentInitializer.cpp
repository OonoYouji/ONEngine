#include "RenderComponentInitializer.h"

/// engine
#include "Core/ONEngine.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"

#include "GraphicManager/PostEffect/Bloom/Bloom.h"
#include "ComponentManager/ParticleSystem/ParticleSystem.h"
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"


void RenderComponentInitializer::Initialize() {
	/// bloomエフェクトの初期化
	Bloom::StaticInitialize(
		ONEngine::GetDxCommon()->GetDxCommand()->GetList(), 2
	);

	/// パーティクルシステムの初期化
	ParticleSystem::SInitialize(
		ONEngine::GetDxCommon()->GetDxCommand()->GetList()
	);

	/// メッシュのインスタンシング描画の初期化
	MeshInstancingRenderer::SInitialize(
		ONEngine::GetDxCommon()->GetDxCommand()->GetList()
	);

	/// アニメーションの初期化
	AnimationRendererCommon::GetInstance()->Initialize();
}



void RenderComponentInitializer::Finalize() {
	AnimationRendererCommon::GetInstance()->Finalize();
	MeshInstancingRenderer::SFinalize();
	ParticleSystem::SFinalize();
	Bloom::StaticFinalize();
}
