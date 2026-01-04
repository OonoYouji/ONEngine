workspace "ONEngine"
    architecture "x64"
    startproject "ONEngine"
    configurations { "Debug", "Release", "Development" }

    -- =========================================================
    -- 【全体共通設定】 Visual Studio 2026 (v145) 対応
    -- =========================================================
    filter "system:windows"
        systemversion "latest"      -- 最新のWindows SDKを使用
        
        -- ★ Visual Studio 2026 のツールセットを指定
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
-- Project: ONEngine
--
project "ONEngine"
    kind "WindowedApp"
    language "C++"
    -- cppdialect "C++latest" -- 共通設定から継承済み

    targetdir ("../Generated/Outputs/%{cfg.buildcfg}")
    objdir ("../Generated/Obj/%{prj.name}/%{cfg.buildcfg}")
    debugdir "%{wks.location}"
    
    files {
        "Engine/**.h",
        "Engine/**.cpp",
        "Game/**.h",
        "Game/**.cpp",
        "main.cpp"
    }

    includedirs {
        "$(ProjectDir)",
        "$(ProjectDir)Externals/assimp/include",
        "$(ProjectDir)Externals/imgui",
        "$(ProjectDir)Externals/glib",
        "$(ProjectDir)Externals/mono",
        "$(ProjectDir)Externals/DirectXTex",
        "$(ProjectDir)Externals"
    }

    libdirs {
        "Externals/assimp/lib",
        "Packages/Scripts/lib"
    }

    dependson { "DirectXTex", "ImGui" }
    
    links {
        "$(ProjectDir)Packages/Scripts/lib/mono-2.0-sgen.lib",
        "DirectXTex",
        "ImGui"
    }

    warnings "Extra"
    -- "/MP", "/utf-8" は共通設定にあるため、ここには "/bigobj" だけ追加
    buildoptions { "/bigobj" } 

    -- Debug Configuration
    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        optimize "Off"
        defines { "_DEBUG", "_WINDOWS", "DEBUG_MODE" }
        staticruntime "On"
        libdirs { "$(ProjectDir)Externals/assimp/lib/Debug" }
        links { "assimp-vc143-mtd.lib" } -- 既存のライブラリ名を使用（必要に応じて更新してください）
        
        postbuildcommands {
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll\" \"$(TargetDir)dxcompiler.dll\"",
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll\" \"$(TargetDir)dxil.dll\"",
            "copy \"$(ProjectDir)Packages\\Scripts\\lib\\mono-2.0-sgen.dll\" \"$(TargetDir)mono-2.0-sgen.dll\"",
        }

    -- Release Configuration
    filter "configurations:Release"
        runtime "Release"
        symbols "On"
        optimize "Speed"
        defines { "NDEBUG", "_WINDOWS" }
        linktimeoptimization "On"
        staticruntime "On"
        libdirs { "$(ProjectDir)Externals/assimp/lib/Release" }
        links { "assimp-vc143-mt.lib" }
        
        postbuildcommands {
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll\" \"$(TargetDir)dxcompiler.dll\"",
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll\" \"$(TargetDir)dxil.dll\"",
            "copy \"$(ProjectDir)Packages\\Scripts\\lib\\mono-2.0-sgen.dll\" \"$(TargetDir)mono-2.0-sgen.dll\"",
            "xcopy /E /Y /I \"$(ProjectDir)Assets\" \"$(TargetDir)Assets\"",
            "xcopy /E /Y /I \"$(ProjectDir)Packages\" \"$(TargetDir)Packages\""
        }

    -- Development Configuration
    filter "configurations:Development"
        runtime "Release"
        symbols "Full"
        optimize "Speed"
        defines { "DEBUG_BUILD", "_WINDOWS", "DEBUG_MODE" }
        
        -- 開発用ビルド向けの高度なデバッグ/最適化オプション
        buildoptions {
            "/Zo",  -- 最適化コードのデバッグ情報強化
            "/Oi",  -- 組み込み関数使用
            "/Ot",  -- 速度優先
            "/GL",  -- プログラム全体の最適化
        }
        linkoptions {
            "/OPT:REF",
            "/OPT:ICF",
            "/LTCG",
        }
        linktimeoptimization "On"
        staticruntime "On"

        libdirs { "$(ProjectDir)Externals/assimp/lib/Release" }
        links { "assimp-vc143-mt.lib" }

        postbuildcommands {
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll\" \"$(TargetDir)dxcompiler.dll\"",
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll\" \"$(TargetDir)dxil.dll\"",
            "copy \"$(ProjectDir)Packages\\Scripts\\lib\\mono-2.0-sgen.dll\" \"$(TargetDir)mono-2.0-sgen.dll\""
        }