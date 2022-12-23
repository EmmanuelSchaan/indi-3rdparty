# - Try to find Pololu Tic library
# Once done this will define
#
#  POLOLU_TIC_FOUND - system has found the library
#  POLOLU_TIC_INCLUDE_DIR - the include directory
#  POLOLU_TIC_LIBRARIES - Link these to use the library

# Copyright (c) 2006, Jasem Mutlaq <mutlaqja@ikarustech.com>
# Based on FindLibfacile by Carsten Niehaus, <cniehaus@gmx.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (POLOLU_TIC_INCLUDE_DIR AND POLOLU_TIC_LIBRARIES)

  # in cache already
  set(POLOLU_TIC_FOUND TRUE)
  message(STATUS "Found libpololu-tic-1: ${POLOLU_TIC_LIBRARIES}")

else (POLOLU_TIC_INCLUDE_DIR AND POLOLU_TIC_LIBRARIES)

   #  find_path(POLOLU_TIC_INCLUDE_DIR libpololu-tic-1.h
   #    PATH_SUFFIXES libpololu-tic-1
   #    ${_obIncDir}
   #    ${GNUWIN32_DIR}/include
   #  )
   #
   #  find_library(POLOLU_TIC_LIBRARIES NAMES libpololu-tic-1 
   #    PATHS
   #    ${_obLinkDir}
   #    ${GNUWIN32_DIR}/lib
   #  )

  # Manu: Setting by hand the include and library paths
  set(POLOLU_TIC_INCLUDE_DIR "/usr/local/include/libpololu-tic-1")
  set(POLOLU_TIC_LIBRARIES "/usr/local/lib/libpololu-tic-1.so")
  #set(POLOLU_TIC_LIBRARIES "/usr/local/lib/")

  set(CMAKE_REQUIRED_INCLUDES ${POLOLU_TIC_INCLUDE_DIR})
  set(CMAKE_REQUIRED_LIBRARIES ${POLOLU_TIC_LIBRARIES})

  if(POLOLU_TIC_INCLUDE_DIR AND POLOLU_TIC_LIBRARIES)
    set(POLOLU_TIC_FOUND TRUE)
  else (POLOLU_TIC_INCLUDE_DIR AND POLOLU_TIC_LIBRARIES)
    set(POLOLU_TIC_FOUND FALSE)
  endif(POLOLU_TIC_INCLUDE_DIR AND POLOLU_TIC_LIBRARIES)

  if (POLOLU_TIC_FOUND)
    if (NOT POLOLU_TIC_FIND_QUIETLY)
      message(STATUS "Found POLOLU_TIC: ${POLOLU_TIC_LIBRARIES}")
    endif (NOT POLOLU_TIC_FIND_QUIETLY)
  else (POLOLU_TIC_FOUND)
    if (POLOLU_TIC_FIND_REQUIRED)
      message(FATAL_ERROR "libpololu-tic-1 not found.")
    endif (POLOLU_TIC_FIND_REQUIRED)
  endif (POLOLU_TIC_FOUND)

  mark_as_advanced(POLOLU_TIC_INCLUDE_DIR POLOLU_TIC_LIBRARIES)
  
endif (POLOLU_TIC_INCLUDE_DIR AND POLOLU_TIC_LIBRARIES)
