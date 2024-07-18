#pragma once

#include <wrl/client.h>
#include <dxcapi.h>

#include <string>

using namespace Microsoft::WRL;



namespace ONE {

	/// ===================================================
	/// DirectX12 Shader Compiler
	/// ===================================================
	class DxShaderCompiler final {
	public:

		DxShaderCompiler();
		~DxShaderCompiler();

		/// ===================================================
		/// public : methods
		/// ===================================================

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// Shaderのコンパイル
		/// </summary>
		/// <param name="filePath"></param>
		/// <param name="profile"></param>
		/// <returns></returns>
		ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);

	private:

		/// ===================================================
		/// private : objects
		/// ===================================================

		ComPtr<IDxcUtils> dxcUtils_;
		ComPtr<IDxcCompiler3> dxcCompiler_;
		ComPtr<IDxcIncludeHandler> includeHandler_;

	private:
		DxShaderCompiler(const DxShaderCompiler&) = delete;
		DxShaderCompiler(DxShaderCompiler&&) = delete;
		DxShaderCompiler& operator=(const DxShaderCompiler&) = delete;
		DxShaderCompiler& operator=(DxShaderCompiler&&) = delete;
	};

}