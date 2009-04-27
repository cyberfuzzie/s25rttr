/// $Id: prebuild-mutex.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#include <windows.h>
#include <string>
#include <vector>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
/**
 *  usage: argv[0] $uniqueid $prebuild-script $params
 *  example: prebuild-mutex projectname prebuild.bat a b c
 *
 *  @author FloSoft
 */
int main(int argc, char *argv[])
{
	if(argc < 3)
		return 1;

	vector<string> args;

	for(int i = 0; i < argc; ++i)
		args.push_back(argv[i]);

	HANDLE mHandle; 

	string mutexname = args[1];

	bool exist = false;
	int timeout = 0;
	do
	{
		mHandle = CreateMutex(NULL, true, mutexname.c_str()); 

		if(mHandle != NULL)
		{ 
			exist = (GetLastError() == ERROR_ALREADY_EXISTS);

			if(!exist)
			{
				// Prebuild-Ereignis mit Parametern starten

				string prebuild = args[2];
				for(size_t i = 3; i < args.size(); i++)
					prebuild += " " + args[i];

				system(prebuild.c_str());

				ReleaseMutex(mHandle);

				return 0;
			}
		}
		Sleep(250);
		++timeout;
	}
	while(exist && timeout < 4);

	return 0;
}