function os.scandir(pth, dironly)
  local m = os.matchstart(pth .. "/*")
  local dirs = {}

  while os.matchnext(m) do
    this = os.matchname(m)
    if not dironly or (dironly and not os.matchisfile(m)) then
      table.insert(dirs, this)
    end
  end

  os.matchdone(m)

  return dirs
end

workspace "yacbnl"
  configurations { "dbg", "dist" }

  language "C"

  flags { "fatalwarnings", "linktimeoptimization" }

  filter { "action:gmake*", "toolset:gcc"}
    buildoptions {
      "-Wall", "-Wextra", "-Wfloat-equal", "-Winline", "-Wundef", "-Werror",
      "-fverbose-asm", "-Wint-to-pointer-cast", "-Wshadow", "-Wpointer-arith",
      "-Wcast-align", "-Wcast-qual", "-Wunreachable-code", "-Wstrict-overflow=5",
      "-Wwrite-strings", "-Wconversion", "--pedantic-errors",
      "-Wredundant-decls", "-Werror=maybe-uninitialized",
      -- "-Wbad-function-cast",
      "-Wmissing-declarations", "-Wmissing-parameter-type",
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
    targetname "test_%{wks.name}"

  project "minify"
    kind "utility"

    local new_test_file = "#include<criterion/criterion.h>\n#include\"../headers.h\"\n"

    for test_file in io.popen("find ./src/test/ -type f -iregex '.*t_[a-z]*\\.c$'"):lines()
    do
      local new_lines = ""
      for _, line in next, string.explode(io.readfile(test_file), "\n")
      do
        if not string.startswith(line, "#include")
        then
          new_lines = new_lines .. line .. "\n"
        end
      end
      new_test_file = new_test_file .. new_lines .. "\n"
    end

    io.writefile(path.join(SOURCEDIR, "test", "_test.c"), new_test_file)


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
