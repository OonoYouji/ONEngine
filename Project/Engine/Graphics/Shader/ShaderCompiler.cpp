#include "ShaderCompiler.h"

/// std
#include <cassert>
#include <iostream>

/// engine
#include "Engine/Utility/DebugTools/Assert.h"

/// comment
#pragma comment(lib, "dxcompiler.lib")



ShaderCompiler::ShaderCompiler() {}
ShaderCompiler::~ShaderCompiler() {}

void ShaderCompiler::Initialize() {
	HRESULT result = S_FALSE;

	/// dxcUtilsの初期化
	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	Assert(SUCCEEDED(result), "Failed to create DxcUtils instance.");

	/// dxcCompilerの初期化
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	Assert(SUCCEEDED(result), "Failed to create DxcCompiler instance.");

	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	Assert(SUCCEEDED(result), "");
}
