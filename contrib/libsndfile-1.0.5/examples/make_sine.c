/* 
** Copyright (C) 1999-2003 Erik de Castro Lopo <erikd@zip.com.au> 
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
*/ 


#include	<stdio.h> 
#include	<string.h> 
#include	<math.h> 

#include	<sndfile.h> 

#ifndef		M_PI
#define		M_PI		3.14159265358979323846264338
#endif

#define		SAMPLE_RATE			8000 
#define		SAMPLE_COUNT		(SAMPLE_RATE*3)  /* 3 seconds */ 
#define		AMPLITUDE			32000.0 
#define		LEFT_FREQ			(3440.0 / SAMPLE_RATE) 
#define		RIGHT_FREQ			(3660.0 / SAMPLE_RATE) 

int 
main (void) 
{	SNDFILE	*file ; 
	SF_INFO	sfinfo ; 
	int		k ; 
	short	*buffer ; 
	 
	if (! (buffer = malloc (2 * SAMPLE_COUNT * sizeof (short)))) 
	{	printf ("Malloc failed.\n") ; 
		exit (0) ; 
		} ; 

	memset (&sfinfo, 0, sizeof (sfinfo)) ; 
	 
	sfinfo.samplerate  = SAMPLE_RATE ; 
	sfinfo.frames     = SAMPLE_COUNT ; 
	sfinfo.channels	   = 1 ; 
	sfinfo.format      = (SF_FORMAT_WAV | SF_FORMAT_PCM_16) ; 

	if (! (file = sf_open ("sine.wav", SFM_WRITE, &sfinfo))) 
	{	printf ("Error : Not able to open output file.\n") ; 
		return 1 ; 
		} ; 

	if (sfinfo.channels == 1)
	{	for (k = 0 ; k < SAMPLE_COUNT ; k++)	 
			buffer [k] = AMPLITUDE * sin (LEFT_FREQ * 2 * k * M_PI) ; 
		}
	else if (sfinfo.channels == 2)
	{	for (k = 0 ; k < SAMPLE_COUNT ; k++)	 
		{	buffer [2*k  ] = AMPLITUDE * sin (LEFT_FREQ * 2 * k * M_PI) ; 
			buffer [2*k+1] = AMPLITUDE * sin (RIGHT_FREQ * 2 * k * M_PI) ; 
			} ; 
		}
	else
	{	printf ("makesine can only generate mono or stereo files.\n") ;
		exit (1) ;
		} ;
		
	if (sf_write_short (file, buffer, sfinfo.channels * SAMPLE_COUNT) != 
											sfinfo.channels * SAMPLE_COUNT) 
		puts (sf_strerror (file)) ; 

	sf_close (file) ; 
	return	 0 ; 
} /* main */ 

