#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

// .NET Hosting
#include "DotNetHost/nethost.h"
#include "DotNetHost/hostfxr.h"
#include "DotNetHost/coreclr_delegates.h"

namespace Engine::Script {

class ScriptHost {
public:
    static ScriptHost& GetInstance() {
        static ScriptHost instance;
        return instance;
    }

    bool Initialize();
    void Shutdown();

    bool LoadAssembly(const std::filesystem::path& assemblyPath);
    
    void Update();

    // デリゲートの取得（C++ -> C# 呼び出し用）
    void* GetMethodDelegate(const std::wstring& typeName, const std::wstring& methodName, const std::wstring& delegateTypeName);

private:
    ScriptHost() = default;
    ~ScriptHost() = default;

    bool LoadHostFxr();
    load_assembly_and_get_function_pointer_fn GetNet8LoadAssembly(const char_t* configPath);

private:
    hostfxr_initialize_for_runtime_config_fn init_fptr = nullptr;
    hostfxr_get_runtime_delegate_fn get_delegate_fptr = nullptr;
    hostfxr_close_fn close_fptr = nullptr;

    load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer = nullptr;

    void* hostfxr_lib = nullptr;
    hostfxr_handle context = nullptr;

    bool initialized_ = false;
};

} // namespace Engine::Script
