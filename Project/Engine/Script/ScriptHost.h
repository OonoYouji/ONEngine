#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <filesystem>

// .NET Hosting
#include "DotNetHost/nethost.h"
#include "DotNetHost/hostfxr.h"
#include "DotNetHost/coreclr_delegates.h"

namespace Engine::Script {

///
/// .NET Core (CoreCLR) をホストし、C# スクリプトを実行するクラス
///
class ScriptHost {
public:
    static ScriptHost& GetInstance() {
        return *instance_;
    }

    static void CreateInstance() {
        if (!instance_) instance_ = new ScriptHost();
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    bool Initialize();
    void Shutdown();

    /// @brief 指定したアセンブリから関数のデリゲートを取得
    void* GetMethodDelegate(const std::wstring& typeName, const std::wstring& methodName, const std::wstring& delegateTypeName = L"");

    void Update();

private:
    ScriptHost();
    ~ScriptHost();

    static ScriptHost* instance_;

    bool LoadHostFxr();
    load_assembly_and_get_function_pointer_fn GetNet8LoadAssembly(const char_t* configPath);

    hostfxr_initialize_for_runtime_config_fn init_fptr = nullptr;
    hostfxr_get_runtime_delegate_fn get_delegate_fptr = nullptr;
    hostfxr_close_fn close_fptr = nullptr;

    load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer = nullptr;

    void* hostfxr_lib = nullptr;
    hostfxr_handle context = nullptr;

    bool initialized_ = false;
};

} // namespace Engine::Script
