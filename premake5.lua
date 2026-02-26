workspace "Scuzzy"
   architecture "x64"
   configurations { "Debug", "Release" }

   -- SDL versions
   sdl2_version = "2.32.4"
   sdl2_image_version = "2.8.8"
   sdl2_ttf_version = "2.24.0"
   sdl2_mixer_version = "2.8.1"

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
      "thirdparty/SDL2_ttf/include",
      "thirdparty/SDL2_mixer/include"
   }

   libdirs {
      "thirdparty/SDL2/lib/x64",
      "thirdparty/SDL2_image/lib/x64",
      "thirdparty/SDL2_ttf/lib/x64",
      "thirdparty/SDL2_mixer/lib/x64"
   }

   links {
      "SDL2",
      "SDL2main",
      "SDL2_image",
      "SDL2_ttf",
      "SDL2_mixer"
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
      -- Copy SDL2_mixer.dll
      "{COPY} %{wks.location}/thirdparty/SDL2_mixer/lib/x64/SDL2_mixer.dll %{cfg.targetdir}",
      -- Copy data/ to be next to executable.
      "{COPYDIR} %{wks.location}/Scuzzy/data %{cfg.targetdir}/data"
   }

    filter "system:linux"
      defines { "PLATFORM_LINUX" }
      links { "SDL2", "SDL2_image", "SDL2_ttf", "SDL2_mixer" }
      libdirs {
         "/usr/lib/x86_64-linux-gnu",
         "/usr/local/lib"
      }
      includedirs {
         "/usr/include/SDL2",
         "/usr/local/include/SDL2"
      }

      postbuildcommands {
         -- Copy the data directory to the output directory
         "{COPYDIR} data %{cfg.targetdir}"
      }


   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      optimize "Off"
      -- buildoptions {
      --  "-fsanitize=address",
      --  "-fno-omit-frame-pointer"
      --}

      --linkoptions {
      --  "-fsanitize=address"
      --}

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
