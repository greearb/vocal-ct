/*
** Copyright (C) 1999-2003 Erik de Castro Lopo <erikd@zip.com.au>
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
#include	<string.h>
#include	<time.h>
#include	<ctype.h>

#include	"sndfile.h"
#include	"config.h"
#include	"sfendian.h"
#include	"common.h"

/*------------------------------------------------------------------------------
 * Macros to handle big/little endian issues.
 */

#define FORM_MARKER	(MAKE_MARKER ('F', 'O', 'R', 'M'))
#define AIFF_MARKER	(MAKE_MARKER ('A', 'I', 'F', 'F'))
#define AIFC_MARKER	(MAKE_MARKER ('A', 'I', 'F', 'C'))
#define COMM_MARKER	(MAKE_MARKER ('C', 'O', 'M', 'M'))
#define SSND_MARKER	(MAKE_MARKER ('S', 'S', 'N', 'D'))
#define MARK_MARKER	(MAKE_MARKER ('M', 'A', 'R', 'K'))
#define INST_MARKER	(MAKE_MARKER ('I', 'N', 'S', 'T'))
#define APPL_MARKER	(MAKE_MARKER ('A', 'P', 'P', 'L'))

#define c_MARKER	(MAKE_MARKER ('(', 'c', ')', ' '))
#define NAME_MARKER	(MAKE_MARKER ('N', 'A', 'M', 'E'))
#define AUTH_MARKER	(MAKE_MARKER ('A', 'U', 'T', 'H'))
#define ANNO_MARKER	(MAKE_MARKER ('A', 'N', 'N', 'O'))
#define COMT_MARKER	(MAKE_MARKER ('C', 'O', 'M', 'T'))
#define FVER_MARKER	(MAKE_MARKER ('F', 'V', 'E', 'R'))
#define SFX_MARKER	(MAKE_MARKER ('S', 'F', 'X', '!'))


#define PEAK_MARKER	(MAKE_MARKER ('P', 'E', 'A', 'K'))

/* Supported AIFC encodings.*/
#define NONE_MARKER	(MAKE_MARKER ('N', 'O', 'N', 'E'))
#define sowt_MARKER	(MAKE_MARKER ('s', 'o', 'w', 't'))
#define twos_MARKER	(MAKE_MARKER ('t', 'w', 'o', 's'))
#define raw_MARKER	(MAKE_MARKER ('r', 'a', 'w', ' '))
#define in32_MARKER	(MAKE_MARKER ('i', 'n', '3', '2'))
#define ni32_MARKER	(MAKE_MARKER ('2', '3', 'n', 'i'))

#define fl32_MARKER	(MAKE_MARKER ('f', 'l', '3', '2'))
#define FL32_MARKER	(MAKE_MARKER ('F', 'L', '3', '2'))
#define fl64_MARKER	(MAKE_MARKER ('f', 'l', '6', '4'))
#define FL64_MARKER	(MAKE_MARKER ('F', 'L', '6', '4'))

#define ulaw_MARKER	(MAKE_MARKER ('u', 'l', 'a', 'w'))
#define ULAW_MARKER	(MAKE_MARKER ('U', 'L', 'A', 'W'))
#define alaw_MARKER	(MAKE_MARKER ('a', 'l', 'a', 'w'))
#define ALAW_MARKER	(MAKE_MARKER ('A', 'L', 'A', 'W'))

#define DWVW_MARKER	(MAKE_MARKER ('D', 'W', 'V', 'W'))
#define GSM_MARKER	(MAKE_MARKER ('G', 'S', 'M', ' '))

/* Unsupported AIFC encodings.*/
#define ima4_MARKER	(MAKE_MARKER ('i', 'm', 'a', '4'))

#define MAC3_MARKER	(MAKE_MARKER ('M', 'A', 'C', '3'))
#define MAC6_MARKER	(MAKE_MARKER ('M', 'A', 'C', '6'))
#define ADP4_MARKER	(MAKE_MARKER ('A', 'D', 'P', '4'))

/* Predfined chunk sizes. */
#define SIZEOF_AIFF_COMM		18
#define SIZEOF_AIFC_COMM_MIN	22
#define SIZEOF_AIFC_COMM		24
#define SIZEOF_SSND_CHUNK		8
#define SIZEOF_INST_CHUNK		20

/*------------------------------------------------------------------------------
 * Typedefs for file chunks.
 */

enum
{	HAVE_FORM	= 0x01,
	HAVE_AIFF	= 0x02,
	HAVE_COMM	= 0x04,
	HAVE_SSND	= 0x08
} ;

typedef struct
{	unsigned int	type ;
	unsigned int	size ;
	short			numChannels ;
	unsigned int	numSampleFrames ;
	short			sampleSize ;
	unsigned char	sampleRate [10] ;
	unsigned int	encoding ;
	char			zero_bytes [2] ;
} COMM_CHUNK ;

typedef struct
{	unsigned int	offset ;
	unsigned int	blocksize ;
} SSND_CHUNK ;

typedef struct
{	short			playMode ;
    unsigned short	beginLoop ;
	unsigned short	endLoop ;
} INST_LOOP ;

typedef struct
{	char		baseNote ;		/* all notes are MIDI note numbers */
	char		detune ;		/* cents off, only -50 to +50 are significant */
	char		lowNote ;
	char		highNote ;
	char		lowVelocity ;	/* 1 to 127 */
	char		highVelocity ;	/* 1 to 127 */
	short		gain ;			/* in dB, 0 is normal */
	INST_LOOP	sustain_loop ;
	INST_LOOP	release_loop ;
} INST_CHUNK ;

/*------------------------------------------------------------------------------
 * Private static functions.
 */

static int	aiff_close	(SF_PRIVATE *psf) ;

static int	tenbytefloat2int (unsigned char *bytes) ;
static void	uint2tenbytefloat (unsigned int num, unsigned char *bytes) ;

static int	aiff_read_comm_chunk (SF_PRIVATE *psf, COMM_CHUNK *comm_fmt) ;

static int	aiff_read_header (SF_PRIVATE *psf, COMM_CHUNK *comm_fmt) ;

static int	aiff_write_header (SF_PRIVATE *psf, int calc_length) ;
static int	aiff_write_tailer (SF_PRIVATE *psf) ;
static void aiff_write_strings (SF_PRIVATE *psf, int location) ;

static int	aiff_command (SF_PRIVATE *psf, int command, void *data, int datasize) ;

static char *get_loop_mode_str (short mode) ;


/*------------------------------------------------------------------------------
** Public function.
*/

int
aiff_open	(SF_PRIVATE *psf)
{	COMM_CHUNK comm_fmt ;
	int error, subformat ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	if (psf->mode == SFM_READ || (psf->mode == SFM_RDWR && psf->filelength > 0))
	{	if ((error = aiff_read_header (psf, &comm_fmt)))
			return error ;

		psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
		} ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_AIFF)
			return	SFE_BAD_OPEN_FORMAT ;

		if (psf->mode == SFM_WRITE && (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE))
		{	psf->has_peak = SF_TRUE ;
			psf->peak_loc = SF_PEAK_START ;
			} ;

		if (psf->mode != SFM_RDWR || psf->filelength < 40)
		{	psf->filelength = 0 ;
			psf->datalength = 0 ;
			psf->dataoffset = 0 ;
			psf->sf.frames = 0 ;
			} ;

		psf->str_flags = SF_STR_ALLOW_START | SF_STR_ALLOW_END ;

		if ((error = aiff_write_header (psf, SF_FALSE)))
			return error ;

		psf->write_header = aiff_write_header ;
		} ;

	psf->close = aiff_close ;
	psf->command = aiff_command ;

	psf->blockwidth = psf->sf.channels * psf->bytewidth ;

	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_U8 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_PCM_S8 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
				error = pcm_init (psf) ;
				break ;

		case SF_FORMAT_FLOAT :
				error = float32_init (psf) ;
				break ;

		case SF_FORMAT_DOUBLE :
				error = double64_init (psf) ;
				break ;

		case SF_FORMAT_ULAW :
				error = ulaw_init (psf) ;
				break ;

		case SF_FORMAT_ALAW :
				error = alaw_init (psf) ;
				break ;

		case SF_FORMAT_DWVW_12 :
				error = dwvw_init (psf, 12) ;
				break ;

		case SF_FORMAT_DWVW_16 :
				error = dwvw_init (psf, 16) ;
				break ;

		case SF_FORMAT_DWVW_24 :
				error = dwvw_init (psf, 24) ;
				break ;

		case SF_FORMAT_DWVW_N :
				if (psf->mode != SFM_READ)
				{	error = SFE_DWVW_BAD_BITWIDTH ;
					break ;
					} ;
				if (comm_fmt.sampleSize >= 8 && comm_fmt.sampleSize < 24)
				{	error = dwvw_init (psf, comm_fmt.sampleSize) ;
					psf->sf.frames = comm_fmt.numSampleFrames ;
					break ;
					} ;
				psf_log_printf (psf, "AIFC : Bad bitwidth %d\n", comm_fmt.sampleSize) ;
				error = SFE_DWVW_BAD_BITWIDTH ;
				break ;

		case SF_FORMAT_GSM610 :
				error = gsm610_init (psf) ;
				break ;

		/*
		case SF_FORMAT_IMA_ADPCM :
				error = aiff_ima_writer_init (psf, 34) ;
				break ;
		*/

		default : return SFE_UNIMPLEMENTED ;
		} ;

	if (psf->mode == SFM_READ)
		psf->blockwidth = psf->sf.channels * psf->bytewidth ;

	return error ;
} /* aiff_open */

/*==========================================================================================
** Private functions.
*/

static int
aiff_read_header (SF_PRIVATE *psf, COMM_CHUNK *comm_fmt)
{	SSND_CHUNK	ssnd_fmt ;
	int			marker, dword, bytesread, k ;
	int			FORMsize, SSNDsize ;
	int			filetype, found_chunk = 0, done = 0, error = 0 ;
	char		*cptr, byte ;

	/* Set position to start of file to begin reading header. */
	psf_binheader_readf (psf, "p", 0) ;

	memset (comm_fmt, 0, sizeof (COMM_CHUNK)) ;

	/* Until recently AIF* file were all BIG endian. */
	psf->endian = SF_ENDIAN_BIG ;

	/* 	AIFF files can apparently have their chunks in any order. However, they
	**	must have a FORM chunk. Approach here is to read all the chunks one by
	**  one and then check for the mandatory chunks at the end.
	*/
	while (! done)
	{	psf_binheader_readf (psf, "m", &marker) ;
		switch (marker)
		{	case FORM_MARKER :
					if (found_chunk)
						return SFE_AIFF_NO_FORM ;

					psf_binheader_readf (psf, "E4", &FORMsize) ;

					if (psf->fileoffset > 0 && psf->filelength > FORMsize + 8)
					{	/* Set file length. */
						psf->filelength = FORMsize + 8  ;
						psf_log_printf (psf, "FORM : %u\n", FORMsize) ;
						}
					else if (FORMsize != psf->filelength - 2 * SIGNED_SIZEOF (dword))
					{	dword = psf->filelength - 2 * sizeof (dword) ;
						psf_log_printf (psf, "FORM : %u (should be %u)\n", FORMsize, dword) ;
						FORMsize = dword ;
						}
					else
						psf_log_printf (psf, "FORM : %u\n", FORMsize) ;
					found_chunk |= HAVE_FORM ;
					break ;

			case AIFC_MARKER :
			case AIFF_MARKER :
					if (! (found_chunk & HAVE_FORM))
						return SFE_AIFF_AIFF_NO_FORM ;
					filetype = marker ;
					psf_log_printf (psf, " %M\n", marker) ;
					found_chunk |= HAVE_AIFF ;
					break ;

			case COMM_MARKER :
					error = aiff_read_comm_chunk (psf, comm_fmt) ;

					psf->sf.samplerate 	= tenbytefloat2int (comm_fmt->sampleRate) ;
					psf->sf.frames 	= comm_fmt->numSampleFrames ;
					psf->sf.channels 	= comm_fmt->numChannels ;
					psf->bytewidth 		= BITWIDTH2BYTES (comm_fmt->sampleSize) ;

					if (error)
						return error ;

					found_chunk |= HAVE_COMM ;
					break ;

			case PEAK_MARKER :
					/* Must have COMM chunk before PEAK chunk. */
					if ((found_chunk & (HAVE_FORM | HAVE_AIFF | HAVE_COMM)) != (HAVE_FORM | HAVE_AIFF | HAVE_COMM))
						return SFE_AIFF_PEAK_B4_COMM ;

					psf_binheader_readf (psf, "E4", &dword) ;

					psf_log_printf (psf, "%M : %d\n", marker, dword) ;
					if (dword > SIGNED_SIZEOF (psf->peak))
					{	psf_binheader_readf (psf, "j", dword) ;
						psf_log_printf (psf, "*** File PEAK chunk bigger than sizeof (PEAK_CHUNK).\n") ;
						return SFE_WAV_BAD_PEAK ;
						} ;
					if (dword != SIGNED_SIZEOF (psf->peak) - SIGNED_SIZEOF (psf->peak.peak) + psf->sf.channels * SIGNED_SIZEOF (PEAK_POS))
					{	psf_binheader_readf (psf, "j", dword) ;
						psf_log_printf (psf, "*** File PEAK chunk size doesn't fit with number of channels.\n") ;
						return SFE_WAV_BAD_PEAK ;
						} ;

					/* read in rest of PEAK chunk. */
					psf_binheader_readf (psf, "E44", &(psf->peak.version), &(psf->peak.timestamp)) ;

					if (psf->peak.version != 1)
						psf_log_printf (psf, "  version    : %d *** (should be version 1)\n", psf->peak.version) ;
					else
						psf_log_printf (psf, "  version    : %d\n", psf->peak.version) ;

					psf_log_printf (psf, "  time stamp : %d\n", psf->peak.timestamp) ;
					psf_log_printf (psf, "    Ch   Position       Value\n") ;

					cptr = (char *) psf->buffer ;
					for (dword = 0 ; dword < psf->sf.channels ; dword++)
					{	psf_binheader_readf (psf, "Ef4", &(psf->peak.peak [dword].value),
														&(psf->peak.peak [dword].position)) ;

						LSF_SNPRINTF (cptr, sizeof (psf->buffer), "    %2d   %-12d   %g\n",
								dword, psf->peak.peak [dword].position, psf->peak.peak [dword].value) ;
						cptr [sizeof (psf->buffer) - 1] = 0 ;
						psf_log_printf (psf, cptr) ;
						} ;

					psf->has_peak = SF_TRUE ; /* Found PEAK chunk. */
					break ;

			case SSND_MARKER :
					psf_binheader_readf (psf, "E444", &SSNDsize, &(ssnd_fmt.offset), &(ssnd_fmt.blocksize)) ;

					psf->datalength = SSNDsize - sizeof (ssnd_fmt) ;
					psf->dataoffset = psf_ftell (psf) ;

					if (psf->datalength > psf->filelength - psf->dataoffset || psf->datalength < 0)
					{	psf_log_printf (psf, " SSND : %u (should be %D)\n", SSNDsize, psf->filelength - psf->dataoffset + sizeof (SSND_CHUNK)) ;
						psf->datalength = psf->filelength - psf->dataoffset ;
						}
					else
					{	psf_log_printf (psf, " SSND : %u\n", SSNDsize) ;
						psf->dataend = psf->datalength + psf->dataoffset ;
						} ;

					psf_log_printf (psf, "  Offset     : %d\n", ssnd_fmt.offset) ;
					psf_log_printf (psf, "  Block Size : %d\n", ssnd_fmt.blocksize) ;

					found_chunk |= HAVE_SSND ;

					if (! psf->sf.seekable)
						break ;

					/* 	Seek to end of SSND chunk. */
					psf_fseek (psf, psf->dataoffset + psf->datalength, SEEK_SET) ;
					break ;

			case c_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					dword += (dword & 1) ;
					if (dword == 0)
						break ;
					if (dword > SIGNED_SIZEOF (psf->buffer))
					{	psf_log_printf (psf, " %M : %d (too big)\n", marker, dword) ;
						return SFE_INTERNAL ;
						} ;

					cptr = (char*) psf->buffer ;
					psf_binheader_readf (psf, "b", cptr, dword) ;
					cptr [dword - 1] = 0 ;
					psf_log_printf (psf, " %M : %s\n", marker, cptr) ;
					psf_store_string (psf, SF_STR_COPYRIGHT, cptr) ;
					break ;

			case AUTH_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					dword += (dword & 1) ;
					if (dword == 0)
						break ;
					if (dword > SIGNED_SIZEOF (psf->buffer))
					{	psf_log_printf (psf, " %M : %d (too big)\n", marker, dword) ;
						return SFE_INTERNAL ;
						} ;

					cptr = (char*) psf->buffer ;
					psf_binheader_readf (psf, "b", cptr, dword) ;
					cptr [dword - 1] = 0 ;
					psf_log_printf (psf, " %M : %s\n", marker, cptr) ;
					psf_store_string (psf, SF_STR_ARTIST, cptr) ;
					break ;

			case COMT_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					dword += (dword & 1) ;
					if (dword == 0)
						break ;
					if (dword > SIGNED_SIZEOF (psf->buffer))
					{	psf_log_printf (psf, " %M : %d (too big)\n", marker, dword) ;
						return SFE_INTERNAL ;
						} ;

					cptr = (char*) psf->buffer ;
					psf_binheader_readf (psf, "b", cptr, dword) ;
					cptr [dword - 1] = 0 ;
					psf_log_printf (psf, " %M : %s\n", marker, cptr) ;
					psf_store_string (psf, SF_STR_COMMENT, cptr) ;
					break ;

			case APPL_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					dword += (dword & 1) ;
					if (dword == 0)
						break ;
					if (dword >= SIGNED_SIZEOF (psf->buffer))
					{	psf_log_printf (psf, " %M : %d (too big, skipping)\n", marker, dword) ;
						psf_binheader_readf (psf, "j", dword) ;
						break ;
						} ;

					cptr = (char*) psf->buffer ;
					psf_binheader_readf (psf, "b", cptr, dword) ;
					cptr [dword - 1] = 0 ;

					for (k = 0 ; k < dword ; k++)
						if (! isprint (cptr [k]))
						{	cptr [k] = 0 ;
							break ;
							} ;

					psf_log_printf (psf, " %M : %s\n", marker, cptr) ;
					psf_store_string (psf, SF_STR_SOFTWARE, cptr) ;
					break ;

			case NAME_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					dword += (dword & 1) ;
					if (dword == 0)
						break ;
					if (dword > SIGNED_SIZEOF (psf->buffer))
					{	psf_log_printf (psf, " %M : %d (too big)\n", marker, dword) ;
						return SFE_INTERNAL ;
						} ;

					cptr = (char*) psf->buffer ;
					psf_binheader_readf (psf, "b", cptr, dword) ;
					cptr [dword - 1] = 0 ;
					psf_log_printf (psf, " %M : %s\n", marker, cptr) ;
					psf_store_string (psf, SF_STR_TITLE, cptr) ;
					break ;

			case ANNO_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					dword += (dword & 1) ;
					if (dword == 0)
						break ;
					if (dword > SIGNED_SIZEOF (psf->buffer))
					{	psf_log_printf (psf, " %M : %d (too big)\n", marker, dword) ;
						return SFE_INTERNAL ;
						} ;

					cptr = (char*) psf->buffer ;
					psf_binheader_readf (psf, "b", cptr, dword) ;
					cptr [dword - 1] = 0 ;
					psf_log_printf (psf, " %M : %s\n", marker, cptr) ;
					break ;

			case INST_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					if (dword != SIZEOF_INST_CHUNK)
					{	psf_log_printf (psf, " %M : %d (should be %d)\n", marker, dword, SIZEOF_INST_CHUNK) ;
						psf_binheader_readf (psf, "j", dword) ;
						break ;
						} ;
					psf_log_printf (psf, " %M : %d\n", marker, dword) ;
					{	unsigned char bytes [6] ;
						short gain ;

						psf_binheader_readf (psf, "b", bytes, 6) ;
						psf_log_printf (psf, 	"  Base Note : %u\n  Detune    : %u\n"
												"  Low  Note : %u\n  High Note : %u\n"
												"  Low  Vel. : %u\n  High Vel. : %u\n",
								bytes [0], bytes [1], bytes [2], bytes [3], bytes [4], bytes [5]) ;

						psf_binheader_readf (psf, "E2", &gain) ;
						psf_log_printf (psf, 	"  Gain (dB) : %d\n", gain) ;

						} ;
					{	short	mode ; /* 0 - no loop, 1 - forward looping, 2 - backward looping */
						char	*loop_mode ;
						unsigned short begin, end ;

						psf_binheader_readf (psf, "E222", &mode, &begin, &end) ;
						loop_mode = get_loop_mode_str (mode) ;
						psf_log_printf (psf, 	"  Sustain\n   mode  : %d => %s\n   begin : %u\n   end   : %u\n",
								mode, loop_mode, begin, end) ;
						psf_binheader_readf (psf, "E222", &mode, &begin, &end) ;
						loop_mode = get_loop_mode_str (mode) ;
						psf_log_printf (psf, 	"  Release\n   mode  : %d => %s\n   begin : %u\n   end   : %u\n",
								mode, loop_mode, begin, end) ;
						} ;
					break ;

			case MARK_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					psf_log_printf (psf, " %M : %d\n", marker, dword) ;
					{	unsigned short	mark_count, mark_id ;
						unsigned char	pstr_len ;
						unsigned int	position ;

						bytesread = psf_binheader_readf (psf, "E2", &mark_count) ;
						psf_log_printf (psf, "  Count : %d\n", mark_count) ;

						while (mark_count && bytesread < dword)
						{	bytesread += psf_binheader_readf (psf, "E241", &mark_id, &position, &pstr_len) ;
							psf_log_printf (psf, "   Mark ID  : %u\n   Position : %u\n", mark_id, position) ;

							pstr_len += (pstr_len & 1) + 1 ; /* fudgy, fudgy, hack, hack */

							bytesread += psf_binheader_readf (psf, "b", psf->buffer, pstr_len) ;
							psf_log_printf (psf, "   Name     : %s\n", psf->buffer) ;

							mark_count -- ;
							} ;
						} ;
					psf_binheader_readf (psf, "j", dword - bytesread) ;
					break ;

			case FVER_MARKER :
			case SFX_MARKER :
					psf_binheader_readf (psf, "E4", &dword) ;
					psf_log_printf (psf, " %M : %d\n", marker, dword) ;

					psf_binheader_readf (psf, "j", dword) ;
					break ;

			case NONE_MARKER :
					/* Fix for broken AIFC files with incorrect COMM chunk length. */
					psf_binheader_readf (psf, "1", &byte) ;
					dword = byte ;
					psf_binheader_readf (psf, "j", dword) ;
					break ;

			default :
					if (isprint ((marker >> 24) & 0xFF) && isprint ((marker >> 16) & 0xFF)
						&& isprint ((marker >> 8) & 0xFF) && isprint (marker & 0xFF))
					{	psf_binheader_readf (psf, "E4", &dword) ;
						psf_log_printf (psf, "%M : %d (unknown marker)\n", marker, dword) ;

						psf_binheader_readf (psf, "j", dword) ;
						break ;
						} ;
					if ((dword = psf_ftell (psf)) & 0x03)
					{	psf_log_printf (psf, "  Unknown chunk marker %X at position %d. Resyncing.\n", marker, dword - 4) ;

						psf_binheader_readf (psf, "j", -3) ;
						break ;
						} ;
					psf_log_printf (psf, "*** Unknown chunk marker %X at position %D. Exiting parser.\n", marker, psf_ftell (psf)) ;
					done = 1 ;
					break ;
			} ;	/* switch (marker) */

		if ((! psf->sf.seekable) && (found_chunk & HAVE_SSND))
			break ;

		if (psf_ftell (psf) >= psf->filelength - (2 * sizeof (dword)))
			break ;

		if (psf->logindex >= sizeof (psf->logbuffer) - 2)
			return SFE_LOG_OVERRUN ;
		} ; /* while (1) */

	if (! (found_chunk & HAVE_FORM))
		return SFE_AIFF_NO_FORM ;

	if (! (found_chunk & HAVE_AIFF))
		return SFE_AIFF_COMM_NO_FORM ;

	if (! (found_chunk & HAVE_COMM))
		return SFE_AIFF_SSND_NO_COMM ;

	if (! psf->dataoffset)
		return SFE_AIFF_NO_DATA ;

	return 0 ;
} /* aiff_read_header */

static int
aiff_close	(SF_PRIVATE *psf)
{
	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	aiff_write_tailer (psf) ;

		aiff_write_header (psf, SF_TRUE) ;
		} ;

	return 0 ;
} /* aiff_close */

static int
aiff_read_comm_chunk (SF_PRIVATE *psf, COMM_CHUNK *comm_fmt)
{	int error = 0, bytesread, subformat ;

	bytesread = psf_binheader_readf (psf, "E4", &(comm_fmt->size)) ;

	/* The COMM chunk has an int aligned to an odd word boundary. Some
	** procesors are not able to deal with this (ie bus fault) so we have
	** to take special care.
	*/

	bytesread +=
	psf_binheader_readf (psf, "E242b", &(comm_fmt->numChannels), &(comm_fmt->numSampleFrames),
			&(comm_fmt->sampleSize), &(comm_fmt->sampleRate), SIGNED_SIZEOF (comm_fmt->sampleRate)) ;

	if (comm_fmt->size == SIZEOF_AIFF_COMM)
		comm_fmt->encoding = NONE_MARKER ;
	else if (comm_fmt->size == SIZEOF_AIFC_COMM_MIN)
		bytesread += psf_binheader_readf (psf, "Em", &(comm_fmt->encoding)) ;
	else if (comm_fmt->size >= SIZEOF_AIFC_COMM)
	{	unsigned char encoding_len ;

		bytesread += psf_binheader_readf (psf, "Em1", &(comm_fmt->encoding), &encoding_len) ;

		memset (psf->buffer, 0, comm_fmt->size) ;

		bytesread += psf_binheader_readf (psf, "b", psf->buffer,
							comm_fmt->size - SIZEOF_AIFC_COMM + 1) ;
		psf->buffer [encoding_len] = 0 ;
		} ;

	psf_log_printf (psf, " COMM : %d\n", comm_fmt->size) ;
	psf_log_printf (psf, "  Sample Rate : %d\n", tenbytefloat2int (comm_fmt->sampleRate)) ;
	psf_log_printf (psf, "  Frames      : %d\n", comm_fmt->numSampleFrames) ;
	psf_log_printf (psf, "  Channels    : %d\n", comm_fmt->numChannels) ;

	/*	Found some broken 'fl32' files with comm.samplesize == 16. Fix it here. */

	if ((comm_fmt->encoding == fl32_MARKER || comm_fmt->encoding == FL32_MARKER) && comm_fmt->sampleSize != 32)
	{	psf_log_printf (psf, "  Sample Size : %d (should be 32)\n", comm_fmt->sampleSize) ;
		comm_fmt->sampleSize = 32 ;
		}
	else if ((comm_fmt->encoding == fl64_MARKER || comm_fmt->encoding == FL64_MARKER) && comm_fmt->sampleSize != 64)
	{	psf_log_printf (psf, "  Sample Size : %d (should be 64)\n", comm_fmt->sampleSize) ;
		comm_fmt->sampleSize = 64 ;
		}
	else
		psf_log_printf (psf, "  Sample Size : %d\n", comm_fmt->sampleSize) ;

	subformat = s_bitwidth_to_subformat (comm_fmt->sampleSize) ;

	psf->endian = SF_ENDIAN_BIG ;

	switch (comm_fmt->encoding)
	{	case NONE_MARKER :
				psf->sf.format = (SF_FORMAT_AIFF | subformat) ;
				break ;

		case twos_MARKER :
		case in32_MARKER :
				psf->sf.format = (SF_ENDIAN_BIG | SF_FORMAT_AIFF | subformat) ;
				break ;

		case sowt_MARKER :
		case ni32_MARKER :
				psf->endian = SF_ENDIAN_LITTLE ;
				psf->sf.format = (SF_ENDIAN_LITTLE | SF_FORMAT_AIFF | subformat) ;
				break ;

		case fl32_MARKER :
		case FL32_MARKER :
				psf->sf.format = (SF_FORMAT_AIFF | SF_FORMAT_FLOAT) ;
				break ;

		case ulaw_MARKER :
		case ULAW_MARKER :
				psf->sf.format = (SF_FORMAT_AIFF | SF_FORMAT_ULAW) ;
				break ;

		case alaw_MARKER :
		case ALAW_MARKER :
				psf->sf.format = (SF_FORMAT_AIFF | SF_FORMAT_ALAW) ;
				break ;

		case fl64_MARKER :
		case FL64_MARKER :
				psf->sf.format = (SF_FORMAT_AIFF | SF_FORMAT_DOUBLE) ;
				break ;

		case raw_MARKER :
				psf->sf.format = (SF_FORMAT_AIFF | SF_FORMAT_PCM_U8) ;
				break ;

		case DWVW_MARKER :
				psf->sf.format = SF_FORMAT_AIFF ;
				switch (comm_fmt->sampleSize)
				{	case 12 :
						psf->sf.format |= SF_FORMAT_DWVW_12 ;
						break ;
					case 16 :
						psf->sf.format |= SF_FORMAT_DWVW_16 ;
						break ;
					case 24 :
						psf->sf.format |= SF_FORMAT_DWVW_24 ;
						break ;

					default :
						psf->sf.format |= SF_FORMAT_DWVW_N ;
						break ;
					} ;
				break ;

		case GSM_MARKER :
				psf->sf.format = SF_FORMAT_AIFF ;
				psf->sf.format = (SF_FORMAT_AIFF | SF_FORMAT_GSM610) ;
				break ;


		/*
		case ima4_MARKER :
				psf->endian = SF_ENDIAN_BIG ;
				psf->sf.format = (SF_FORMAT_AIFF | SF_FORMAT_IMA_ADPCM) ;
				break ;
		*/

		default :
			psf_log_printf (psf, "AIFC : Unimplemented format : %M\n", comm_fmt->encoding) ;
			error = SFE_UNIMPLEMENTED ;
		} ;

	if (! psf->buffer [0])
		psf_log_printf (psf, "  Encoding    : %M\n", comm_fmt->encoding) ;
	else
		psf_log_printf (psf, "  Encoding    : %M => %s\n", comm_fmt->encoding, (char*) psf->buffer) ;

	return error ;
} /* aiff_read_comm_chunk */

static int
aiff_write_header (SF_PRIVATE *psf, int calc_length)
{	sf_count_t		current ;
	unsigned char 	comm_sample_rate [10], comm_zero_bytes [2] = { 0, 0 } ;
	unsigned int 	comm_type, comm_size, comm_encoding ;
	int				k, endian ;
	short			bit_width ;

	current = psf_ftell (psf) ;

	if (calc_length)
	{	psf->filelength = psf_get_filelen (psf) ;

		psf->datalength = psf->filelength - psf->dataoffset ;
		if (psf->dataend)
			psf->datalength -= psf->filelength - psf->dataend ;

		if (psf->bytewidth > 0)
			psf->sf.frames = psf->datalength / (psf->bytewidth * psf->sf.channels) ;
		} ;

	endian = psf->sf.format & SF_FORMAT_ENDMASK ;
	if (CPU_IS_LITTLE_ENDIAN && endian == SF_ENDIAN_CPU)
		endian = SF_ENDIAN_LITTLE ;

	/* Standard value here. */
	bit_width = psf->bytewidth * 8 ;

	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_S8 :
		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_PCM_24 :
		case SF_FORMAT_PCM_32 :
				switch (endian)
				{	case SF_ENDIAN_BIG :
							psf->endian = SF_ENDIAN_BIG ;
							comm_type = AIFC_MARKER ;
							comm_size = SIZEOF_AIFC_COMM ;
							comm_encoding = twos_MARKER ;
							break ;

					case SF_ENDIAN_LITTLE :
							psf->endian = SF_ENDIAN_LITTLE ;
							comm_type = AIFC_MARKER ;
							comm_size = SIZEOF_AIFC_COMM ;
							comm_encoding = sowt_MARKER ;
							break ;

					default : /* SF_ENDIAN_FILE */
							psf->endian = SF_ENDIAN_BIG ;
							comm_type = AIFF_MARKER ;
							comm_size = SIZEOF_AIFF_COMM ;
							comm_encoding = 0 ;
							break ;
					} ;
				break ;

		case SF_FORMAT_FLOAT :					/* Big endian floating point. */
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = FL32_MARKER ; /* Use 'FL32' because its easier to read. */
				break ;

		case SF_FORMAT_DOUBLE :					/* Big endian double precision floating point. */
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = FL64_MARKER ; /* Use 'FL64' because its easier to read. */
				break ;

		case SF_FORMAT_ULAW :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = ulaw_MARKER ;
				break ;

		case SF_FORMAT_ALAW :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = alaw_MARKER ;
				break ;

		case SF_FORMAT_PCM_U8 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = raw_MARKER ;
				break ;

		case SF_FORMAT_DWVW_12 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = DWVW_MARKER ;

				/* Override standard value here.*/
				bit_width = 12 ;
				break ;

		case SF_FORMAT_DWVW_16 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = DWVW_MARKER ;

				/* Override standard value here.*/
				bit_width = 16 ;
				break ;

		case SF_FORMAT_DWVW_24 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = DWVW_MARKER ;

				/* Override standard value here.*/
				bit_width = 24 ;
				break ;

		case SF_FORMAT_GSM610 :
				psf->endian = SF_ENDIAN_BIG ;
				comm_type = AIFC_MARKER ;
				comm_size = SIZEOF_AIFC_COMM ;
				comm_encoding = GSM_MARKER ;

				/* Override standard value here.*/
				bit_width = 16 ;
				break ;

		default : return SFE_BAD_OPEN_FORMAT ;
		} ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;
	psf_fseek (psf, 0, SEEK_SET) ;

	psf_binheader_writef (psf, "Etm8", FORM_MARKER, psf->filelength - 8) ;

	/* Write COMM chunk. */
	psf_binheader_writef (psf, "Emm4", comm_type, COMM_MARKER, comm_size) ;

	uint2tenbytefloat (psf->sf.samplerate, comm_sample_rate) ;

	psf_binheader_writef (psf, "Et282", psf->sf.channels, psf->sf.frames, bit_width) ;
	psf_binheader_writef (psf, "b", comm_sample_rate, sizeof (comm_sample_rate)) ;

	/* AIFC chunks have some extra data. */
	if (comm_type == AIFC_MARKER)
		psf_binheader_writef (psf, "mb", comm_encoding, comm_zero_bytes, sizeof (comm_zero_bytes)) ;

	if (psf->str_flags & SF_STR_LOCATE_START)
		aiff_write_strings (psf, SF_STR_LOCATE_START) ;

	if (psf->has_peak && psf->peak_loc == SF_PEAK_START)
	{	psf_binheader_writef (psf, "Em4", PEAK_MARKER,
			sizeof (psf->peak) - sizeof (psf->peak.peak) + psf->sf.channels * sizeof (PEAK_POS)) ;
		psf_binheader_writef (psf, "E44", 1, time (NULL)) ;
		for (k = 0 ; k < psf->sf.channels ; k++)
			psf_binheader_writef (psf, "Ef4", psf->peak.peak [k].value, psf->peak.peak [k].position) ; /* XXXXX */
		} ;

	/* Write SSND chunk. */
	psf_binheader_writef (psf, "Etm844", SSND_MARKER, psf->datalength + SIZEOF_SSND_CHUNK, 0, 0) ;

	/* Header construction complete so write it out. */
	psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	if (psf->error)
		return psf->error ;

	psf->dataoffset = psf->headindex ;

	if (current < psf->dataoffset)
		psf_fseek (psf, psf->dataoffset, SEEK_SET) ;
	else if (current > 0)
		psf_fseek (psf, current, SEEK_SET) ;


	return psf->error ;
} /* aiff_write_header */

static int
aiff_write_tailer (SF_PRIVATE *psf)
{	int		k ;

	/* Reset the current header length to zero. */
	psf->header [0] = 0 ;
	psf->headindex = 0 ;

	psf->dataend = psf_fseek (psf, 0, SEEK_END) ;

	if (psf->has_peak && psf->peak_loc == SF_PEAK_END)
	{	psf_binheader_writef (psf, "Em4", PEAK_MARKER,
			sizeof (psf->peak) - sizeof (psf->peak.peak) + psf->sf.channels * sizeof (PEAK_POS)) ;
		psf_binheader_writef (psf, "E44", 1, time (NULL)) ;
		for (k = 0 ; k < psf->sf.channels ; k++)
			psf_binheader_writef (psf, "Ef4", psf->peak.peak [k].value, psf->peak.peak [k].position) ; /* XXXXX */
		} ;

	if (psf->str_flags & SF_STR_LOCATE_END)
		aiff_write_strings (psf, SF_STR_LOCATE_END) ;

	/* Write the tailer. */
	if (psf->headindex)
		psf_fwrite (psf->header, psf->headindex, 1, psf) ;

	return 0 ;
} /* aiff_write_tailer */

static void
aiff_write_strings (SF_PRIVATE *psf, int location)
{	int	k ;

	for (k = 0 ; k < SF_MAX_STRINGS ; k++)
	{	if (psf->strings [k].type == 0)
			break ;

		if (psf->strings [k].flags != location)
			continue ;

		switch (psf->strings [k].type)
		{	case SF_STR_SOFTWARE :
				psf_binheader_writef (psf, "Ems", APPL_MARKER, psf->strings [k].str) ;
				break ;

			case SF_STR_TITLE :
				psf_binheader_writef (psf, "Ems", NAME_MARKER, psf->strings [k].str) ;
				break ;

			case SF_STR_COPYRIGHT :
				psf_binheader_writef (psf, "Ems", c_MARKER, psf->strings [k].str) ;
				break ;

			case SF_STR_ARTIST :
				psf_binheader_writef (psf, "Ems", AUTH_MARKER, psf->strings [k].str) ;
				break ;

			case SF_STR_COMMENT :
				psf_binheader_writef (psf, "Ems", COMT_MARKER, psf->strings [k].str) ;
				break ;

			/*
			case SF_STR_DATE :
				psf_binheader_writef (psf, "Ems", ICRD_MARKER, psf->strings [k].str) ;
				break ;
			*/
			} ;
		} ;

	return ;
} /* aiff_write_strings */

static int
aiff_command (SF_PRIVATE *psf, int command, void *data, int datasize)
{
	/* Avoid compiler warnings. */
	psf = psf ;
	data = data ;
	datasize = datasize ;

	switch (command)
	{	default : break ;
		} ;

	return 0 ;
} /* aiff_command */

static char*
get_loop_mode_str (short mode)
{	switch (mode)
	{	case 0 : return "none" ;
		case 1 : return "forawrd" ;
		case 2 : return "backward" ;
		} ;

	return "*** unknown" ;
} /* get_loop_mode_str */

/*==========================================================================================
**	Rough hack at converting from 80 bit IEEE float in AIFF header to an int and
**	back again. It assumes that all sample rates are between 1 and 800MHz, which
**	should be OK as other sound file formats use a 32 bit integer to store sample
**	rate.
**	There is another (probably better) version in the source code to the SoX but it
**	has a copyright which probably prevents it from being allowable as GPL/LGPL.
*/

static int
tenbytefloat2int (unsigned char *bytes)
{	int val = 3 ;

	if (bytes [0] & 0x80)	/* Negative number. */
		return 0 ;

	if (bytes [0] <= 0x3F)	/* Less than 1. */
		return 1 ;

	if (bytes [0] > 0x40)	/* Way too big. */
		return 0x4000000 ;

	if (bytes [0] == 0x40 && bytes [1] > 0x1C) /* Too big. */
		return 800000000 ;

	/* Ok, can handle it. */

	val = (bytes [2] << 23) | (bytes [3] << 15) | (bytes [4] << 7) | (bytes [5] >> 1) ;

	val >>= (29 - bytes [1]) ;

	return val ;
} /* tenbytefloat2int */

static void
uint2tenbytefloat (unsigned int num, unsigned char *bytes)
{	unsigned int mask = 0x40000000 ;
	int		count ;


	memset (bytes, 0, 10) ;

	if (num <= 1)
	{	bytes [0] = 0x3F ;
		bytes [1] = 0xFF ;
		bytes [2] = 0x80 ;
		return ;
		} ;

	bytes [0] = 0x40 ;

	if (num >= mask)
	{	bytes [1] = 0x1D ;
		return ;
		} ;

	for (count = 0 ; count <= 32 ; count ++)
	{	if (num & mask)
			break ;
		mask >>= 1 ;
		} ;

	num <<= count + 1 ;
	bytes [1] = 29 - count ;
	bytes [2] = (num >> 24) & 0xFF ;
	bytes [3] = (num >> 16) & 0xFF ;
	bytes [4] = (num >> 8) & 0xFF ;
	bytes [5] = num & 0xFF ;

} /* uint2tenbytefloat */

