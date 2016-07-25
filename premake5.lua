-- Clean Function --
newaction {
   trigger     = "clean",
   description = "clean the software",
   execute     = function ()
      print("clean the build...")
      os.rmdir("./build")
      os.rmdir("./bin")
      os.rmdir("./obj")
      print("done.")
   end
}

workspace "SoLoader"
  location ("build/" .. _ACTION)
  configurations { "ReleaseDLL", "DebugDLL" }
  flags { "No64BitChecks", "StaticRuntime", "MultiProcessorCompile", "C++11" }

  filter "configurations:DebugDLL"
    defines { "DEBUG", "_DEBUG" }
    flags {"Symbols"}
    -- symbols "On"

  filter "configurations:ReleaseDLL"
    defines { "NDEBUG" }
    optimize "Full"
    flags { "NoBufferSecurityCheck", "NoRuntimeChecks" }

project "SoLoader"
  targetname  "soloader_i386"
  kind        "SharedLib"
  language    "C++"
  targetdir   "bin/%{cfg.buildcfg}"
  includedirs {
    os.getenv("HLSDK") .. "/dlls",
    os.getenv("HLSDK") .. "/engine",
    os.getenv("HLSDK") .. "/common",
    os.getenv("HLSDK") .. "/public",
    os.getenv("METAMOD")
  }
  
  files { "src/**.cc" }

  toolset "gcc"
  buildoptions { "-fpermissive", "-mtune=i386" } -- and therefore clang not supported
  linkoptions { "-static-libstdc++" }
