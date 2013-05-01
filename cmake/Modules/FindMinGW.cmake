# Find mingw toolchain
# The 'arch' parameter specifies whether to search for i?86 or x86_64
#
# Set variables:
# MINGW_AR
# MINGW_CC
# MINGW_CXX
# MINGW_RANLIB
# MINGW_WINDRES
# MINGW_SYSROOT
# MINGW_MACHINE

FUNCTION (FindMinGW arch)
    # First find toolchain executable names
    IF (${arch} MATCHES "^i.86$")
        FIND_PROGRAM (MINGW_CC NAMES i686-w64-mingw32-gcc
                                     i686-pc-mingw32-gcc
                                     i686-mingw32-gcc)
        FIND_PROGRAM (MINGW_CXX NAMES i686-w64-mingw32-g++
                                      i686-pc-mingw32-g++
                                      i686-mingw32-g++)
        FIND_PROGRAM (MINGW_RANLIB NAMES i686-w64-mingw32-ranlib
                                         i686-pc-mingw32-ranlib
                                         i686-mingw32-ranlib)
        FIND_PROGRAM (MINGW_WINDRES NAMES i686-w64-mingw32-windres
                                          i686-pc-mingw32-windres
                                          i686-mingw32-windres)
# TODO: Really try to use 'ar' for mingw cross compiling?
#       Has been there in original implementation
#       Leaving it for now as archiving object code
#       may really be platform independent
        FIND_PROGRAM (CMAKE_AR NAMES i686-w64-mingw32-ar
                                     i686-pc-mingw32-ar
                                     i686-mingw32-ar
                                     ar)
        SET (MINGW_AR "${CMAKE_AR}")
    ELSE (${arch} MATCHES "^i.86$")
        FIND_PROGRAM (MINGW_CC NAMES x86_64-w64-mingw32-gcc
                                     x86_64-pc-mingw32-gcc
                                     x86_64-mingw32-gcc)
        FIND_PROGRAM (MINGW_CXX NAMES x86_64-w64-mingw32-g++
                                      x86_64-pc-mingw32-g++
                                      x86_64-mingw32-g++)
        FIND_PROGRAM (MINGW_RANLIB NAMES x86_64-w64-mingw32-ranlib
                                         x86_64-pc-mingw32-ranlib
                                         x86_64-mingw32-ranlib)
        FIND_PROGRAM (MINGW_WINDRES NAMES x86_64-w64-mingw32-windres
                                          x86_64-pc-mingw32-windres
                                          x86_64-mingw32-windres)
# TODO: Really try to use 'ar' for mingw cross compiling?
#       Has been there in original implementation
#       Leaving it for now as archiving object code
#       may really be platform independent
        FIND_PROGRAM (CMAKE_AR NAMES x86_64-w64-mingw32-ar
                                     x86_64-pc-mingw32-ar
                                     x86_64-mingw32-ar
                                     ar)
        SET (MINGW_AR "${CMAKE_AR}")
    ENDIF (${arch} MATCHES "^i.86$")

    # Get the full target name
    EXECUTE_PROCESS (COMMAND ${MINGW_CC} -dumpmachine
                     OUTPUT_VARIABLE gcc_machine)
    SET (MINGW_MACHINE "${gcc_machine}" CACHE STRING "MinGW machine")

    # Find the root directory now that the compiler is known
    EXECUTE_PROCESS (COMMAND ${MINGW_CC} -print-search-dirs
                     OUTPUT_VARIABLE gcc_search_dirs)
    STRING (REGEX MATCH "libraries.[^\n]*\n" gcc_lib_line "${gcc_search_dirs}")
    STRING (REGEX REPLACE ".* =([^\n]*)\n" "\\1" gcc_lib_dirs_single "${gcc_lib_line}")
    STRING (REGEX REPLACE ":" ";" gcc_lib_dirs "${gcc_lib_dirs_single}")
    # normalize paths and remove duplicates
    FOREACH (lib_dir ${gcc_lib_dirs})
        GET_FILENAME_COMPONENT (lib_dir ${lib_dir} REALPATH)
        LIST (FIND gcc_lib_dir_list ${lib_dir} found)
        IF (${found} EQUAL -1)
            LIST (APPEND gcc_lib_dir_list ${lib_dir})
        ENDIF (${found} EQUAL -1)
    ENDFOREACH (lib_dir ${gcc_lib_dirs})
    # only use those paths which end in /lib*
    FOREACH (lib_dir ${gcc_lib_dir_list})
        IF (NOT ${lib_dir} MATCHES "/lib[^/]*$")
            LIST (REMOVE_ITEM gcc_lib_dir_list ${lib_dir})
        ENDIF (NOT ${lib_dir} MATCHES "/lib[^/]*$")
    ENDFOREACH (lib_dir ${gcc_lib_dir_list})
    # use first element in list (hopefully there is exactly 1)
    # and strip last path component to get the mingw root dir
    LIST (LENGTH gcc_lib_dir_list gcc_lib_dir_count)
    IF (NOT ${gcc_lib_dir_count} EQUAL 1)
        MESSAGE(AUTHOR_WARNING "There are ${gcc_lib_dir_count} mingw library directories available: ${gcc_lib_dir_list}\nUsing the first one!")
    ENDIF (NOT ${gcc_lib_dir_count} EQUAL 1)
    LIST (GET gcc_lib_dir_list 0 gcc_lib_dir)
    STRING (REGEX REPLACE "/[^/]*$" "" sysroot ${gcc_lib_dir})
    SET (MINGW_SYSROOT "${sysroot}" CACHE STRING "MinGW system root directory")
ENDFUNCTION (FindMinGW)
