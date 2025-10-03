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
    filter "configurations:Develop"
        runtime "Release" -- 開発用のリリースビルド
        symbols "On"
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
        staticruntime "On"
    filter "configurations:Develop"
        staticruntime "On"
    filter "configurations:Release"
        staticruntime "On"


project "ONEngine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "On"

    targetdir ("../Generated/Outputs/%{cfg.buildcfg}")
    objdir ("../Generated/Obj/%{prj.name}/%{cfg.buildcfg}")
    debugdir "%{wks.location}"
    files {
        "Engine/**.h",
        "Engine/**.cpp",
        "Game/**.h",
        "Game/**.cpp"
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

    links {
        "assimp-vc143-mtd.lib",
        "mono-2.0-sgen.lib",
        "DirectXTex",  -- Premake で追加したプロジェクト
        "ImGui"        -- Premake で追加したプロジェクト
    }

    postbuildcommands {
        "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll\" \"$(TargetDir)dxcompiler.dll\"",
        "copy \"$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll\" \"$(TargetDir)dxil.dll\"",
        "copy \"$(ProjectDir)Packages\\Scripts\\lib\\mono-2.0-sgen.dll\" \"$(TargetDir)mono-2.0-sgen.dll\"",
        "xcopy /E /Y /I \"$(ProjectDir)Assets\" \"$(TargetDir)Assets\"",
        "xcopy /E /Y /I \"$(ProjectDir)Packages\" \"$(TargetDir)Packages\""
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        optimize "Off"
        defines { "_DEBUG", "_WINDOWS" }
        buildoptions { "/utf-8" }

    filter "configurations:Release"
        runtime "Release"
        symbols "On"
        optimize "Speed"
        defines { "NDEBUG", "_WINDOWS" }
        buildoptions { "/utf-8" }
        linktimeoptimization "On"

    filter "configurations:Development"
        runtime "Release"
        symbols "On"
        optimize "Off"
        defines { "DEBUG_BUILD", "_WINDOWS" }
        buildoptions { "/utf-8" }
        linktimeoptimization "On"

