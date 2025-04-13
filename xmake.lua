set_languages("cxx17")

set_toolchains("clang")
set_warnings("allextra", "error")
set_optimize("fastest")
set_symbols("debug")

add_requires("magic_enum")

includes("submodules/Engine/")

set_config("PATH_TO_ENGINE", "submodules/Engine/")

target("42Run")
    set_targetdir("./")
    set_kind("binary")
    add_files("srcs/**.cpp")
    add_includedirs("srcs")
    add_packages("magic_enum")
    add_deps("Engine")