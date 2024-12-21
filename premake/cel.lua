workspace "cel"
   configurations { "Debug", "Release" }
   location "../build"

   libdirs { "../dep/libuv/build" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   filter { "system:not Windows" }
      buildoptions { "-std=c++17"}

   filter { "system:windows" }
      buildoptions { "/std:c++17" }

project "cel"
   kind "StaticLib"
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"

   files { "../src/**.h", "../src/**.cpp" }
   includedirs { "../src", "../dep/libuv/include" }

project "EchoTest"
   kind "ConsoleApp"
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"

   files { "../test/EchoServer.cpp" }
   includedirs { "../src" }
   links { "cel" }

   -- -Bstatic doesn't work on mac :(
   filter { "system:macosx" }
      linkoptions { "../dep/libuv/build/libuv.a" }

   filter { "system:unix" }
      links { "uv:static" }
