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

#include	<string.h>
#include	<ctype.h>

#include	"sndfile.h"
#include	"config.h"
#include	"sfendian.h"
#include	"common.h"


#define		SNDFILE_MAGICK	0x1234C0DE

typedef struct
{	int 	error ;
	char	*str ;
} ErrorStruct ;

static
ErrorStruct SndfileErrors [] =
{
	/* Public error values and their associated strings. */
	{	SF_ERR_NO_ERROR				, "No Error." },
	{	SF_ERR_UNRECOGNISED_FORMAT	, "File opened for read. Format not recognised." },
	{	SF_ERR_SYSTEM				, "System error." /* Often replaced. */ 	},

	/* Private error values and their associated strings. */
	{	SFE_BAD_FILE			, "File does not exist or is not a regular file (possibly a pipe?)." },
	{	SFE_BAD_FILE_READ		, "File exists but no data could be read." },
	{	SFE_OPEN_FAILED			, "Could not open file." },
	{	SFE_BAD_SNDFILE_PTR		, "Not a valid SNDFILE* pointer." },
	{	SFE_BAD_SF_INFO_PTR		, "NULL SF_INFO pointer passed to libsndfile." },
	{	SFE_BAD_SF_INCOMPLETE	, "SF_PRIVATE struct incomplete and end of header parsing." },
	{	SFE_BAD_FILE_PTR		, "Bad FILE pointer." },
	{	SFE_BAD_INT_PTR			, "Internal error, Bad pointer." },
	{	SFE_MALLOC_FAILED		, "Internal malloc () failed." },
	{	SFE_UNIMPLEMENTED		, "File contains data in an unimplemented format." },
	{	SFE_BAD_READ_ALIGN  	, "Attempt to read a non-integer number of channels." },
	{	SFE_BAD_WRITE_ALIGN 	, "Attempt to write a non-integer number of channels." },
	{	SFE_UNKNOWN_FORMAT		, "File contains data in an unknown format." },
	{	SFE_NOT_READMODE		, "Read attempted on file currently open for write." },
	{	SFE_NOT_WRITEMODE		, "Write attempted on file currently open for read." },
	{	SFE_BAD_MODE_RW			, "This file format does not support read/write mode." },
	{	SFE_BAD_SF_INFO			, "Internal error : SF_INFO struct incomplete." },
	{	SFE_BAD_OFFSET			, "Error : supplied offset beyond end of file." },
	{	SFE_NO_EMBED_SUPPORT	, "Error : embedding not supported for this file format." },
	{	SFE_NO_EMBEDDED_RDWR	, "Error : cannot open embedded file read/write." },
	{	SFE_BAD_RDWR_FORMAT		, "Attempted to open read only format for RDWR." },

	{	SFE_INTERLEAVE_MODE		, "Attempt to write to file with non-interleaved data." },
	{	SFE_INTERLEAVE_SEEK		, "Bad karma in seek during interleave read operation." },
	{	SFE_INTERLEAVE_READ		, "Bad karma in read during interleave read operation." },

	{	SFE_INTERNAL			, "Unspecified internal error." },
	{	SFE_LOG_OVERRUN			, "Log buffer has overrun. File probably broken." },
	{	SFE_BAD_CONTROL_CMD		, "Bad command passed to function sf_command()." },
	{	SFE_BAD_ENDIAN			, "Bad endian-ness. Try default endian-ness" },
	{	SFE_CHANNEL_COUNT		, "Too many channels specified." },

	{	SFE_BAD_SEEK			, "Internal psf_fseek() failed." },
	{	SFE_NOT_SEEKABLE		, "Seek attempted on unseekable file type." },
	{	SFE_AMBIGUOUS_SEEK		, "Error: combination of file open mode and seek command is ambiguous." },
	{	SFE_WRONG_SEEK			, "Error: invalid seek parameters." },
	{	SFE_SEEK_FAILED			, "Error: parameters OK, but psf_seek() failed." },

	{	SFE_BAD_OPEN_MODE		, "Error: bad mode parameter for file open." },
	{	SFE_OPEN_PIPE_RDWR		, "Error: attempt toopen a pipe in read/write mode." },
	{	SFE_RDWR_POSITION		, "Error on RDWR position (cryptic)." },

	{	SFE_STR_NO_SUPPORT		, "Error : File type does not support string data." },	
	{	SFE_STR_MAX_DATA		, "Error : Maximum string data storage reached." },	
	{	SFE_STR_MAX_COUNT		, "Error : Maximum string data count reached." },	
	{	SFE_STR_BAD_TYPE		, "Error : Bad string data type." },	
	{	SFE_STR_NO_ADD_END		, "Error : file type does not support strings added at end of file." },	
	{	SFE_STR_WEIRD			, "Error : Weird string error." },	

	{	SFE_WAV_NO_RIFF			, "Error in WAV file. No 'RIFF' chunk marker." },
	{	SFE_WAV_NO_WAVE			, "Error in WAV file. No 'WAVE' chunk marker." },
	{	SFE_WAV_NO_FMT			, "Error in WAV file. No 'fmt ' chunk marker." },
	{	SFE_WAV_FMT_SHORT		, "Error in WAV file. Short 'fmt ' chunk." },

	{	SFE_WAV_FMT_TOO_BIG		, "Error in WAV file. 'fmt ' chunk too large." },
	{	SFE_WAV_BAD_FACT		, "Error in WAV file. 'fact' chunk out of place." },
	{	SFE_WAV_BAD_PEAK		, "Error in WAV file. Bad 'PEAK' chunk." },
	{	SFE_WAV_PEAK_B4_FMT		, "Error in WAV file. 'PEAK' chunk found before 'fmt ' chunk." },

	{	SFE_WAV_BAD_FORMAT		, "Error in WAV file. Errors in 'fmt ' chunk." },
	{	SFE_WAV_BAD_BLOCKALIGN	, "Error in WAV file. Block alignment in 'fmt ' chunk is incorrect." },
	{	SFE_WAV_NO_DATA			, "Error in WAV file. No 'data' chunk marker." },
	{	SFE_WAV_UNKNOWN_CHUNK	, "Error in WAV file. File contains an unknown chunk marker." },
	{	SFE_WAV_WVPK_DATA		, "Error in WAV file. Data is in WAVPACK format." },

	{	SFE_WAV_ADPCM_NOT4BIT	, "Error in ADPCM WAV file. Invalid bit width." },
	{	SFE_WAV_ADPCM_CHANNELS	, "Error in ADPCM WAV file. Invalid number of channels." },
	{	SFE_WAV_GSM610_FORMAT	, "Error in GSM610 WAV file. Invalid format chunk." },

	{	SFE_AIFF_NO_FORM		, "Error in AIFF file, bad 'FORM' marker." },
	{	SFE_AIFF_AIFF_NO_FORM	, "Error in AIFF file, 'AIFF' marker without 'FORM'." },
	{	SFE_AIFF_COMM_NO_FORM	, "Error in AIFF file, 'COMM' marker without 'FORM'." },
	{	SFE_AIFF_SSND_NO_COMM	, "Error in AIFF file, 'SSND' marker without 'COMM'." },
	{	SFE_AIFF_UNKNOWN_CHUNK	, "Error in AIFF file, unknown chunk." },
	{	SFE_AIFF_COMM_CHUNK_SIZE, "Error in AIFF file, bad 'COMM' chunk size." },
	{	SFE_AIFF_BAD_COMM_CHUNK , "Error in AIFF file, bad 'COMM' chunk." },
	{	SFE_AIFF_PEAK_B4_COMM	, "Error in AIFF file. 'PEAK' chunk found before 'COMM' chunk." },
	{	SFE_AIFF_BAD_PEAK		, "Error in AIFF file. Bad 'PEAK' chunk." },
	{	SFE_AIFF_NO_SSND		, "Error in AIFF file, bad 'SSND' chunk." },
	{	SFE_AIFF_NO_DATA		, "Error in AIFF file, no sound data." },

	{	SFE_AU_UNKNOWN_FORMAT	, "Error in AU file, unknown format." },
	{	SFE_AU_NO_DOTSND		, "Error in AU file, missing '.snd' or 'dns.' marker." },
	{	SFE_AU_EMBED_BAD_LEN	, "Embedded AU file with unknown length." },

	{	SFE_RAW_READ_BAD_SPEC	, "Error while opening RAW file for read. Must specify format and channels.\n"
									"Possibly trying to open unsupported format."
									 },
	{	SFE_RAW_BAD_BITWIDTH	, "Error. RAW file bitwidth must be a multiple of 8." },
	{	SFE_RAW_BAD_FORMAT		, "Error. Bad format field in SF_INFO struct when openning a RAW file for read." },

	{	SFE_PAF_NO_MARKER		, "Error in PAF file, no marker." },
	{	SFE_PAF_VERSION			, "Error in PAF file, bad version." },
	{	SFE_PAF_UNKNOWN_FORMAT	, "Error in PAF file, unknown format." },
	{	SFE_PAF_SHORT_HEADER	, "Error in PAF file. File shorter than minimal header." },

	{	SFE_SVX_NO_FORM			, "Error in 8SVX / 16SV file, no 'FORM' marker." },
	{	SFE_SVX_NO_BODY			, "Error in 8SVX / 16SV file, no 'BODY' marker." },
	{	SFE_SVX_NO_DATA			, "Error in 8SVX / 16SV file, no sound data." },
	{	SFE_SVX_BAD_COMP		, "Error in 8SVX / 16SV file, unsupported compression format." },
	{	SFE_SVX_BAD_NAME_LENGTH	, "Error in 8SVX / 16SV file, NAME chunk too long." },

	{	SFE_NIST_BAD_HEADER		, "Error in NIST file, bad header." },
	{	SFE_NIST_CRLF_CONVERISON, "Error : NIST file damaged by Windows CR -> CRLF conversion process."	},
	{	SFE_NIST_BAD_ENCODING	, "Error in NIST file, unsupported compression format." },

	{	SFE_VOC_NO_CREATIVE		, "Error in VOC file, no 'Creative Voice File' marker." },
	{	SFE_VOC_BAD_FORMAT		, "Error in VOC file, bad format." },
	{	SFE_VOC_BAD_VERSION		, "Error in VOC file, bad version number." },
	{	SFE_VOC_BAD_MARKER		, "Error in VOC file, bad marker in file." },
	{	SFE_VOC_BAD_SECTIONS	, "Error in VOC file, incompatible VOC sections." },
	{	SFE_VOC_MULTI_SAMPLERATE, "Error in VOC file, more than one sample rate defined." },
	{	SFE_VOC_MULTI_SECTION	, "Unimplemented VOC file feature, file contains multiple sound sections." },
	{	SFE_VOC_MULTI_PARAM		, "Error in VOC file, file contains multiple bit or channel widths." },
	{	SFE_VOC_SECTION_COUNT	, "Error in VOC file, too many sections." },

	{	SFE_IRCAM_NO_MARKER		, "Error in IRCAM file, bad IRCAM marker." },
	{	SFE_IRCAM_BAD_CHANNELS	, "Error in IRCAM file, bad channel count." },
	{	SFE_IRCAM_UNKNOWN_FORMAT, "Error in IRCAM file, unknow encoding format." },

	{	SFE_W64_64_BIT			, "Error in W64 file, file contains 64 bit offset." },

	{	SFE_W64_NO_RIFF			, "Error in W64 file. No 'riff' chunk marker." },
	{	SFE_W64_NO_WAVE			, "Error in W64 file. No 'wave' chunk marker." },
	{	SFE_W64_NO_FMT			, "Error in W64 file. No 'fmt ' chunk marker." },
	{	SFE_W64_NO_DATA			, "Error in W64 file. No 'data' chunk marker." },

	{	SFE_W64_FMT_SHORT		, "Error in W64 file. Short 'fmt ' chunk." },
	{	SFE_W64_FMT_TOO_BIG		, "Error in W64 file. 'fmt ' chunk too large." },

	{	SFE_W64_ADPCM_NOT4BIT	, "Error in ADPCM W64 file. Invalid bit width." },
	{	SFE_W64_ADPCM_CHANNELS	, "Error in ADPCM W64 file. Invalid number of channels." },
	{	SFE_W64_GSM610_FORMAT	, "Error in GSM610 W64 file. Invalid format chunk." },

	{	SFE_MAT4_BAD_NAME		, "Error in MAT4 file. No variable name." },
	{	SFE_MAT4_NO_SAMPLERATE	, "Error in MAT4 file. No sample rate." },
	{	SFE_MAT4_ZERO_CHANNELS	, "Error in MAT4 file. Channel count is zero." },

	{	SFE_MAT5_BAD_ENDIAN		, "Error in MAT5 file. Not able to determine endian-ness." },
	{	SFE_MAT5_NO_BLOCK		, "Error in MAT5 file. Bad block structure." },
	{	SFE_MAT5_SAMPLE_RATE	, "Error in MAT5 file. Not able to determine sample rate." },
	{	SFE_MAT5_ZERO_CHANNELS	, "Error in MAT5 file. Channel count is zero." },

	{	SFE_PVF_NO_PVF1			, "Error in PVF file. No PVF1 marker." },
	{	SFE_PVF_BAD_HEADER		, "Error in PVF file. Bad header." },
	{	SFE_PVF_BAD_BITWIDTH	, "Error in PVF file. Bad bit width." },

	{	SFE_XI_BAD_HEADER		, "Error in XI file. Bad header." },
	{	SFE_XI_EXCESS_SAMPLES	, "Error in XI file. Excess samples in file." },
	{	SFE_XI_WHOOPS			, "Error. Whoops." },
	
	{	SFE_DWVW_BAD_BITWIDTH	, "Error : Bad bit width for DWVW encoding. Must be 12, 16 or 24." },
	{	SFE_G72X_NOT_MONO		, "Error : G72x encoding does not support more than 1 channel." },

	{	SFE_MAX_ERROR			, "Maximum error number." },
	{	SFE_MAX_ERROR + 1		, NULL }
} ;

/*------------------------------------------------------------------------------
*/

static int 	format_from_extension (const char *filename) ;
static int	guess_file_type (SF_PRIVATE *psf, const char *filename) ;
static int	validate_sfinfo (SF_INFO *sfinfo) ;
static int	validate_psf (SF_PRIVATE *psf) ;
static void	save_header_info (SF_PRIVATE *psf) ;
static void	copy_filename (SF_PRIVATE *psf, const char *path) ;
static int	psf_close (SF_PRIVATE  *psf) ;
static int	psf_open_file (SF_PRIVATE *psf, int mode, SF_INFO *sfinfo) ;

/*------------------------------------------------------------------------------
** Private (static) variables.
*/

static int	sf_errno = 0 ;
static char	sf_logbuffer [SF_BUFFER_LEN] = { 0 } ;
static char	sf_syserr [SF_SYSERR_LEN] = { 0 } ;

/*------------------------------------------------------------------------------
*/

#define	VALIDATE_SNDFILE_AND_ASSIGN_PSF(a,b,c)		\
		{	if (! (a))								\
				return SFE_BAD_SNDFILE_PTR ;		\
			(b) = (SF_PRIVATE*) (a) ;				\
			if (((b)->filedes) < 0)					\
				return SFE_BAD_FILE_PTR ;			\
			if ((b)->Magick != SNDFILE_MAGICK)		\
				return	SFE_BAD_SNDFILE_PTR ;		\
			if (c) (b)->error = 0 ;					\
			}

/*------------------------------------------------------------------------------
**	Public functions.
*/

SNDFILE*
sf_open	(const char *path, int mode, SF_INFO *sfinfo)
{	SF_PRIVATE 	*psf ;
	int			error = 0 ;

	if ((psf = calloc (1, sizeof (SF_PRIVATE))) == NULL)
	{	sf_errno = SFE_MALLOC_FAILED ;
		return	NULL ;
		} ;

	memset (psf, 0, sizeof (SF_PRIVATE)) ;

	psf_log_printf (psf, "File : %s\n", path) ;
	copy_filename (psf, path) ;

	if (strcmp (path, "-") == 0)
	{	/* File is a either stdin or stdout. */
		psf->sf.seekable = SF_FALSE ;
		switch (mode)
		{	case SFM_RDWR :
					error = SFE_OPEN_PIPE_RDWR ;
					break ;

			case SFM_READ :
					psf->filedes = 0 ;
					break ;

			case SFM_WRITE :
					psf->filedes = 1 ;
					break ;

			default :
					error = SFE_BAD_OPEN_MODE ;
					break ;
			} ;
		psf->filelength = 0 ;
		}
	else if ((psf->filedes = psf_open (psf, path, mode)) < 0)
		error = SFE_OPEN_FAILED ;

	if (error == 0)
		error = psf_open_file (psf, mode, sfinfo) ;
	
	if (error)
	{	sf_errno = error ;
		if (error == SFE_SYSTEM)
			LSF_SNPRINTF (sf_syserr, sizeof (sf_syserr), "%s", psf->syserr) ;
		LSF_SNPRINTF (sf_logbuffer, sizeof (sf_logbuffer), "%s", psf->logbuffer) ;
		psf_close (psf) ;
		return NULL ;
		} ;

	memcpy (sfinfo, &(psf->sf), sizeof (SF_INFO)) ;

	return (SNDFILE*) psf ;
} /* sf_open */

SNDFILE*
sf_open_fd	(int fd, int mode, SF_INFO *sfinfo, int close_desc)
{	SF_PRIVATE 	*psf ;
	int			error ;
	
	if ((psf = calloc (1, sizeof (SF_PRIVATE))) == NULL)
	{	sf_errno = SFE_MALLOC_FAILED ;
		return	NULL ;
		} ;

	psf->filedes = fd ;
	psf->fileoffset = psf_ftell (psf) ;
	
	if (! close_desc)
		psf->do_not_close_descritor = SF_TRUE ;

	error = psf_open_file (psf, mode, sfinfo) ;
	
	if (error)
	{	sf_errno = error ;
		if (error == SFE_SYSTEM)
			LSF_SNPRINTF (sf_syserr, sizeof (sf_syserr), "%s", psf->syserr) ;
		LSF_SNPRINTF (sf_logbuffer, sizeof (sf_logbuffer), "%s", psf->logbuffer) ;
		psf_close (psf) ;
		return NULL ;
		} ;

	memcpy (sfinfo, &(psf->sf), sizeof (SF_INFO)) ;

	return (SNDFILE*) psf ;
} /* sf_open_fd */

/*------------------------------------------------------------------------------
*/

int
sf_close	(SNDFILE *sndfile)
{	SF_PRIVATE  *psf ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	return psf_close (psf) ;
} /* sf_close */

/*==============================================================================
*/

const char*
sf_error_number	(int errnum)
{	int	k ;

	if (errnum < 0 || errnum >= SFE_MAX_ERROR)
		return SndfileErrors [0].str ;

	for (k = 0 ; SndfileErrors[k].str ; k++)
		if (errnum == SndfileErrors[k].error)
			return SndfileErrors[k].str ;

	return "No error defined for this error number. This is a bug in libsndfile." ;
} /* sf_error_number */

const char*
sf_strerror (SNDFILE *sndfile)
{	SF_PRIVATE 	*psf = NULL ;
	int errnum ;

	if (! sndfile)
	{	errnum = sf_errno ;
	    if (errnum == SFE_SYSTEM && sf_syserr [0])
			return sf_syserr ;
		}
	else
	{	psf = (SF_PRIVATE *) sndfile ;

		if (psf->Magick != SNDFILE_MAGICK)
			return	"sf_strerror : Bad magic number." ;

		errnum = psf->error ;

	    if (errnum == SFE_SYSTEM && psf->syserr [0])
			return psf->syserr ;
		} ;

	return sf_error_number (errnum) ;
} /* sf_strerror */

/*------------------------------------------------------------------------------
*/

int
sf_error (SNDFILE *sndfile)
{	SF_PRIVATE	*psf ;

	if (! sndfile)
	{	if (sf_error != 0)
			return 1 ;
		return 0 ;
		} ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 0) ;

	if (psf->error)
		return 1 ;

	return 0 ;
} /* sf_error */

/*------------------------------------------------------------------------------
*/

int
sf_perror (SNDFILE *sndfile)
{	SF_PRIVATE 	*psf ;
	int 		errnum ;

	if (! sndfile)
	{	errnum = sf_errno ;
		}
	else
	{	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 0) ;
		errnum = psf->error ;
		} ;

	fprintf (stderr, "%s\n", sf_error_number (errnum)) ;
	return SFE_NO_ERROR ;
} /* sf_perror */


/*------------------------------------------------------------------------------
*/

int
sf_error_str (SNDFILE *sndfile, char *str, size_t maxlen)
{	SF_PRIVATE 	*psf ;
	int 		errnum ;

	if (! str)
		return SFE_INTERNAL ;

	if (! sndfile)
		errnum = sf_errno ;
	else
	{	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 0) ;
		errnum = psf->error ;
		} ;

	LSF_SNPRINTF (str, maxlen, "%s", sf_error_number (errnum)) ;

	return SFE_NO_ERROR ;
} /* sf_error_str */

/*==============================================================================
*/

int
sf_format_check	(const SF_INFO *info)
{	int	subformat, endian ;

	subformat = info->format & SF_FORMAT_SUBMASK ;
	endian = info->format & SF_FORMAT_SUBMASK ;

	/* This is the place where each file format can check if the suppiled
	** SF_INFO struct is valid.
	** Return 0 on failure, 1 ons success.
	*/

	if (info->channels < 1 || info->channels > 256)
		return 0 ;

	if (info->samplerate < 0)
		return 0 ;

	switch (info->format & SF_FORMAT_TYPEMASK)
	{	case SF_FORMAT_WAV :
				/* WAV is strictly little endian. */
				if (endian == SF_ENDIAN_BIG || endian == SF_ENDIAN_CPU)
					return 0 ;
				if (subformat == SF_FORMAT_PCM_U8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				if (subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if ((subformat == SF_FORMAT_IMA_ADPCM || subformat == SF_FORMAT_MS_ADPCM) && info->channels <= 2)
					return 1 ;
				if (subformat == SF_FORMAT_GSM610 && info->channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_ULAW || subformat == SF_FORMAT_ALAW)
					return 1 ;
				if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
					return 1 ;
				break ;

		case SF_FORMAT_AIFF :
				/* AIFF does allow both endian-nesses for PCM data.*/
				if (subformat == SF_FORMAT_PCM_16 || subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				/* Other encodings. Check for endian-ness. */
				if (endian == SF_ENDIAN_LITTLE || endian == SF_ENDIAN_CPU)
					return 0 ;
				if (subformat == SF_FORMAT_PCM_U8 || subformat == SF_FORMAT_PCM_S8)
					return 1 ;
				if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
					return 1 ;
				if (subformat == SF_FORMAT_ULAW || subformat == SF_FORMAT_ALAW)
					return 1 ;
				if ((subformat == SF_FORMAT_DWVW_12 || subformat == SF_FORMAT_DWVW_16 ||
							subformat == SF_FORMAT_DWVW_24) && info-> channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_GSM610 && info->channels == 1)
					return 1 ;
				break ;

		case SF_FORMAT_AU :
				if (subformat == SF_FORMAT_PCM_S8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				if (subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if (subformat == SF_FORMAT_ULAW || subformat == SF_FORMAT_ALAW)
					return 1 ;
				if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
					return 1 ;
				if (subformat == SF_FORMAT_G721_32 && info->channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_G723_24 && info->channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_G723_40 && info->channels == 1)
					return 1 ;
				break ;

		case SF_FORMAT_RAW :
				if (subformat == SF_FORMAT_PCM_U8 || subformat == SF_FORMAT_PCM_S8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				if (subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
					return 1 ;
				if (subformat == SF_FORMAT_ALAW || subformat == SF_FORMAT_ULAW)
					return 1 ;
				if ((subformat == SF_FORMAT_DWVW_12 || subformat == SF_FORMAT_DWVW_16 ||
							subformat == SF_FORMAT_DWVW_24) && info-> channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_GSM610 && info->channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_VOX_ADPCM && info->channels == 1)
					return 1 ;
				break ;

		case SF_FORMAT_PAF :
				if (subformat == SF_FORMAT_PCM_S8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				if (subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				break ;

		case SF_FORMAT_SVX :
				/* SVX currently does not support more than one channel for write.
				** Read will allow more than one channel but only allow one here.
				*/
				if (info->channels != 1)
					return 0 ;
				/* Always big endian. */
				if (endian == SF_ENDIAN_LITTLE || endian == SF_ENDIAN_CPU)
					return 0 ;

				if ((subformat == SF_FORMAT_PCM_S8 || subformat == SF_FORMAT_PCM_16) && info->channels == 1)
					return 1 ;
				break ;

		case SF_FORMAT_NIST :
				if (subformat == SF_FORMAT_PCM_S8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				if (subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if (subformat == SF_FORMAT_ULAW || subformat == SF_FORMAT_ALAW)
					return 1 ;
				break ;

		case SF_FORMAT_IRCAM :
				if (subformat == SF_FORMAT_PCM_16 || subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if (subformat == SF_FORMAT_ULAW || subformat == SF_FORMAT_ALAW || subformat == SF_FORMAT_FLOAT)
					return 1 ;
				break ;

		case SF_FORMAT_VOC :
				/* VOC is strictly little endian. */
				if (endian == SF_ENDIAN_BIG || endian == SF_ENDIAN_CPU)
					return 0 ;
				if (subformat == SF_FORMAT_PCM_U8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				if (subformat == SF_FORMAT_ULAW || subformat == SF_FORMAT_ALAW)
					return 1 ;
				break ;

		case SF_FORMAT_W64 :
				/* W64 is strictly little endian. */
				if (endian == SF_ENDIAN_BIG || endian == SF_ENDIAN_CPU)
					return 0 ;
				if (subformat == SF_FORMAT_PCM_U8 || subformat == SF_FORMAT_PCM_16)
					return 1 ;
				if (subformat == SF_FORMAT_PCM_24 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if ((subformat == SF_FORMAT_IMA_ADPCM || subformat == SF_FORMAT_MS_ADPCM) && info->channels <= 2)
					return 1 ;
				if (subformat == SF_FORMAT_GSM610 && info->channels == 1)
					return 1 ;
				if (subformat == SF_FORMAT_ULAW || subformat == SF_FORMAT_ALAW)
					return 1 ;
				if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
					return 1 ;
				break ;

		case SF_FORMAT_MAT4 :
				if (subformat == SF_FORMAT_PCM_16 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
					return 1 ;
				break ;

		case SF_FORMAT_MAT5 :
				if (subformat == SF_FORMAT_PCM_U8 || subformat == SF_FORMAT_PCM_16 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
					return 1 ;
				break ;

		case SF_FORMAT_PVF :
				if (subformat == SF_FORMAT_PCM_S8 || subformat == SF_FORMAT_PCM_16 || subformat == SF_FORMAT_PCM_32)
					return 1 ;
				break ;

		case SF_FORMAT_XI :
				if (info->channels != 1)
					return 0 ;
				if (subformat == SF_FORMAT_DPCM_8 || subformat == SF_FORMAT_DPCM_16)
					return 1 ;
				break ;

		case SF_FORMAT_HTK :
				if (info->channels != 1)
					return 0 ;
				if (subformat == SF_FORMAT_PCM_16)
					return 1 ;
				break ;

		/*-
		case SF_FORMAT_SD2 :
				/+* SD2 is strictly big endian. *+/
				if (endian == SF_ENDIAN_LITTLE || endian == SF_ENDIAN_CPU)
					return 0 ;
				if (subformat == SF_FORMAT_PCM_16)
					return 1 ;
				break ;
		-*/

		default : break ;
		} ;

	return 0 ;
} /* sf_format_check */

/*------------------------------------------------------------------------------
*/

int
sf_command	(SNDFILE *sndfile, int command, void *data, int datasize)
{	SF_PRIVATE 		*psf = NULL ;

	/* This set of commands do not need the sndfile parameter. */
	switch (command)
	{	case  SFC_GET_LIB_VERSION :
		if (data == NULL)
			return (psf->error = SFE_BAD_CONTROL_CMD) ;
		if (ENABLE_EXPERIMENTAL_CODE)
			LSF_SNPRINTF (data, datasize, "%s-%s-exp", PACKAGE, VERSION) ;
		else
			LSF_SNPRINTF (data, datasize, "%s-%s", PACKAGE, VERSION) ;
		return 0 ;

		case SFC_GET_SIMPLE_FORMAT_COUNT :
			if (data == NULL || datasize != SIGNED_SIZEOF (int))
				return (sf_errno = SFE_BAD_CONTROL_CMD) ;
			*((int*) data) = psf_get_format_simple_count () ;
			return 0 ;

		case SFC_GET_SIMPLE_FORMAT :
			if (data == NULL || datasize != SIGNED_SIZEOF (SF_FORMAT_INFO))
				return (sf_errno = SFE_BAD_CONTROL_CMD) ;
			return psf_get_format_simple (data) ;

		case SFC_GET_FORMAT_MAJOR_COUNT :
			if (data == NULL || datasize != SIGNED_SIZEOF (int))
				return (sf_errno = SFE_BAD_CONTROL_CMD) ;
			*((int*) data) = psf_get_format_major_count () ;
			return 0 ;

		case SFC_GET_FORMAT_MAJOR :
			if (data == NULL || datasize != SIGNED_SIZEOF (SF_FORMAT_INFO))
				return (sf_errno = SFE_BAD_CONTROL_CMD) ;
			return psf_get_format_major (data) ;

		case SFC_GET_FORMAT_SUBTYPE_COUNT :
			if (data == NULL || datasize != SIGNED_SIZEOF (int))
				return (sf_errno = SFE_BAD_CONTROL_CMD) ;
			*((int*) data) = psf_get_format_subtype_count () ;
			return 0 ;

		case SFC_GET_FORMAT_SUBTYPE :
			if (data == NULL || datasize != SIGNED_SIZEOF (SF_FORMAT_INFO))
				return (sf_errno = SFE_BAD_CONTROL_CMD) ;
			return psf_get_format_subtype (data) ;

		case SFC_GET_FORMAT_INFO :
			if (data == NULL || datasize != SIGNED_SIZEOF (SF_FORMAT_INFO))
				return (sf_errno = SFE_BAD_CONTROL_CMD) ;
			return psf_get_format_info (data) ;
		} ;

	if (! sndfile && command == SFC_GET_LOG_INFO)
	{	if (data == NULL)
			return (psf->error = SFE_BAD_CONTROL_CMD) ;
		LSF_SNPRINTF (data, datasize, "%s", sf_logbuffer) ;
		return 0 ;
		} ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	switch (command)
	{	case SFC_SET_NORM_FLOAT :
			psf->norm_float = (datasize) ? SF_TRUE : SF_FALSE ;
			return psf->norm_float ;

		case SFC_SET_NORM_DOUBLE :
			psf->norm_double = (datasize) ? SF_TRUE : SF_FALSE ;
			return psf->norm_double ;

		case SFC_GET_NORM_FLOAT :
			return psf->norm_float ;

		case SFC_GET_NORM_DOUBLE :
			return psf->norm_double ;

		case SFC_SET_ADD_PEAK_CHUNK :
			{	int format = psf->sf.format & SF_FORMAT_TYPEMASK ;

				/* Only WAV and AIFF support the PEAK chunk. */
				if (format != SF_FORMAT_WAV && format != SF_FORMAT_AIFF)
					return SF_FALSE ;

				format = psf->sf.format & SF_FORMAT_SUBMASK ;

				/* Only files containg the following data types support the PEAK chunk. */
				if (format != SF_FORMAT_FLOAT && format != SF_FORMAT_DOUBLE)
					return SF_FALSE ;

				} ;
			/* Can only do this is in SFM_WRITE mode. */
			if (psf->mode != SFM_WRITE)
				return SF_FALSE ;
			/* If data has already been written this must fail. */
			if (psf->have_written)
				return psf->has_peak ;
			/* Everything seems OK, so set psf->has_peak and re-write header. */
			psf->has_peak = (datasize) ? SF_TRUE : SF_FALSE ;
			if (psf->write_header)
				psf->write_header (psf, SF_TRUE) ;
			return psf->has_peak ;

		case SFC_GET_LOG_INFO :
			if (data == NULL)
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			LSF_SNPRINTF (data, datasize, "%s", psf->logbuffer) ;
			break ;

		case SFC_CALC_SIGNAL_MAX :
			if (data == NULL || datasize != sizeof (double))
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			*((double*) data) = psf_calc_signal_max (psf, SF_FALSE) ;
			break ;

		case SFC_CALC_NORM_SIGNAL_MAX :
			if (data == NULL || datasize != sizeof (double))
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			*((double*) data) = psf_calc_signal_max (psf, SF_TRUE) ;
			break ;

		case SFC_CALC_MAX_ALL_CHANNELS :
			if (data == NULL || datasize != SIGNED_SIZEOF (double) * psf->sf.channels)
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			return psf_calc_max_all_channels (psf, (double *)data, SF_FALSE) ;

		case SFC_CALC_NORM_MAX_ALL_CHANNELS :
			if (data == NULL || datasize != SIGNED_SIZEOF (double) * psf->sf.channels)
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			return psf_calc_max_all_channels (psf, (double *)data, SF_TRUE) ;

		case SFC_UPDATE_HEADER_NOW :
			if (psf->write_header)
				psf->write_header (psf, SF_TRUE) ;
			break ;

		case SFC_SET_UPDATE_HEADER_AUTO :
			psf->auto_header = datasize ? SF_TRUE : SF_FALSE ;
			return psf->auto_header ;
			break ;

		case SFC_SET_ADD_DITHER_ON_WRITE :
		case SFC_SET_ADD_DITHER_ON_READ :
			/*
			** FIXME !
			** These are obsolete. Just return.
			** Remove some time after version 1.0.8.
			*/
			break ;

		case SFC_SET_DITHER_ON_WRITE :
			if (data == NULL || datasize != SIGNED_SIZEOF (SF_DITHER_INFO))
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			memcpy (&psf->write_dither, data, sizeof (psf->write_dither)) ;
			if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
				dither_init (psf, SFM_WRITE) ;
			break ;

		case SFC_SET_DITHER_ON_READ :
			if (data == NULL || datasize != SIGNED_SIZEOF (SF_DITHER_INFO))
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			memcpy (&psf->read_dither, data, sizeof (psf->read_dither)) ;
			if (psf->mode == SFM_READ || psf->mode == SFM_RDWR)
				dither_init (psf, SFM_READ) ;
			break ;

		case SFC_FILE_TRUNCATE :
			if (psf->mode != SFM_WRITE && psf->mode != SFM_RDWR)
				return SF_TRUE ;
			if (datasize != sizeof (sf_count_t))
				return SF_TRUE ;
			{   sf_count_t position ;

				position = *((sf_count_t*) data) ;

				if (sf_seek (sndfile, position, SEEK_SET) != position)
					return SF_TRUE ;

				psf->sf.frames = position ;

				position = psf_fseek (psf, 0, SEEK_CUR) ;

				return psf_ftruncate (psf, position) ;
				} ;
			break ;

		case SFC_SET_RAW_START_OFFSET :
			if (data == NULL || datasize != sizeof (sf_count_t))
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
				return (psf->error = SFE_BAD_CONTROL_CMD) ;

			psf->dataoffset = *((sf_count_t*) data) ;
			sf_seek (sndfile, 0, SEEK_CUR) ;
			break ;

		case SFC_GET_EMBED_FILE_INFO :
			if (data == NULL || datasize != sizeof (SF_EMBED_FILE_INFO))
				return (psf->error = SFE_BAD_CONTROL_CMD) ;

			((SF_EMBED_FILE_INFO*) data)->offset = psf->fileoffset ;
			((SF_EMBED_FILE_INFO*) data)->length = psf->filelength ;
			break ;

		case SFC_TEST_IEEE_FLOAT_REPLACE :
			psf->ieee_replace = (datasize) ? SF_TRUE : SF_FALSE ;
			if ((psf->sf.format & SF_FORMAT_SUBMASK) == SF_FORMAT_FLOAT)
				float32_init (psf) ;
			else if ((psf->sf.format & SF_FORMAT_SUBMASK) == SF_FORMAT_DOUBLE)
				double64_init (psf) ;
			else
				return (psf->error = SFE_BAD_CONTROL_CMD) ;
			break ;

		default :
			/* Must be a file specific command. Pass it on. */
			if (psf->command)
				return psf->command (psf, command, data, datasize) ;

			psf_log_printf (psf, "*** sf_command : cmd = 0x%X\n", command) ;
			return (psf->error = SFE_BAD_CONTROL_CMD) ;
		} ;

	return 0 ;
} /* sf_command */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_seek	(SNDFILE *sndfile, sf_count_t offset, int whence)
{	SF_PRIVATE 	*psf ;
	sf_count_t	seek_from_start = 0, retval ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (! psf->sf.seekable)
	{	psf->error = SFE_NOT_SEEKABLE ;
		return	((sf_count_t) -1) ;
		} ;

	/* If the whence parameter has a mode ORed in, check to see that
	** it makes sense.
	*/
	if (((whence & SFM_MASK) == SFM_WRITE && psf->mode == SFM_READ) ||
			((whence & SFM_MASK) == SFM_WRITE && psf->mode == SFM_WRITE))
	{	psf->error = SFE_WRONG_SEEK ;
		return ((sf_count_t) -1) ;
		} ;

	/* Convert all SEEK_CUR and SEEK_END into seek_from_start to be
	** used with SEEK_SET.
	*/
	switch (whence)
	{	/* The SEEK_SET behaviour is independant of mode. */
		case SEEK_SET :
		case SEEK_SET | SFM_READ :
		case SEEK_SET | SFM_WRITE :
		case SEEK_SET | SFM_RDWR :
				seek_from_start = offset ;
				break ;

		/* The SEEK_CUR is a little more tricky. */
		case SEEK_CUR :
				if (offset == 0)
				{	if (psf->mode == SFM_READ)
						return psf->read_current ;
					if (psf->mode == SFM_WRITE)
						return psf->write_current ;
					} ;
				if (psf->mode == SFM_READ)
					seek_from_start = psf->read_current + offset ;
				else if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
					seek_from_start = psf->write_current + offset ;
				else
					psf->error = SFE_AMBIGUOUS_SEEK ;
				break ;

		case SEEK_CUR | SFM_READ :
				if (offset == 0)
					return psf->read_current ;
				seek_from_start = psf->read_current + offset ;
				break ;

		case SEEK_CUR | SFM_WRITE :
				if (offset == 0)
					return psf->write_current ;
				seek_from_start = psf->write_current + offset ;
				break ;

		/* The SEEK_END */
		case SEEK_END :
		case SEEK_END | SFM_READ :
		case SEEK_END | SFM_WRITE :
				seek_from_start = psf->sf.frames + offset ;
				break ;

		default :
				psf->error = SFE_BAD_SEEK ;
				break ;
		} ;

	if (psf->error)
		return ((sf_count_t) -1) ;

	if (seek_from_start < 0 || seek_from_start > psf->sf.frames)
	{	psf->error = SFE_BAD_SEEK ;
		return ((sf_count_t) -1) ;
		} ;

	if (psf->seek)
	{	int new_mode = (whence & SFM_MASK) ? (whence & SFM_MASK) : psf->mode ;

		retval = psf->seek (psf, new_mode, seek_from_start) ;

		switch (new_mode)
		{	case SFM_READ :
					psf->read_current = retval ;
					break ;
			case SFM_WRITE :
					psf->write_current = retval ;
					break ;
			case SFM_RDWR :
					psf->read_current = retval ;
					psf->write_current = retval ;
					new_mode = SFM_READ ;
					break ;
			} ;

		psf->last_op = new_mode ;

		return retval ;
		} ;

	psf->error = SFE_AMBIGUOUS_SEEK ;
	return ((sf_count_t) -1) ;
} /* sf_seek */

/*------------------------------------------------------------------------------
*/

const char*
sf_get_string (SNDFILE *sndfile, int str_type)
{	SF_PRIVATE 	*psf ;

	if ((psf = (SF_PRIVATE*) sndfile) == NULL)
		return NULL ;
	if (psf->Magick != SNDFILE_MAGICK)
		return NULL ;

	return psf_get_string (psf, str_type) ;
} /* sf_get_string */

int
sf_set_string (SNDFILE *sndfile, int str_type, const char* str)
{	SF_PRIVATE 	*psf ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	return psf_store_string (psf, str_type, str) ;
} /* sf_get_string */

/*==============================================================================
*/

sf_count_t
sf_read_raw		(SNDFILE *sndfile, void *ptr, sf_count_t bytes)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return	0 ;
		} ;

	if (bytes < 0 || psf->read_current >= psf->datalength)
	{	psf_memset (ptr, 0, bytes) ;
		return 0 ;
		} ;

	if (bytes % (psf->sf.channels * psf->bytewidth))
	{	psf->error = SFE_BAD_READ_ALIGN ;
		return 0 ;
		} ;

	count = psf_fread (ptr, 1, bytes, psf) ;

	if (count < bytes)
		psf_memset (((char*)ptr) + count, 0, bytes - count) ;

	psf->read_current += count / psf->blockwidth ;

	psf->last_op = SFM_READ ;

	return count ;
} /* sf_read_raw */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_read_short	(SNDFILE *sndfile, short *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_READ_ALIGN ;
		return 0 ;
		} ;

	if (len <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, len * sizeof (short)) ;
		return 0 ; /* End of file. */
		} ;

	if (! psf->read_short || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return	0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_short (psf, ptr, len) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = len - count ;
		psf_memset (ptr + count, 0, extra * sizeof (short)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count ;
} /* sf_read_short */

sf_count_t
sf_readf_short		(SNDFILE *sndfile, short *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (frames <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, frames * psf->sf.channels * sizeof (short)) ;
		return 0 ; /* End of file. */
		} ;

	if (! psf->read_short || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_short (psf, ptr, frames * psf->sf.channels) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra =  frames * psf->sf.channels - count ;
		psf_memset (ptr + count, 0, extra * sizeof (short)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count / psf->sf.channels ;
} /* sf_readf_short */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_read_int		(SNDFILE *sndfile, int *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF   (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_READ_ALIGN ;
		return 0 ;
		} ;

	if (len <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, len * sizeof (int)) ;
		return 0 ;
		} ;

	if (! psf->read_int || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_int (psf, ptr, len) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = len - count ;
		psf_memset (ptr + count, 0, extra * sizeof (int)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count ;
} /* sf_read_int */

sf_count_t
sf_readf_int	(SNDFILE *sndfile, int *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile,psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (frames <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, frames * psf->sf.channels * sizeof (int)) ;
		return 0 ;
		} ;

	if (! psf->read_int || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return	0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_int (psf, ptr, frames * psf->sf.channels) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = frames * psf->sf.channels - count ;
		psf_memset (ptr + count, 0, extra * sizeof (int)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count / psf->sf.channels ;
} /* sf_readf_int */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_read_float	(SNDFILE *sndfile, float *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_READ_ALIGN ;
		return 0 ;
		} ;

	if (len <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, len * sizeof (float)) ;
		return 0 ;
		} ;

	if (! psf->read_float || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return	0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_float (psf, ptr, len) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = len - count ;
		psf_memset (ptr + count, 0, extra * sizeof (float)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count ;
} /* sf_read_float */

sf_count_t
sf_readf_float	(SNDFILE *sndfile, float *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (frames <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, frames * psf->sf.channels * sizeof (float)) ;
		return 0 ;
		} ;

	if (! psf->read_float || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return	0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_float (psf, ptr, frames * psf->sf.channels) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = frames * psf->sf.channels - count ;
		psf_memset (ptr + count, 0, extra * sizeof (float)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count / psf->sf.channels ;
} /* sf_readf_float */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_read_double	(SNDFILE *sndfile, double *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_READ_ALIGN ;
		return 0 ;
		} ;

	if (len <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, len * sizeof (double)) ;
		return 0 ;
		} ;

	if (! psf->read_double || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return	0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_double (psf, ptr, len) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = len - count ;
		psf_memset (ptr + count, 0, extra * sizeof (double)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count ;
} /* sf_read_double */

sf_count_t
sf_readf_double	(SNDFILE *sndfile, double *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count, extra ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_WRITE)
	{	psf->error = SFE_NOT_READMODE ;
		return 0 ;
		} ;

	if (frames <= 0 || psf->read_current >= psf->sf.frames)
	{	psf_memset (ptr, 0, frames * psf->sf.channels * sizeof (double)) ;
		return 0 ;
		} ;

	if (! psf->read_double || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return	0 ;
		} ;

	if (psf->last_op != SFM_READ)
		if (psf->seek (psf, SFM_READ, psf->read_current) < 0)
			return 0 ;

	count = psf->read_double (psf, ptr, frames * psf->sf.channels) ;

	if (psf->read_current + count / psf->sf.channels > psf->sf.frames)
	{	count = (psf->sf.frames - psf->read_current) * psf->sf.channels ;
		extra = frames * psf->sf.channels - count ;
		psf_memset (ptr + count, 0, extra * sizeof (double)) ;
		psf->read_current = psf->sf.frames ;
		} ;

	psf->read_current += count / psf->sf.channels ;

	psf->last_op = SFM_READ ;

	if (psf->read_current > psf->sf.frames)
	{	count = psf->sf.channels * (psf->read_current - psf->sf.frames) ;
		psf->read_current = psf->sf.frames ;
		} ;

	return count / psf->sf.channels ;
} /* sf_readf_double */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_write_raw	(SNDFILE *sndfile, void *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (len % (psf->sf.channels * psf->bytewidth))
	{	psf->error = SFE_BAD_WRITE_ALIGN ;
		return 0 ;
		} ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf_fwrite (ptr, 1, len, psf) ;

	psf->write_current += count / psf->blockwidth ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	psf->last_op = SFM_WRITE ;

	return count ;
} /* sf_write_raw */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_write_short	(SNDFILE *sndfile, short *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_WRITE_ALIGN ;
		return 0 ;
		} ;

	if (! psf->write_short || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_short (psf, ptr, len) ;

	psf->write_current += count / psf->sf.channels ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	return count ;
} /* sf_write_short */

sf_count_t
sf_writef_short	(SNDFILE *sndfile, short *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (! psf->write_short || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_short (psf, ptr, frames * psf->sf.channels) ;

	psf->write_current += count / psf->sf.channels ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	return count / psf->sf.channels ;
} /* sf_writef_short */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_write_int	(SNDFILE *sndfile, int *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_WRITE_ALIGN ;
		return 0 ;
		} ;

	if (! psf->write_int || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_int (psf, ptr, len) ;

	psf->write_current += count / psf->sf.channels ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	return count ;
} /* sf_write_int */

sf_count_t
sf_writef_int	(SNDFILE *sndfile, int *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (! psf->write_int || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_int (psf, ptr, frames * psf->sf.channels) ;

	psf->write_current += count / psf->sf.channels ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	return count / psf->sf.channels ;
} /* sf_writef_int */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_write_float	(SNDFILE *sndfile, float *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_WRITE_ALIGN ;
		return 0 ;
		} ;

	if (! psf->write_float || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_float (psf, ptr, len) ;

	psf->write_current += count / psf->sf.channels ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	return count ;
} /* sf_write_float */

sf_count_t
sf_writef_float	(SNDFILE *sndfile, float *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (! psf->write_float || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_float (psf, ptr, frames * psf->sf.channels) ;

	psf->write_current += count / psf->sf.channels ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	return count / psf->sf.channels ;
} /* sf_writef_float */

/*------------------------------------------------------------------------------
*/

sf_count_t
sf_write_double	(SNDFILE *sndfile, double *ptr, sf_count_t len)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (len % psf->sf.channels)
	{	psf->error = SFE_BAD_WRITE_ALIGN ;
		return	0 ;
		} ;

	if (! psf->write_double || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_double (psf, ptr, len) ;

	psf->write_current += count / psf->sf.channels ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	return count ;
} /* sf_write_double */

sf_count_t
sf_writef_double	(SNDFILE *sndfile, double *ptr, sf_count_t frames)
{	SF_PRIVATE 	*psf ;
	sf_count_t	count ;

	VALIDATE_SNDFILE_AND_ASSIGN_PSF (sndfile, psf, 1) ;

	if (psf->mode == SFM_READ)
	{	psf->error = SFE_NOT_WRITEMODE ;
		return 0 ;
		} ;

	if (! psf->write_double || ! psf->seek)
	{	psf->error = SFE_UNIMPLEMENTED ;
		return 0 ;
		} ;

	if (psf->last_op != SFM_WRITE)
		if (psf->seek (psf, SFM_WRITE, psf->write_current) < 0)
			return 0 ;

	if (psf->have_written == SF_FALSE && psf->write_header != NULL)
		psf->write_header (psf, SF_FALSE) ;
	psf->have_written = SF_TRUE ;

	count = psf->write_double (psf, ptr, frames * psf->sf.channels) ;

	psf->write_current += count / psf->sf.channels ;

	if (psf->write_current > psf->sf.frames)
		psf->sf.frames = psf->write_current ;

	psf->last_op = SFM_WRITE ;

	if (psf->auto_header)
		psf->write_header (psf, SF_TRUE) ;

	return count / psf->sf.channels ;
} /* sf_writef_double */

/*=========================================================================
** Private functions.
*/

static int
format_from_extension (const char *filename)
{	char *cptr ;
	char buffer [16] ;

	if (filename == NULL)
		return 0 ;

	if ((cptr = strrchr (filename, '.')) == NULL)
		return 0 ;

	cptr ++ ;
	if (strlen (cptr) > sizeof (buffer) - 1)
		return 0 ;

	strncpy (buffer, cptr, sizeof (buffer)) ;
	buffer [sizeof (buffer) - 1] = 0 ;

	/* Convert everything in the buffer to lower case. */
	cptr = buffer ;
	while (*cptr)
	{	*cptr = tolower (*cptr) ;
		cptr ++ ;
		} ;

	cptr = buffer ;

	if (strcmp (cptr, "au") == 0)
		return SF_FORMAT_AU | SF_FORMAT_ULAW ;

	if (strcmp (cptr, "snd") == 0)
		return SF_FORMAT_AU | SF_FORMAT_ULAW ;

	if (strcmp (cptr, "vox") == 0)
		return SF_FORMAT_RAW | SF_FORMAT_VOX_ADPCM ;

	return 0 ;
} /* format_from_extension */

static int
guess_file_type (SF_PRIVATE *psf, const char *filename)
{	int 			buffer [3], format ;
	unsigned char	cptr [0x40] ;

	if (psf_binheader_readf (psf, "b", &buffer, SIGNED_SIZEOF (buffer)) != SIGNED_SIZEOF (buffer))
	{	psf->error = SFE_BAD_FILE_READ ;
		return 0 ;
		} ;

	if (buffer [0] == MAKE_MARKER ('R','I','F','F') && buffer [2] == MAKE_MARKER ('W','A','V','E'))
		return SF_FORMAT_WAV ;

	if (buffer [0] == MAKE_MARKER ('F','O','R','M'))
	{	if (buffer [2] == MAKE_MARKER ('A','I','F','F') || buffer [2] == MAKE_MARKER ('A','I','F','C'))
			return SF_FORMAT_AIFF ;
		if (buffer [2] == MAKE_MARKER ('8','S','V','X') || buffer [2] == MAKE_MARKER ('1','6','S','V'))
			return SF_FORMAT_SVX ;
		return 0 ;
		} ;

	if (buffer [0] == MAKE_MARKER ('.','s','n','d') || buffer [0] == MAKE_MARKER ('d','n','s','.'))
		return SF_FORMAT_AU ;

	if ((buffer [0] == MAKE_MARKER ('f','a','p',' ') || buffer [0] == MAKE_MARKER (' ','p','a','f')))
		return SF_FORMAT_PAF ;

	if (buffer [0] == MAKE_MARKER ('N','I','S','T'))
		return SF_FORMAT_NIST ;

	if (buffer [0] == MAKE_MARKER ('C','r','e','a') && buffer [1] == MAKE_MARKER ('t','i','v','e'))
		return SF_FORMAT_VOC ;

	if ((buffer [0] & MAKE_MARKER (0xFF, 0xFF, 0xF8, 0xFF)) == MAKE_MARKER (0x64, 0xA3, 0x00, 0x00) ||
		(buffer [0] & MAKE_MARKER (0xFF, 0xF8, 0xFF, 0xFF)) == MAKE_MARKER (0x00, 0x00, 0xA3, 0x64))
		return SF_FORMAT_IRCAM ;

	if (buffer [0] == MAKE_MARKER ('r', 'i', 'f', 'f'))
		return SF_FORMAT_W64 ;

	if (buffer [0] == MAKE_MARKER (0, 0, 0x03, 0xE8) && buffer [1] == MAKE_MARKER (0, 0, 0, 1) &&
								buffer [2] == MAKE_MARKER (0, 0, 0, 1))
		return SF_FORMAT_MAT4 ;

	if (buffer [0] == MAKE_MARKER (0, 0, 0, 0) && buffer [1] == MAKE_MARKER (1, 0, 0, 0) &&
								buffer [2] == MAKE_MARKER (1, 0, 0, 0))
		return SF_FORMAT_MAT4 ;

	if (buffer [0] == MAKE_MARKER ('M', 'A', 'T', 'L') && buffer [1] == MAKE_MARKER ('A', 'B', ' ', '5'))
		return SF_FORMAT_MAT5 ;

	if (buffer [0] == MAKE_MARKER ('P', 'V', 'F', '1'))
		return SF_FORMAT_PVF ;

	if (buffer [0] == MAKE_MARKER ('E', 'x', 't', 'e') && buffer [1] == MAKE_MARKER ('n', 'd', 'e', 'd') &&
								buffer [2] == MAKE_MARKER (' ', 'I',  'n', 's'))
		return SF_FORMAT_XI ;

	if (ENABLE_EXPERIMENTAL_CODE && buffer [0] == MAKE_MARKER ('O', 'g', 'g', 'S'))
		return SF_FORMAT_OGG ;

	if (buffer [0] == MAKE_MARKER ('A', 'L', 'a', 'w') && buffer [1] == MAKE_MARKER ('S', 'o', 'u', 'n')
			&& buffer [2] == MAKE_MARKER ('d', 'F', 'i', 'l'))
		return SF_FORMAT_WVE ;

	if (buffer [0] == MAKE_MARKER ('D', 'i', 'a', 'm') && buffer [1] == MAKE_MARKER ('o', 'n', 'd', 'W')
			&& buffer [2] == MAKE_MARKER ('a', 'r', 'e', ' '))
		return SF_FORMAT_DWD ;

	if (buffer [0] == MAKE_MARKER ('L', 'M', '8', '9') || buffer [0] == MAKE_MARKER ('5', '3', 0, 0))
		return SF_FORMAT_TXW ;

	if ((buffer [0] & MAKE_MARKER (0xFF, 0xFF, 0x80, 0xFF)) == MAKE_MARKER (0xF0, 0x7E, 0, 0x01))
		return SF_FORMAT_SDS ;

	if (buffer [0] == MAKE_MARKER ('C', 'A', 'T', ' ') && buffer [2] == MAKE_MARKER ('R', 'E', 'X', '2'))
		return SF_FORMAT_REX2 ;

	if (buffer [0] == MAKE_MARKER (0x30, 0x26, 0xB2, 0x75) && buffer [1] == MAKE_MARKER (0x8E, 0x66, 0xCF, 0x11))
		return 0 /*-SF_FORMAT_WMA-*/ ;

	/* HMM (Hidden Markov Model) Tool Kit. */
	if (2 * BEI2H_INT (buffer [0]) + 12 == psf->filelength && buffer [2] == MAKE_MARKER (0, 2, 0, 0))
		return SF_FORMAT_HTK ;

	if (buffer [0] == MAKE_MARKER ('f', 'L', 'a', 'C'))
		return 0 /*-SF_FORMAT_FLAC-*/ ;

	/* Turtle Beach SMP 16-bit */
	if (buffer [0] == MAKE_MARKER ('S', 'O', 'U', 'N') && buffer [1] == MAKE_MARKER ('D', ' ', 'S', 'A'))
		return 0 ;

	if (buffer [0] == MAKE_MARKER ('S', 'Y', '8', '0') || buffer [0] == MAKE_MARKER ('S', 'Y', '8', '5'))
		return 0 ;

	if (buffer [0] == MAKE_MARKER ('a', 'j', 'k', 'g'))
		return 0 /*-SF_FORMAT_SHN-*/ ;

	/*	Detect wacky MacOS header stuff. This might be "Sound Designer II". */
	memcpy (cptr , buffer, sizeof (buffer)) ;
	if (cptr [0] == 0 && cptr [1] > 0 && psf->sf.seekable)
	{	psf_binheader_readf (psf, "pb", 0, &cptr, SIGNED_SIZEOF (cptr)) ;

		if (cptr [1] < (sizeof (cptr) - 3) && cptr [cptr [1] + 2] == 0 && strlen (((char*) cptr) + 2) == cptr [1])
		{	psf_log_printf (psf, "Weird MacOS Header.\n") ;
			psf_binheader_readf (psf, "em", &buffer) ;
			if (buffer [0] == MAKE_MARKER (0, 'S', 'd', '2'))
				return SF_FORMAT_SD2 ;
			} ;
		} ;

	/* This must be the last one. */
	if ((format = format_from_extension (filename)) != 0)
		return format ;

	/* Default to header-less RAW PCM file type. */
	return SF_FORMAT_RAW ;
} /* guess_file_type */


static int
validate_sfinfo (SF_INFO *sfinfo)
{	if (sfinfo->samplerate < 1)
		return 0 ;
	if (sfinfo->frames < 0)
		return 0 ;
	if (sfinfo->channels < 1)
		return 0 ;
	if ((sfinfo->format & SF_FORMAT_TYPEMASK) == 0)
		return 0 ;
	if ((sfinfo->format & SF_FORMAT_SUBMASK) == 0)
		return 0 ;
	if (sfinfo->sections < 1)
		return 0 ;
	return 1 ;
} /* validate_sfinfo */

static int
validate_psf (SF_PRIVATE *psf)
{
	if (psf->datalength < 0)
	{	psf_log_printf (psf, "Invalid SF_PRIVATE field : datalength == %D.\n", psf->datalength) ;
		return 0 ;
		} ;
	if (psf->dataoffset < 0)
	{	psf_log_printf (psf, "Invalid SF_PRIVATE field : dataoffset == %D.\n", psf->dataoffset) ;
		return 0 ;
		} ;
	if (psf->blockwidth && psf->blockwidth != psf->sf.channels * psf->bytewidth)
	{	psf_log_printf (psf, "Invalid SF_PRIVATE field : channels * bytewidth == %d.\n",
								psf->sf.channels * psf->bytewidth) ;
		return 0 ;
		} ;
	return 1 ;
} /* validate_psf */

static void
save_header_info (SF_PRIVATE *psf)
{	LSF_SNPRINTF (sf_logbuffer, sizeof (sf_logbuffer), "%s", psf->logbuffer) ;
} /* save_header_info */

static void
copy_filename (SF_PRIVATE *psf, const char *path)
{	const char *cptr ;

	if ((cptr = strrchr (path, '/')) || (cptr = strrchr (path, '\\')))
		cptr ++ ;
	else
		cptr = path ;

	memset (psf->filename, 0, SF_FILENAME_LEN) ;

	LSF_SNPRINTF (psf->filename, sizeof (psf->filename), "%s", cptr) ;

} /* copy_filename */

/*==============================================================================
*/

static int
psf_close (SF_PRIVATE  *psf)
{	int			error ;

	if (psf->close)
		error = psf->close (psf) ;

	if (psf->do_not_close_descritor)
		psf->filedes = -1 ;
	else
		psf_fclose (psf) ;

	if (psf->fdata)
		free (psf->fdata) ;

	if (psf->interleave)
		free (psf->interleave) ;

	if (psf->dither)
		free (psf->dither) ;

	memset (psf, 0, sizeof (SF_PRIVATE)) ;

	free (psf) ;

	return 0 ;
} /* psf_close */

static int
psf_open_file (SF_PRIVATE *psf, int mode, SF_INFO *sfinfo)
{	int		error ;
	
	if (mode != SFM_READ && mode != SFM_WRITE && mode != SFM_RDWR)
		return SFE_BAD_OPEN_MODE ;

	if (! sfinfo)
		return SFE_BAD_SF_INFO_PTR ;

	if (mode == SFM_READ)
	{	if ((sfinfo->format & SF_FORMAT_TYPEMASK) == SF_FORMAT_RAW)
		{	if (sf_format_check (sfinfo) == 0)
				return SFE_RAW_BAD_FORMAT ;
			}
		else
			memset (sfinfo, 0, sizeof (SF_INFO)) ;
		} ;

	sf_errno = error = 0 ;
	sf_logbuffer [0] = 0 ;

	memcpy (&(psf->sf), sfinfo, sizeof (SF_INFO)) ;

	psf->Magick 		= SNDFILE_MAGICK ;
	psf->norm_float 	= SF_TRUE ;
	psf->norm_double	= SF_TRUE ;
	psf->mode 			= mode ;
	psf->dataoffset		= -1 ;
	psf->datalength		= -1 ;
	psf->read_current	= -1 ;
	psf->write_current	= -1 ;
	psf->auto_header 	= SF_FALSE ;
	psf->rwf_endian		= SF_ENDIAN_LITTLE ;
	psf->seek			= psf_default_seek ;

	/* File is open, so get the length. */
	psf->filelength = psf_get_filelen (psf) ;

	if (psf->fileoffset > 0)
	{	switch (psf->mode)
		{	case SFM_READ :
				if (psf->filelength < 44)
				{	psf_log_printf (psf, "Short filelength: %D (fileoffset: %D)\n", psf->filelength, psf->fileoffset) ;
					return SFE_BAD_OFFSET ;
					} ;
				break ;
		
			case SFM_WRITE :
				psf->fileoffset = 0 ;
				psf_fseek (psf, 0, SEEK_END) ;
				psf->fileoffset = psf_ftell (psf) ;
				break ;

			case SFM_RDWR :
				return SFE_NO_EMBEDDED_RDWR ;
			} ;

		psf_log_printf (psf, "Embedded file offset : %D\n", psf->fileoffset) ;
		} ;

	psf->sf.seekable = SF_TRUE ;
	psf->sf.sections = 1 ;

	psf_log_printf (psf, "Length : %D\n", psf->filelength) ;

	if (mode == SFM_WRITE || (mode == SFM_RDWR && psf->filelength == 0))
	{	/* If the file is being opened for write or RDWR and the file is currently
		** empty, then the SF_INFO struct must contain valid data.
		*/
		if (sf_format_check (&(psf->sf)) == 0)
			return SFE_BAD_OPEN_FORMAT ;
		}
	else if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
	{	/* If type RAW has not been specified then need to figure out file type. */
		psf->sf.format = guess_file_type (psf, psf->filename) ;
		} ;

	/* Set bytewidth if known. */
	switch (psf->sf.format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_S8 :
		case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_ULAW :
		case SF_FORMAT_ALAW :
		case SF_FORMAT_DPCM_8 :
				psf->bytewidth = 1 ;
				break ;

		case SF_FORMAT_PCM_16 :
		case SF_FORMAT_DPCM_16 :
				psf->bytewidth = 2 ;
				break ;

		case SF_FORMAT_PCM_24 :
				psf->bytewidth = 3 ;
				break ;

		case SF_FORMAT_PCM_32 :
		case SF_FORMAT_FLOAT :
				psf->bytewidth = 4 ;
				break ;

		case SF_FORMAT_DOUBLE :
				psf->bytewidth = 8 ;
				break ;
		} ;

	/* Call the initialisation function for the relevant file type. */
	switch (psf->sf.format & SF_FORMAT_TYPEMASK)
	{	case	SF_FORMAT_WAV :
				error = wav_open (psf) ;
				break ;

		case	SF_FORMAT_AIFF :
				error = aiff_open (psf) ;
				break ;

		case	SF_FORMAT_AU :
				error = au_open (psf) ;
				break ;

		case	SF_FORMAT_AU | SF_FORMAT_ULAW :
				error = au_nh_open (psf) ;
				break ;

		case	SF_FORMAT_RAW :
				error = raw_open (psf) ;
				break ;

		case	SF_FORMAT_PAF :
				error = paf_open (psf) ;
				break ;

		case	SF_FORMAT_SVX :
				error = svx_open (psf) ;
				break ;

		case	SF_FORMAT_NIST :
				error = nist_open (psf) ;
				break ;

		case	SF_FORMAT_IRCAM :
				error = ircam_open (psf) ;
				break ;

		case	SF_FORMAT_VOC :
				error = voc_open (psf) ;
				break ;

		case	SF_FORMAT_W64 :
				error = w64_open (psf) ;
				break ;

		case	SF_FORMAT_OGG :
				error = ogg_open (psf) ;
				break ;

		case	SF_FORMAT_TXW :
				error = txw_open (psf) ;
				break ;

		case	SF_FORMAT_WVE :
				error = wve_open (psf) ;
				break ;

		case	SF_FORMAT_DWD :
				error = dwd_open (psf) ;
				break ;

		case	SF_FORMAT_MAT4 :
				error = mat4_open (psf) ;
				break ;

		case	SF_FORMAT_MAT5 :
				error = mat5_open (psf) ;
				break ;

		case	SF_FORMAT_PVF :
				error = pvf_open (psf) ;
				break ;

		case	SF_FORMAT_XI :
				error = xi_open (psf) ;
				break ;

		case	SF_FORMAT_HTK :
				error = htk_open (psf) ;
				break ;

		case	SF_FORMAT_SD2 :
				error = sd2_open (psf) ;
				break ;

		case	SF_FORMAT_REX2 :
				error = rx2_open (psf) ;
				break ;

		default :
				error = SFE_UNKNOWN_FORMAT ;
		} ;

	if (error)
		return error ;

	/* For now, check whether embedding is supported. */
	if (psf->fileoffset > 0 && 
			((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) && 
			((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_AIFF) &&
			((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_AU)
			)
		return SFE_NO_EMBED_SUPPORT ;

	if (psf->fileoffset > 0)
		psf_log_printf (psf, "Embedded file length : %D\n", psf->filelength) ;

	if (mode == SFM_RDWR && sf_format_check (&(psf->sf)) == 0)
		return SFE_BAD_RDWR_FORMAT ;

	if (validate_sfinfo (&(psf->sf)) == 0)
	{	psf_log_SF_INFO (psf) ;
		save_header_info (psf) ;
		return SFE_BAD_SF_INFO ;
		} ;

	if (validate_psf (psf) == 0)
	{	save_header_info (psf) ;
		return SFE_INTERNAL ;
		} ;

	psf->read_current  = 0 ;
	psf->write_current = (psf->mode == SFM_RDWR) ? psf->sf.frames : 0 ;

	memcpy (sfinfo, &(psf->sf), sizeof (SF_INFO)) ;

	return 0 ;
} /* psf_open_file */
