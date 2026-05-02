#include "ShaderCompiler.h"

#include <format>
#include <vector>
#include <d3d12shader.h>
#include <Windows.h>

#include "Engine/Common/Assert.h"
#include "Engine/Common/Console.h"

#pragma comment(lib, "dxcompiler.lib")

namespace Engine::Graphics {

ShaderCompiler::ShaderCompiler() = default;
ShaderCompiler::~ShaderCompiler() = default;

void ShaderCompiler::Initialize() {
    HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils_));
    Assert(SUCCEEDED(hr), "Failed to create DxcUtils");

    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler_));
    Assert(SUCCEEDED(hr), "Failed to create DxcCompiler");

    hr = utils_->CreateDefaultIncludeHandler(&includeHandler_);
    Assert(SUCCEEDED(hr), "Failed to create IncludeHandler");
}

std::unique_ptr<ShaderObject> ShaderCompiler::Compile(
    const std::wstring& filePath,
    const std::wstring& entryPoint,
    const std::wstring& profile
) {
    // 1. ファイルのロード
    ComPtr<IDxcBlobEncoding> sourceBlob;
    HRESULT hr = utils_->LoadFile(filePath.c_str(), nullptr, &sourceBlob);
    if (FAILED(hr)) {
        Console::Log(std::format(L"Failed to load shader file: {}", filePath));
        return nullptr;
    }

    // 2. コンパイル引数の設定
    std::vector<LPCWSTR> arguments;
    arguments.push_back(filePath.c_str());
    arguments.push_back(L"-E");
    arguments.push_back(entryPoint.c_str());
    arguments.push_back(L"-T");
    arguments.push_back(profile.c_str());
    arguments.push_back(DXC_ARG_PACK_MATRIX_ROW_MAJOR);
    arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);
#ifdef _DEBUG
    arguments.push_back(DXC_ARG_DEBUG);
    arguments.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);
#else
    arguments.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
#endif

    // 3. コンパイル実行
    DxcBuffer sourceBuffer;
    sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
    sourceBuffer.Size = sourceBlob->GetBufferSize();
    sourceBuffer.Encoding = DXC_CP_UTF8;

    ComPtr<IDxcResult> result;
    hr = compiler_->Compile(
        &sourceBuffer,
        arguments.data(),
        static_cast<uint32_t>(arguments.size()),
        includeHandler_.Get(),
        IID_PPV_ARGS(&result)
    );

    if (FAILED(hr)) {
        Console::Log(std::format(L"Failed to compile shader: {}", filePath));
        return nullptr;
    }

    // 4. エラー・警告の確認
    ComPtr<IDxcBlobUtf8> errorBlob;
    result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errorBlob), nullptr);
    if (errorBlob && errorBlob->GetStringLength() > 0) {
        Console::Log(std::format("Shader compile error/warning:\n{}", errorBlob->GetStringPointer()));
    }

    // コンパイル成否の確認
    hr = result->GetStatus(&hr);
    if (FAILED(hr)) {
        Console::Log(std::format(L"Shader compilation failed for file: {}", filePath));
        return nullptr;
    }

    // 5. バイナリと反射情報の取得
    auto shaderObject = std::make_unique<ShaderObject>();
    
    // バイナリ取得
    hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderObject->blob), nullptr);
    Assert(SUCCEEDED(hr), "Failed to get DXC_OUT_OBJECT");

    // 反射情報取得
    ComPtr<IDxcBlob> reflectionBlob;
    hr = result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflectionBlob), nullptr);
    if (SUCCEEDED(hr)) {
        ExtractReflection(reflectionBlob.Get(), shaderObject->reflectionData);
    }

    return shaderObject;
}

void ShaderCompiler::ExtractReflection(IDxcBlob* reflectionBlob, ShaderReflectionData& outData) {
    DxcBuffer reflectionBuffer;
    reflectionBuffer.Ptr = reflectionBlob->GetBufferPointer();
    reflectionBuffer.Size = reflectionBlob->GetBufferSize();
    reflectionBuffer.Encoding = DXC_CP_ACP;

    ComPtr<ID3D12ShaderReflection> reflection;
    HRESULT hr = utils_->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(&reflection));
    if (FAILED(hr)) {
        Console::Log("Failed to create shader reflection");
        return;
    }

    D3D12_SHADER_DESC shaderDesc;
    reflection->GetDesc(&shaderDesc);

    // バインドされている全リソースを走査
    for (uint32_t i = 0; i < shaderDesc.BoundResources; ++i) {
        D3D12_SHADER_INPUT_BIND_DESC bindDesc;
        reflection->GetResourceBindingDesc(i, &bindDesc);

        std::string resName = bindDesc.Name;
        
        // Debug出力
        OutputDebugStringA(std::format("Shader Resource Found: {} (Type: {})\n", resName, (int)bindDesc.Type).c_str());

        if (bindDesc.Type == D3D_SIT_CBUFFER) {
            // 定数バッファの場合
            ID3D12ShaderReflectionConstantBuffer* cb = reflection->GetConstantBufferByName(resName.c_str());
            if (cb) {
                D3D12_SHADER_BUFFER_DESC bufferDesc;
                cb->GetDesc(&bufferDesc);

                ShaderConstantBufferInfo cbInfo;
                cbInfo.name = resName; // bindDesc.Name を使用
                cbInfo.bindPoint = bindDesc.BindPoint;
                cbInfo.bindCount = bindDesc.BindCount;
                cbInfo.space = bindDesc.Space;
                cbInfo.size = bufferDesc.Size;

                for (uint32_t j = 0; j < bufferDesc.Variables; ++j) {
                    ID3D12ShaderReflectionVariable* var = cb->GetVariableByIndex(j);
                    D3D12_SHADER_VARIABLE_DESC varDesc;
                    var->GetDesc(&varDesc);

                    ShaderVariableInfo varInfo;
                    varInfo.name = varDesc.Name;
                    varInfo.offset = varDesc.StartOffset;
                    varInfo.size = varDesc.Size;
                    cbInfo.variables.push_back(varInfo);
                }
                outData.constantBuffers.push_back(cbInfo);
            }
        }
        else {
            // SRV, UAV, Sampler の場合
            ShaderResourceInfo resInfo;
            resInfo.name = resName;
            resInfo.bindPoint = bindDesc.BindPoint;
            resInfo.bindCount = bindDesc.BindCount;
            resInfo.space = bindDesc.Space;

            switch (bindDesc.Type) {
            case D3D_SIT_TEXTURE:
            case D3D_SIT_STRUCTURED:
            case D3D_SIT_BYTEADDRESS:
                outData.srvs.push_back(resInfo);
                break;
            case D3D_SIT_UAV_RWTYPED:
            case D3D_SIT_UAV_RWSTRUCTURED:
            case D3D_SIT_UAV_RWBYTEADDRESS:
            case D3D_SIT_UAV_APPEND_STRUCTURED:
            case D3D_SIT_UAV_CONSUME_STRUCTURED:
            case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
                outData.uavs.push_back(resInfo);
                break;
            case D3D_SIT_SAMPLER:
                outData.samplers.push_back(resInfo);
                break;
            default:
                break;
            }
        }
    }
}

} // namespace Engine::Graphics
