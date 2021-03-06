newoption {
   trigger     = "lang",
   description = "Force to compile as a language"
}

workspace "yacbnl"
  configurations { "dbg", "dist" }

  local ca = (_OPTIONS["lang"] or "c")
  language (ca)
  compileas (ca)

  premake.info("compiling as " .. ca)

  flags { "fatalwarnings", "linktimeoptimization" }

  filter { "action:gmake*", "toolset:gcc"}
    buildoptions {
      "-Wall", "-Wextra", "-Wfloat-equal", "-Winline", "-Wundef", "-Werror",
      "-fverbose-asm", "-Wint-to-pointer-cast", "-Wshadow", "-Wpointer-arith",
      "-Wcast-align", "-Wcast-qual", "-Wunreachable-code", "-Wstrict-overflow=5",
      "-Wwrite-strings", "-Wconversion", "--pedantic-errors",
      "-Wredundant-decls", "-Werror=maybe-uninitialized",
      "-Wmissing-declarations",
    }
  filter { "language:c" }
    buildoptions {
      "-xc", "-std=c11", "-Wmissing-parameter-type",
      "-Wmissing-prototypes",
      "-Wnested-externs", "-Wold-style-declaration",
      "-Wold-style-definition", "-Wstrict-prototypes", "-Wpointer-sign"
    }

  filter { "language:c++" }
    buildoptions { "-xc++", "-std=c++14" }

  filter "configurations:dbg"
    optimize "off"
    symbols "on"
    buildoptions { "-ggdb3", "-O0", "-DDEBUG" }

  filter "configurations:dist"
    optimize "full"
    symbols "off"

  project "interact"
    kind "consoleapp"

    files { "src/main.c" }

    links { "m", "yacbnl" }

    targetdir "bin/%{cfg.buildcfg}/"

    filter "configurations:dist"
      buildoptions { "-O3", "-fomit-frame-pointer" }

  project "yacbnl"
    kind "staticlib"

    files { "src/yacbnl.c" }

    links { "m" }

    targetdir "bin/%{cfg.buildcfg}/lib"

    filter "configurations:dist"
      buildoptions { "-O3", "-fomit-frame-pointer" }

  project "test"
    kind "consoleapp"

    files { "src/test/*.c" }

    links { "m", "criterion", "yacbnl" }

    targetdir "bin/%{cfg.buildcfg}"
    targetname "test_%{wks.name}"
    buildoptions { "-DDEBUG" }

    filter "configurations:dist"
      buildoptions { "-O0", "-fomit-frame-pointer" }

  -- test the minified code is syntactically correct and compiles
  project "compile_minified"
    kind "consoleapp"
    files { "util/_min_test.c", "min/yacbnl.min.c" }
    links { "m" }
    targetdir "bin/%{cfg.buildcfg}"
    targetname "min_build_%{wks.name}"

  project "minify"
    kind "utility"

    local min_exe = path.join("util", "minify.py")

    -- find python27

    local python_interp = ""
    if "windows" == os.target() then
      python_interp = path.join("C:", "Python27", "python.exe")
    else
      local pth, err = os.outputof("which python2.7")
      if err > 0 then
        print(string.format("can't minify without Python2.7 in $PATH (got %s)", pth))
        os.exit(1)
      end
      python_interp = string.explode(pth, " ")[1]
    end

    local copyleft = io.readfile(".copyleft_notice")

    -- contains includes we don't want to filter out
    local all_contents = "#ifndef YACBNL\n#define YACBNL\n" .. io.readfile(path.join("src", "lib", "bn_common.h"))

    -- c files all have exactly one include
    for _, file in next, os.matchfiles(path.join("src", "lib", "*.c")) do
      local this = io.readfile(file)
      local lines = {}
      for s in this:gmatch("[^\r\n]+") do
        if not string.startswith(s, "#include") then
          table.insert(lines, s .. "\n")
        end
      end
      local final = table.concat(lines, "")

      all_contents = all_contents .. "\n" .. final .. "\n"
    end

    all_contents = all_contents .. "#endif\n"

    -- write out full unminified contents
    local unmin = path.join("min", "yacbnl.full.c")
    io.writefile(unmin, all_contents)

    -- minify
    local min = path.join("min", "yacbnl.min.c")
    local cmd = string.format("%s %s %s", python_interp, min_exe, unmin)
    local min_contents, err = os.outputof(cmd)
    if err > 0 then
      premake.error("error in minifying")
      os.exit(1)
    end
    io.writefile(min, copyleft .. min_contents .. "\n")

    -- minify .h
    local header = path.join("src", "lib", "bn_common.h")
    local header_out = path.join("min", "yacbnl.min.h")
    local out_cmd = string.format("%s %s %s", python_interp, min_exe, header)
    local header_min, herr = os.outputof(out_cmd)
    if herr > 0 then
      premake.error("error in minifying")
      os.exit(1)
    end
    io.writefile(header_out, copyleft .. "#ifndef YACBNL_H\n#define YACBNL_H\n" .. header_min .. "\n#endif\n")

  project "clobber"
    kind "makefile"

    local dirs = " bin obj "

    -- on windows, clean like this
    filter "system:not windows"
      cleancommands {
        "({RMDIR}" .. dirs .."*.make Makefile *.o -r 2>/dev/null; echo)"
      }

    -- otherwise, clean like this
    filter "system:windows"
      cleancommands {
        "{DELETE} *.make Makefile *.o",
        "{RMDIR}" .. dirs
      }
