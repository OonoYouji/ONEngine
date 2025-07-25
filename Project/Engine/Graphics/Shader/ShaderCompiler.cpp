#include "ShaderCompiler.h"

/// std
#include <iostream>
#include <format>

/// engine
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Engine/Core/Utility/Tools/Log.h"

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

	/// includeHandlerの初期化
	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	Assert(SUCCEEDED(result), "Failed to create include handler.");
}


ComPtr<IDxcBlob> ShaderCompiler::CompileShader(const std::wstring& _filePath, const wchar_t* _profile) {
	return CompileShader(_filePath, _profile, L"main");
}

ComPtr<IDxcBlob> ShaderCompiler::CompileShader(const std::wstring& _filePath, const wchar_t* _profile, const std::wstring& _main) {
	HRESULT hr = S_FALSE;

	/// hlslを読み込む
	ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	hr = dxcUtils_->LoadFile(_filePath.c_str(), nullptr, &shaderSource);
	Assert(SUCCEEDED(hr), "Compile Not Succeended");

	/// ファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; /// 文字コード

	/// Compileの設定
	LPCWSTR arguments[] = {
		_filePath.c_str(),			/// Compile対象のhlslファイル名
		L"-E", _main.c_str(),		/// エントリーポイントの指定; 基本的にmain以外にはしない
		L"-T", _profile,			/// ShaderProfileの設定
		L"-Zi", L"-Qembed_debug",	/// デバッグ用の情報を埋め込む
		L"-Od",						/// 最適化を外す
		L"-Zpr",					/// メモリレイアウトは行優先
	};

	/// 実際にCompileする
	ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,		/// 読み込んだファイル
		arguments,					/// コンパイルオプション
		_countof(arguments),		/// コンパイルオプションの数
		includeHandler_.Get(),		/// includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)	/// コンパイル結果
	);
	Assert(SUCCEEDED(hr), "compile not succeeded");

	/// 警告・エラーが出たらログに出力して止める
	ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Console::Log(shaderError->GetStringPointer());
		Assert(false, shaderError->GetStringPointer());
	}

	/// Compile結果を受け取りreturnする
	ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	Assert(SUCCEEDED(hr), "compile Not succeeded");

	/// 成功したログ出力
	Console::Log(std::format(L"[Load Resource] type:Shader, path:\"{}\", profile:\"{}\"", _filePath, _profile));

	return shaderBlob;
}
