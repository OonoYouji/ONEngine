#include "ScriptHost.h"
#include <Windows.h>
#include <iostream>
#include "Engine/Common/Assert.h"
#include "Engine/Common/Console.h"

namespace Engine::Script {

ScriptHost* ScriptHost::instance_ = nullptr;

ScriptHost::ScriptHost() = default;
ScriptHost::~ScriptHost() {
    Shutdown();
}

bool ScriptHost::Initialize() {
    if (initialized_) return true;

    if (!LoadHostFxr()) {
        Console::LogError("Failed to load hostfxr.");
        return false;
    }

    // assemblyPath.runtimeconfig.json が必要。
    // 今回は ONEngine.Scripting.runtimeconfig.json を想定。
    std::filesystem::path configPath = std::filesystem::current_path() / "Packages/Scripts/ONEngine.Scripting.runtimeconfig.json";
    
    load_assembly_and_get_function_pointer = GetNet8LoadAssembly(configPath.c_str());
    if (!load_assembly_and_get_function_pointer) {
        Console::LogError("Failed to get load_assembly_and_get_function_pointer.");
        return false;
    }

    initialized_ = true;
    Console::Log("ScriptHost initialized successfully.");
    return true;
}

void ScriptHost::Shutdown() {
    if (context) {
        close_fptr(context);
        context = nullptr;
    }
    delegateCache_.clear();
    initialized_ = false;
}

bool ScriptHost::LoadHostFxr() {
    char_t buffer[MAX_PATH];
    size_t buffer_size = sizeof(buffer) / sizeof(char_t);
    int rc = get_hostfxr_path(buffer, &buffer_size, nullptr);
    if (rc != 0) return false;

    hostfxr_lib = LoadLibraryW(buffer);
    if (!hostfxr_lib) return false;

    init_fptr = (hostfxr_initialize_for_runtime_config_fn)GetProcAddress((HMODULE)hostfxr_lib, "hostfxr_initialize_for_runtime_config");
    get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)GetProcAddress((HMODULE)hostfxr_lib, "hostfxr_get_runtime_delegate");
    close_fptr = (hostfxr_close_fn)GetProcAddress((HMODULE)hostfxr_lib, "hostfxr_close");

    return (init_fptr && get_delegate_fptr && close_fptr);
}

load_assembly_and_get_function_pointer_fn ScriptHost::GetNet8LoadAssembly(const char_t* configPath) {
    void* load_assembly_fptr = nullptr;
    int rc = init_fptr(configPath, nullptr, &context);
    if (rc != 0 || context == nullptr) {
        if (context) close_fptr(context);
        return nullptr;
    }

    rc = get_delegate_fptr(
        context,
        hdt_load_assembly_and_get_function_pointer,
        &load_assembly_fptr);
    
    if (rc != 0 || load_assembly_fptr == nullptr) {
        return nullptr;
    }

    return (load_assembly_and_get_function_pointer_fn)load_assembly_fptr;
}

void* ScriptHost::GetMethodDelegate(const std::wstring& typeName, const std::wstring& methodName, const std::wstring& delegateTypeName) {
    DelegateKey key{ typeName, methodName };
    if (delegateCache_.count(key)) return delegateCache_[key];

    if (!load_assembly_and_get_function_pointer) {
        Console::LogError("GetMethodDelegate failed: load_assembly_and_get_function_pointer is null.");
        return nullptr;
    }

    // 今回は ONEngine.Scripting.dll をメインアセンブリとする
    std::filesystem::path assemblyPath = std::filesystem::current_path() / "Packages/Scripts/ONEngine.Scripting.dll";
    
    if (!std::filesystem::exists(assemblyPath)) {
        Console::LogError(std::format("Assembly not found: {}", assemblyPath.string()));
        return nullptr;
    }

    void* delegate_ptr = nullptr;
    int rc = load_assembly_and_get_function_pointer(
        assemblyPath.c_str(),
        typeName.c_str(),
        methodName.c_str(),
        delegateTypeName.length() > 0 ? delegateTypeName.c_str() : UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        &delegate_ptr);

    if (rc != 0 || delegate_ptr == nullptr) {
        Console::LogError(ConvertString(std::format(L"Failed to get method delegate: {} . {}", typeName, methodName)));
        return nullptr;
    }

    Console::Log(std::format(L"Successfully retrieved delegate: {} . {}", typeName, methodName));

    delegateCache_[key] = delegate_ptr;
    return delegate_ptr;
}

void ScriptHost::Update() {
    // TODO: C# 側の Update を呼び出す
}

} // namespace Engine::Script
