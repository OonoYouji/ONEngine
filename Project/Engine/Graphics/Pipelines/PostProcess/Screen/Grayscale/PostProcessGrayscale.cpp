#include "PostProcessGrayscale.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

void PostProcessGrayscale::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{
		Shader shader;
		//shader.CompileShader(L"Assets/Shader/PostProcess/Grayscale/Grayscale.cs.hlsl", L"cs_6_6", Shader::Type::cs);
		
	}

}

void PostProcessGrayscale::Execute(DxCommand* _dxCommand, GraphicsResourceCollection* _resourceCollection, EntityComponentSystem* _entityComponentSystem) {

}
