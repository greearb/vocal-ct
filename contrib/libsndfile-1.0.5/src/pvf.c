/*
** Copyright (C) 2002,2003,2003 Erik de Castro Lopo <erikd@zip.com.au>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	<stdio.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<string.h>
#include	<ctype.h>

#include	"sndfile.h"
#include	"config.h"
#include	"sfendian.h"
#include	"common.h"

/*------------------------------------------------------------------------------
** Macros to handle big/little endian issues.
*/

#define PVF1_MARKER		(MAKE_MARKER ('P', 'V', 'F', '1'))

/*------------------------------------------------------------------------------
** Private static functions.
*/

static	int		pvf_close		(SF_PRIVATE *psf) ;

static int		pvf_write_header (SF_PRIVATE *psf, int calc_length) ;
static int		pvf_read_header (SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
pvf_open	(SF_PRIVATE *psf)
{	int		subformat ;
	int		error = 0 ;
	
	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = pvf_read_header (psf)))
			return error ;
		} ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_PVF)
			return	SFE_BAD_OPEN_FORMAT ;

		psf->endian = SF_ENDIAN_BIG ;

		if (pvf_write_header (psf, SF_FALSE))
			return psf->error ;

		psf->write_header = pvf_write_header ;
		} ;

	psf->close = pvf_close ;

	psf->blockwidth  = psf->bytewidth * psf->sf.channels ;

	switch (subformat)
	{	case  SF_FORMAT_PCM_S8 :	/* 8-bit linear PCM. */
		case  SF_FORMAT_PCM_16 :	/* 16-bit linear PCM. */
		case  SF_FORMAT_PCM_32 :	/* 32-bit linear PCM. */
				error = pcm_init (psf) ;
				break ;

		default :   break ;
		} ;

	return error ;
} /* pvf_open */

/*------------------------------------------------------------------------------
*/

static int
pvf_close	(SF_PRIVATE  *psf)
{
	psf = psf ;

	return 0 ;
} /* pvf_close */

static int
pvf_write_header (SF_PRIVATE *psf, int calc_length)
{	sf_count_t	current ;

	calc_length = calc_length ; /* Avoid a compiler warning. */

	current = psf_ftell (psf) ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;
	psf_fseek (psf, 0, SEEK_SET) ;
	
	LSF_SNPRINTF ((char*) psf->header, sizeof (psf->header), "PVF1\n%d %d %d\n", 
		psf->sf.channels, psf->sf.samplerate, psf->bytewidth * 8) ; 

	psf->headindex = strlen ((char*) psf->header) ;

	/* Header construction complete so write it out. */
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;

	return psf->error ;
} /* pvf_write_header */

static int
pvf_read_header (SF_PRIVATE *psf)
{	char	buffer [32] ;
	int		marker, channels, samplerate, bitwidth ;

	psf_binheader_readf (psf, "pmj", 0, &marker, 1) ;
	psf_log_printf (psf, "%M\n", marker) ;

	if (marker != PVF1_MARKER)
		return SFE_PVF_NO_PVF1 ;

	/* FIXME : This should use something like psf_binheader_readf so that
	** at some time in the future, cached header reading can be done.
	*/

	psf_fgets (buffer, sizeof (buffer), psf) ;

	if (sscanf (buffer, "%d %d %d", &channels, &samplerate, &bitwidth) != 3)
		return SFE_PVF_BAD_HEADER ;

	psf_log_printf (psf, " Channels    : %d\n Sample rate : %d\n Bit width   : %d\n", 
				channels, samplerate, bitwidth) ;

	psf->sf.channels = channels ;
	psf->sf.samplerate = samplerate ;
	
	switch (bitwidth)
	{	case 8 : 
				psf->sf.format = SF_FORMAT_PVF | SF_FORMAT_PCM_S8 ;
				psf->bytewidth  = 1 ;
				break ;

		case 16 : 
				psf->sf.format = SF_FORMAT_PVF | SF_FORMAT_PCM_16 ;
				psf->bytewidth  = 2 ;
				break ;
		case 32 : 
				psf->sf.format = SF_FORMAT_PVF | SF_FORMAT_PCM_32 ;
				psf->bytewidth  = 4 ;
				break ;

		default :
				return SFE_PVF_BAD_BITWIDTH ;
		} ;
		
	psf->dataoffset = psf_fseek (psf, 0, SEEK_CUR) ;
	psf_log_printf (psf, " Data Offset : %D\n", psf->dataoffset) ;

	psf->datalength = psf->filelength - psf->dataoffset ;

 	if (psf_fseek (psf, psf->dataoffset, SEEK_SET) != psf->dataoffset)
		return SFE_BAD_SEEK ;

	psf->close = pvf_close ;

	psf->endian = SF_ENDIAN_BIG ;
	
	psf->blockwidth = psf->sf.channels * psf->bytewidth ;

	if (! psf->sf.frames && psf->blockwidth)
		psf->sf.frames = (psf->filelength - psf->dataoffset) / psf->blockwidth ;

	return 0 ;
} /* pvf_read_header */
