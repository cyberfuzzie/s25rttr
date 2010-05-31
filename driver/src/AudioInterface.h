// $Id: AudioInterface.h 6458 2010-05-31 11:38:51Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef AUDIOINTERFACE_H_INCLUDED
#define AUDIOINTERFACE_H_INCLUDED


#include "AudioDriver.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#ifdef _WIN32
#	if defined _USRDLL || defined _LIB || defined BUILD_DLL
#		define DRIVERDLLAPI extern "C" __declspec(dllexport)
#		define class        class __declspec(dllexport)
#	else
#		define DRIVERDLLAPI extern "C" __declspec(dllimport)
#	endif // !_USRDLL
#else
#	define DRIVERDLLAPI extern "C"
#endif // !_WIN32

/// Instanzierungsfunktion der Treiber.
DRIVERDLLAPI AudioDriver *CreateAudioInstance(AudioDriverLoaderInterface * CallBack, void * device_dependent);

/// 
typedef AudioDriver *(*PDRIVER_CREATEAUDIOINSTANCE)(AudioDriverLoaderInterface *, void * );

const char *GetWindowTitle();
const char *GetWindowVersion();
const char *GetWindowRevision();

#endif // !AUDIOINTERFACE_H_INCLUDED
