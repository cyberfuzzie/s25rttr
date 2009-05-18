// $Id: main.cpp 4893 2009-05-18 22:25:32Z sicherha $
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

#ifdef HAVE_CONFIG_H
#	include "../config.h"
#endif // HAVE_CONFIG_H

#ifdef __linux__
	#undef _WIN32
#endif

#ifdef _WIN32
#	include <windows.h>
#	define chdir SetCurrentDirectory
#else
#	include <unistd.h>
#	include <cerrno>
#endif

#include <cstdlib>
#include <cstring>
#include <ctime>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
	if(argc >= 2)
		chdir(argv[1]);

	ifstream bzr(".bzr/branch/last-revision");
	const int bzr_errno = errno;

	ifstream svn(".svn/entries");
	const int svn_errno = errno;

	if(!svn && !bzr)
	{
		cerr << "failed to read any of:" << endl;
		cerr << ".bzr/branch/last-revision: " << strerror(bzr_errno) << endl;
		cerr << ".svn/entries: " << strerror(svn_errno) << endl;

		return 1;
	}

	int revision = 0;

	if(bzr) // use bazaar revision if exist
	{
		bzr >> revision;
		bzr.close();
	}
	else if(svn) // using subversion revision, if no bazaar one exists
	{
		string t;

		getline(svn, t); // entry count
		getline(svn, t); // empty
		getline(svn, t); // "dir"

		svn >> revision;
		svn.close();
	}

	ifstream versionh("version.h");
	const int versionh_errno = errno;

	if(!versionh)
	{
		versionh.clear();
		versionh.open("version.h.in");
	}

	if(!versionh)
	{
		cerr << "failed to read any of:" << endl;
		cerr << "version.h: " << strerror(versionh_errno) << endl;
		cerr << "version.h.in: " << strerror(errno) << endl;

		return 1;
	}

	vector<string> newversionh;

	string l;
	bool changed = false;
	while(getline(versionh, l))
	{
		stringstream ll(l);
		string d, n;
		char q;
		int v;

		ll >> d; // define
		ll >> n; // name
		ll >> q; // "
		ll >> v; // value
		ll >> q; // "

		if(n == "WINDOW_VERSION")
		{
			time_t t;
			time(&t);

			char tv[64];
			strftime(tv, 63, "%Y%m%d", localtime(&t) );
			if(v > 20000101 && v < atoi(tv))
			{
				// set new day
				ll.clear();
				ll.str("");
				ll << d << " " << n << " \"" << tv << "\"";
				l = ll.str();

				cout << "renewing version to day \"" << tv << "\"" << endl;
				changed = true;
			}
		}

		if(n == "WINDOW_REVISION")
		{
			if(v < revision)
			{
				// set new revision
				ll.clear();
				ll.str("");
				ll << d << " " << n << " \"" << revision << "\"";
				l = ll.str();

				cout << "renewing version to revision \"" << revision << "\"" << endl;
				changed = true;
			}
		}

		newversionh.push_back(l);
	}
	versionh.close();

	if(changed) // only write if changed
	{
		ofstream versionh("version.h");
		const int versionh_errno = errno;

		if(!versionh)
		{
			cerr << "failed to write to version.h: " << strerror(versionh_errno) << endl;
			return 1;
		}

		for(vector<string>::const_iterator l = newversionh.begin(); l != newversionh.end(); ++l)
			versionh << *l << endl;

		versionh.close();
	}
}
