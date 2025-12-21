workspace "ONEngine"
    architecture "x64"
    startproject "ONEngine"
    configurations { "Debug", "Release", "Development" }

    
project "DirectXTex"
    kind "StaticLib"
    language "C++"

    location "Externals/DirectXTex/"
    targetdir "../Generated/Outputs/%{cfg.buildcfg}/"
    objdir "../Generated/Obj/%{cfg.buildcfg}/DirectXTex/"
    targetname "DirectXTex"
    files { "Externals/DirectXTex/**.h", "Externals/DirectXTex/**.cpp" }
    includedirs { "$(ProjectDir)","$(ProjectDir)Shaders/Compiled" }

    filter "system:windows"
        cppdialect "C++20"
        systemversion "latest"
    filter "configurations:Debug"
         runtime "Debug"       -- Debug ランタイム (MTd) を使用
         symbols "On"
         staticruntime "On"
    filter "configurations:Development"
        runtime "Release" -- 開発用のリリースビルド
        symbols "On"
        editandcontinue "Off"
        staticruntime "On"
    filter "configurations:Release"
         runtime "Release"     -- Release ランタイム (MT) を使用
         optimize "Full"
         staticruntime "On"

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

    filter "system:windows"
        cppdialect "C++20"
        systemversion "latest"
    filter "configurations:Debug"
         runtime "Debug"       -- Debug ランタイム (MTd) を使用
         symbols "On"
         staticruntime "On"
    filter "configurations:Development"
        runtime "Release" -- 開発用のリリースビルド
        symbols "On"
        editandcontinue "Off"
        staticruntime "On"
    filter "configurations:Release"
         runtime "Release"     -- Release ランタイム (MT) を使用
         optimize "Full"
         staticruntime "On"


project "ONEngine"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"

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
        "DirectXTex",  -- Premake で追加したプロジェクト
        "ImGui"        -- Premake で追加したプロジェクト
    }

    warnings "Extra"
    buildoptions { "/utf-8","/bigobj", "/MP" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        optimize "Off"
        defines { "_DEBUG", "_WINDOWS", "DEBUG_MODE" }
        buildoptions { "/utf-8" }
        staticruntime "On"
        libdirs { "$(ProjectDir)Externals/assimp/lib/Debug" }
        links { "assimp-vc143-mtd.lib" }
        postbuildcommands {
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll\" \"$(TargetDir)dxcompiler.dll\"",
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll\" \"$(TargetDir)dxil.dll\"",
            "copy \"$(ProjectDir)Packages\\Scripts\\lib\\mono-2.0-sgen.dll\" \"$(TargetDir)mono-2.0-sgen.dll\"",
        }

    filter "configurations:Release"
        runtime "Release"
        symbols "On"
        optimize "Speed"
        defines { "NDEBUG", "_WINDOWS" }
        buildoptions { "/utf-8" }
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

    filter "configurations:Development"
        runtime "Release"                 -- Releaseランタイム (/MT)
        symbols "Full"                    -- /Zi （Edit and Continueなし）
        optimize "Speed"                  -- /O2 とほぼ同等の最適化
        defines { "DEBUG_BUILD", "_WINDOWS", "DEBUG_MODE" }
        buildoptions {
            "/utf-8",
            "/Zo",                        -- デバッグ時のソース位置情報強化（任意）
            "/Oi",                        -- インライン展開ヒントを有効化
            "/Ot",                        -- スピード優先最適化
            "/GL",                        -- リンク時コード生成（LTCG）
        }
        linkoptions {
            "/OPT:REF",                   -- 未使用関数を削除
            "/OPT:ICF",                   -- 同一関数の統合（ICF）
            "/LTCG",                      -- /GL対応リンク
        }
        linktimeoptimization "On"         -- PremakeのLTO指定（/GLと整合）
        staticruntime "On"

        libdirs { "$(ProjectDir)Externals/assimp/lib/Release" }
        links { "assimp-vc143-mt.lib" }

        postbuildcommands {
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll\" \"$(TargetDir)dxcompiler.dll\"",
            "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll\" \"$(TargetDir)dxil.dll\"",
            "copy \"$(ProjectDir)Packages\\Scripts\\lib\\mono-2.0-sgen.dll\" \"$(TargetDir)mono-2.0-sgen.dll\""
        }

