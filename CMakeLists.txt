cmake_minimum_required(VERSION 3.1) ##TODO: which version is better

project(STK VERSION 4.6.1)
set (CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()
SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "Debug" "RelWithDebInfo" "MinSizeRel") 
message("Build type: " ${CMAKE_BUILD_TYPE})

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7 /Ob0 /Od /RTC1 /D_ITERATOR_DEBUG_LEVEL=2 /D_STK_DEBUG_ /D__RTAUDIO_DEBUG__ /D__RTMIDI_DEBUG__")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -D_STK_DEBUG_ -D__RTAUDIO_DEBUG__ -D__RTMIDI_DEBUG__")
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    message("GCC.")
    set(CMAKE_CXX_FLAGS "-Wall")
endif()

option(BUILD_SHARED "Whether to build the shared library" ON)
option(BUILD_STATIC "Whether to build the static library" ON)
option(REALTIME "Realtime support" ON)
option(ENABLE_JACK "Enable JACK" ON)
option(ENABLE_ALSA "Enable ALSA API support (linux only)" ON)
# option(ENABLE_OSS "Enable OSS API Support (unixes only)" ON)
option(ENABLE_ASIO "Enable ASIO API support (windows only)" OFF)
option(ENABLE_DS "Enable DirectSound API support (windows only)" ON)
option(ENABLE_WASAPI "Enable Windows Audio Session API support (windows only)" OFF)
# option(ENABLE_CORE "Enable CoreAudio API support (mac only)" ON)
option(COMPILE_PROJECTS "Compile all the example projects" ON)
option(INSTALL_HEADERS "Install headers" ON)

include_directories("./include")
file(GLOB STK_SRC "./src/*.cpp") # GLOB instead of GLOB_RECURSE as the asio depends on system

#========================================#
#========== Realtime Support ============#
#========================================#
if(REALTIME)
    if(ENABLE_JACK)
        # Try to find PkgConfig
        find_package(PkgConfig QUIET)

        if(PkgConfig_FOUND)
            # PkgConfig is available, use it
            pkg_check_modules(JACK QUIET jack)
        endif()

        if(NOT JACK_FOUND)
            # PkgConfig was not found or Jack was not found through it, try a fallback
            message(STATUS "PkgConfig not found or failed to find Jack, attempting fallback")

            # Fallback: Search in common locations
            find_path(JACK_INCLUDE_DIR
                NAMES jack/jack.h
                HINTS
                    ENV JACK_ROOT
                    "$ENV{ProgramFiles}/Jack"
                    /usr/local/include
                    /usr/include
            )

            find_library(JACK_LIBRARY
                NAMES jack
                HINTS
                    ENV JACK_ROOT
                    "$ENV{ProgramFiles}/Jack"
                    /usr/local/lib
                    /usr/lib
            )

            # Check if the fallback was successful
            if(JACK_INCLUDE_DIR AND JACK_LIBRARY)
                set(JACK_FOUND TRUE)
                set(JACK_INCLUDE_DIRS ${JACK_INCLUDE_DIR})
                set(JACK_LIBRARIES ${JACK_LIBRARY})
                message(STATUS "Found Jack (fallback):")
                message(STATUS "  Includes: ${JACK_INCLUDE_DIRS}")
                message(STATUS "  Libraries: ${JACK_LIBRARIES}")
            else()
                message(WARNING "Failed to find Jack library even with fallback. Please install Jack development package or ensure it is in a standard location.")
            endif()
        endif()

        if(JACK_FOUND)
            include_directories(${JACK_INCLUDE_DIRS})
            link_libraries(${JACK_LIBRARIES})
            add_definitions(-D__UNIX_JACK__)
        endif()
    endif()

    message("${CMAKE_SYSTEM_NAME}")
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads REQUIRED)
    link_libraries(Threads::Threads)
    if(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
        # TODO: Finish Linux configuration, include different audio API supports
        #==============    LINUX       ================#
        message("Linux DETECTED!")
        if(ENABLE_ALSA)
            find_package(ALSA REQUIRED)
            if(ALSA_FOUND)
                include_directories(${ALSA_INCLUDE_DIRS})
                link_libraries(${ALSA_LIBRARIES})
                add_definitions(-D__LINUX_ALSA__)
            endif()
        endif()
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
        #==============    MAC OS    ================#
        message("Machintosh DETECTED!")
        find_package(CoreAudio REQUIRED)
        include_directories(${COREAUDIO_INCLUDE_DIRS})
        add_definitions(-D__MACOSX_CORE__)
        link_libraries(${COREAUDIO_LIBRARY} ${COREAUDIO_FOUNDATION} ${COREAUDIO_MIDI})
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
        # TODO: MORE SUPPORT (e.g., MSYS)?
        # Tested under MSYS2 with Mingw64 toolchain
        #==============      WINDOWS      ================#
        message("Windows DETECTED!")
        link_libraries(winmm ole32 wsock32)
        add_definitions(-D__WINDOWS_MM__)

        # TODO: ASIO NOT WORKING YET
        if(ENABLE_ASIO)
            message("ENALBING ASIO")
            include_directories("./src/include")
            # target_sources(stk PUBLIC "${CMAKE_SOURCE_DIR}/src/include/asio.cpp" "${CMAKE_SOURCE_DIR}/src/include/asiodrivers.cpp"
            #                     "${CMAKE_SOURCE_DIR}/src/include/asiolist.cpp" "${CMAKE_SOURCE_DIR}/src/include/iasiothiscallresolver.cpp")
            add_definitions(-D__WINDOWS_ASIO__)
        endif()

        if(ENABLE_WASAPI)
            message("ENALBING WASAPI")
            link_libraries(mfuuid mfplat wmcodecdspuuid ksuser)
            add_definitions(-D__WINDOWS_WASAPI__)
        endif()
        
        if(ENABLE_DS)
            message("ENALBING Directsound")
            link_libraries(dsound)
            add_definitions(-D__WINDOWS_DS__)
        endif()
    else()
        message("CMAKE_SYSTEM_NAME:" ${CMAKE_SYSTEM_NAME})
        message(FATAL_ERROR "Unknown system type for realtime support.")
    endif()
endif()

include(TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(NOT IS_BIG_ENDIAN)
  add_definitions(-D__LITTLE_ENDIAN__)
endif()
add_definitions(-D_USE_MATH_DEFINES)
if(INSTALL_HEADERS)
    file(GLOB STK_HEADERS "include/*.h")
    install(FILES ${STK_HEADERS} DESTINATION include/stk)
endif()
#========================================#
#========== Build the Library ===========#
#========================================#
if(BUILD_STATIC)
    add_library(stk STATIC ${STK_SRC} )
    target_include_directories(stk PRIVATE include PUBLIC $<INSTALL_INTERFACE:include>)
    set_target_properties(stk PROPERTIES PUBLIC_HEADER "${LIBSTK_HEADERS}")
    list(APPEND STK_TARGETS stk)
endif()

if(BUILD_SHARED)
    add_library(stk_SHARED SHARED ${STK_SRC})
    set_target_properties(stk_SHARED PROPERTIES OUTPUT_NAME stk) # rename the shared library name
    target_include_directories(stk_SHARED PRIVATE include PUBLIC $<INSTALL_INTERFACE:include>)
    set_target_properties(stk_SHARED PROPERTIES PUBLIC_HEADER "${LIBSTK_HEADERS}")
    list(APPEND STK_TARGETS stk_SHARED)
endif()

#========================================#
#========= Build the examples ===========#
#========================================#
if(COMPILE_PROJECTS)
    message("COMPILE PROJECTS!")
    add_subdirectory(projects/examples)
    add_subdirectory(projects/eguitar)
    add_subdirectory(projects/demo)
    add_subdirectory(projects/effects)
    add_subdirectory(projects/ragamatic)
endif()

#========================================#
#========= Install ======================#
#========================================#

install(TARGETS ${STK_TARGETS} EXPORT stk-config
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	PUBLIC_HEADER DESTINATION include/stk)

install(EXPORT stk-config DESTINATION lib/cmake/stk)
