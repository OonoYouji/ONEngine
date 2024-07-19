#include <DxShaderCompiler.h>

#include <cassert>
#include <iostream>

#include <Logger.h>

#pragma comment(lib, "dxcompiler.lib")



ONE::DxShaderCompiler::DxShaderCompiler() {}
ONE::DxShaderCompiler::~DxShaderCompiler() {}


/// ===================================================
/// 初期化
/// ===================================================
void ONE::DxShaderCompiler::Initialize() {
	HRESULT hr = S_FALSE;

	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	///- includeに対応するために設定を行う
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}



/// ===================================================
/// shaderのコンパイル
/// ===================================================
ComPtr<IDxcBlob> ONE::DxShaderCompiler::CompileShader(const std::wstring& filePath, const wchar_t* profile) {
	HRESULT hr = S_FALSE;

	Logger::ConsolePrint(std::format(L"Begin CompileShader,   path:{},   profile:{}\n", filePath, profile));

	///- hlslを読み込む
	ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	assert(SUCCEEDED(hr));

	///- ファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; //- 文字コード

	///- Compileの設定
	LPCWSTR arguments[] = {
		filePath.c_str(),			//- Compile対象のhlslファイル名
		L"-E", L"main",				//- エントリーポイントの指定; 基本的にmain以外にはしない
		L"-T", profile,				//- ShaderProfileの設定
		L"-Zi", L"-Qembed_debug",	//- デバッグ用の情報を埋め込む
		L"-Od",						//- 最適化を外す
		L"-Zpr",					//- メモリレイアウトは行優先
	};

	///- 実際にCompileする
	ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,		//- 読み込んだファイル
		arguments,					//- コンパイルオプション
		_countof(arguments),		//- コンパイルオプションの数
		includeHandler_.Get(),		//- includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)	//- コンパイル結果
	);
	assert(SUCCEEDED(hr));

	///- 警告・エラーが出たらログに出力して止める
	ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if(shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Logger::ConsolePrint(shaderError->GetStringPointer());
		assert(false);
	}

	///- Compile結果を受け取りreturnする
	ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//- 成功したログ出力
	Logger::ConsolePrint(std::format(L"Compile Succeended,    path:{},   profile:{}\n", filePath, profile));

	///- 不要になったリソースの解放
	shaderSource.Reset();
	shaderResult.Reset();

	return shaderBlob;
}
