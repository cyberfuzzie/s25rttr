dnl AC_LIB_SDLMIXER([[ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for SDLmixer, and define SDLMIXER_CFLAGS and SDLMIXER_LIBS
dnl
AC_DEFUN([AC_LIB_SDLMIXER], [
  AC_ARG_WITH(sdlmixerlib, [  --with-sdlmixerlib=DIR  prefix for SDL_mixer library files and headers], [
    if test "$withval" = "no"; then
      ac_sdlmixer_path=
      $2
    elif test "$withval" = "yes"; then
      ac_sdlmixer_path=/usr
    else
      ac_sdlmixer_path="$withval"
    fi
  ],[ac_sdlmixer_path=/usr])
  if test "$ac_sdlmixer_path" != ""; then
    saved_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS -I$ac_sdlmixer_path/include"
    AC_CHECK_HEADER([SDL/SDL_mixer.h], [
      saved_LDFLAGS="$LDFLAGS"
      LDFLAGS="$LDFLAGS -L$ac_sdlmixer_path/lib"
      AC_CHECK_LIB(SDL_mixer, Mix_OpenAudio, [
        AC_SUBST(SDLMIXER_CFLAGS, [-I$ac_sdlmixer_path/include])
        AC_SUBST(SDLMIXER_LDFLAGS, [-L$ac_sdlmixer_path/lib])
        AC_SUBST(SDLMIXER_LIBS, [-lSDL_mixer])
        $1
      ], [
        :
        $2
      ])
      LDFLAGS="$saved_LDFLAGS"
    ], [
      #AC_MSG_RESULT([not found])
      $2
    ])
    CPPFLAGS="$saved_CPPFLAGS"
  fi
])