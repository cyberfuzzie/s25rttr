// $Id: main.cpp 5221 2009-07-08 09:28:46Z FloSoft $
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
#include "md5sum.h"

using namespace std;

#ifdef _LINUX_x86_64
#	undef ARCH
#	define ARCH "x86_64"
#endif

#ifdef _LINUX_i686
#	undef ARCH
#	define ARCH "i686"
#endif

#ifdef _WIN32
#	undef ARCH
#	define ARCH "win32"
#endif

#ifdef __APPLE__
#	undef ARCH
#	define ARCH "macos"
#endif

#ifndef ARCH
#	error You have to set ARCH to your processor architecture
#endif

#define HTTPHOST "http://nightly.ra-doersch.de/s25client/" ARCH "/"
#define FILELIST HTTPHOST "files"

#ifdef _WIN32
///////////////////////////////////////////////////////////////////////////////
/**
 *  \r fix-function for the stupid windows-console
 *  NOT THREADSAFE!!!
 *
 *  @author FloSoft
 */
static short backslashfix_y;

static short backslashrfix(short y)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE hConsoleOutput;
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);

	COORD Cursor_an_Pos = { 0, csbi.dwCursorPosition.Y + y};
	SetConsoleCursorPosition(hConsoleOutput , Cursor_an_Pos);

	return csbi.dwCursorPosition.Y + y;
}
#endif // !_WIN32

///////////////////////////////////////////////////////////////////////////////
/**
 *  replace all 'a' in a string 's' with 'b's
 *
 *  @author FloSoft
 */
string &replace_all(string &s, char a, char b)
{
	int idx;
	char bb[2] = {b, '\0'};
	while( (idx = s.find_first_of(a)) >= 0 ) {
		s.replace( idx, 1, bb );
	}
	return s;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  curl filewriter callback
 *
 *  @author FloSoft
 */
static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t realsize = size * nmemb;

	if(stream && realsize == fwrite(ptr, size, nmemb, stream))
		return realsize;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  curl stringwriter callback
 *
 *  @author FloSoft
 */
static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, string *data)
{
	size_t realsize = size * nmemb;

	char *cstr = new char[realsize + 1];
	memset(cstr, 0, realsize + 1);
	memcpy(cstr, ptr, realsize);
	*data += cstr;
	delete[] cstr;
	
	return realsize;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  create a directory-tree ( "mkdir -p" )
 *
 *  @author FloSoft
 */
static bool CreateDirRecursive(string dir)
{
	string::size_type npos = 0;
	while(npos != string::npos)
	{
		npos = dir.find('/', npos + 1);
		string ndir = (npos == string::npos ? dir : dir.substr(0, npos));

#ifdef _WIN32
		replace_all(ndir, '/', '\\');
#endif
		mkdir(ndir.c_str(), 0755);

		//cout << setw(npos) << "ndir: " << ndir << endl;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  curl progressbar callback
 *
 *  @author FloSoft
 */
static int ProgressBarCallback(string *data, double dltotal, double dlnow, double ultotal, double ulnow)
{
#ifdef _WIN32
	// \r not working fix
	if(backslashrfix(0) != backslashfix_y)
		backslashfix_y = backslashrfix(-1);
#endif // !_WIN32

	cout << "\r" << *data << setw(5) << setprecision(2) << setiosflags(ios:: fixed) << (dlnow * 100.0 / dltotal) << "%" << flush;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  httpdownload function (to string or to file, with or without progressbar)
 *
 *  @author FloSoft
 */
static bool DownloadFile(string url, string &to, string path = "", string progress = "")
{
	CURL *curl_handle;
	FILE *tofp = NULL;
	bool ok = true;

	curl_handle = curl_easy_init();

	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "s25update/1.0");
	curl_easy_setopt(curl_handle, CURLOPT_FAILONERROR, 1);

	// Write file to Memory?
	if(path == "")
	{
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, static_cast<void *>(&to));
	}
	else
	{
		tofp = fopen(path.c_str(), "wb");
		if(!tofp)
			cout << "Can't open file!!!!" << endl;
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, static_cast<void *>(tofp));
	}

	// Show Progress?
	if(progress != "")
	{
		curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl_handle, CURLOPT_PROGRESSFUNCTION, ProgressBarCallback);
		curl_easy_setopt(curl_handle, CURLOPT_PROGRESSDATA, static_cast<void *>(&progress));
	}

	//curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
	
	if(curl_easy_perform(curl_handle) != 0)
		ok = false;
	
	curl_easy_cleanup(curl_handle);

	if(tofp)
		fclose(tofp);

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  calculate md5sum for a file
 *
 *  @author FloSoft
 */
string md5sum(string file)
{
	string digest = "";

	FILE *fp = fopen(file.c_str(), "rb");
	if(fp)
	{
		md5file(fp, digest);
		fclose(fp);
	}
	return digest;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  main function
 *
 *  @author FloSoft
 */
int main(int argc, char *argv[])
{
	// initialize curl
	curl_global_init(CURL_GLOBAL_ALL);
	atexit(curl_global_cleanup);

	string filelist;
	string hash, file;
	size_t longestname = 0,longestpath = 0;
	map<string,string> files;

	// download filelist
	if(!DownloadFile(FILELIST, filelist))
		return 1;

	stringstream flstream(filelist);

	cout << filelist << endl;
	
	// parse filelist
	string line;
	while( getline(flstream, line) )
	{
		hash = line.substr(0, 32);
		file = line.substr(34);

		files.insert(pair<string,string>(hash,file));
		
		if(flstream.fail())
			break;

		string name = file.substr(file.rfind('/') + 1);
		string path = file.substr(0, file.rfind('/'));

		// find longest name and path
		if(name.length() > longestname)
			longestname = name.length();
		if(path.length() > longestname)
			longestpath = path.length();
	}

	// check md5 of files and download them
	map<string,string>::iterator it = files.begin();
	while(it != files.end())
	{
		hash = it->first;
		file = it->second;
		++it;
		
		string tfile = file;
#ifdef _WIN32
		replace_all(tfile, '/', '\\');
#endif

		// check hash of file
		if(hash != md5sum(tfile))
		{
			string name = file.substr(file.rfind('/') + 1);
			string path = file.substr(0, file.rfind('/'));
			string bzfile = file + ".bz2";

			// create path of file
			CreateDirRecursive(path);

			stringstream progress;
			progress << "Downloading file \"" << setw(longestname) << setiosflags(ios::left) << name << "\" to \"" << setw(longestpath) << path << "\": ";
			string url = HTTPHOST + bzfile;
			string fdata = "";

#ifdef _WIN32
			replace_all(bzfile, '/', '\\');
#endif
			// download the file
			DownloadFile(url, fdata, bzfile, progress.str().c_str());

			cout << " - ";

			// extract the file
			int bzerror = BZ_OK;
			FILE *bzfp = fopen(bzfile.c_str(), "rb");
			if(!bzfp)
			{
				cerr << "decompression failed: download failure?" << endl;
				return 1;
			}
			else
			{
				bzerror = BZ_OK;
				BZFILE *bz2fp = BZ2_bzReadOpen( &bzerror, bzfp, 0, 0, NULL, 0);
				if(!bz2fp)
				{
					cout << "decompression failed: compressed file corrupt?" << endl;
					return 1;
				}
				else
				{
					FILE *fp = fopen(tfile.c_str(), "wb");
					if(!fp)
					{
						cout << "decompression failed: compressed data corrupt?" << endl;
						return 1;
					}
					else
					{
						while(bzerror == BZ_OK)
						{
							char buffer[1024];
							int read = BZ2_bzRead ( &bzerror, bz2fp, buffer, 1024 );
							fwrite(buffer, 1, read, fp);
						
						}
						fclose(fp);

						cout << "ok";
					}

					BZ2_bzReadClose(&bzerror, bz2fp);
				}
				fclose(bzfp);

				// remove compressed file
				unlink(bzfile.c_str());
			}

			cout << endl;
#ifdef _WIN32
			// \r not working fix
			backslashfix_y = backslashrfix(0);
#endif // !_WIN32
		}
	}

#if defined _DEBUG && defined _MSC_VER
	cout << "Press return to continue . . ." << flush;
	cin.get();
#endif

	return 0;
}
