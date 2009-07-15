// $Id: main.cpp 5269 2009-07-15 09:50:44Z FloSoft $
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

#include <samplerate.h>
#include <sndfile.h>

#ifdef _WIN32
#	include <windows.h>
#	define unlink DeleteFile
#	ifndef __CYGWIN__
#		define snprintf _snprintf
#	endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fstream>

#include "getopt.h"
#include "tempname.h"
#include "tokenizer.hpp"
#include "sample_rate_convert.h"

#include "../libsiedler2/src/libsiedler2.h"

int usage(int argc, char *argv[])
{
	printf("Usage: %s -s script.scs -f source.lst -t destination.lst", argv[0]);

	return EXIT_FAILURE;
}

void myexit()
{
	//printf("Press enter to exit\n");
	//getc(stdin);
}

int main(int argc, char *argv[])
{
	int index;
	const char *from = NULL, *to = NULL, *scs = NULL;

	atexit(myexit);

	while((index = getopt(argc, argv, "s:f:t:")) != EOF)
	{
		switch(index)
		{
		case 's': // Pidfile
			{
				scs = optarg;
			} break;
		case 'f':
			{
				from = optarg;
			} break;
		case 't':
			{
				to = optarg;
			} break;
		}
	}

	if(!scs || !to || !from)
		return usage(argc, argv);

	libsiedler2::ArchivInfo input, output;
	if(libsiedler2::Load(from, &input) != 0)
	{
		printf("Can't open input file \"%s\"\n", from);
		return EXIT_FAILURE;
	}

	std::ifstream in(scs);
	if(in.fail())
	{
		printf("Can't open script file \"%s\"\n", scs);
		return EXIT_FAILURE;
	}

	int linenr = 0;
	while(!in.eof())
	{
		std::string line;
		std::getline(in, line);
		++linenr;

		if(line.length() == 0 || line.at(0) == '#')
			continue;
		
		tokenizer token(line);

		std::string number = token.next();
		int nr = atoi(number.c_str());
		int frequency = atoi(token.next().c_str());

		/*if(frequency == 0)
		{
			printf("Script error on line  \"%d\": frequency unparseable or null\n", linenr, nr, frequency);
			return EXIT_FAILURE;
		}*/

		if(number == "copy")
		{
			printf("Copying item %d at line \"%d\"\n", frequency, linenr);
			output.pushC(input.get(frequency));
			continue;
		}

		libsiedler2::ArchivItem *item = input.get(nr);
		if(frequency == 0 || item == NULL || number == "empty")
		{
			printf("Inserting empty item at line \"%d\"\n", linenr);
			output.pushC(NULL);
			continue;
		}

		if(item->getBobType() != libsiedler2::BOBTYPE_SOUND)
		{
			printf("Script error on line  \"%d\": item %d does not exist or is not a sound\n", linenr, nr);
			return EXIT_FAILURE;
		}

		libsiedler2::ArchivItem_Sound_Wave *wave = dynamic_cast<libsiedler2::ArchivItem_Sound_Wave*>(item);
		if(!wave)
		{
			printf("Script error on line  \"%d\": item %d is not a wave-sound\n", linenr, nr);
			return EXIT_FAILURE;
		}

		char file[256];
		if(!tempname(file, 256))
		{
			printf("Can't find temporary filename for conversion - disk full?\n");
			return EXIT_FAILURE;
		}
		strncat(file, ".wav", 256);

		FILE *tmp = fopen(file, "wb");
		if(!tmp)
		{
			printf("Can't write to temporary file \"%s\" - disk full?\n", file);
			return EXIT_FAILURE;
		}

		unsigned char *data = new unsigned char[wave->getLength()];
		memcpy(data, wave->getData(), wave->getLength());
		unsigned short bitrate = 8;
		memcpy(&data[24], &frequency, 4);
		memcpy(&data[28], &frequency, 4);
		memcpy(&data[34], &bitrate, 2);

		fwrite(data, 1, wave->getLength(), tmp);
		fclose(tmp);

		SNDFILE	*infile, *outfile;
		SF_INFO sfinfo;

		infile = sf_open(file, SFM_READ, &sfinfo);
		if(!infile)
		{
			printf("Can't open temporary file \"%s\" for conversion - invalid type?\n", file);
			return EXIT_FAILURE;
		}

		double src_ratio = (44100.0) / sfinfo.samplerate;
		sfinfo.samplerate = 44100;
		sfinfo.channels = 1;
		sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16 | SF_ENDIAN_LITTLE;

		char file2[256];
		if(!tempname(file2, 256))
		{
			printf("Can't find temporary filename for conversion - disk full?\n");
			return EXIT_FAILURE;
		}
		strncat(file2, "-out.wav", 256);

		outfile = sf_open (file2, SFM_WRITE, &sfinfo);
		if(!outfile)
		{
			printf("Can't write to temporary file \"%s\" for conversion - disk full?\n", file2);
			return EXIT_FAILURE;
		}
		sf_command (outfile, SFC_SET_UPDATE_HEADER_AUTO, NULL, SF_TRUE);
		sf_command (outfile, SFC_SET_CLIPPING, NULL, SF_TRUE);

		sf_count_t count = 0;
		double gain = 0.5;

		do
		{
			count = sample_rate_convert (infile, outfile, SRC_SINC_BEST_QUALITY, src_ratio, sfinfo.channels, &gain);
		}
		while (count < 0);

		sf_close (infile);
		sf_close (outfile);

		//exit(1);
		unlink(file);

		FILE *tmp2 = fopen(file2, "rb");
		if(!tmp2)
		{
			printf("Can't open temporary file \"%s\" for reading\n", file2);
			return EXIT_FAILURE;
		}
		fseek(tmp2, 0, SEEK_END);
		unsigned int length = (unsigned int)ftell(tmp2);
		fseek(tmp2, 0, SEEK_SET);

		libsiedler2::ArchivItem_Sound_Wave result;
		if(result.load(tmp2, length) != 0)
		{
			printf("Can't read from temporary file \"%s\"\n", file2);
			return EXIT_FAILURE;
		}
		fclose(tmp2);
		unlink(file2);

		output.pushC(&result);
	}
	in.close();

	if(libsiedler2::loader::WriteLST(to, NULL, &output) != 0)
	{
		printf("Conversion failed - was not able to save results to \"%s\"\n", to);
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
