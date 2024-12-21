workspace "Tests"
   configurations { "Debug", "Release" }
   location "../build"

project "TestMain"
   kind "ConsoleApp"
   language "C++"
   targetdir "../bin/%{cfg.buildcfg}"

   files { "../test/**.h", "../test/**.cpp" }
   includedirs { "../src" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
