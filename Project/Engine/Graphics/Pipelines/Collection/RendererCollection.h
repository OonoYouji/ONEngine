#pragma once

/// std
#include <unordered_map>
#include <memory>

/// engine
#include "../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Shader/ShaderCompiler.h"
#include "Engine/DirectX12/Device/DxDevice.h"

/// ===================================================
/// renderer collection
/// ===================================================
class RenderingPipelineCollection final {
public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	RenderingPipelineCollection(ShaderCompiler* _shaderCompiler, DxDevice* _dxDevice);
	~RenderingPipelineCollection();

	void Initialize();


	template <class T>
	void GenerateRenderingPipeline() requires std::is_base_of_v<IRenderingPipeline, T>;

	template <class T>
	T* GetRenderingPipeline() requires std::is_base_of_v<IRenderingPipeline, T>;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ShaderCompiler* shaderCompiler_ = nullptr;
	DxDevice*       dxDevice_       = nullptr;

	std::unordered_map<size_t, std::unique_ptr<IRenderingPipeline>> renderers_;

};

template<class T>
inline void RenderingPipelineCollection::GenerateRenderingPipeline() requires std::is_base_of_v<IRenderingPipeline, T> {
	std::unique_ptr<T> renderer = std::make_unique<T>();
	renderer->Initialize(shaderCompiler_, dxDevice_);
	renderers_.emplace(typeid(T).hash_code(), std::move(renderer));
}

template<class T>
inline T* RenderingPipelineCollection::GetRenderingPipeline() requires std::is_base_of_v<IRenderingPipeline, T> {
	return dynamic_cast<T*>(renderers_[typeid(T).hash_code()].get());
}
