workspace "ONEngine"
    architecture "x64"
    startproject "ONEngine"
    configurations { "Debug", "Release", "Development" }

project "ONEngine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("../Generated/Outputs/%{cfg.buildcfg}")
    objdir ("../Generated/Obj/%{prj.name}/%{cfg.buildcfg}")

    files {
        "Engine/**.cpp",
        "Engine/**.h",
        "Game/**.cpp",
        "Game/**.h",
        "subprojects/csharplibrary/scripts/game/*.cs",
        "Externals/**.h",
        "Externals/**.cpp"
    }

    includedirs {
        "Game",
        "Resources",
        "Lib",
        "ONEngine",
        "Externals/assimp/include",
        "Externals/imgui",
        "Externals/glib",
        "Externals/mono",
        "Externals/DirectXTex",
        "Externals"
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

