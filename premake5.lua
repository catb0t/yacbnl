workspace "yacbnl"
  configurations { "dbg", "dist" }

  language "C"

  flags { "fatalwarnings", "linktimeoptimization" }

  filter "action:gmake*"
    buildoptions {
      "-Wall", "-Wextra", "-Wfloat-equal", "-Winline", "-Wundef", "-Werror",
      "-fverbose-asm", "-Wint-to-pointer-cast", "-Wshadow", "-Wpointer-arith",
      "-Wcast-align", "-Wcast-qual", "-Wunreachable-code", "-Wstrict-overflow=5",
      "-Wwrite-strings", "-Wconversion", "--pedantic-errors",
      "-Wredundant-decls", "-Werror=maybe-uninitialized",
      "-Wbad-function-cast", "-Wmissing-declarations", "-Wmissing-parameter-type",
      "-Wmissing-prototypes", "-Wnested-externs", "-Wold-style-declaration",
      "-Wold-style-definition", "-Wstrict-prototypes", "-Wpointer-sign"
    }

  filter "configurations:dbg"
    optimize "Off"
    symbols "On"
    buildoptions { "-ggdb3", "-O0" }

  filter "configurations:dist"
    optimize "Full"
    symbols "Off"
    buildoptions { "-O3" }

  filter {}

  project "interact"
    kind "consoleapp"

    files { "src/main.c" }

    links { "m", "yacbnl" }

    targetdir "bin/%{cfg.buildcfg}/"

  project "yacbnl"
    kind "staticlib"

    files { "src/yacbnl.c" }

    links { "m" }

    targetdir "bin/%{cfg.buildcfg}/lib"

  project "test"
    kind "consoleapp"

    files { "src/test/*.c" }

    links { "m", "criterion", "yacbnl" }

    targetdir "bin/%{cfg.buildcfg}"
    targetprefix "test_"

--  project "minify"
--    kind "utility"

    -- TODO: write and port to windows

  project "clobber"
    kind "makefile"

    -- on windows, clean like this
    filter "system:not windows"
      cleancommands {
        "({RMDIR} bin obj *.make Makefile *.o -r 2>/dev/null; echo)"
      }

    -- otherwise, clean like this
    filter "system:windows"
      cleancommands {
        "{DELETE} *.make Makefile *.o",
        "{RMDIR} bin obj"
      }
