/*
** Copyright (C) 2002-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <samplerate.h>
#include <sndfile.h>

static double apply_gain (float * data, long frames, int channels, double max, double gain);

static const long BUFFER_LEN = 4096;

extern "C" sf_count_t sample_rate_convert (SNDFILE *infile, SNDFILE *outfile, int converter, double src_ratio, int channels, double *gain)
{
	static float input [BUFFER_LEN];
	static float output [BUFFER_LEN];

	SRC_STATE	*src_state;
	SRC_DATA	src_data;
	int			error;
	double		max = 0.0;
	sf_count_t	output_count = 0;

	sf_seek (infile, 0, SEEK_SET);
	sf_seek (outfile, 0, SEEK_SET);

	/* Initialize the sample rate converter. */
	if ((src_state = src_new (converter, channels, &error)) == NULL)
	{	
		printf ("Error : src_new() failed : %s.\n", src_strerror (error));
		exit (1);
	}

	src_data.end_of_input = 0; /* Set this later. */

	/* Start with zero to force load in while loop. */
	src_data.input_frames = 0;
	src_data.data_in = input;

	src_data.src_ratio = src_ratio;

	src_data.data_out = output;
	src_data.output_frames = BUFFER_LEN / channels;

	while (1)
	{
		/* If the input buffer is empty, refill it. */
		if (src_data.input_frames == 0)
		{	
			src_data.input_frames = (long)sf_readf_float (infile, input, BUFFER_LEN / channels);
			src_data.data_in = input;

			/* The last read will not be a full buffer, so snd_of_input. */
			if (src_data.input_frames < BUFFER_LEN / channels)
				src_data.end_of_input = SF_TRUE;
		}

		if ((error = src_process (src_state, &src_data)))
		{	
			printf ("Error : %s\n", src_strerror (error));
			exit (1);
		}

		/* Terminate if done. */
		if (src_data.end_of_input && src_data.output_frames_gen == 0)
			break;

		max = apply_gain (src_data.data_out, src_data.output_frames_gen, channels, max, *gain);

		/* Write output. */
		sf_writef_float (outfile, output, src_data.output_frames_gen);
		output_count += src_data.output_frames_gen;

		src_data.data_in += src_data.input_frames_used * channels;
		src_data.input_frames -= src_data.input_frames_used;
	}

	src_state = src_delete (src_state);

	if (max > 1.0)
	{	
		*gain = 1.0 / max;
		printf ("Output has clipped. Restarting conversion to prevent clipping.\n");
		output_count = 0;
		sf_command (outfile, SFC_FILE_TRUNCATE, &output_count, sizeof (output_count));
		return -1;
	}

	return output_count;
} /* sample_rate_convert */

static double apply_gain (float *data, long frames, int channels, double max, double gain)
{
	long k;

	for (k = 0; k < frames * channels; k++)
	{	
		data[k] *= (float)gain;

		if (fabs (data [k]) > max)
			max = fabs (data [k]);
	}

	return max;
}
