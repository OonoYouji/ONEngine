#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <assert.h>

// .NET Hosting headers
#include <nethost.h>
#include <coreclr_delegates.h>
#include <hostfxr.h>

// ホスティング用の関数ポインタ型
typedef void (CORECLR_DELEGATE_CALLTYPE *EngineFunc)();

#ifndef UNMANAGED_CALLERS_ONLY_METHOD
#define UNMANAGED_CALLERS_ONLY_METHOD ((const char_t*)-1)
#endif

class ScriptHost {
    hostfxr_initialize_for_runtime_config_fn init_fptr = nullptr;
    hostfxr_get_runtime_delegate_fn get_delegate_fptr = nullptr;
    hostfxr_close_fn close_fptr = nullptr;
    load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer = nullptr;

    hostfxr_handle context = nullptr;

    EngineFunc initFunc = nullptr;
    EngineFunc updateFunc = nullptr;
    EngineFunc shutdownFunc = nullptr;
    
    FILETIME lastWriteTime = { 0, 0 };
    std::wstring currentDllPath;
    std::wstring lastTempDll;

    bool LoadHostfxr() {
        char_t buffer[MAX_PATH];
        size_t buffer_size = sizeof(buffer) / sizeof(char_t);
        int rc = get_hostfxr_path(buffer, &buffer_size, nullptr);
        if (rc != 0) return false;

        HMODULE lib = LoadLibraryW(buffer);
        init_fptr = (hostfxr_initialize_for_runtime_config_fn)GetProcAddress(lib, "hostfxr_initialize_for_runtime_config");
        get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)GetProcAddress(lib, "hostfxr_get_runtime_delegate");
        close_fptr = (hostfxr_close_fn)GetProcAddress(lib, "hostfxr_close");

        return (init_fptr && get_delegate_fptr && close_fptr);
    }

    FILETIME GetDllWriteTime(const std::wstring& path) {
        WIN32_FILE_ATTRIBUTE_DATA data;
        if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &data)) {
            return data.ftLastWriteTime;
        }
        return { 0, 0 };
    }

    void CleanupOldDlls() {
        // パターンを作成: EcsApp.dll.*.active.dll
        std::wstring pattern = currentDllPath + L".*.active.dll";
        WIN32_FIND_DATAW findData;
        HANDLE hFind = FindFirstFileW(pattern.c_str(), &findData);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                wchar_t drive[MAX_PATH], dir[MAX_PATH];
                _wsplitpath_s(currentDllPath.c_str(), drive, MAX_PATH, dir, MAX_PATH, nullptr, 0, nullptr, 0);
                std::wstring fullPath = std::wstring(drive) + dir + findData.cFileName;

                if (fullPath != lastTempDll) {
                    DeleteFileW(fullPath.c_str());
                    std::wstring pdbPath = fullPath.substr(0, fullPath.find_last_of(L".")) + L".pdb";
                    DeleteFileW(pdbPath.c_str());
                }
            } while (FindNextFileW(hFind, &findData));
            FindClose(hFind);
        }
    }

public:
    bool InitializeRuntime(const std::wstring& configPath) {
        if (!LoadHostfxr()) return false;

        wchar_t fullConfigPath[MAX_PATH];
        GetFullPathNameW(configPath.c_str(), MAX_PATH, fullConfigPath, nullptr);
        std::wcerr << L"[C++] Using runtime config: " << fullConfigPath << std::endl;

        int rc = init_fptr(fullConfigPath, nullptr, &context);
        if (rc != 0 || context == nullptr) {
            std::cerr << "Init failed: " << std::hex << rc << std::endl;
            return false;
        }

        rc = get_delegate_fptr(context, hdt_load_assembly_and_get_function_pointer, (void**)&load_assembly_and_get_function_pointer);
        if (rc != 0 || load_assembly_and_get_function_pointer == nullptr) {
            std::cerr << "Get delegate failed: " << std::hex << rc << std::endl;
            return false;
        }

        return true;
    }

    bool Load(const std::wstring& dllPath) {
        wchar_t absPath[MAX_PATH];
        GetFullPathNameW(dllPath.c_str(), MAX_PATH, absPath, nullptr);
        currentDllPath = absPath;
        
        lastWriteTime = GetDllWriteTime(currentDllPath);

        CleanupOldDlls();
// ユニークなコピーを作成
std::wstring timestamp = std::to_wstring(GetTickCount64());
lastTempDll = currentDllPath + L"." + timestamp + L".active.dll";
// PDB名もDLL名に合わせる (EcsApp.dll.XXX.active.pdb)
std::wstring tempPdb = lastTempDll.substr(0, lastTempDll.find_last_of(L".")) + L".pdb";

CopyFileW(currentDllPath.c_str(), lastTempDll.c_str(), FALSE);

std::wstring originalPdb = currentDllPath.substr(0, currentDllPath.find_last_of(L".")) + L".pdb";
if (!CopyFileW(originalPdb.c_str(), tempPdb.c_str(), FALSE)) {
    std::wcerr << L"[C++] Warning: Could not copy PDB for debugging." << std::endl;
}

wchar_t fullPath[MAX_PATH];
GetFullPathNameW(lastTempDll.c_str(), MAX_PATH, fullPath, nullptr);
std::wcerr << L"[C++] Loading assembly copy with symbols: " << fullPath << std::endl;

        const char_t* dotnet_type = L"Engine.Core.EngineHost, EcsApp";
        
        auto get_fn = [&](const char_t* method_name) -> EngineFunc {
            EngineFunc f = nullptr;
            int rc = load_assembly_and_get_function_pointer(
                fullPath,
                dotnet_type,
                method_name,
                UNMANAGED_CALLERS_ONLY_METHOD,
                nullptr,
                (void**)&f);
            
            if (rc != 0) std::wcerr << L"Failed to load " << method_name << L": 0x" << std::hex << rc << std::endl;
            return f;
        };

        initFunc = get_fn(L"Initialize");
        updateFunc = get_fn(L"Update");
        shutdownFunc = get_fn(L"Shutdown");

        return initFunc && updateFunc && shutdownFunc;
    }

    void CheckForReload() {
        FILETIME newTime = GetDllWriteTime(currentDllPath);
        if (newTime.dwLowDateTime != 0 && CompareFileTime(&lastWriteTime, &newTime) != 0) {
            std::cout << "\n[C++] Change detected. Reloading (via CoreCLR)..." << std::endl;
            lastWriteTime = newTime;
            Sleep(1500); // ビルド完了を確実に待つ
            if (Load(currentDllPath)) {
                Init();
                std::cout << "[C++] Reload successful!" << std::endl;
            }
        }
    }


    void Init() { if (initFunc) initFunc(); }
    void Update() { if (updateFunc) updateFunc(); }
    
    ~ScriptHost() {
        if (shutdownFunc) shutdownFunc();
        if (context) close_fptr(context);
    }
};

int main() {
    std::cout << "[C++] Engine Host Starting (CoreCLR Mode)..." << std::endl;

    // 実行ファイルのディレクトリを取得して基準パスにする
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring binDir = exePath;
    binDir = binDir.substr(0, binDir.find_last_of(L"\\/")) + L"\\";

    ScriptHost host;
    std::wstring dllPath = binDir + L"EcsApp.dll";
    std::wstring configPath = binDir + L"EcsApp.runtimeconfig.json";

    std::wcerr << L"[C++] Base directory: " << binDir << std::endl;

    if (!host.InitializeRuntime(configPath)) {
        std::cout << "[C++] Error: Could not initialize .NET Runtime." << std::endl;
        return 1;
    }

    if (!host.Load(dllPath)) {
        std::cout << "[C++] Error: Could not load script assembly." << std::endl;
        return 1;
    }

    host.Init();

    std::cout << "[C++] Running... (Modify EcsApp.cs and build to see Hot Reload)" << std::endl;
    while (true) {
        host.Update();
        host.CheckForReload();
        Sleep(500);
    }

    return 0;
}
