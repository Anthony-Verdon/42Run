set_languages("cxx17")

set_toolchains("clang")
set_warnings("allextra", "error")
set_optimize("fastest")
set_symbols("debug")

add_requires("magic_enum")
add_requires("imgui", {configs = {glfw = true, opengl3 = true}})
add_requires("doctest")

includes("submodules/Engine/")

set_config("PATH_TO_ENGINE", "submodules/Engine/")
set_config("FULL_SCREEN", "0")

add_rules("plugin.compile_commands.autoupdate")

function add_common_sources()
    add_files("srcs/Game/**.cpp")
    add_files("srcs/MapManager/**.cpp")
    add_files("srcs/Player/**.cpp")
    add_includedirs("srcs")
    add_packages("magic_enum")
    add_packages("imgui")
    add_deps("Engine")
end

target("42Run")
    set_targetdir("./")
    set_kind("binary")
    add_files("srcs/main.cpp")
    add_includedirs("srcs")
    add_defines("DRAW_IMGUI=1")
    add_defines("SPAWN_OBSTACLES=0")
    add_defines("FLAT_TERRAIN=1")
    add_defines("SPAWN_TURN=0")
    add_defines("CAMERA_DETACH=0")
    add_common_sources()

target("42RunTester")
    set_targetdir("./")
    set_kind("binary")
    add_files("tests_srcs/main.cpp")
    add_includedirs("srcs")
    add_packages("doctest")
    add_common_sources()