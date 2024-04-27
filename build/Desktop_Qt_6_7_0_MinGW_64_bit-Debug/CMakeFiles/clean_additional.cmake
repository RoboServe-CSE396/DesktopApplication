# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\deneme_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\deneme_autogen.dir\\ParseCache.txt"
  "deneme_autogen"
  )
endif()
