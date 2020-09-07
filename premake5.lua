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

function include_sfml()
    includedirs { "ext/sfml/include" }
    libdirs { "ext/sfml/build/lib" }
    links { "sfml-graphics", "sfml-window", "sfml-system" }
end

project "bigbong"
    kind "ConsoleApp"
    files { "src/**.cpp" }
    includedirs { "src/" }

    include_sfml()

newaction {
    trigger = "build",
    description = "build",
    execute = function()
        os.execute("rm -rf build")
        os.execute("premake5 gmake2 && cd build && make -j")
    end
}

newaction {
    trigger = "debug",
    description = "debug",
    execute = function()
        os.execute("(premake5 gmake2 && cd build && make -j) && gdb -ex run ./build/bin/debug/bigbong")
    end
}
