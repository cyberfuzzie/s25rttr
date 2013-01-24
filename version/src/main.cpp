// $Id: main.cpp 8595 2013-01-20 19:34:54Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#ifdef HAVE_CONFIG_H
#   include "../config.h"
#endif // HAVE_CONFIG_H

#ifdef __linux__
    #undef _WIN32
#endif

#ifdef _WIN32
#   include <windows.h>
#   define chdir !SetCurrentDirectoryA
#else
#   include <unistd.h>
#   include <dirent.h>
#   include <sys/types.h>
#   include <sys/stat.h>
#endif

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cctype>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

using namespace std;

/**
 * Get the current working directory as a string, terminated by a forward slash.
 */
string getcwd() {
    char curdir[4096];
#ifdef _WIN32
    GetCurrentDirectoryA(4096, curdir);
#else
    std::string ignorestupidgccwarning = getcwd(curdir, 4096);
    ignorestupidgccwarning = "";
#endif

    return std::string(curdir) + '/';
}

/**
 * List the files of a directory (non-recursively, exclude directories, don't sort resulting file list)
 * (copied from /src/ListDir.cpp and heavily modified)
 * @param path  The path of the directory to list. Must end in a path separator.
 * @param liste A pointer to the list into which the result will be written.
 */
void listDir(const string& path, std::list<std::string> *liste)
{
    if (!liste)
        return;
#ifdef _WIN32
    HANDLE hFile;
    WIN32_FIND_DATAA wfd;

    hFile=FindFirstFileA(path.c_str(), &wfd);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) {
                liste->push_back(wfd.cFileName);
            }
        } while(FindNextFileA(hFile,&wfd));

        FindClose(hFile);
    }
#else // == !_WIN32
    DIR *dir_d;
    dirent *dir = NULL;
    if ((dir_d = opendir(path.c_str())) != NULL)
    {
        while( (dir = readdir(dir_d)) != NULL)
        {
            struct stat file_stat;
            std::string whole_path = path + dir->d_name;

            stat(whole_path.c_str(), &file_stat);

            if(!S_ISDIR(file_stat.st_mode)) {
                liste->push_back(dir->d_name);
            }
        }
        closedir(dir_d);
    }
#endif // !_WIN32

}

/**
 * Print a notification when exit()ing the program
 */
void finish() {
    cerr << "       version: finished" << endl;
}

/**
 * Get the latest merged-in bzr revision from existing Git tags. Tags have to be named "bzr<rev>", that is,
 * the word "bzr" followed by the revision number. This function minds tags both in .git/refs/tags and in
 * .git/packed-refs.
 * @param source_dir The directory which contains the .git folder
 * @return the highest bzr revision number found on success, or 0 on failure
 */
int getLatestBzrRevFromGitTag(const string& source_dir) {
    int rev = 0;

    ifstream packrefs( (source_dir + ".git/packed-refs").c_str() );
    if (packrefs) {
        string l;
        while(getline(packrefs, l)) {
            stringstream ll(l);
            string hash, ref;
            ll >> hash;
            ll >> ref;
            if (ref.length() >= 16 && ref.substr(0,13) == "refs/tags/bzr") {
                int thisrev = atoi(ref.substr(13).c_str());
                if (thisrev > rev)
                    rev = thisrev;
            }
        }
        packrefs.close();
    }

    std::list<string> slist;
    listDir(source_dir + ".git/refs/tags/", &slist);
    for (std::list<string>::iterator it = slist.begin(); it != slist.end(); it++) {
        int thisrev = atoi(it->substr(3).c_str());
        if (thisrev > rev)
            rev = thisrev;
    }

    return rev;
}

/**
 * Resolve a given Git ref string against the .git/packed-refs file if it exists.
 * @param source_dir    The directory which contains the .git folder.
 * @param ref   The ref string you're looking for.
 * @param hash  A pointer to the string into which the matching SHA will be written if it is found.
 * @returns 0 on success, an errcode (from the ifstream constructor) on failure
 */
int getGitSHAFromPackedRefs(const string& source_dir, const string& ref, string *hash) {
    ifstream packrefs( (source_dir + ".git/packed-refs").c_str() );
    const int git_errno = errno;
    if (packrefs) {
        string l;
        while(getline(packrefs, l)) {
            stringstream ll(l);
            string thisHash, thisRef;
            ll >> thisHash;
            ll >> thisRef;
            if (thisRef == ref) {
                *hash = thisHash;
                break;
            }
        }
        packrefs.close();
        return 0;
    }
    return git_errno;
}

/**
 * Get the Git revision string from an existing .git/HEAD.
 * @param git   the (already open) input stream for the .git/HEAD file
 * @param source_dir    the source dir
 * @returns "git<SHA>" on success (where <SHA> are the first 8 digits of the full SHA) or "" (empty string)
 *      on failure.
 */
string getGitRevision(const string& source_dir, ifstream& git) {
    string firstpart;
    git >> firstpart;
    if (firstpart == "ref:") { //if HEAD is a sym-ref (pointing to a branch or tag)
        string secondpart;
        git >> secondpart;
        ifstream sha( (source_dir + ".git/" + secondpart).c_str() ); //try to get the ref's file from the "refs" folder
        const int git_errno = errno;
        if (sha) {
            sha >> firstpart;
            sha.close();
        }
        else {
            const int git_errno2 = getGitSHAFromPackedRefs(source_dir, secondpart, &firstpart);
            if (git_errno2 != 0) {
                cerr << "                Could not resolve Git HEAD:" << endl;
                cerr << "                .git/" << secondpart << ": " << strerror(git_errno) << endl;
                cerr << "                .git/packed-refs: " << strerror(git_errno2) << endl;
                return "";
            }
        }
    }
    //at this point, first part contains the full SHA of HEAD; if the repo is in a "detached HEAD" state,
    //  the SHA comes right from the HEAD file, otherwise the sym-ref in HEAD has been resolved
    return "git" + firstpart.substr(0, 8); //take the first 8 digits of the SHA and return them
}

int getRevString(const string& source_dir, ifstream& bzr, ifstream& svn, ifstream& git, string& revstring) {
    int revision = 0; //numeric revision (if bzr or svn are available)

    if(bzr) // use bazaar revision if exist
    {
        bzr >> revision;
        bzr.close();
    }
    else if(svn) // use subversion revision, if no bazaar one exists
    {
        string t;

        getline(svn, t); // entry count
        getline(svn, t); // empty
        getline(svn, t); // "dir"

        svn >> revision;
        svn.close();
    }

    stringstream revstrb(""); //a buffer to put together the revision string
    if (revision > 0) //if a bzr or svn revision number has been found
        revstrb << revision; //use that for now (if .git exists, it will override this)

    if(git) //if available, use git revision (SHA)
    {
        //extract latest merged-in bzr revision from existing git tags
        if (revision <= 0 && (revision = getLatestBzrRevFromGitTag(source_dir)) > 0)
            revstrb << revision << "-";

        string gitrev = getGitRevision(source_dir, git);
        if (gitrev.size() <= 0)
            return 1; //error was already printed in getGitRevision()
        revstrb << gitrev;
        git.close();
    }

    revstring = revstrb.str();
    return 0;
}

int main(int argc, char *argv[])
{
    std::string binary_dir = getcwd();

    if(argc >= 2) { //if given, change to the source directory
        if(chdir(argv[1]) != 0) {
            cerr << "chdir to directory \"" << argv[1] << "\" failed!" << endl;
            return 1;
        }
    }

    std::string source_dir = getcwd();
    cerr << "       version: started" << endl;
    cerr << "                source directory: \"" << source_dir << "\"" << endl;
    cerr << "                build  directory: \"" << binary_dir << "\"" << endl;

    atexit(finish); //register info output function for exit()

    //check for an existing Git repo
    ifstream git( (source_dir + ".git/HEAD").c_str() );
    const int git_errno = errno;

    //check for an existing bzr branch
    ifstream bzr( (source_dir + ".bzr/branch/last-revision").c_str() );
    const int bzr_errno = errno;

    //check for an existing SVN meta folder
    ifstream svn( (source_dir + ".svn/entries").c_str() );
    const int svn_errno = errno;

    if(!git && !svn && !bzr) {
        cerr << "                failed to read any of:" << endl;
        cerr << "                .git/HEAD: " << strerror(git_errno) << endl;
        cerr << "                .bzr/branch/last-revision: " << strerror(bzr_errno) << endl;
        cerr << "                .svn/entries: " << strerror(svn_errno) << endl;
        
        return 1;
    }

    //get the revision string for the existing repo
    string revstring;
    if (getRevString(source_dir, bzr, svn, git, revstring) != 0) //getRevString() will close all open ifstreams
        return 1;
    
    //check if the current build_version is forced to stay in place
    ifstream versionhforce( (binary_dir + "build_version.h.force").c_str() );
    if(versionhforce) {
        cerr << "                the file \"build_version.h.force\" does exist."<< endl;
        cerr << "                i will not change \"build_version.h\"." << endl;
        versionhforce.close();
        
        return 0;
    }

    //if a build_version.h already exists, open it
    ifstream versionh( (binary_dir + "build_version.h").c_str() );
    const int versionh_errno = errno;

    if(!versionh) { //if it doesnt exist, open the template build_version.h.in
        versionh.clear();
        versionh.open( (source_dir + "build_version.h.in").c_str() );
    }

    if(!versionh) { //if neither file could be opened, something went wrong
        cerr << "                failed to read any of:" << endl;
        cerr << "                build_version.h:    " << strerror(versionh_errno) << endl;
        cerr << "                build_version.h.in: " << strerror(errno) << endl;

        return 1;
    }

    vector<string> newversionh; //line buffer for the new file contents

    string l;
    bool changed = false;
    while(getline(versionh, l))
    {
        stringstream ll(l);
        string d, n, nv;

        ll >> d;  // '#define'
        ll >> n;  // name
        ll >> nv; // '"value"' (including the double quotes!)
        if (nv.length() > 2) //if there is indeed a value:
            nv = nv.substr(1, nv.length()-2); //strip the quotes

        if(n == "FORCE")
        {
                    cerr << "                the define \"FORCE\" does exist in the file \"build_version.h\""<< endl;
                    cerr << "                i will not change \"build_version.h\"" << endl;
        }

        if(n == "WINDOW_VERSION")
        {
            time_t t;
            time(&t);

            char tv[64];
            strftime(tv, 63, "%Y%m%d", localtime(&t) );
            string datestr(tv);
            if (nv != tv) //if the current date has changed
            {
                // set new day
                ll.clear();
                ll.str("");
                ll << d << " " << n << " \"" << tv << "\"";
                l = ll.str();

                cout << "                renewing version to day \"" << tv << "\"" << endl;
                changed = true;
            }
        }

        if(n == "WINDOW_REVISION")
        {
            if(nv != revstring) //if the revision string has changed
            {
                // set new revision
                ll.clear();
                ll.str("");
                ll << d << " " << n << " \"" << revstring << "\"";
                l = ll.str();

                cout << "                renewing version to revision \"" << revstring << "\"" << endl;
                changed = true;
            }
        }

        newversionh.push_back(l);
    }
    versionh.close();

    if(changed) // only write if changed
    {
        std::cerr << "                build_version.h has changed" << std::endl;

        ofstream versionh( (binary_dir + "build_version.h").c_str() );
        const int versionh_errno = errno;

        if(!versionh)
        {
            cerr << "failed to write to build_version.h: " << strerror(versionh_errno) << endl;
            return 1;
        }

        for(vector<string>::const_iterator l = newversionh.begin(); l != newversionh.end(); ++l)
            versionh << *l << endl;

        versionh.close();
    }
    else
        std::cerr << "                build_version.h is unchanged" << std::endl;

    return 0;
}
