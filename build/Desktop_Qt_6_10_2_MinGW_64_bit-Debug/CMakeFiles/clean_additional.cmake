# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\planche_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\planche_autogen.dir\\ParseCache.txt"
  "planche_autogen"
  )
endif()
