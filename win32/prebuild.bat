:: ============================================================================
:: $Id: prebuild.bat 4478 2009-02-02 16:01:36Z FloSoft $
:: ============================================================================
@ECHO OFF

SET P1=%1
SET P2=%2

CALL :DeQuote P1
CALL :DeQuote P2

:: ============================================================================

CD %P1%

IF NOT EXIST version.h (
	COPY /Y /D version.h.in version.h
)

IF NOT EXIST local.h (
	COPY /Y /D local.h.in local.h
)

sh "%P1%\dailyversion.sh"

"%P2%\version.exe" --no-dots

echo Erzeuge Sprachfiles

msgmerge --quiet --update --backup=none -s RTTR/languages/de_DE/LC_MESSAGES/rttr.po RTTR/languages/rttr.pot
msgfmt -o RTTR/languages/de_DE/LC_MESSAGES/rttr.mo RTTR/languages/de_DE/LC_MESSAGES/rttr.po

msgmerge --quiet --update --backup=none -s RTTR/languages/es_ES/LC_MESSAGES/rttr.po RTTR/languages/rttr.pot
msgfmt -o RTTR/languages/es_ES/LC_MESSAGES/rttr.mo RTTR/languages/es_ES/LC_MESSAGES/rttr.po

msgmerge --quiet --update --backup=none -s RTTR/languages/fi_FI/LC_MESSAGES/rttr.po RTTR/languages/rttr.pot
msgfmt -o RTTR/languages/fi_FI/LC_MESSAGES/rttr.mo RTTR/languages/fi_FI/LC_MESSAGES/rttr.po

msgmerge --quiet --update --backup=none -s RTTR/languages/fr_FR/LC_MESSAGES/rttr.po RTTR/languages/rttr.pot
msgfmt -o RTTR/languages/fr_FR/LC_MESSAGES/rttr.mo RTTR/languages/fr_FR/LC_MESSAGES/rttr.po

msgmerge --quiet --update --backup=none -s RTTR/languages/sv_SE/LC_MESSAGES/rttr.po RTTR/languages/rttr.pot
msgfmt -o RTTR/languages/sv_SE/LC_MESSAGES/rttr.mo RTTR/languages/sv_SE/LC_MESSAGES/rttr.po

msgmerge --quiet --update --backup=none -s RTTR/languages/hu_HU/LC_MESSAGES/rttr.po RTTR/languages/rttr.pot
msgfmt -o RTTR/languages/hu_HU/LC_MESSAGES/rttr.mo RTTR/languages/hu_HU/LC_MESSAGES/rttr.po

:: msgmerge --quiet --update --backup=none -s RTTR/languages/en_EN/LC_MESSAGES/rttr.po RTTR/languages/rttr.pot
:: msgfmt -o RTTR/languages/en_EN/LC_MESSAGES/rttr.mo RTTR/languages/en_EN/LC_MESSAGES/rttr.po

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
