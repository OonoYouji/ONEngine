workspace "ONEngine"
    architecture "x64"
    startproject "Runtime"
    configurations { "Debug", "Release", "Development" }

    -- =========================================================
    -- 【全体共通設定】 Visual Studio 2026 (v145) 対応
    -- =========================================================
    filter "system:windows"
        systemversion "latest"      -- 最新のWindows SDKを使用
        
        -- (まだリリースの過渡期で v145 がない場合は "v144" や "ClangCL" に変更してください)
        toolset "v145"

        cppdialect "C++latest"      -- C++23 / C++26 (プレビュー) を使用
        conformancemode "On"        -- 準拠モード (/permissive-)
        buildoptions { "/utf-8", "/MP" } -- UTF-8エンコード、マルチプロセスコンパイル

    -- 設定のリセット（次の定義に影響しないように）
    filter {}

-- 
-- Project: DirectXTex
--
project "DirectXTex"
    kind "StaticLib"
    language "C++"

    location "Externals/DirectXTex/"
    targetdir "../Generated/Outputs/%{cfg.buildcfg}/"
    objdir "../Generated/Obj/%{cfg.buildcfg}/DirectXTex/"
    targetname "DirectXTex"
    
    files { "Externals/DirectXTex/**.h", "Externals/DirectXTex/**.cpp" }
    includedirs { "$(ProjectDir)","$(ProjectDir)Shaders/Compiled" }

    -- ※共通設定が自動適用されるため、ここでの cppdialect 等の記述は不要です

    filter "configurations:Debug"
         runtime "Debug"
         symbols "On"
         staticruntime "On"

    filter "configurations:Development"
        runtime "Release" -- 開発用ビルドだがランタイムはRelease
        symbols "On"
        editandcontinue "Off"
        staticruntime "On"

    filter "configurations:Release"
         runtime "Release"
         optimize "Full"
         staticruntime "On"

-- 
-- Project: ImGui
--
project "ImGui"
    kind "StaticLib"
    language "C++"
    location "Externals/ImGui/"
    targetdir "../Generated/Outputs/%{cfg.buildcfg}/"
    objdir "../Generated/Obj/%{cfg.buildcfg}/ImGui/"

    includedirs {
        "$(ProjectDir)",
        "$(ProjectDir)/ImGui"
    }

    files { "Externals/ImGui/**.h", "Externals/ImGui/**.cpp" }

    filter "configurations:Debug"
         runtime "Debug"
         symbols "On"
         staticruntime "On"

    filter "configurations:Development"
        runtime "Release"
        symbols "On"
        editandcontinue "Off"
        staticruntime "On"

    filter "configurations:Release"
         runtime "Release"
         optimize "Full"
         staticruntime "On"


-- 
-- Project: D3D12MA
--
project "D3D12MA"
    kind "StaticLib"
    language "C++"
    location "Externals/D3D12MA/"
    targetdir "../Generated/Outputs/%{cfg.buildcfg}/"
    objdir "../Generated/Obj/%{cfg.buildcfg}/D3D12MA/"

    includedirs {
        "$(ProjectDir)",
        "$(ProjectDir)/D3D12MA"
    }

    files { "Externals/D3D12MA/**.h", "Externals/D3D12MA/**.cpp" }

    filter "configurations:Debug"
         runtime "Debug"
         symbols "On"
         staticruntime "On"

    filter "configurations:Development"
        runtime "Release"
        symbols "On"
        editandcontinue "Off"
        staticruntime "On"

    filter "configurations:Release"
         runtime "Release"
         optimize "Full"
         staticruntime "On"


-- 
-- Project: Engine
--
project "Engine"
    kind "StaticLib"
    language "C++"
    targetdir ("../Generated/Outputs/%{cfg.buildcfg}")
    objdir ("../Generated/Obj/%{prj.name}/%{cfg.buildcfg}")

    files {
        "Engine/**.h",
        "Engine/**.cpp"
    }

    includedirs {
        "$(ProjectDir)",
        "$(ProjectDir)Engine",
        "$(ProjectDir)Externals/assimp/include",
        "$(ProjectDir)Externals/imgui",
        "$(ProjectDir)Externals/D3D12MA",
        "$(ProjectDir)Externals/DotNetHost",
        "$(ProjectDir)Externals/DirectXTex",
        "$(ProjectDir)Externals"
    }

    libdirs {
        "Externals/assimp/lib",
        "Externals/DotNetHost"
    }

    dependson { "DirectXTex", "ImGui", "D3D12MA" }
    
    links {
        "DirectXTex",
        "ImGui",
        "D3D12MA",
        "nethost.lib"
    }

    buildoptions { "/bigobj" } 

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        defines { "_DEBUG", "_WINDOWS", "DEBUG_MODE", "ENGINE_BUILD" }
        staticruntime "On"
        libdirs { "$(ProjectDir)Externals/assimp/lib/Debug" }
        links { "assimp-vc143-mtd.lib" }

    filter "configurations:Release"
        runtime "Release"
        symbols "On"
        optimize "Speed"
        defines { "NDEBUG", "_WINDOWS", "ENGINE_BUILD" }
        linktimeoptimization "On"
        staticruntime "On"
        libdirs { "$(ProjectDir)Externals/assimp/lib/Release" }
        links { "assimp-vc143-mt.lib" }

    filter "configurations:Development"
        runtime "Release"
        symbols "Full"
        optimize "Speed"
        defines { "DEBUG_BUILD", "_WINDOWS", "DEBUG_MODE", "ENGINE_BUILD" }
        staticruntime "On"
        libdirs { "$(ProjectDir)Externals/assimp/lib/Release" }
        links { "assimp-vc143-mt.lib" }


-- 
-- Project: Runtime
--
project "Runtime"
    kind "WindowedApp"
    language "C++"
    targetdir ("../Generated/Outputs/%{cfg.buildcfg}")
    objdir ("../Generated/Obj/%{prj.name}/%{cfg.buildcfg}")
    debugdir "%{wks.location}"
    
    files {
        "Game/**.h",
        "Game/**.cpp",
        "main.cpp"
    }

    includedirs {
        "$(ProjectDir)",
        "$(ProjectDir)Engine",
        "$(ProjectDir)Externals/D3D12MA",
        "$(ProjectDir)Externals/imgui",
        "$(ProjectDir)Externals"
    }

    dependson { "Engine" }
    links { "Engine" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        staticruntime "On"
        postbuildcommands {
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll\" \"$(TargetDir)dxcompiler.dll\"",
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll\" \"$(TargetDir)dxil.dll\"",
            "copy \"$(ProjectDir)Externals\\DotNetHost\\nethost.dll\" \"$(TargetDir)nethost.dll\""
        }

    filter "configurations:Release"
        runtime "Release"
        symbols "On"
        optimize "Speed"
        staticruntime "On"
        postbuildcommands {
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll\" \"$(TargetDir)dxcompiler.dll\"",
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll\" \"$(TargetDir)dxil.dll\"",
            "copy \"$(ProjectDir)Externals\\DotNetHost\\nethost.dll\" \"$(TargetDir)nethost.dll\""
        }

    filter "configurations:Development"
        runtime "Release"
        symbols "Full"
        optimize "Speed"
        staticruntime "On"
        postbuildcommands {
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll\" \"$(TargetDir)dxcompiler.dll\"",
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll\" \"$(TargetDir)dxil.dll\"",
            "copy \"$(ProjectDir)Externals\\DotNetHost\\nethost.dll\" \"$(TargetDir)nethost.dll\""
        }


-- 
-- Project: Editor
--
project "Editor"
    kind "WindowedApp"
    language "C++"
    targetdir ("../Generated/Outputs/%{cfg.buildcfg}")
    objdir ("../Generated/Obj/%{prj.name}/%{cfg.buildcfg}")
    debugdir "%{wks.location}"
    
    files {
        "Editor/**.h",
        "Editor/**.cpp"
    }

    includedirs {
        "$(ProjectDir)",
        "$(ProjectDir)Engine",
        "$(ProjectDir)Externals/D3D12MA",
        "$(ProjectDir)Externals/imgui",
        "$(ProjectDir)Externals"
    }

    dependson { "Engine" }
    links { "Engine" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        staticruntime "On"
        postbuildcommands {
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll\" \"$(TargetDir)dxcompiler.dll\"",
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll\" \"$(TargetDir)dxil.dll\"",
            "copy \"$(ProjectDir)Externals\\DotNetHost\\nethost.dll\" \"$(TargetDir)nethost.dll\"",
            "xcopy /E /Y /I \"$(ProjectDir)Assets\" \"$(TargetDir)Assets\"",
            "xcopy /E /Y /I \"$(ProjectDir)Packages\" \"$(TargetDir)Packages\""
        }

    filter "configurations:Release"
        runtime "Release"
        symbols "On"
        optimize "Speed"
        staticruntime "On"
        postbuildcommands {
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll\" \"$(TargetDir)dxcompiler.dll\"",
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll\" \"$(TargetDir)dxil.dll\"",
            "copy \"$(ProjectDir)Externals\\DotNetHost\\nethost.dll\" \"$(TargetDir)nethost.dll\"",
            "xcopy /E /Y /I \"$(ProjectDir)Assets\" \"$(TargetDir)Assets\"",
            "xcopy /E /Y /I \"$(ProjectDir)Packages\" \"$(TargetDir)Packages\""
        }

    filter "configurations:Development"
        runtime "Release"
        symbols "Full"
        optimize "Speed"
        staticruntime "On"
        postbuildcommands {
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll\" \"$(TargetDir)dxcompiler.dll\"",
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll\" \"$(TargetDir)dxil.dll\"",
            "copy \"$(ProjectDir)Externals\\DotNetHost\\nethost.dll\" \"$(TargetDir)nethost.dll\""
        }
