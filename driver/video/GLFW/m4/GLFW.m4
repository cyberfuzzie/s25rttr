dnl AC_LIB_GLFW([[ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for GLFW, and define GLFW_CFLAGS and GLFW_LIBS
dnl
AC_DEFUN([AC_LIB_GLFW], [
  AC_ARG_WITH(glfwlib, [  --with-glfwlib=DIR      prefix for glfw library files and headers], [
    if test "$withval" = "no"; then
      ac_glfw_path=
      $2
    elif test "$withval" = "yes"; then
      ac_glfw_path=/usr
    else
      ac_glfw_path="$withval"
    fi
  ],[ac_glfw_path=/usr])
  if test "$ac_glfw_path" != ""; then
    saved_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS -I$ac_glfw_path/include"
    AC_CHECK_HEADER([GL/glfw.h], [
      saved_LDFLAGS="$LDFLAGS"
      LDFLAGS="$LDFLAGS -L$ac_glfw_path/lib -lGL"
      AC_CHECK_LIB(glfw, glfwInit, [
        AC_SUBST(GLFW_CFLAGS, [-I$ac_glfw_path/include])
        AC_SUBST(GLFW_LDFLAGS, [-L$ac_glfw_path/lib])
        AC_SUBST(GLFW_LIBS, ["-lglfw -lGL"])
        $1
      ], [
        :
        $2
      ])
      LDFLAGS="$saved_LDFLAGS"
    ], [
      AC_MSG_RESULT([not found])
      $2
    ])
    CPPFLAGS="$saved_CPPFLAGS"
  fi
])
