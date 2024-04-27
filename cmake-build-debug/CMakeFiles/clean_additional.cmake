# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "BlankNGL_autogen"
  "CMakeFiles/BlankNGL_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/BlankNGL_autogen.dir/ParseCache.txt"
  )
endif()
