#/bin/sh

if grep -q '^\s*#define WINDOW_VERSION "[0-9]\+"' version.h ; then
	DATE=$(date +%Y%m%d)
	sed -i -e "s~\(^\s*#define WINDOW_VERSION \"\)[0-9]\+\"~\1$DATE\"~" version.h
fi