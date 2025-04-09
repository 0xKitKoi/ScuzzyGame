workspace "Scuzzy"
   architecture "x64"
   configurations { "Debug", "Release" }

   -- SDL versions
   sdl2_version = "2.32.4"
   sdl2_image_version = "2.8.8"
   sdl2_ttf_version = "2.24.0"

project "Scuzzy"
   location "Scuzzy"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   staticruntime "off"

   targetdir ("bin/%{cfg.buildcfg}")
   objdir ("bin-int/%{cfg.buildcfg}")

   files {
      "Scuzzy/Source/**.h",
      "Scuzzy/Source/**.hpp",
      "Scuzzy/Source/**.cpp"
   }

   includedirs {
      "Scuzzy",
      "thirdparty/SDL2/include",
      "thirdparty/SDL2_image/include",
      "thirdparty/SDL2_ttf/include"
   }

   libdirs {
      "thirdparty/SDL2/lib/x64",
      "thirdparty/SDL2_image/lib/x64",
      "thirdparty/SDL2_ttf/lib/x64"
   }

   links {
      "SDL2",
      "SDL2main",
      "SDL2_image",
      "SDL2_ttf"
   }

 filter "system:windows"
   systemversion "latest"
   defines { "PLATFORM_WINDOWS" }

   postbuildcommands {
      -- Copy SDL2.dll
      "{COPY} %{wks.location}/thirdparty/SDL2/lib/x64/SDL2.dll %{cfg.targetdir}",
      -- Copy SDL2_image.dll
      "{COPY} %{wks.location}/thirdparty/SDL2_image/lib/x64/SDL2_image.dll %{cfg.targetdir}",
      -- Copy SDL2_ttf.dll
      "{COPY} %{wks.location}/thirdparty/SDL2_ttf/lib/x64/SDL2_ttf.dll %{cfg.targetdir}",
      "{COPYDIR} %{wks.location}/Scuzzy/data %{cfg.targetdir}/data"
   }





   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
