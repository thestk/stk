# Process this file with autoconf to produce a configure script.
AC_INIT(STK, 4.4, gary@music.mcgill.ca, stk)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src/Stk.cpp)
AC_CONFIG_FILES(Makefile src/Makefile projects/demo/Makefile projects/effects/Makefile projects/ragamatic/Makefile projects/examples/Makefile projects/examples/libMakefile)

# Fill GXX with something before test.
AC_SUBST( GXX, ["no"] )

# Checks for programs.
AC_PROG_CXX(g++ CC c++ cxx)
AC_PROG_RANLIB
AC_PATH_PROG(AR, ar, no)
if [[ $AR = "no" ]] ; then
    AC_MSG_ERROR("Could not find ar - needed to create a library");
fi

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

# Check for debug
AC_MSG_CHECKING(whether to compile debug version)
AC_ARG_ENABLE(debug,
  [  --enable-debug = enable various debug output],
  [AC_SUBST( cppflag, ["-D_STK_DEBUG_ -D__RTAUDIO_DEBUG__ -D__RTMIDI_DEBUG__"] ) AC_SUBST( cxxflag, ["-g"] ) AC_SUBST( object_path, [Debug] ) AC_MSG_RESULT(yes)],
  [AC_SUBST( cppflag, [] ) AC_SUBST( cxxflag, [-O3] ) AC_SUBST( object_path, [Release] ) AC_MSG_RESULT(no)])

# Checks for functions
if test $realtime = yes; then
  AC_CHECK_FUNCS(select socket)
  AC_CHECK_FUNC(gettimeofday, [cppflag="$cppflag -DHAVE_GETTIMEOFDAY"], )
fi

# For -I and -D flags
CPPFLAGS="$CPPFLAGS $cppflag"

# For debugging and optimization ... overwrite default because it has both -g and -O2
CXXFLAGS="$cxxflag"

# Check compiler and use -Wall if gnu.
if [test $GXX = "yes" ;] then
  AC_SUBST( cxxflag, [-Wall] )
fi

CXXFLAGS="$CXXFLAGS $cxxflag"

if test $realtime = yes; then
  # Checks for package options and external software
  AC_CANONICAL_HOST
  AC_MSG_CHECKING(for audio API)
  case $host in
    *-*-linux*)
    AC_ARG_WITH(jack, [  --with-jack = choose JACK server support (mac and linux only)], [
    api="$api -D__UNIX_JACK__"
    AC_MSG_RESULT(using JACK)
    AC_CHECK_LIB(jack, jack_client_open, , AC_MSG_ERROR(JACK support requires the jack library!))
    AC_CHECK_LIB(asound, snd_pcm_open, , AC_MSG_ERROR(Jack support also requires the asound library!))], )

    # Look for ALSA flag
    AC_ARG_WITH(alsa, [  --with-alsa = choose native ALSA API support (linux only)], [
    api="$api -D__LINUX_ALSA__"
    AC_MSG_RESULT(using ALSA)
    AC_CHECK_LIB(asound, snd_pcm_open, , AC_MSG_ERROR(ALSA support requires the asound library!))], )

    # Look for OSS flag
    AC_ARG_WITH(oss, [  --with-oss = choose OSS API support (linux only)], [
    api="$api -D__LINUX_OSS__"
    AC_MSG_RESULT(using OSS)], )

    # If no audio api flags specified, use ALSA
    if [test "$api" == "";] then
      AC_MSG_RESULT(using ALSA)
      AC_SUBST( api, [-D__LINUX_ALSA__] )
      AC_CHECK_LIB(asound, snd_pcm_open, , AC_MSG_ERROR(ALSA support requires the asound library!))
    fi

    # Look for ALSA library because we need it for RtMidi
    AC_CHECK_LIB(asound, snd_pcm_open, , AC_MSG_ERROR(STK in Linux requires the ALSA asound library for RtMidi!))
    api="$api -D__LINUX_ALSASEQ__" 

    AC_CHECK_LIB(pthread, pthread_create, , AC_MSG_ERROR(realtime support requires the pthread library!))
    ;;

    *-apple*)
    AC_ARG_WITH(jack, [  --with-jack = choose JACK server support (unix only)], [
    api="$api -D__UNIX_JACK__"
    AC_MSG_RESULT(using JACK)
    AC_CHECK_LIB(jack, jack_client_new, , AC_MSG_ERROR(JACK support requires the jack library!))], )

    # Look for Core flag
    AC_ARG_WITH(core, [  --with-core = choose CoreAudio API support (mac only)], [
    api="$api -D__MACOSX_CORE__"
    AC_MSG_RESULT(using CoreAudio)
    AC_CHECK_HEADER(CoreAudio/CoreAudio.h, [], [AC_MSG_ERROR(CoreAudio header files not found!)] )
    LIBS="$LIBS -framework CoreAudio -framework CoreFoundation -framework CoreMidi" ], )

    # If no audio api flags specified, use CoreAudio
    if [test "$api" == ""; ] then
      AC_SUBST( api, [-D__MACOSX_CORE__] )
      AC_MSG_RESULT(using CoreAudio)
      AC_CHECK_HEADER(CoreAudio/CoreAudio.h,
      [],
      [AC_MSG_ERROR(CoreAudio header files not found!)] )
      AC_SUBST( LIBS, ["-framework CoreAudio -framework CoreFoundation -framework CoreMidi"] )
    fi

    AC_CHECK_LIB(pthread, pthread_create, , AC_MSG_ERROR(RtAudio requires the pthread library!))
    ;;

    *-mingw32*)
    AC_ARG_WITH(asio, [  --with-asio = choose ASIO API support (windoze only)], [
    api="$api -D__WINDOWS_ASIO__"
    AC_MSG_RESULT(using ASIO)
    AC_SUBST( objects, ["asio.o asiodrivers.o asiolist.o iasiothiscallresolver.o"] ) ], )

    # Look for DirectSound flag
    AC_ARG_WITH(ds, [  --with-ds = choose DirectSound API support (windoze only)], [
    api="$api -D__WINDOWS_DS__"
    AC_MSG_RESULT(using DirectSound)
    LIBS="-ldsound -lwinmm $LIBS" ], )

    # If no audio api flags specified, use DirectSound
    if [test "$api" == "";] then
      AC_SUBST( api, [-D__WINDOWS_DS__] )
      AC_MSG_RESULT(using DirectSound)
      LIBS="-ldsound -lwinmm $LIBS"
    fi

    api="$api -D__WINDOWS_MM__"
    LIBS="-lole32 -lwinmm -lWsock32 $LIBS"
    ;;

    *)
    # Default case for unknown realtime systems.
    AC_MSG_ERROR(Unknown system type for realtime support ... try --disable-realtime argument!)
    ;;
  esac

CPPFLAGS="$CPPFLAGS $api"
fi

AC_OUTPUT
