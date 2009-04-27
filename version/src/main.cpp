// $Id: main.cpp 4652 2009-03-29 10:10:02Z FloSoft $
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
#	include <conio.h>
#	define chdir SetCurrentDirectory
#	ifdef _MSC_VER
#		pragma warning ( disable : 4996 )
#		pragma comment ( lib, "libxml2" )
#	endif
#else
#	include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

int main(int argc, char *argv[])
{
	if(argc >= 2)
		chdir(argv[1]);

	FILE *entries = fopen(".svn/entries", "r");
	if(!entries)
	{
		fprintf(stderr, "Failed to read source %s: %s\n", ".svn/entries", strerror(errno));
		return 1;
	}
	
	bool use_xml = false;
	char buffer[4096], *revision = NULL;
	fgets(buffer, 4096, entries);

	if(atoi(buffer) < 6)
	{
		use_xml = true;
	}
	else
	{
		fgets(buffer, 4096, entries);
		fgets(buffer, 4096, entries);
		fgets(buffer, 4096, entries);

		revision = buffer;
		for(size_t i = 0; i < strlen(revision) && i < 4096; ++i)
			if(!isdigit(revision[i]))
				revision[i] = '\0';
	}

	fclose(entries);

	if(use_xml)
	{
		LIBXML_TEST_VERSION;
		
		xmlKeepBlanksDefault(0);

		xmlDocPtr doc;

		doc = xmlParseFile(".svn/entries");
		if(doc == NULL)
		{
			fprintf(stderr, "Failed to parse source %s: %s\n", ".svn/entries", strerror(errno));
			return 1;
		}

		xmlNodePtr root_file, cur;

		root_file = xmlDocGetRootElement(doc);
		if (root_file == NULL) 
		{
			fprintf(stderr, "Failed to get root-element: Empty Document?\n");
			xmlFreeDoc(doc);
			return 1;
		}

		printf("%s\n", root_file->name);

		cur = root_file->xmlChildrenNode;
		if(cur == NULL)
		{
			fprintf(stderr, "Failed to get child-element: Empty Document?\n");
			xmlFreeDoc(doc);
			return 1;
		}

		revision = (char*)xmlGetProp(cur, (const xmlChar*)"revision");

		xmlFreeDoc(doc);
		xmlCleanupParser();
	}

	if(revision)
	{
		char version[100];
		memset(version, 0, 100);

		if(argc > 1 && strcmp(argv[1], "--no-dots") == 0)
		{
			sprintf(version, "%s", revision);
		}
		else
		{
			if(atoi(revision) < 10)
				sprintf(version, "0.0.0.%c", revision[0]);
			else if(atoi(revision) < 100)
				sprintf(version, "0.0.%c.%c", revision[0], revision[1]);
			else if(atoi(revision) < 1000)
				sprintf(version, "0.%c.%c.%c", revision[0], revision[1], revision[2]);
			else
			{
				char a,b,c;
				a = revision[strlen(revision)-3];
				b = revision[strlen(revision)-2];
				c = revision[strlen(revision)-1];
				revision[strlen(revision)-3] = '\0';
				sprintf(version, "%s.%c.%c.%c", revision, a, b, c);
			}
		}

		FILE *in = fopen("version.h", "rb");

		if(!in)
			exit(1);
		else
		{
			fseek(in, 0, SEEK_END);
			unsigned long size = ftell(in);
			fseek(in, 0, SEEK_SET);

			char *buffer = new char[size+1];
			memset(buffer, 0, sizeof(char)*(size+1));
			fread(buffer, 1, size, in);
			fclose(in);

			char *buffer2 = new char[size*2+1];
			memset(buffer2, 0, sizeof(char)*(size*2+1));

			char v[100];
			memset(v, 0, 100);

			unsigned long y = 0;
			for(unsigned long x = 0; x < size; ++x)
			{
				if(x < size-14 && strncmp(&buffer[x], "WINDOW_REVISION", 15) == 0)
				{
					unsigned long k = x;
					while(buffer[x] != '\n' && buffer[x] != '\r')
						++x;

					buffer[x-1] = '\0';
					strncpy(v, &buffer[k+17], 100);

					sprintf(&buffer2[y], "WINDOW_REVISION \"%s\"", version);
					y += (unsigned long)strlen(&buffer2[y]);
				}
				buffer2[y++] = buffer[x];
			}

			if(strcmp(version, v) != 0)
			{
				FILE *out = fopen("version.h", "wb");
				{
					printf("renewing version to \"%s\"\r\n", version);
					fwrite(buffer2, 1, y, out);
					fclose(out);
				}
			}

			delete[] buffer;
			delete[] buffer2;
		}
	}
}
