# Process this file with autoconf to produce a configure script.
AC_INIT(STK, 4.2.0, gary@music.mcgill.ca, stk)
AC_CONFIG_SRCDIR(src/Stk.cpp)
AC_CONFIG_FILES(src/Makefile projects/demo/Makefile projects/effects/Makefile projects/ragamatic/Makefile projects/examples/Makefile)

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX(g++ CC c++ cxx)
AC_PROG_CXX

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

# Check for math library
AC_CHECK_LIB(m, cos, , AC_MSG_ERROR(math library is needed!))

if test $realtime = yes; then
  AC_CHECK_LIB(pthread, pthread_create, , AC_MSG_ERROR(realtime support requires the pthread library!))
  AC_CHECK_FUNCS(gettimeofday select socket)
fi

# Check for debug
AC_MSG_CHECKING(whether to compile debug version)
AC_ARG_ENABLE(debug,
  [  --enable-debug = enable various debug output],
  [AC_SUBST( debug, ["-D_STK_DEBUG_ -D__RTAUDIO_DEBUG__"] ) AC_SUBST( cflags, ["-g -O2"] ) AC_SUBST( object_path, [Debug] ) AC_MSG_RESULT(yes)],
  [AC_SUBST( debug, [] ) AC_SUBST( cflags, [-O3] ) AC_SUBST( object_path, [Release] ) AC_MSG_RESULT(no)])

# Check compiler and use -Wall if gnu.
if test $GXX = "yes" ; then
  AC_SUBST( warn, ["-Wall -g -Woverloaded-virtual -D__GXX__"] )
fi

if test $realtime = yes; then
  # Checks for package options and external software
  AC_CANONICAL_HOST
  AC_MSG_CHECKING(for audio API)
  case $host in
    *-*-linux*)
    AC_SUBST( sound_api, [_NO_API_] )

    # Look for ALSA library because we need it for RtMidi
    AC_CHECK_LIB(asound, snd_pcm_open, , AC_MSG_ERROR(STK in Linux requires the ALSA asound library for RtMidi!))
    audio_apis="-D__LINUX_ALSASEQ__" 

    # Look for Jack flag
    AC_ARG_WITH(jack, [  --with-jack = choose JACK server support (linux only)], [AC_SUBST( sound_api, [-D__LINUX_JACK__] ) AC_MSG_RESULT(using JACK)] , )
    if [test $sound_api = -D__LINUX_JACK__;] then
      TEMP_LIBS=$LIBS
      AC_CHECK_LIB(jack, jack_client_new, , AC_MSG_ERROR(JACK support requires the jack library!))
      LIBS="`pkg-config --cflags --libs jack` $TEMP_LIBS -lasound"
      audio_apis="-D__LINUX_JACK__ $audio_apis"
    fi

    # Look for Alsa flag
    AC_ARG_WITH(alsa, [  --with-alsa = choose native ALSA API support (linux only)], [AC_SUBST( sound_api, [-D__LINUX_ALSA__] ) AC_MSG_RESULT(using ALSA)], )
    if test $sound_api = -D__LINUX_ALSA__; then
      audio_apis="-D__LINUX_ALSA__ $audio_apis"
    fi

    # Look for OSS flag
    AC_ARG_WITH(oss, [  --with-oss = choose OSS API support (linux only)], [AC_SUBST( sound_api, [-D__LINUX_OSS__] ) AC_MSG_RESULT(using OSS)], )
    if test $sound_api = -D__LINUX_OSS__; then
      audio_apis="-D__LINUX_OSS__ $audio_apis"
    fi

    # If no audio api flags specified, use ALSA
    if [test $sound_api = _NO_API_;] then
      AC_MSG_RESULT(using ALSA)
      audio_apis="-D__LINUX_ALSA__ $audio_apis"
    fi
    ;;

    *-sgi*)
    AC_SUBST( audio_apis, ["-D__IRIX_AL__ -D__IRIX_MD__ -LANG:std -w"] )
    AC_MSG_RESULT(using IRIX AL)
    AC_CHECK_LIB(audio, alOpenPort, , AC_MSG_ERROR(IRIX audio support requires the audio library!) )
    AC_CHECK_LIB(md, mdOpenInPort, , AC_MSG_ERROR(IRIX MIDI support requires the md library!) )
    ;;

    *-apple*)
    # Check for CoreAudio and CoreMIDI framework
    AC_CHECK_HEADERS(CoreAudio/CoreAudio.h CoreMIDI/CoreMIDI.h CoreServices/CoreServices.h,
      [AC_SUBST( audio_apis, [-D__MACOSX_CORE__] )],
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
