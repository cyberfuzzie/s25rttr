:: ============================================================================
:: $Id: postbuild.bat 4182 2008-10-29 10:03:29Z FloSoft $
:: ============================================================================
@ECHO OFF

SET P1=%1
SET P2=%2

CALL :DeQuote P1
CALL :DeQuote P2

:: ============================================================================

CD %P1%

COPY /Y /D %P2%\sound-convert.exe %1\RTTR\sound-convert.exe
COPY /Y /D %P2%\libsiedler2.dll %1\RTTR\libsiedler2.dll

GOTO :EOF

:: ============================================================================
:: De-Quote Routine zum entfernen der "" um eine Variable
:: Benutzung: CALL :DeQuote Variablenname
:: ============================================================================

:DeQuote
   SET DeQuote.Variable=%1
   CALL Set DeQuote.Contents=%%%DeQuote.Variable%%%
   ECHO.%DeQuote.Contents%|FindStr/brv ""^">NUL:&&GOTO :EOF
   ECHO.%DeQuote.Contents%|FindStr/erv ""^">NUL:&&GOTO :EOF
   
   SET DeQuote.Contents=####%DeQuote.Contents%####
   SET DeQuote.Contents=%DeQuote.Contents:####"=%
   SET DeQuote.Contents=%DeQuote.Contents:"####=%
   SET %DeQuote.Variable%=%DeQuote.Contents%
   
   SET DeQuote.Variable=
   SET DeQuote.Contents=
   GOTO :EOF

:: ============================================================================
