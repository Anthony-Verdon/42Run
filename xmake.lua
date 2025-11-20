set_languages("cxx20")

set_toolchains("clang")
set_warnings("allextra", "error")
set_optimize("fastest")
set_symbols("debug")

add_requires("magic_enum")
add_requires("imgui", {configs = {glfw = true, opengl3 = true}})
add_requires("doctest")

option("HOTRELOAD")
    set_showmenu(true)
    set_description("Enable Hotreload")
    set_default(false)
    set_category("Features")
option_end()

TARGET_NAME = "42Run"
PATH_TO_ENGINE = "submodules/Engine/"
DEBUG_DRAW_PHYSIC_3D = 1

includes(PATH_TO_ENGINE)

set_config("PATH_TO_ENGINE", PATH_TO_ENGINE)
set_config("TARGET_NAME", TARGET_NAME)
set_config("PATH_TO_ENGINE", "submodules/Engine/")
set_config("FULL_SCREEN", "0")
set_config("DEBUG_DRAW_PHYSIC_3D", tostring(DEBUG_DRAW_PHYSIC_3D))

if has_config("HOTRELOAD") then
    add_defines("HOTRELOAD")
end 

add_rules("plugin.compile_commands.autoupdate")

function add_common_sources()
    add_files("srcs/**.cpp")
    add_includedirs("srcs")
    add_packages("magic_enum")
    add_packages("imgui")
    add_deps("Engine")
end

target("Game")
    set_targetdir("./")
    if has_config("HOTRELOAD") then
        set_kind("shared")
    else 
        set_kind("binary")
        add_files("srcs/main.cpp")
    end
    set_basename(TARGET_NAME)
    add_includedirs("srcs")
    add_defines("DRAW_IMGUI=1")
    add_defines("SPAWN_OBSTACLES=1")
    add_defines("SPAWN_COLLECTIBLES=1")
    add_defines("FLAT_TERRAIN=1")
    add_defines("SPAWN_TURN=0")
    add_defines("DEBUG_DRAW_PHYSIC_3D=" .. tostring(DEBUG_DRAW_PHYSIC_3D))
    add_defines("CAMERA_MODE=2") -- 0: FOLLOWING PLAYER, 1: DETACHED, 2: SIDE
    add_common_sources()

target("42RunTester")
    set_targetdir("./")
    set_kind("binary")
    add_files("tests_srcs/main.cpp")
    add_includedirs("srcs")
    add_packages("doctest")
    add_common_sources()
    remove_files("srcs/main.cpp")