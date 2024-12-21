workspace "Tests"
   configurations { "Debug", "Release" }
   location "../build"

project "TestMain"
   kind "ConsoleApp"
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"

   files { "../test/**.h", "../test/**.cpp", "../src/**.h", "../src/**.cpp" }
   includedirs { "../src", "../dep/libuv/include" }
   links { "uv" }
   libdirs { "../dep/libuv/build" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   filter { "system:not windows", "action:gmake" }
      buildoptions { "-std=c++17"}
