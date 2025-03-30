#pragma once

///std
#include <array>

/// engine
#include "Engine/Graphics/Pipelines/Interface/IPostProcessPipeline.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"

/// ///////////////////////////////////////////////////
/// PostProcessLighting
/// ///////////////////////////////////////////////////
class PostProcessLighting : public IPostProcessPipeline {
private:

	/// ===================================================
	/// private : sub class
	/// ===================================================

	struct DirectionalLightBufferData final {
		Vector3 direction;
		float intensity;
		Vector4 color;
	};

	struct CameraBufferData final {
		Vector4 position;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	PostProcessLighting();
	~PostProcessLighting();
	
	/// @brief pipelineの初期化を行う
	/// @param _shaderCompiler ShaderCompilerへのポインタ
	/// @param _dxManager DxManagerへのポインタ
	void Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) override;
	
	/// @brief post processの実行
	void Execute(
		class DxCommand* _dxCommand, 
		class GraphicsResourceCollection* _resourceCollection,
		class EntityCollection* _entityCollection
	) override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::array<size_t, 4> textureIndices_;
	std::unique_ptr<ConstantBuffer<DirectionalLightBufferData>> directionalLightBufferData_;
	std::unique_ptr<ConstantBuffer<CameraBufferData>> cameraBufferData_;

};

