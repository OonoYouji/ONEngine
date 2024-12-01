#include "ShaderBlob.h"

/// engine
#include "Core/ONEngine.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxShaderCompiler.h"

#include "Debugger/Assertion.h"


void ShaderBlob::Compile(const std::wstring& _filePath, const std::wstring& _profile, SHADER_TYPE _shaderType) {

	if(_shaderType == COUNT) {
		Assert(false, "used count");
		return;
	}

	/// 範囲外参照の防止
	if(blobs_.size() < _shaderType) {
		blobs_.resize(_shaderType + 1);
	}

	ONE::DxCommon* dxCommon = ONEngine::GetDxCommon();
	blobs_[_shaderType] = dxCommon->GetDxShaderCompiler()->CompileShader(
		_filePath, _profile.c_str()
	);
}

IDxcBlob* ShaderBlob::GetBlob(SHADER_TYPE _shaderType) {
	return blobs_[_shaderType].Get();
}
