// $Id: tempname.cpp 5593 2009-10-05 16:05:20Z FloSoft $
//
// Copyright (c) 2005-2009 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Siedler II.5 RTTR.
//
// Siedler II.5 RTTR is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Siedler II.5 RTTR is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Siedler II.5 RTTR. If not, see <http://www.gnu.org/licenses/>.

///////////////////////////////////////////////////////////////////////////////
// Systemheader
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Header
#include "tempname.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @name mkdir_p
 *
 *  erzeugt rekursiv ein Verzeichnis
 *
 *  @author     FloSoft
 */
int mkdir_p(const std::string dir)
{
	if (
#ifdef _WIN32
		!CreateDirectory(dir.c_str(), NULL)
#else
		mkdir(dir.c_str(), 0750) < 0
#endif
	)
	{
		size_t slash = dir.rfind('/');
		if (slash != std::string::npos) 
		{
			std::string prefix = dir.substr(0, slash);
			if(mkdir_p(prefix) == 0)
			{
				return (
#ifdef _WIN32
					CreateDirectory(dir.c_str(), NULL) ? 0 : -1
#else
					mkdir(dir.c_str(), 0750)
#endif
				);
			}
		}
		return -1;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/** @name tempname
 *
 *  liefert den Namen und Pfad einer temporären Datei.
 *
 *  @author     FloSoft
 *
 *  @param[in,out] name   Pfad/Name der Datei
 *  @param[in]     length Größe des Speicherblocks von @p name
 *
 *  @return               bei Erfolg wird true geliefert, bei Fehler false
 */
bool tempname(char *name, unsigned int length)
{
	if(!name)
		return false;

	memset(name, 0, length);

	const char *tempdir = NULL;

#ifdef _WIN32
	static char ptemp[MAX_PATH];
	tempdir = ptemp;
	if(GetTempPath(MAX_PATH, ptemp) == 0)
		tempdir = NULL;
#endif

	// Ort des Temporären Verzeichnisses holen
	if (tempdir == NULL || *tempdir == '\0')
		tempdir = getenv("TMPDIR");
	if (tempdir == NULL || *tempdir == '\0')
		tempdir = getenv("TMP");
	if (tempdir == NULL || *tempdir == '\0')
		tempdir = getenv("TEMP");

#ifndef _WIN32
	if (tempdir == NULL)
		tempdir = "/tmp";
#endif

	if (tempdir == NULL)
		return false;

	unsigned int dirlen = (unsigned int)strlen(tempdir);

	while(dirlen > 0 && tempdir[dirlen - 1] == '/')
		dirlen--;

	static bool srand_init = false;
	if(!srand_init)
	{
		srand( (unsigned int)time(NULL) );
		srand_init = true;
	}

	char temp[512];
	FILE *test = NULL;

	// nach einer unbenutzten Datei suchen
	do {
		if(test)
			fclose(test);

		memset(temp, 0, 512);

		for(unsigned int i = 0; i < 10; i++)
			sprintf(temp, "%s%c", temp, (rand() % 25)+65 );
#ifdef _WIN32
	#ifndef __CYGWIN__
	#define snprintf _snprintf
	#endif
	snprintf(name, length, "%s\\tmp.%s", tempdir, temp);
#else
		snprintf(name, length, "%s/tmp.%s", tempdir, temp);
#endif // !_WIN32

		test = fopen(name, "r");
	} while ( test != NULL);

	mkdir_p(tempdir);

	return true;
}
