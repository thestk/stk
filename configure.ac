# Process this file with autoconf to produce a configure script.
AC_INIT(STK, 4.1, gary@ccrma.stanford.edu, stk)
AC_CONFIG_SRCDIR(src/Stk.cpp)
AC_CONFIG_FILES(src/Makefile projects/demo/Makefile projects/effects/Makefile projects/ragamatic/Makefile projects/examples/Makefile)

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX(CC g++ c++ cxx)
AC_PROG_CXX
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB(stdc++, printf, , AC_MSG_ERROR(Stk requires the C++ library!) )

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h sys/ioctl.h sys/time.h termio.h unistd.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_BIGENDIAN
AC_EGREP_CPP(yes,
[#ifndef WORDS_BIGENDIAN
  yes
#endif
], [AC_SUBST( byte_order, [-D__LITTLE_ENDIAN__] )] )

AC_MSG_CHECKING(for RAWWAVE_PATH argument)
AC_SUBST( rawwaves, $RAWWAVE_PATH )
AC_MSG_RESULT($RAWWAVE_PATH)

AC_MSG_CHECKING(for INCLUDE_PATH argument)
AC_SUBST( include, $INCLUDE_PATH )
AC_MSG_RESULT($INCLUDE_PATH)

# Check for realtime support disable
AC_MSG_CHECKING(whether to compile realtime support)
AC_ARG_ENABLE(realtime, [  --disable-realtime = only compile generic non-realtime classes], [AC_SUBST( realtime, [no] ) AC_SUBST( sound_api, [] )], [AC_SUBST( realtime, [yes] ) ] )
AC_MSG_RESULT($realtime)

if test $realtime = yes; then
  AC_CHECK_LIB(pthread, pthread_create, , AC_MSG_ERROR(realtime support requires the pthread library!))
  AC_CHECK_FUNCS(gettimeofday select socket)
fi

# Check for debug
AC_MSG_CHECKING(whether to compile debug version)
AC_ARG_ENABLE(debug,
  [  --enable-debug = enable various debug output],
  [AC_SUBST( debug, [-D_STK_DEBUG_] ) AC_SUBST( cflags, [-g] ) AC_SUBST( object_path, [Debug] ) AC_MSG_RESULT(yes)],
  [AC_SUBST( debug, [] ) AC_SUBST( cflags, [-O2] ) AC_SUBST( object_path, [Release] ) AC_MSG_RESULT(no)])

# Check compiler and use -Wall if gnu.
if test $GXX = "yes" ; then
  AC_SUBST( warn, [-Wall] )
fi

if test $realtime = yes; then
  # Checks for package options and external software
  AC_CANONICAL_HOST
  AC_MSG_CHECKING(for audio API)
  case $host in
    *-*-linux*)
    AC_ARG_WITH(alsa, [  --with-alsa = choose native ALSA API support (linux only)], [AC_SUBST( sound_api, [-D__LINUX_ALSA__] ) AC_MSG_RESULT(using ALSA) ], [AC_SUBST( sound_api, [-D__LINUX_OSS__] ) AC_MSG_RESULT(using OSS)])

    if test $sound_api = -D__LINUX_ALSA__; then
      AC_CHECK_LIB(asound, snd_pcm_open, , AC_MSG_ERROR(ALSA support requires the asound library!))
    fi
    AC_ARG_ENABLE(midiator, [  --enable-midiator = enable native MS-124W MIDI support (linux only)], [AC_SUBST( midiator, [-D__MIDIATOR__] )], [AC_SUBST( midiator, [] )])
    ;;

    *-sgi*)
    AC_SUBST( sound_api, [-D__IRIX_AL__] )
    AC_MSG_RESULT(using IRIX AL)
    AC_CHECK_LIB(audio, alOpenPort, , AC_MSG_ERROR(IRIX audio support requires the audio library!) )
    AC_CHECK_LIB(md, mdOpenInPort, , AC_MSG_ERROR(IRIX MIDI support requires the md library!) )
    ;;

    *-apple*)
    # Check for CoreAudio and CoreMIDI framework
    AC_CHECK_HEADERS(CoreAudio/CoreAudio.h CoreMIDI/CoreMIDI.h CoreServices/CoreServices.h,
      [AC_SUBST( sound_api, [-D__MACOSX_CORE__] )],
      [AC_MSG_ERROR(CoreAudio and/or CoreMIDI header files not found!)] )
    AC_SUBST( frameworks, ["-framework CoreAudio -framework CoreMIDI -framework CoreFoundation"] )
    ;;

    *)
    # Default case for unknown realtime systems.
    AC_MSG_ERROR(Unknown system type for realtime support ... try --disable-realtime argument!)
    ;;
  esac
fi

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_CHECK_FUNCS(strstr)

AC_OUTPUT
