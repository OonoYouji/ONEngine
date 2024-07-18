#include <PipelineState.h>

#include <DxCommon.h>
#include <DxShaderCompiler.h>


/// ===================================================
/// static objects initialize
/// ===================================================
const std::wstring PipelineState::kDirectoryPath_ = L"./Resources/Shaders";


PipelineState::PipelineState() {}
PipelineState::~PipelineState() {}


/// ===================================================
/// Shaderの初期化
/// ===================================================
void PipelineState::InitializeShader(const std::wstring& vsFilePath, const wchar_t* vsProfile, const std::wstring& psFilePath, const wchar_t* psProfile) {
	ONE::DxCommon* dxCommon = ONE::DxCommon::GetInstance();

	shader_.vs = dxCommon->GetShaderCompiler()->CompileShader(kDirectoryPath_ + vsFilePath, vsProfile);
	shader_.ps = dxCommon->GetShaderCompiler()->CompileShader(kDirectoryPath_ + psFilePath, psProfile);

}
