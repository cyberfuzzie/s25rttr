// $Id: DriverWrapper.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include "VideoDriverWrapper.h"
#include "ListDir.h"
#include "files.h"
#include "../driver/src/Interface.h"
#include "../driver/src/AudioInterface.h"
#include "../driver/src/VideoInterface.h"
#include "../driver/src/DriverInterfaceVersion.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor der DriverWrapper Klasse.
 *
 *  @author FloSoft
 */
DriverWrapper::DriverWrapper() :  dll(0)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor der DriverWrapper Klasse.
 *
 *  @author FloSoft
 */
DriverWrapper::~DriverWrapper()
{
	Unload();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
void DriverWrapper::Unload()
{
	if(dll)
	{
		FreeLibrary(dll);
		dll = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author OLiver
 */
bool DriverWrapper::Load(const DriverType dt, std::string& preference)
{
	// ggf. aufräumen vorher
	Unload();

	/// Verfügbare Treiber auflisten
	list<DriverItem> drivers;
	const std::string DIRECTORY[2] = { "video", "audio" };

	LoadDriverList(dt, drivers);

	LOG.lprintf("%u %s drivers found!\n", unsigned(drivers.size()), DIRECTORY[dt].c_str());

	// Welche gefunden?
	if(!drivers.size())
		return false;

	/// Suche, ob der Treiber dabei ist, den wir wünschen
	for(list<DriverItem>::iterator it = drivers.begin(); it.valid(); ++it)
	{
		if(it->GetName() == preference)
		{
			// Dann den gleich nehmen
			dll = LoadLibrary(it->GetFile().c_str());
			break;
		}
	}

	// ersten Treiber laden
	if(!dll)
	{
		dll = LoadLibrary(drivers.begin()->GetFile().c_str());

		// Standardwert zuweisen
		preference = drivers.begin()->GetName();
	}

	if(!dll)
	{
		fatal_error("kritischer Treiberfehler: DLL konnte nicht geladen werden\n");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author OLiver
 */
void *DriverWrapper::GetDLLFunction(const std::string& name)
{
	return (void*)GetProcAddress(dll,name.c_str());
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author FloSoft
 */
void DriverWrapper::LoadDriverList(const DriverType dt, list<DriverItem>& driver_list)
{
	/// Verfügbare Treiber auflisten
	std::list<std::string> driver_files;

	const std::string DIRECTORY[2] = { "video", "audio" };

#ifdef _WIN32
#	ifdef _DEBUG
		std::string path = FILE_PATHS[46] + DIRECTORY[dt] + "/" + "dbg_*.dll";
#	else
		std::string path = FILE_PATHS[46] + DIRECTORY[dt] + "/" + "rls_*.dll";
#	endif
#else
	std::string path = FILE_PATHS[46] + DIRECTORY[dt] + "/" + "*.so";
#endif // !_WIN32

	ListDir(path, 0, 0, &driver_files);

	/// Welcher Treiber letzendliche genommen wird
	std::string choice;

	HINSTANCE dll;
	for(std::list<std::string>::iterator it = driver_files.begin();it!=driver_files.end();++it)
	{
		std::string test(*it);
		if( (dll = LoadLibrary(it->c_str())) )
		{
			PDRIVER_GETDRIVERAPIVERSION GetDriverAPIVersion = NULL;
			PDRIVER_GETDRIVERNAME GetDriverName = NULL;
			PDRIVER_CREATEAUDIOINSTANCE CreateAudioInstance = NULL;
			PDRIVER_CREATEVIDEOINSTANCE CreateVideoInstance = NULL;

			*(void**)(&GetDriverAPIVersion) = (void*)GetProcAddress(dll, "GetDriverAPIVersion");
			*(void**)(&GetDriverName) = (void*)GetProcAddress(dll, "GetDriverName");

			if(GetDriverAPIVersion)
			{
				if(GetDriverAPIVersion() == DRIVERAPIVERSION)
				{
					if(GetDriverName)
					{

						*(void**)(&CreateVideoInstance) = (void*)GetProcAddress(dll, "CreateVideoInstance");
						*(void**)(&CreateAudioInstance) = (void*)GetProcAddress(dll, "CreateAudioInstance");

						if((dt == DT_VIDEO && CreateVideoInstance) || (dt == DT_AUDIO && CreateAudioInstance))
						{
							DriverItem di(*it, GetDriverName());
							driver_list.push_back(di);
						}
					}
				}
			}
			
			

			FreeLibrary(dll);
			dll = 0;
		}
	}
}

