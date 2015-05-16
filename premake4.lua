solution "operating_systems"
   configurations { "Debug", "Release" }

   project "oscontext"
      kind "SharedLib"
      language "C"
      location "build"
      targetdir "build"
      files { "src/*.h", "src/*.c" }
      links { "rt" }

      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }

   project "ospthread"
      kind "SharedLib"
      language "C"
      location "build"
      targetdir "build"
      files { "src/*.h", "src/*.c" }
      excludes { "src/thread.c" }
      links { "pthread" }

      configuration "Debug"
         defines { "DEBUG", "USE_PTHREAD" }
         flags { "Symbols" }

      configuration "Release"
         defines { "NDEBUG", "USE_PTHREAD" }
         flags { "Optimize" }
