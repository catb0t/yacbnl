function string:split (sep)
    local sep, fields = sep or ":", {}
    local pattern = string.format("([^%s]+)", sep)
    self:gsub(pattern, function(c) fields[#fields+1] = c end)
    return fields
end

workspace "yacbnl"
  configurations { "dbg", "dist" }

  language "C"

  flags { "fatalwarnings", "linktimeoptimization" }

  filter { "action:gmake*", "toolset:gcc"}
    buildoptions {
      "-Wall", "-std=c11", "-Wextra", "-Wfloat-equal", "-Winline", "-Wundef", "-Werror",
      "-fverbose-asm", "-Wint-to-pointer-cast", "-Wshadow", "-Wpointer-arith",
      "-Wcast-align", "-Wcast-qual", "-Wunreachable-code", "-Wstrict-overflow=5",
      "-Wwrite-strings", "-Wconversion", "--pedantic-errors",
      "-Wredundant-decls", "-Werror=maybe-uninitialized",
      "-Wmissing-declarations", "-Wmissing-parameter-type",
      "-Wmissing-prototypes", "-Wnested-externs", "-Wold-style-declaration",
      "-Wold-style-definition", "-Wstrict-prototypes", "-Wpointer-sign"
    }

  filter "configurations:dbg"
    optimize "off"
    symbols "on"
    buildoptions { "-ggdb3", "-O0" }

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

    filter "configurations:dist"
      buildoptions { "-O0", "-fomit-frame-pointer" }

  project "minify"
    kind "utility"

    -- find python27

    local python_interp = ""
    if "windows" == os.target then
      python_interp = path.join("C:", "Python27", "python.exe")
    else
      local pth, err = os.outputof("which python2.7")
      if err > 0 then
        print("can't minify without Python2.7 in $PATH")
        os.exit(1)
      end
      python_interp = pth:split(" ")[1]
    end

    -- contains includes we don't want to filter out
    local all_contents = io.readfile(path.join("src", "lib", "bn_common.h"))

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

    -- write out full unminified contents
    local unmin = path.join("min", "yacbnl.full.c")
    io.writefile(unmin, all_contents)

    -- minify
    local min = path.join("min", "yacbnl.min.c")
    local cmd = string.format("%s %s %s %s", python_interp, path.join("util", "minify.py"), "-w", unmin)
    local min_contents, err = os.outputof(cmd)
    if err > 0 then
      print("error in minifying")
      os.exit(1)
    end
    io.writefile(min, min_contents)

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
