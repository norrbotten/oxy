workspace "bigbong"
    language "C++"
    cppdialect "C++17"

    location "build"

    configurations { "debug", "release" }

    buildoptions { "-std=c++2a" }

    filter { "configurations:debug" }
        symbols "On"
    
    filter { "configurations:release" }
        symbols "Off"
        optimize "Full"
        buildoptions { "-Werror", "-Wextra", "-Wall", "-Wpedantic" }
    
    filter { }

project "bigbong"
    kind "ConsoleApp"
    files { "src/**.cpp" }
    includedirs { "src/" }

