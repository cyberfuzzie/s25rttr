@ECHO OFF

SET VERSION=0.6
SET COMPILER=vs2008

ECHO Stelle sicher, dass ein vollstaendiger Release-Build erstellt wurde
PAUSE


DEL /S /Q s25rttr_%VERSION%
MKDIR s25rttr_%VERSION%
CD s25rttr_%VERSION%

COPY /Y ..\bin\rttr.bat start.bat
COPY /Y ..\..\win32\%COMPILER%\release\mygettext.dll .
COPY /Y ..\..\win32\%COMPILER%\release\libsiedler2.dll .
COPY /Y ..\..\win32\%COMPILER%\release\s25client.exe .
COPY /Y ..\..\RTTR\texte\readme.txt .
COPY /Y ..\..\RTTR\texte\keyboardlayout.txt .

COPY /Y "F:\programmierung\VS 2005\bin\SDL.dll" .
COPY /Y "F:\programmierung\VS 2005\bin\SDL_mixer.dll" .
COPY /Y "F:\programmierung\VS 2005\bin\libbz2.dll" .
COPY /Y "F:\programmierung\VS 2008\bin\msvcr90.dll" .
COPY /Y "F:\programmierung\VS 2008\bin\msvcp90.dll" .
COPY /Y "F:\programmierung\VS 2008\bin\msvcrt.dll" .

DEL /S /Q RTTR
"F:\programmierung\VS 2008\bin\svn" --force export  ..\..\RTTR RTTR
DEL RTTR\REPLAYS\*.rpl
COPY /Y ..\..\RTTR\sound.lst RTTR\sound.lst
DEL RTTR\settings.bin
DEL RTTR\languages\rttr.pot

DEL RTTR\languages\de_DE\LC_MESSAGES\rttr.po
COPY /Y ..\..\RTTR\languages\de_DE\LC_MESSAGES\rttr.mo RTTR\languages\de_DE\LC_MESSAGES\rttr.mo

DEL RTTR\languages\es_ES\LC_MESSAGES\rttr.po
COPY /Y ..\..\RTTR\languages\es_ES\LC_MESSAGES\rttr.mo RTTR\languages\es_ES\LC_MESSAGES\rttr.mo

DEL RTTR\languages\fi_FI\LC_MESSAGES\rttr.po
COPY /Y ..\..\RTTR\languages\fi_FI\LC_MESSAGES\rttr.mo RTTR\languages\fi_FI\LC_MESSAGES\rttr.mo

DEL RTTR\languages\fr_FR\LC_MESSAGES\rttr.po
COPY /Y ..\..\RTTR\languages\fr_FR\LC_MESSAGES\rttr.mo RTTR\languages\fr_FR\LC_MESSAGES\rttr.mo

DEL RTTR\languages\hu_HU\LC_MESSAGES\rttr.po
COPY /Y ..\..\RTTR\languages\hu_HU\LC_MESSAGES\rttr.mo RTTR\languages\hu_HU\LC_MESSAGES\rttr.mo

DEL RTTR\languages\sv_SE\LC_MESSAGES\rttr.po
COPY /Y ..\..\RTTR\languages\sv_SE\LC_MESSAGES\rttr.mo RTTR\languages\sv_SE\LC_MESSAGES\rttr.mo

DEL RTTR\languages\ru_RU\LC_MESSAGES\rttr.po
COPY /Y ..\..\RTTR\languages\ru_RU\LC_MESSAGES\rttr.mo RTTR\languages\ru_RU\LC_MESSAGES\rttr.mo

DEL RTTR\languages\nl_NL\LC_MESSAGES\rttr.po
COPY /Y ..\..\RTTR\languages\nl_NL\LC_MESSAGES\rttr.mo RTTR\languages\nl_NL\LC_MESSAGES\rttr.mo

MKDIR driver\audio
COPY /Y ..\..\driver\audio\*.dll driver\audio
DEL driver\audio\dbg_*.dll

MKDIR driver\video
COPY /Y ..\..\driver\video\*.dll driver\video
DEL driver\video\dbg_*.dll

ECHO > "put your S2-Installation here"

COPY /Y ..\..\win32\%COMPILER%\release\libsiedler2.dll RTTR
COPY /Y ..\..\win32\%COMPILER%\release\sound-convert.exe RTTR
COPY /Y "F:\programmierung\VS 2008\bin\libsamplerate.dll" RTTR
COPY /Y "F:\programmierung\VS 2008\bin\libsndfile.dll" RTTR
COPY /Y "F:\programmierung\VS 2008\bin\msvcr90.dll" RTTR
COPY /Y "F:\programmierung\VS 2008\bin\msvcp90.dll" RTTR
COPY /Y "F:\programmierung\VS 2008\bin\msvcrt.dll" RTTR

DEL *.bak

CD ..
DEL s25rttr_%VERSION%.zip
ZIP -r -9 s25rttr_%VERSION%.zip s25rttr_%VERSION% -x .svn

ECHO Release Version %VERSION% erstellt

PAUSE
