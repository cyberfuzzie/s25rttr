// $Id: files.cpp 5247 2009-07-11 19:13:17Z FloSoft $
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
// Header
#include "main.h"
#include "files.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  formt Pfade korrekt um.
 *
 *  @param[in] file
 *
 *  @return liefert den umgeformten Pfad zurück
 *
 *  @author FloSoft
 */
std::string GetFilePath(std::string file)
{
	std::string to = file;

	// ist der Pfad ein Home-Dir?
	if(file.at(0) == '~') 
	{
		std::stringstream s;
		s << getenv("HOME") << file.substr(1);
		to = s.str();
	}

	return to;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  prüft ob eine Datei existiert (bzw ob sie lesbar ist)
 *
 *  @param[in] file
 *
 *  @return liefert ja oder nein zurück
 *
 *  @author FloSoft
 */
bool FileExists(std::string file)
{
	FILE *test = fopen(GetFilePath(file).c_str(), "rb");
	if(test)
	{
		fclose(test);
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  prüft ob eine Verzeichnis existiert (bzw ob es ein Verzeichnis ist)
 *
 *  @param[in] dir
 *
 *  @return liefert ja oder nein zurück
 *
 *  @author FloSoft
 */
bool IsDir(std::string dir)
{
	if(dir.at(dir.size()-1) == '/')
		dir.erase(dir.size()-1, 1);

#ifdef _WIN32
	std::string path = GetFilePath(dir).c_str();
	std::replace(path.begin(), path.end(), '/', '\\');

	HANDLE test;
	WIN32_FIND_DATAA wfd;

	test = FindFirstFileA(path.c_str(), &wfd);
	if(test != INVALID_HANDLE_VALUE)
	{
		FindClose(test);
		if( (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			return true;
	}
#else
	DIR *test = opendir(dir.c_str());
	if(test)
	{
		closedir(test);
		return true;
	}
#endif // !_WIN32

	return false;
}
