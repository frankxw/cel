workspace "Tests"
   configurations { "Debug", "Release" }
   location "../build"

project "TestMain"
   kind "ConsoleApp"
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"

   files { "../test/**.h", "../test/**.cpp", "../src/**.h", "../src/**.cpp" }
   includedirs { "../src", "../dep/libuv/include" }
   libdirs { "../dep/libuv/build" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   -- -Bstatic doesn't work on mac :(
   filter { "system:macosx" }
      linkoptions { "../dep/libuv/build/libuv.a" }

   filter { "system:unix" }
      links { "uv:static" }

   filter { "system:not Windows", "action:gmake" }
      buildoptions { "-std=c++17"}
