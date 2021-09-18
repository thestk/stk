cmake_minimum_required(VERSION 3.1) ##TODO: which version is better

project(STK VERSION 4.6.1)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

if(NOT CMAKE_BUILD_TYPE) ##TODO: options
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()
# message("Generated with config types: ${CMAKE_CONFIGURATION_TYPES}")
message(${CMAKE_BUILD_TYPE}) ##TODO: necessary or not

## TODO, compiler dependent?
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -D_STK_DEBUG_ -D__RTAUDIO_DEBUG__ -D__RTMIDI_DEBUG__")

if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    message("GCC.")
    set(CMAKE_CXX_FLAGS "-Wall")
endif()

option(BUILD_SHARED "Whether to build the shared library" OFF)
option(BUILD_STATIC "Whether to build the static library" ON)
option(REALTIME "Realtime suppor" ON)
option(ENABLE_JACK "Enable JACK" ON)
option(ENABLE_ALSA "Enable ALSA API support (linux only)" ON)
option(ENABLE_OSS "Enable OSS API Support (unixes only)" ON)
option(ENABLE_ASIO "Enable ASIO API support (windows only)" ON)
option(ENABLE_DS "Enable DirectSound API support (windows only)" ON)
option(ENABLE_WASAPI "Enable Windows Audio Session API support (windows only)" ON)
option(ENABLE_CORE "Enable CoreAudio API support (mac only)" ON)
option(COMPILE_PROJECTS "Compile all the example projects" OFF)

include_directories("./include")
file(GLOB STK_SRC "./src/*.cpp") # GLOB instead of GLOB_RECURSE as the asio depends on system

add_library(stk STATIC ${STK_SRC} )

#========================================#
#========== Realtime Support ============#
#========================================#
if(REALTIME)

  # find_package(JACK) # TODO NEED FindJACK.cmake
  find_library(JACK_LIBRARY Jack)
  message("${JACK_LIBRARY}")

  set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
  set(THREADS_PREFER_PTHREAD_FLAG TRUE)
  find_package(Threads)
  if(Threads_FOUND)
	target_link_libraries(stk PUBLIC Threads:Threads)
  else()
	message(FATAL_ERROR "RtAudio requires the pthread library")
  endif()

  if(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
	#==============    LINUX       ================#
    message("Linux DETECTED!")
    if(ENABLE_ALSA)
        find_package(ALSA REQUIRED)
        if(ALSA_FOUND)
            include_directories(${ALSA_INCLUDE_DIRS})
			target_link_libraries(stk PUBLIC ${ALSA_LIBRARIES})
            target_compile_definitions(stk PUBLIC __LINUX_ALSA__)
        endif()
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
        #==============    MAC OS    ================#
        message("Machintosh!")
        find_package(CoreAudio REQUIRED)
        include_directories(${COREAUDIO_INCLUDE_DIRS})
        target_compile_definitions(stk PUBLIC -D__MACOSX_CORE__)
        target_link_libraries(stk PUBLIC COREAUDIO_LIBRARY COREAUDIO_FOUNDATION COREAUDIO_MIDI)
    endif()

  # TODO: WINDOWS SUPPORT
  elseif(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
  #==============      WINDOWS      ================#
    message("Windows!")
  #   target_compile_definitions(stk PUBLIC __OS_WINDOWS__)
  else()
	message(FATAL_ERROR "Unknown system type for realtime support.")
	# TODO: try --disable-realtime argument!
  endif()
endif()


if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
  target_compile_definitions(stk PUBLIC _STK_DEBUG_)
endif()


include(TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(NOT IS_BIG_ENDIAN)
  target_compile_definitions(stk PUBLIC __LITTLE_ENDIAN__)
endif()
# if(CMAKE_CXX_BYTE_ORDER STREQUAL LITTLE_ENDIAN)
#   target_compile_definitions(stk PUBLIC __LITTLE_ENDIAN__)
# endif()

if(COMPILE_PROJECTS)
    add_subdirectory(projects/examples)
    add_subdirectory(projects/eguitar)
    add_subdirectory(projects/demo)
    add_subdirectory(projects/effects)
    add_subdirectory(projects/ragamatic)
endif()