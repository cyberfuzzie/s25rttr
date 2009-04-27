dnl
dnl AC_ENABLE_DEBUG
dnl
dnl enables argument --enable-debug
dnl
dnl also defines _DEBUG for use in sourcecode
dnl
AC_DEFUN([AC_ENABLE_DEBUG], [
  AH_TEMPLATE([_DEBUG], [Define to built with DEBUG-Code])
  AC_ARG_ENABLE(debug, [  --enable-debug          enable building of libraries with debugging code (default no)], [
  	CPPFLAGS="$CPPFLAGS -g"
  	AC_DEFINE([_DEBUG])
  ],[])
])
