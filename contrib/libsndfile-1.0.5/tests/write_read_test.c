/*
** Copyright (C) 1999-2002 Erik de Castro Lopo <erikd@zip.com.au>
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
#include	<unistd.h>
#include	<math.h>

#if (defined (WIN32) || defined (_WIN32))
#include	<fcntl.h>
static int truncate (const char *filename, int ignored) ;
#endif

#include	<sndfile.h>

#include	"utils.h"

#define	SAMPLE_RATE			11025
#define	DATA_LENGTH			(1<<12)

#define	SILLY_WRITE_COUNT	(234)

static void	pcm_test_char	(char *str, int filetype, int long_file_ok) ;
static void	pcm_test_short	(char *str, int filetype, int long_file_ok) ;
static void	pcm_test_24bit	(char *str, int filetype, int long_file_ok) ;
static void	pcm_test_int	(char *str, int filetype, int long_file_ok) ;
static void	pcm_test_float	(char *str, int filetype, int long_file_ok) ;
static void	pcm_test_double	(char *str, int filetype, int long_file_ok) ;


static	double	orig_data [DATA_LENGTH] ;
static	double	test_data [DATA_LENGTH] ;

int
main (int argc, char **argv)
{	int		do_all = 0 ;
	int		test_count = 0 ;

	if (argc != 2)
	{	printf ("Usage : %s <test>\n", argv [0]) ;
		printf ("    Where <test> is one of the following:\n") ;
		printf ("           wav   - test WAV file functions (little endian)\n") ;
		printf ("           aiff  - test AIFF file functions (big endian)\n") ;
		printf ("           au    - test AU file functions\n") ;
		printf ("           raw   - test RAW header-less PCM file functions\n") ;
		printf ("           paf   - test PAF file functions\n") ;
		printf ("           svx   - test 8SVX/16SV file functions\n") ;
		printf ("           nist  - test NIST Sphere file functions\n") ;
		printf ("           ircam - test IRCAM file functions\n") ;
		printf ("           voc   - Create Voice file functions\n") ;
		printf ("           w64   - Sonic Foundry's W64 file functions\n") ;
		printf ("           all   - perform all tests\n") ;
		exit (1) ;
		} ;

	do_all = !strcmp (argv [1], "all");

	if (do_all || ! strcmp (argv [1], "wav"))
	{	pcm_test_char 	("char.wav"  , SF_FORMAT_WAV | SF_FORMAT_PCM_U8, SF_FALSE) ;
		pcm_test_short	("short.wav" , SF_FORMAT_WAV | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_24bit	("24bit.wav" , SF_FORMAT_WAV | SF_FORMAT_PCM_24, SF_FALSE) ;
		pcm_test_int	("int.wav"   , SF_FORMAT_WAV | SF_FORMAT_PCM_32, SF_FALSE) ;
		pcm_test_float	("float.wav" , SF_FORMAT_WAV | SF_FORMAT_FLOAT , SF_FALSE) ;
		pcm_test_double	("double.wav", SF_FORMAT_WAV | SF_FORMAT_DOUBLE, SF_FALSE) ;
		test_count++ ;
		} ;

	if (do_all || ! strcmp (argv [1], "aiff"))
	{	pcm_test_char	("char_u8.aiff", SF_FORMAT_AIFF | SF_FORMAT_PCM_U8, SF_FALSE) ;
		pcm_test_char	("char_s8.aiff", SF_FORMAT_AIFF | SF_FORMAT_PCM_S8, SF_FALSE) ;
		pcm_test_short	("short.aiff"  , SF_FORMAT_AIFF | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_24bit	("24bit.aiff"  , SF_FORMAT_AIFF | SF_FORMAT_PCM_24, SF_FALSE) ;
		pcm_test_int	("int.aiff"    , SF_FORMAT_AIFF | SF_FORMAT_PCM_32, SF_FALSE) ;

		pcm_test_short	("short_sowt.aifc", SF_ENDIAN_LITTLE | SF_FORMAT_AIFF | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_24bit	("24bit_sowt.aifc", SF_ENDIAN_LITTLE | SF_FORMAT_AIFF | SF_FORMAT_PCM_24, SF_FALSE) ;
		pcm_test_int	("int_sowt.aifc"  , SF_ENDIAN_LITTLE | SF_FORMAT_AIFF | SF_FORMAT_PCM_32, SF_FALSE) ;

		pcm_test_short	("short_twos.aifc", SF_ENDIAN_BIG | SF_FORMAT_AIFF | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_24bit	("24bit_twos.aifc", SF_ENDIAN_BIG | SF_FORMAT_AIFF | SF_FORMAT_PCM_24, SF_FALSE) ;
		pcm_test_int	("int_twos.aifc"  , SF_ENDIAN_BIG | SF_FORMAT_AIFF | SF_FORMAT_PCM_32, SF_FALSE) ;

		pcm_test_short	("dwvw16.aifc", SF_FORMAT_AIFF | SF_FORMAT_DWVW_16, SF_TRUE) ;
		pcm_test_24bit	("dwvw24.aifc", SF_FORMAT_AIFF | SF_FORMAT_DWVW_24, SF_TRUE) ;

		pcm_test_float	("float.aifc" , SF_FORMAT_AIFF | SF_FORMAT_FLOAT , SF_FALSE) ;
		pcm_test_double	("double.aifc", SF_FORMAT_AIFF | SF_FORMAT_DOUBLE, SF_FALSE) ;

		test_count++ ;
		} ;

	if (do_all || ! strcmp (argv [1], "au"))
	{	pcm_test_char	("char.au"  , SF_FORMAT_AU | SF_FORMAT_PCM_S8, SF_FALSE) ;
		pcm_test_short	("short.au" , SF_FORMAT_AU | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_24bit	("24bit.au" , SF_FORMAT_AU | SF_FORMAT_PCM_24, SF_FALSE) ;
		pcm_test_int	("int.au"   , SF_FORMAT_AU | SF_FORMAT_PCM_32, SF_FALSE) ;
		pcm_test_float	("float.au" , SF_FORMAT_AU | SF_FORMAT_FLOAT , SF_FALSE) ;
		pcm_test_double	("double.au", SF_FORMAT_AU | SF_FORMAT_DOUBLE, SF_FALSE) ;

		pcm_test_char	("char_le.au"  , SF_ENDIAN_LITTLE | SF_FORMAT_AU | SF_FORMAT_PCM_S8, SF_FALSE) ;
		pcm_test_short	("short_le.au" , SF_ENDIAN_LITTLE | SF_FORMAT_AU | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_24bit	("24bit_le.au" , SF_ENDIAN_LITTLE | SF_FORMAT_AU | SF_FORMAT_PCM_24, SF_FALSE) ;
		pcm_test_int	("int_le.au"   , SF_ENDIAN_LITTLE | SF_FORMAT_AU | SF_FORMAT_PCM_32, SF_FALSE) ;
		pcm_test_float	("float_le.au" , SF_ENDIAN_LITTLE | SF_FORMAT_AU | SF_FORMAT_FLOAT , SF_FALSE) ;
		pcm_test_double	("double_le.au", SF_ENDIAN_LITTLE | SF_FORMAT_AU | SF_FORMAT_DOUBLE, SF_FALSE) ;

		test_count++ ;
		} ;

	if (do_all || ! strcmp (argv [1], "raw"))
	{	pcm_test_char	("char_s8.raw", SF_FORMAT_RAW | SF_FORMAT_PCM_S8, SF_FALSE) ;
		pcm_test_char	("char_u8.raw", SF_FORMAT_RAW | SF_FORMAT_PCM_U8, SF_FALSE) ;

		pcm_test_short	("short_le.raw", SF_ENDIAN_LITTLE | SF_FORMAT_RAW | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_short	("short_be.raw", SF_ENDIAN_BIG    | SF_FORMAT_RAW | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_24bit	("24bit_le.raw", SF_ENDIAN_LITTLE | SF_FORMAT_RAW | SF_FORMAT_PCM_24, SF_FALSE) ;
		pcm_test_24bit	("24bit_be.raw", SF_ENDIAN_BIG    | SF_FORMAT_RAW | SF_FORMAT_PCM_24, SF_FALSE) ;
		pcm_test_int	("int_le.raw"  , SF_ENDIAN_LITTLE | SF_FORMAT_RAW | SF_FORMAT_PCM_32, SF_FALSE) ;
		pcm_test_int	("int_be.raw"  , SF_ENDIAN_BIG    | SF_FORMAT_RAW | SF_FORMAT_PCM_32, SF_FALSE) ;

		pcm_test_float	("float_le.raw", SF_ENDIAN_LITTLE | SF_FORMAT_RAW | SF_FORMAT_FLOAT , SF_FALSE) ;
		pcm_test_float	("float_be.raw", SF_ENDIAN_BIG    | SF_FORMAT_RAW | SF_FORMAT_FLOAT , SF_FALSE) ;

		pcm_test_double	("double_le.raw", SF_ENDIAN_LITTLE | SF_FORMAT_RAW | SF_FORMAT_DOUBLE, SF_FALSE) ;
		pcm_test_double	("double_be.raw", SF_ENDIAN_BIG    | SF_FORMAT_RAW | SF_FORMAT_DOUBLE, SF_FALSE) ;

		test_count++ ;
		} ;

	if (do_all || ! strcmp (argv [1], "paf"))
	{	pcm_test_char	("char_le.paf", SF_ENDIAN_LITTLE | SF_FORMAT_PAF | SF_FORMAT_PCM_S8, SF_FALSE) ;
		pcm_test_char	("char_be.paf", SF_ENDIAN_BIG    | SF_FORMAT_PAF | SF_FORMAT_PCM_S8, SF_FALSE) ;
		pcm_test_short	("short_le.paf", SF_ENDIAN_LITTLE | SF_FORMAT_PAF | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_short	("short_be.paf", SF_ENDIAN_BIG    | SF_FORMAT_PAF | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_24bit	("24bit_le.paf", SF_ENDIAN_LITTLE | SF_FORMAT_PAF | SF_FORMAT_PCM_24, SF_TRUE) ;
		pcm_test_24bit	("24bit_be.paf", SF_ENDIAN_BIG    | SF_FORMAT_PAF | SF_FORMAT_PCM_24, SF_TRUE) ;

		test_count++ ;
		} ;

	if (do_all || ! strcmp (argv [1], "svx"))
	{	pcm_test_char	("char.svx" , SF_FORMAT_SVX | SF_FORMAT_PCM_S8, SF_FALSE) ;
		pcm_test_short	("short.svx", SF_FORMAT_SVX | SF_FORMAT_PCM_16, SF_FALSE) ;

		test_count++ ;
		} ;

	if (do_all || ! strcmp (argv [1], "nist"))
	{	pcm_test_short	("short_le.nist", SF_ENDIAN_LITTLE | SF_FORMAT_NIST | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_short	("short_be.nist", SF_ENDIAN_BIG    | SF_FORMAT_NIST | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_24bit	("24bit_le.nist", SF_ENDIAN_LITTLE | SF_FORMAT_NIST | SF_FORMAT_PCM_24, SF_FALSE) ;
		pcm_test_24bit	("24bit_be.nist", SF_ENDIAN_BIG    | SF_FORMAT_NIST | SF_FORMAT_PCM_24, SF_FALSE) ;
		pcm_test_int	("int_le.nist"  , SF_ENDIAN_LITTLE | SF_FORMAT_NIST | SF_FORMAT_PCM_32, SF_FALSE) ;
		pcm_test_int 	("int_be.nist"  , SF_ENDIAN_BIG    | SF_FORMAT_NIST | SF_FORMAT_PCM_32, SF_FALSE) ;

		test_count++ ;
		} ;

	if (do_all || ! strcmp (argv [1], "ircam"))
	{	pcm_test_short	("short_be.ircam", SF_ENDIAN_BIG    | SF_FORMAT_IRCAM | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_short	("short_le.ircam", SF_ENDIAN_LITTLE | SF_FORMAT_IRCAM | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_int	("int_be.ircam"  , SF_ENDIAN_BIG    | SF_FORMAT_IRCAM | SF_FORMAT_PCM_32, SF_FALSE) ;
		pcm_test_int 	("int_le.ircam"  , SF_ENDIAN_LITTLE | SF_FORMAT_IRCAM | SF_FORMAT_PCM_32, SF_FALSE) ;
		pcm_test_float	("float_be.ircam", SF_ENDIAN_BIG    | SF_FORMAT_IRCAM | SF_FORMAT_FLOAT , SF_FALSE) ;
		pcm_test_float	("float_le.ircam", SF_ENDIAN_LITTLE | SF_FORMAT_IRCAM | SF_FORMAT_FLOAT , SF_FALSE) ;

		test_count++ ;
		} ;

	if (do_all || ! strcmp (argv [1], "voc"))
	{	pcm_test_char 	("char.voc" , SF_FORMAT_VOC | SF_FORMAT_PCM_U8, SF_FALSE) ;
		pcm_test_short	("short.voc", SF_FORMAT_VOC | SF_FORMAT_PCM_16, SF_FALSE) ;

		test_count++ ;
		} ;

	if (do_all || ! strcmp (argv [1], "w64"))
	{	pcm_test_char 	("char.w64"  , SF_FORMAT_W64 | SF_FORMAT_PCM_U8, SF_FALSE) ;
		pcm_test_short	("short.w64" , SF_FORMAT_W64 | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_24bit	("24bit.w64" , SF_FORMAT_W64 | SF_FORMAT_PCM_24, SF_FALSE) ;
		pcm_test_int	("int.w64"   , SF_FORMAT_W64 | SF_FORMAT_PCM_32, SF_FALSE) ;
		pcm_test_float	("float.w64" , SF_FORMAT_W64 | SF_FORMAT_FLOAT , SF_FALSE) ;
		pcm_test_double	("double.w64", SF_FORMAT_W64 | SF_FORMAT_DOUBLE, SF_FALSE) ;

		test_count++ ;
		} ;

	if (do_all || ! strcmp (argv [1], "mat4"))
	{	pcm_test_short	("short_be.mat4" , SF_ENDIAN_BIG    | SF_FORMAT_MAT4 | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_short	("short_le.mat4" , SF_ENDIAN_LITTLE | SF_FORMAT_MAT4 | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_int	("int_be.mat4"   , SF_ENDIAN_BIG    | SF_FORMAT_MAT4 | SF_FORMAT_PCM_32, SF_FALSE) ;
		pcm_test_int 	("int_le.mat4"   , SF_ENDIAN_LITTLE | SF_FORMAT_MAT4 | SF_FORMAT_PCM_32, SF_FALSE) ;
		pcm_test_float	("float_be.mat4" , SF_ENDIAN_BIG    | SF_FORMAT_MAT4 | SF_FORMAT_FLOAT , SF_FALSE) ;
		pcm_test_float	("float_le.mat4" , SF_ENDIAN_LITTLE | SF_FORMAT_MAT4 | SF_FORMAT_FLOAT , SF_FALSE) ;
		pcm_test_double	("double_be.mat4", SF_ENDIAN_BIG    | SF_FORMAT_MAT4 | SF_FORMAT_DOUBLE, SF_FALSE) ;
		pcm_test_double	("double_le.mat4", SF_ENDIAN_LITTLE | SF_FORMAT_MAT4 | SF_FORMAT_DOUBLE, SF_FALSE) ;

		test_count++ ;
		} ;

	if (do_all || ! strcmp (argv [1], "mat5"))
	{	pcm_test_char 	("char_be.mat5"  , SF_ENDIAN_BIG    | SF_FORMAT_MAT5 | SF_FORMAT_PCM_U8, SF_FALSE) ;
		pcm_test_char 	("char_le.mat5"  , SF_ENDIAN_LITTLE | SF_FORMAT_MAT5 | SF_FORMAT_PCM_U8, SF_FALSE) ;
		pcm_test_short	("short_be.mat5" , SF_ENDIAN_BIG    | SF_FORMAT_MAT5 | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_short	("short_le.mat5" , SF_ENDIAN_LITTLE | SF_FORMAT_MAT5 | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_int	("int_be.mat5"   , SF_ENDIAN_BIG    | SF_FORMAT_MAT5 | SF_FORMAT_PCM_32, SF_FALSE) ;
		pcm_test_int 	("int_le.mat5"   , SF_ENDIAN_LITTLE | SF_FORMAT_MAT5 | SF_FORMAT_PCM_32, SF_FALSE) ;
		pcm_test_float	("float_be.mat5" , SF_ENDIAN_BIG    | SF_FORMAT_MAT5 | SF_FORMAT_FLOAT , SF_FALSE) ;
		pcm_test_float	("float_le.mat5" , SF_ENDIAN_LITTLE | SF_FORMAT_MAT5 | SF_FORMAT_FLOAT , SF_FALSE) ;
		pcm_test_double	("double_be.mat5", SF_ENDIAN_BIG    | SF_FORMAT_MAT5 | SF_FORMAT_DOUBLE, SF_FALSE) ;
		pcm_test_double	("double_le.mat5", SF_ENDIAN_LITTLE | SF_FORMAT_MAT5 | SF_FORMAT_DOUBLE, SF_FALSE) ;

		test_count++ ;
		} ;

	if (do_all || ! strcmp (argv [1], "pvf"))
	{	pcm_test_char 	("char.pvf"  , SF_FORMAT_PVF | SF_FORMAT_PCM_S8, SF_FALSE) ;
		pcm_test_short	("short.pvf" , SF_FORMAT_PVF | SF_FORMAT_PCM_16, SF_FALSE) ;
		pcm_test_int	("int.pvf"   , SF_FORMAT_PVF | SF_FORMAT_PCM_32, SF_FALSE) ;

		test_count++ ;
		} ;

	if (do_all || ! strcmp (argv [1], "htk"))
	{	pcm_test_short	("short.htk" , SF_FORMAT_HTK | SF_FORMAT_PCM_16, SF_FALSE) ;

		test_count++ ;
		} ;

	if (test_count == 0)
	{	printf ("Mono : ************************************\n") ;
		printf ("Mono : *  No '%s' test defined.\n", argv [1]) ;
		printf ("Mono : ************************************\n") ;
		return 1 ;
		} ;

	return 0;
} /* main */

/*============================================================================================
**	Helper functions and macros.
*/

static void	create_short_file (char *filename) ;

#define	CHAR_ERROR(x,y)		(abs ((x) - (y)) > 255)
#define	INT_ERROR(x,y)		(((x) - (y)) != 0)
#define	TRIBYTE_ERROR(x,y)	(abs ((x) - (y)) > 255)
#define	FLOAT_ERROR(x,y)	(fabs ((x) - (y)) > 1e-5)

#define CONVERT_DATA(k,len,new,orig)	\
			{	for ((k) = 0 ; (k) < (len) ; (k)++)	\
					(new) [k] = (orig) [k] ;		\
				}


/*======================================================================================
*/

static void
pcm_test_char (char *filename, int filetype, int long_file_ok)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	short		*orig, *test ;
	sf_count_t	count ;
	int			k, items, frames, pass ;

	print_test_name ("pcm_test_char", filename) ;

	sfinfo.samplerate  = 44100 ;
	sfinfo.frames     = SILLY_WRITE_COUNT ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels    = 1 ;
	sfinfo.format 	   = filetype ;

	gen_windowed_sine (orig_data, DATA_LENGTH, 32000.0) ;

	orig = (short*) orig_data ;
	test = (short*) test_data ;

	/* Make this a macro so gdb steps over it in one go. */
	CONVERT_DATA (k, DATA_LENGTH, orig, orig_data) ;

	items = DATA_LENGTH ;

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;

	sf_set_string (file, SF_STR_ARTIST, "Your name here") ;

	test_write_short_or_die (file, 0, orig, items, __LINE__) ;

	/* Add non-audio data after the audio. */
	sf_set_string (file, SF_STR_COPYRIGHT, "Copyright (c) 2003") ;

	sf_close (file) ;

	memset (test, 0, items * sizeof (short)) ;

	if ((filetype & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
		memset (&sfinfo, 0, sizeof (sfinfo)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Mono : Returned format incorrect (0x%08X => 0x%08X).\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < items)
	{	printf ("\n\nLine %d : Mono : Incorrect number of frames in file (too short). (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), items) ;
		exit (1) ;
		} ;

	if (! long_file_ok && sfinfo.frames > items)
	{	printf ("\n\nLine %d : Mono : Incorrect number of frames in file (too long). (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), items) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d : Mono : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file) ;

	test_read_short_or_die (file, 0, test, items, __LINE__) ;
	for (k = 0 ; k < items ; k++)
		if (CHAR_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d: Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to start of file. */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	test_read_short_or_die (file, 0, test, 4, __LINE__) ;
	for (k = 0 ; k < 4 ; k++)
		if (CHAR_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	if ((filetype & SF_FORMAT_SUBMASK) == SF_FORMAT_DWVW_16 ||
			(filetype & SF_FORMAT_SUBMASK) == SF_FORMAT_DWVW_24)
	{	unlink (filename) ;
		printf ("ok\n") ;
		return ;
		} ;

	/* Seek to offset from start of file. */
	test_seek_or_die (file, 10, SEEK_SET, 10, sfinfo.channels, __LINE__) ;

	test_read_short_or_die (file, 0, test + 10, 4, __LINE__) ;
	for (k = 10 ; k < 14 ; k++)
		if (CHAR_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from current position. */
	test_seek_or_die (file, 6, SEEK_CUR, 20, sfinfo.channels, __LINE__) ;

	test_read_short_or_die (file, 0, test + 20, 4, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (CHAR_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from end of file. */
	test_seek_or_die (file, -(sfinfo.frames - 10), SEEK_END, 10, sfinfo.channels, __LINE__) ;

	test_read_short_or_die (file, 0, test + 10, 4, __LINE__) ;
	for (k = 10 ; k < 14 ; k++)
		if (CHAR_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample D (#%d : 0x%X => 0x%X).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Check read past end of file followed by sf_seek (sndfile, 0, SEEK_CUR). */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	count = 0 ;
	while (count < sfinfo.frames)
		count += sf_read_short (file, test, 311) ;

	/* Check that no error has occurred. */
	if (sf_error (file))
	{	printf ("\n\nLine %d : Mono : error where there shouldn't have been one.\n", __LINE__) ;
		puts (sf_strerror (file)) ;
		exit (1) ;
		} ;

	/* Check that we haven't read beyond EOF. */
	if (count > sfinfo.frames)
	{	printf ("\n\nLines %d : read past end of file (%ld should be %ld)\n", __LINE__, (long) count, (long) sfinfo.frames) ;
		exit (1) ;
		} ;

	test_seek_or_die (file, 0, SEEK_CUR, sfinfo.frames, sfinfo.channels, __LINE__) ;

	sf_close (file) ;

	/*==================================================================================
	** Now test Mono RDWR.
	*/

	sfinfo.samplerate = SAMPLE_RATE ;
	sfinfo.frames     = DATA_LENGTH ;
	sfinfo.channels   = 1 ;
	sfinfo.format 	  = filetype ;

	if ((filetype & SF_FORMAT_TYPEMASK) == SF_FORMAT_RAW)
		unlink (filename) ;
	else
	{	/* Create a short file. */
		create_short_file (filename) ;

		/* Opening a already existing short file (ie invalid header) RDWR is disallowed.
		** If this returns a valif pointer sf_open() screwed up.
		*/
		if ((file = sf_open (filename, SFM_RDWR, &sfinfo)))
		{	printf ("\n\nLine %d: sf_open should (SFM_RDWR) have failed but didn't.\n", __LINE__) ;
			exit (1) ;
			} ;

		/* Truncate the file to zero bytes. */
		if (truncate (filename, 0) < 0)
		{	printf ("\n\nLine %d: truncate (%s) failed", __LINE__, filename) ;
			perror (NULL) ;
			exit (1) ;
			} ;
		} ;

	/* Opening a zero length file RDWR is allowed, but the SF_INFO struct must contain
	** all the usual data required when opening the file in WRITE mode.
	*/
	sfinfo.samplerate = SAMPLE_RATE ;
	sfinfo.frames     = DATA_LENGTH ;
	sfinfo.channels   = 1 ;
	sfinfo.format 	  = filetype ;

	file = test_open_file_or_die (filename, SFM_RDWR, &sfinfo, __LINE__) ;

	/* Do 3 writes followed by reads. After each, check the data and the current
	** read and write offsets.
	*/
	for (pass = 1 ; pass <= 3 ; pass ++)
	{	orig [20] = pass * 2 ;

		/* Write some data. */
		test_write_short_or_die (file, 0, orig, DATA_LENGTH, __LINE__) ;

		test_read_write_position_or_die (file, __LINE__, pass, (pass - 1) * DATA_LENGTH, pass * DATA_LENGTH) ;

		/* Read what we just wrote. */
		test_read_short_or_die (file, 0, test, DATA_LENGTH, __LINE__) ;

		/* Check the data. */
		for (k = 0 ; k < DATA_LENGTH ; k++)
			if (CHAR_ERROR (orig [k], test [k]))
			{	printf ("\n\nLine %d (pass %d): Error at sample %d (0x%X => 0x%X).\n", __LINE__, pass, k, orig [k], test [k]) ;
				oct_save_short (orig, test, DATA_LENGTH) ;
				exit (1) ;
				} ;

		test_read_write_position_or_die (file, __LINE__, pass, pass * DATA_LENGTH, pass * DATA_LENGTH) ;
		} ; /* for (pass ...) */

	sf_close (file) ;

	/* Open the file again to check the data. */
	file = test_open_file_or_die (filename, SFM_RDWR, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Returned format incorrect (0x%08X => 0x%08X).\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < 3 * DATA_LENGTH)
	{	printf ("\n\nLine %d : Not enough frames in file. (%ld < %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), 3 * DATA_LENGTH ) ;
		exit (1) ;
		}

	if (! long_file_ok && sfinfo.frames != 3 * DATA_LENGTH)
	{	printf ("\n\nLine %d : Incorrect number of frames in file. (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), 3 * DATA_LENGTH ) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	if (! long_file_ok)
		test_read_write_position_or_die (file, __LINE__, 0, 0, 3 * DATA_LENGTH) ;
	else
		test_seek_or_die (file, 3 * DATA_LENGTH, SFM_WRITE | SEEK_SET, 3 * DATA_LENGTH, sfinfo.channels, __LINE__) ;


	for (pass = 1 ; pass <= 3 ; pass ++)
	{	orig [20] = pass * 2 ;

		test_read_write_position_or_die (file, __LINE__, pass, (pass - 1) * DATA_LENGTH, 3 * DATA_LENGTH) ;

		/* Read what we just wrote. */
		test_read_short_or_die (file, pass, test, DATA_LENGTH, __LINE__) ;

		/* Check the data. */
		for (k = 0 ; k < DATA_LENGTH ; k++)
			if (CHAR_ERROR (orig [k], test [k]))
			{	printf ("\n\nLine %d (pass %d): Error at sample %d (0x%X => 0x%X).\n", __LINE__, pass, k, orig [k], test [k]) ;
				oct_save_short (orig, test, DATA_LENGTH) ;
				exit (1) ;
				} ;

		} ; /* for (pass ...) */

	sf_close (file) ;

	/*==================================================================================
	** Now test Stereo.
	*/

	sfinfo.samplerate  = 44100 ;
	sfinfo.frames     = SILLY_WRITE_COUNT ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels    = 2 ;
	sfinfo.format 	   = filetype ;

	if (! sf_format_check (&sfinfo))
	{	unlink (filename) ;
		printf ("ok, (no stereo)\n") ;
		return ;
		} ;

	gen_windowed_sine (orig_data, DATA_LENGTH, 32000.0) ;

	orig = (short*) orig_data ;
	test = (short*) test_data ;

	/* Make this a macro so gdb steps over it in one go. */
	CONVERT_DATA (k, DATA_LENGTH, orig, orig_data) ;

	items = DATA_LENGTH ;
	frames = items / sfinfo.channels ;

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;

	sf_set_string (file, SF_STR_ARTIST, "Your name here") ;

	test_writef_short_or_die (file, 0, orig, frames, __LINE__) ;

	sf_set_string (file, SF_STR_COPYRIGHT, "Copyright (c) 2003") ;

	sf_close (file) ;

	memset (test, 0, items * sizeof (short)) ;

	if ((filetype & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
		memset (&sfinfo, 0, sizeof (sfinfo)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Stereo : Returned format incorrect (0x%08X => 0x%08X).\n",
				__LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < frames)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of frames in file (too short). (%ld should be %d)\n",
				__LINE__, SF_COUNT_TO_LONG (sfinfo.frames), frames) ;
		exit (1) ;
		} ;

	if (! long_file_ok && sfinfo.frames > frames)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of frames in file (too long). (%ld should be %d)\n",
				__LINE__, SF_COUNT_TO_LONG (sfinfo.frames), frames) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 2)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file) ;

	test_readf_short_or_die (file, 0, test, frames, __LINE__) ;
	for (k = 0 ; k < items ; k++)
		if (CHAR_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to start of file. */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	test_readf_short_or_die (file, 0, test, 2, __LINE__) ;
	for (k = 0 ; k < 4 ; k++)
		if (CHAR_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from start of file. */
	test_seek_or_die (file, 10, SEEK_SET, 10, sfinfo.channels, __LINE__) ;

	/* Check for errors here. */
	if (sf_error (file))
	{	printf ("Line %d: Should NOT return an error.\n", __LINE__) ;
		puts (sf_strerror (file)) ;
		exit (1) ;
		} ;

	if (sf_read_short (file, test, 1) > 0)
	{	printf ("Line %d: Should return 0.\n", __LINE__) ;
		exit (1) ;
		} ;

	if (! sf_error (file))
	{	printf ("Line %d: Should return an error.\n", __LINE__) ;
		exit (1) ;
		} ;
	/*-----------------------*/

	test_readf_short_or_die (file, 0, test + 10, 2, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (CHAR_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from current position. */
	test_seek_or_die (file, 8, SEEK_CUR, 20, sfinfo.channels, __LINE__) ;

	test_readf_short_or_die (file, 0, test + 20, 2, __LINE__) ;
	for (k = 40 ; k < 44 ; k++)
		if (CHAR_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from end of file. */
	test_seek_or_die (file, -(sfinfo.frames - 10), SEEK_END, 10, sfinfo.channels, __LINE__) ;

	test_readf_short_or_die (file, 0, test + 20, 2, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (CHAR_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	sf_close (file) ;

	unlink (filename) ;
	printf ("ok\n") ;

	return ;
} /* pcm_test_char */


/*======================================================================================
*/

static void
pcm_test_short (char *filename, int filetype, int long_file_ok)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	short		*orig, *test ;
	sf_count_t	count ;
	int			k, items, frames, pass ;

	print_test_name ("pcm_test_short", filename) ;

	sfinfo.samplerate  = 44100 ;
	sfinfo.frames     = SILLY_WRITE_COUNT ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels    = 1 ;
	sfinfo.format 	   = filetype ;

	gen_windowed_sine (orig_data, DATA_LENGTH, 32000.0) ;

	orig = (short*) orig_data ;
	test = (short*) test_data ;

	/* Make this a macro so gdb steps over it in one go. */
	CONVERT_DATA (k, DATA_LENGTH, orig, orig_data) ;

	items = DATA_LENGTH ;

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;

	sf_set_string (file, SF_STR_ARTIST, "Your name here") ;

	test_write_short_or_die (file, 0, orig, items, __LINE__) ;

	/* Add non-audio data after the audio. */
	sf_set_string (file, SF_STR_COPYRIGHT, "Copyright (c) 2003") ;

	sf_close (file) ;

	memset (test, 0, items * sizeof (short)) ;

	if ((filetype & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
		memset (&sfinfo, 0, sizeof (sfinfo)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Mono : Returned format incorrect (0x%08X => 0x%08X).\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < items)
	{	printf ("\n\nLine %d : Mono : Incorrect number of frames in file (too short). (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), items) ;
		exit (1) ;
		} ;

	if (! long_file_ok && sfinfo.frames > items)
	{	printf ("\n\nLine %d : Mono : Incorrect number of frames in file (too long). (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), items) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d : Mono : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file) ;

	test_read_short_or_die (file, 0, test, items, __LINE__) ;
	for (k = 0 ; k < items ; k++)
		if (INT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d: Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to start of file. */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	test_read_short_or_die (file, 0, test, 4, __LINE__) ;
	for (k = 0 ; k < 4 ; k++)
		if (INT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	if ((filetype & SF_FORMAT_SUBMASK) == SF_FORMAT_DWVW_16 ||
			(filetype & SF_FORMAT_SUBMASK) == SF_FORMAT_DWVW_24)
	{	unlink (filename) ;
		printf ("ok\n") ;
		return ;
		} ;

	/* Seek to offset from start of file. */
	test_seek_or_die (file, 10, SEEK_SET, 10, sfinfo.channels, __LINE__) ;

	test_read_short_or_die (file, 0, test + 10, 4, __LINE__) ;
	for (k = 10 ; k < 14 ; k++)
		if (INT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from current position. */
	test_seek_or_die (file, 6, SEEK_CUR, 20, sfinfo.channels, __LINE__) ;

	test_read_short_or_die (file, 0, test + 20, 4, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (INT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from end of file. */
	test_seek_or_die (file, -(sfinfo.frames - 10), SEEK_END, 10, sfinfo.channels, __LINE__) ;

	test_read_short_or_die (file, 0, test + 10, 4, __LINE__) ;
	for (k = 10 ; k < 14 ; k++)
		if (INT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample D (#%d : 0x%X => 0x%X).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Check read past end of file followed by sf_seek (sndfile, 0, SEEK_CUR). */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	count = 0 ;
	while (count < sfinfo.frames)
		count += sf_read_short (file, test, 311) ;

	/* Check that no error has occurred. */
	if (sf_error (file))
	{	printf ("\n\nLine %d : Mono : error where there shouldn't have been one.\n", __LINE__) ;
		puts (sf_strerror (file)) ;
		exit (1) ;
		} ;

	/* Check that we haven't read beyond EOF. */
	if (count > sfinfo.frames)
	{	printf ("\n\nLines %d : read past end of file (%ld should be %ld)\n", __LINE__, (long) count, (long) sfinfo.frames) ;
		exit (1) ;
		} ;

	test_seek_or_die (file, 0, SEEK_CUR, sfinfo.frames, sfinfo.channels, __LINE__) ;

	sf_close (file) ;

	/*==================================================================================
	** Now test Mono RDWR.
	*/

	sfinfo.samplerate = SAMPLE_RATE ;
	sfinfo.frames     = DATA_LENGTH ;
	sfinfo.channels   = 1 ;
	sfinfo.format 	  = filetype ;

	if ((filetype & SF_FORMAT_TYPEMASK) == SF_FORMAT_RAW)
		unlink (filename) ;
	else
	{	/* Create a short file. */
		create_short_file (filename) ;

		/* Opening a already existing short file (ie invalid header) RDWR is disallowed.
		** If this returns a valif pointer sf_open() screwed up.
		*/
		if ((file = sf_open (filename, SFM_RDWR, &sfinfo)))
		{	printf ("\n\nLine %d: sf_open should (SFM_RDWR) have failed but didn't.\n", __LINE__) ;
			exit (1) ;
			} ;

		/* Truncate the file to zero bytes. */
		if (truncate (filename, 0) < 0)
		{	printf ("\n\nLine %d: truncate (%s) failed", __LINE__, filename) ;
			perror (NULL) ;
			exit (1) ;
			} ;
		} ;

	/* Opening a zero length file RDWR is allowed, but the SF_INFO struct must contain
	** all the usual data required when opening the file in WRITE mode.
	*/
	sfinfo.samplerate = SAMPLE_RATE ;
	sfinfo.frames     = DATA_LENGTH ;
	sfinfo.channels   = 1 ;
	sfinfo.format 	  = filetype ;

	file = test_open_file_or_die (filename, SFM_RDWR, &sfinfo, __LINE__) ;

	/* Do 3 writes followed by reads. After each, check the data and the current
	** read and write offsets.
	*/
	for (pass = 1 ; pass <= 3 ; pass ++)
	{	orig [20] = pass * 2 ;

		/* Write some data. */
		test_write_short_or_die (file, 0, orig, DATA_LENGTH, __LINE__) ;

		test_read_write_position_or_die (file, __LINE__, pass, (pass - 1) * DATA_LENGTH, pass * DATA_LENGTH) ;

		/* Read what we just wrote. */
		test_read_short_or_die (file, 0, test, DATA_LENGTH, __LINE__) ;

		/* Check the data. */
		for (k = 0 ; k < DATA_LENGTH ; k++)
			if (INT_ERROR (orig [k], test [k]))
			{	printf ("\n\nLine %d (pass %d): Error at sample %d (0x%X => 0x%X).\n", __LINE__, pass, k, orig [k], test [k]) ;
				oct_save_short (orig, test, DATA_LENGTH) ;
				exit (1) ;
				} ;

		test_read_write_position_or_die (file, __LINE__, pass, pass * DATA_LENGTH, pass * DATA_LENGTH) ;
		} ; /* for (pass ...) */

	sf_close (file) ;

	/* Open the file again to check the data. */
	file = test_open_file_or_die (filename, SFM_RDWR, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Returned format incorrect (0x%08X => 0x%08X).\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < 3 * DATA_LENGTH)
	{	printf ("\n\nLine %d : Not enough frames in file. (%ld < %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), 3 * DATA_LENGTH ) ;
		exit (1) ;
		}

	if (! long_file_ok && sfinfo.frames != 3 * DATA_LENGTH)
	{	printf ("\n\nLine %d : Incorrect number of frames in file. (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), 3 * DATA_LENGTH ) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	if (! long_file_ok)
		test_read_write_position_or_die (file, __LINE__, 0, 0, 3 * DATA_LENGTH) ;
	else
		test_seek_or_die (file, 3 * DATA_LENGTH, SFM_WRITE | SEEK_SET, 3 * DATA_LENGTH, sfinfo.channels, __LINE__) ;


	for (pass = 1 ; pass <= 3 ; pass ++)
	{	orig [20] = pass * 2 ;

		test_read_write_position_or_die (file, __LINE__, pass, (pass - 1) * DATA_LENGTH, 3 * DATA_LENGTH) ;

		/* Read what we just wrote. */
		test_read_short_or_die (file, pass, test, DATA_LENGTH, __LINE__) ;

		/* Check the data. */
		for (k = 0 ; k < DATA_LENGTH ; k++)
			if (INT_ERROR (orig [k], test [k]))
			{	printf ("\n\nLine %d (pass %d): Error at sample %d (0x%X => 0x%X).\n", __LINE__, pass, k, orig [k], test [k]) ;
				oct_save_short (orig, test, DATA_LENGTH) ;
				exit (1) ;
				} ;

		} ; /* for (pass ...) */

	sf_close (file) ;

	/*==================================================================================
	** Now test Stereo.
	*/

	sfinfo.samplerate  = 44100 ;
	sfinfo.frames     = SILLY_WRITE_COUNT ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels    = 2 ;
	sfinfo.format 	   = filetype ;

	if (! sf_format_check (&sfinfo))
	{	unlink (filename) ;
		printf ("ok, (no stereo)\n") ;
		return ;
		} ;

	gen_windowed_sine (orig_data, DATA_LENGTH, 32000.0) ;

	orig = (short*) orig_data ;
	test = (short*) test_data ;

	/* Make this a macro so gdb steps over it in one go. */
	CONVERT_DATA (k, DATA_LENGTH, orig, orig_data) ;

	items = DATA_LENGTH ;
	frames = items / sfinfo.channels ;

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;

	sf_set_string (file, SF_STR_ARTIST, "Your name here") ;

	test_writef_short_or_die (file, 0, orig, frames, __LINE__) ;

	sf_set_string (file, SF_STR_COPYRIGHT, "Copyright (c) 2003") ;

	sf_close (file) ;

	memset (test, 0, items * sizeof (short)) ;

	if ((filetype & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
		memset (&sfinfo, 0, sizeof (sfinfo)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Stereo : Returned format incorrect (0x%08X => 0x%08X).\n",
				__LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < frames)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of frames in file (too short). (%ld should be %d)\n",
				__LINE__, SF_COUNT_TO_LONG (sfinfo.frames), frames) ;
		exit (1) ;
		} ;

	if (! long_file_ok && sfinfo.frames > frames)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of frames in file (too long). (%ld should be %d)\n",
				__LINE__, SF_COUNT_TO_LONG (sfinfo.frames), frames) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 2)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file) ;

	test_readf_short_or_die (file, 0, test, frames, __LINE__) ;
	for (k = 0 ; k < items ; k++)
		if (INT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to start of file. */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	test_readf_short_or_die (file, 0, test, 2, __LINE__) ;
	for (k = 0 ; k < 4 ; k++)
		if (INT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from start of file. */
	test_seek_or_die (file, 10, SEEK_SET, 10, sfinfo.channels, __LINE__) ;

	/* Check for errors here. */
	if (sf_error (file))
	{	printf ("Line %d: Should NOT return an error.\n", __LINE__) ;
		puts (sf_strerror (file)) ;
		exit (1) ;
		} ;

	if (sf_read_short (file, test, 1) > 0)
	{	printf ("Line %d: Should return 0.\n", __LINE__) ;
		exit (1) ;
		} ;

	if (! sf_error (file))
	{	printf ("Line %d: Should return an error.\n", __LINE__) ;
		exit (1) ;
		} ;
	/*-----------------------*/

	test_readf_short_or_die (file, 0, test + 10, 2, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (INT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from current position. */
	test_seek_or_die (file, 8, SEEK_CUR, 20, sfinfo.channels, __LINE__) ;

	test_readf_short_or_die (file, 0, test + 20, 2, __LINE__) ;
	for (k = 40 ; k < 44 ; k++)
		if (INT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from end of file. */
	test_seek_or_die (file, -(sfinfo.frames - 10), SEEK_END, 10, sfinfo.channels, __LINE__) ;

	test_readf_short_or_die (file, 0, test + 20, 2, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (INT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	sf_close (file) ;

	unlink (filename) ;
	printf ("ok\n") ;

	return ;
} /* pcm_test_short */


/*======================================================================================
*/

static void
pcm_test_24bit (char *filename, int filetype, int long_file_ok)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	int		*orig, *test ;
	sf_count_t	count ;
	int			k, items, frames, pass ;

	print_test_name ("pcm_test_24bit", filename) ;

	sfinfo.samplerate  = 44100 ;
	sfinfo.frames     = SILLY_WRITE_COUNT ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels    = 1 ;
	sfinfo.format 	   = filetype ;

	gen_windowed_sine (orig_data, DATA_LENGTH, (1.0 * 0x7F000000)) ;

	orig = (int*) orig_data ;
	test = (int*) test_data ;

	/* Make this a macro so gdb steps over it in one go. */
	CONVERT_DATA (k, DATA_LENGTH, orig, orig_data) ;

	items = DATA_LENGTH ;

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;

	sf_set_string (file, SF_STR_ARTIST, "Your name here") ;

	test_write_int_or_die (file, 0, orig, items, __LINE__) ;

	/* Add non-audio data after the audio. */
	sf_set_string (file, SF_STR_COPYRIGHT, "Copyright (c) 2003") ;

	sf_close (file) ;

	memset (test, 0, items * sizeof (int)) ;

	if ((filetype & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
		memset (&sfinfo, 0, sizeof (sfinfo)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Mono : Returned format incorrect (0x%08X => 0x%08X).\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < items)
	{	printf ("\n\nLine %d : Mono : Incorrect number of frames in file (too short). (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), items) ;
		exit (1) ;
		} ;

	if (! long_file_ok && sfinfo.frames > items)
	{	printf ("\n\nLine %d : Mono : Incorrect number of frames in file (too long). (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), items) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d : Mono : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file) ;

	test_read_int_or_die (file, 0, test, items, __LINE__) ;
	for (k = 0 ; k < items ; k++)
		if (TRIBYTE_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d: Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to start of file. */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	test_read_int_or_die (file, 0, test, 4, __LINE__) ;
	for (k = 0 ; k < 4 ; k++)
		if (TRIBYTE_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	if ((filetype & SF_FORMAT_SUBMASK) == SF_FORMAT_DWVW_16 ||
			(filetype & SF_FORMAT_SUBMASK) == SF_FORMAT_DWVW_24)
	{	unlink (filename) ;
		printf ("ok\n") ;
		return ;
		} ;

	/* Seek to offset from start of file. */
	test_seek_or_die (file, 10, SEEK_SET, 10, sfinfo.channels, __LINE__) ;

	test_read_int_or_die (file, 0, test + 10, 4, __LINE__) ;
	for (k = 10 ; k < 14 ; k++)
		if (TRIBYTE_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from current position. */
	test_seek_or_die (file, 6, SEEK_CUR, 20, sfinfo.channels, __LINE__) ;

	test_read_int_or_die (file, 0, test + 20, 4, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (TRIBYTE_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from end of file. */
	test_seek_or_die (file, -(sfinfo.frames - 10), SEEK_END, 10, sfinfo.channels, __LINE__) ;

	test_read_int_or_die (file, 0, test + 10, 4, __LINE__) ;
	for (k = 10 ; k < 14 ; k++)
		if (TRIBYTE_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample D (#%d : 0x%X => 0x%X).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Check read past end of file followed by sf_seek (sndfile, 0, SEEK_CUR). */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	count = 0 ;
	while (count < sfinfo.frames)
		count += sf_read_int (file, test, 311) ;

	/* Check that no error has occurred. */
	if (sf_error (file))
	{	printf ("\n\nLine %d : Mono : error where there shouldn't have been one.\n", __LINE__) ;
		puts (sf_strerror (file)) ;
		exit (1) ;
		} ;

	/* Check that we haven't read beyond EOF. */
	if (count > sfinfo.frames)
	{	printf ("\n\nLines %d : read past end of file (%ld should be %ld)\n", __LINE__, (long) count, (long) sfinfo.frames) ;
		exit (1) ;
		} ;

	test_seek_or_die (file, 0, SEEK_CUR, sfinfo.frames, sfinfo.channels, __LINE__) ;

	sf_close (file) ;

	/*==================================================================================
	** Now test Mono RDWR.
	*/

	sfinfo.samplerate = SAMPLE_RATE ;
	sfinfo.frames     = DATA_LENGTH ;
	sfinfo.channels   = 1 ;
	sfinfo.format 	  = filetype ;

	if ((filetype & SF_FORMAT_TYPEMASK) == SF_FORMAT_RAW)
		unlink (filename) ;
	else
	{	/* Create a short file. */
		create_short_file (filename) ;

		/* Opening a already existing short file (ie invalid header) RDWR is disallowed.
		** If this returns a valif pointer sf_open() screwed up.
		*/
		if ((file = sf_open (filename, SFM_RDWR, &sfinfo)))
		{	printf ("\n\nLine %d: sf_open should (SFM_RDWR) have failed but didn't.\n", __LINE__) ;
			exit (1) ;
			} ;

		/* Truncate the file to zero bytes. */
		if (truncate (filename, 0) < 0)
		{	printf ("\n\nLine %d: truncate (%s) failed", __LINE__, filename) ;
			perror (NULL) ;
			exit (1) ;
			} ;
		} ;

	/* Opening a zero length file RDWR is allowed, but the SF_INFO struct must contain
	** all the usual data required when opening the file in WRITE mode.
	*/
	sfinfo.samplerate = SAMPLE_RATE ;
	sfinfo.frames     = DATA_LENGTH ;
	sfinfo.channels   = 1 ;
	sfinfo.format 	  = filetype ;

	file = test_open_file_or_die (filename, SFM_RDWR, &sfinfo, __LINE__) ;

	/* Do 3 writes followed by reads. After each, check the data and the current
	** read and write offsets.
	*/
	for (pass = 1 ; pass <= 3 ; pass ++)
	{	orig [20] = pass * 2 ;

		/* Write some data. */
		test_write_int_or_die (file, 0, orig, DATA_LENGTH, __LINE__) ;

		test_read_write_position_or_die (file, __LINE__, pass, (pass - 1) * DATA_LENGTH, pass * DATA_LENGTH) ;

		/* Read what we just wrote. */
		test_read_int_or_die (file, 0, test, DATA_LENGTH, __LINE__) ;

		/* Check the data. */
		for (k = 0 ; k < DATA_LENGTH ; k++)
			if (TRIBYTE_ERROR (orig [k], test [k]))
			{	printf ("\n\nLine %d (pass %d): Error at sample %d (0x%X => 0x%X).\n", __LINE__, pass, k, orig [k], test [k]) ;
				oct_save_int (orig, test, DATA_LENGTH) ;
				exit (1) ;
				} ;

		test_read_write_position_or_die (file, __LINE__, pass, pass * DATA_LENGTH, pass * DATA_LENGTH) ;
		} ; /* for (pass ...) */

	sf_close (file) ;

	/* Open the file again to check the data. */
	file = test_open_file_or_die (filename, SFM_RDWR, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Returned format incorrect (0x%08X => 0x%08X).\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < 3 * DATA_LENGTH)
	{	printf ("\n\nLine %d : Not enough frames in file. (%ld < %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), 3 * DATA_LENGTH ) ;
		exit (1) ;
		}

	if (! long_file_ok && sfinfo.frames != 3 * DATA_LENGTH)
	{	printf ("\n\nLine %d : Incorrect number of frames in file. (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), 3 * DATA_LENGTH ) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	if (! long_file_ok)
		test_read_write_position_or_die (file, __LINE__, 0, 0, 3 * DATA_LENGTH) ;
	else
		test_seek_or_die (file, 3 * DATA_LENGTH, SFM_WRITE | SEEK_SET, 3 * DATA_LENGTH, sfinfo.channels, __LINE__) ;


	for (pass = 1 ; pass <= 3 ; pass ++)
	{	orig [20] = pass * 2 ;

		test_read_write_position_or_die (file, __LINE__, pass, (pass - 1) * DATA_LENGTH, 3 * DATA_LENGTH) ;

		/* Read what we just wrote. */
		test_read_int_or_die (file, pass, test, DATA_LENGTH, __LINE__) ;

		/* Check the data. */
		for (k = 0 ; k < DATA_LENGTH ; k++)
			if (TRIBYTE_ERROR (orig [k], test [k]))
			{	printf ("\n\nLine %d (pass %d): Error at sample %d (0x%X => 0x%X).\n", __LINE__, pass, k, orig [k], test [k]) ;
				oct_save_int (orig, test, DATA_LENGTH) ;
				exit (1) ;
				} ;

		} ; /* for (pass ...) */

	sf_close (file) ;

	/*==================================================================================
	** Now test Stereo.
	*/

	sfinfo.samplerate  = 44100 ;
	sfinfo.frames     = SILLY_WRITE_COUNT ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels    = 2 ;
	sfinfo.format 	   = filetype ;

	if (! sf_format_check (&sfinfo))
	{	unlink (filename) ;
		printf ("ok, (no stereo)\n") ;
		return ;
		} ;

	gen_windowed_sine (orig_data, DATA_LENGTH, (1.0 * 0x7F000000)) ;

	orig = (int*) orig_data ;
	test = (int*) test_data ;

	/* Make this a macro so gdb steps over it in one go. */
	CONVERT_DATA (k, DATA_LENGTH, orig, orig_data) ;

	items = DATA_LENGTH ;
	frames = items / sfinfo.channels ;

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;

	sf_set_string (file, SF_STR_ARTIST, "Your name here") ;

	test_writef_int_or_die (file, 0, orig, frames, __LINE__) ;

	sf_set_string (file, SF_STR_COPYRIGHT, "Copyright (c) 2003") ;

	sf_close (file) ;

	memset (test, 0, items * sizeof (int)) ;

	if ((filetype & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
		memset (&sfinfo, 0, sizeof (sfinfo)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Stereo : Returned format incorrect (0x%08X => 0x%08X).\n",
				__LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < frames)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of frames in file (too short). (%ld should be %d)\n",
				__LINE__, SF_COUNT_TO_LONG (sfinfo.frames), frames) ;
		exit (1) ;
		} ;

	if (! long_file_ok && sfinfo.frames > frames)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of frames in file (too long). (%ld should be %d)\n",
				__LINE__, SF_COUNT_TO_LONG (sfinfo.frames), frames) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 2)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file) ;

	test_readf_int_or_die (file, 0, test, frames, __LINE__) ;
	for (k = 0 ; k < items ; k++)
		if (TRIBYTE_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to start of file. */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	test_readf_int_or_die (file, 0, test, 2, __LINE__) ;
	for (k = 0 ; k < 4 ; k++)
		if (TRIBYTE_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from start of file. */
	test_seek_or_die (file, 10, SEEK_SET, 10, sfinfo.channels, __LINE__) ;

	/* Check for errors here. */
	if (sf_error (file))
	{	printf ("Line %d: Should NOT return an error.\n", __LINE__) ;
		puts (sf_strerror (file)) ;
		exit (1) ;
		} ;

	if (sf_read_int (file, test, 1) > 0)
	{	printf ("Line %d: Should return 0.\n", __LINE__) ;
		exit (1) ;
		} ;

	if (! sf_error (file))
	{	printf ("Line %d: Should return an error.\n", __LINE__) ;
		exit (1) ;
		} ;
	/*-----------------------*/

	test_readf_int_or_die (file, 0, test + 10, 2, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (TRIBYTE_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from current position. */
	test_seek_or_die (file, 8, SEEK_CUR, 20, sfinfo.channels, __LINE__) ;

	test_readf_int_or_die (file, 0, test + 20, 2, __LINE__) ;
	for (k = 40 ; k < 44 ; k++)
		if (TRIBYTE_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from end of file. */
	test_seek_or_die (file, -(sfinfo.frames - 10), SEEK_END, 10, sfinfo.channels, __LINE__) ;

	test_readf_int_or_die (file, 0, test + 20, 2, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (TRIBYTE_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	sf_close (file) ;

	unlink (filename) ;
	printf ("ok\n") ;

	return ;
} /* pcm_test_24bit */


/*======================================================================================
*/

static void
pcm_test_int (char *filename, int filetype, int long_file_ok)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	int		*orig, *test ;
	sf_count_t	count ;
	int			k, items, frames, pass ;

	print_test_name ("pcm_test_int", filename) ;

	sfinfo.samplerate  = 44100 ;
	sfinfo.frames     = SILLY_WRITE_COUNT ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels    = 1 ;
	sfinfo.format 	   = filetype ;

	gen_windowed_sine (orig_data, DATA_LENGTH, (1.0 * 0x7F000000)) ;

	orig = (int*) orig_data ;
	test = (int*) test_data ;

	/* Make this a macro so gdb steps over it in one go. */
	CONVERT_DATA (k, DATA_LENGTH, orig, orig_data) ;

	items = DATA_LENGTH ;

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;

	sf_set_string (file, SF_STR_ARTIST, "Your name here") ;

	test_write_int_or_die (file, 0, orig, items, __LINE__) ;

	/* Add non-audio data after the audio. */
	sf_set_string (file, SF_STR_COPYRIGHT, "Copyright (c) 2003") ;

	sf_close (file) ;

	memset (test, 0, items * sizeof (int)) ;

	if ((filetype & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
		memset (&sfinfo, 0, sizeof (sfinfo)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Mono : Returned format incorrect (0x%08X => 0x%08X).\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < items)
	{	printf ("\n\nLine %d : Mono : Incorrect number of frames in file (too short). (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), items) ;
		exit (1) ;
		} ;

	if (! long_file_ok && sfinfo.frames > items)
	{	printf ("\n\nLine %d : Mono : Incorrect number of frames in file (too long). (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), items) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d : Mono : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file) ;

	test_read_int_or_die (file, 0, test, items, __LINE__) ;
	for (k = 0 ; k < items ; k++)
		if (INT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d: Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to start of file. */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	test_read_int_or_die (file, 0, test, 4, __LINE__) ;
	for (k = 0 ; k < 4 ; k++)
		if (INT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	if ((filetype & SF_FORMAT_SUBMASK) == SF_FORMAT_DWVW_16 ||
			(filetype & SF_FORMAT_SUBMASK) == SF_FORMAT_DWVW_24)
	{	unlink (filename) ;
		printf ("ok\n") ;
		return ;
		} ;

	/* Seek to offset from start of file. */
	test_seek_or_die (file, 10, SEEK_SET, 10, sfinfo.channels, __LINE__) ;

	test_read_int_or_die (file, 0, test + 10, 4, __LINE__) ;
	for (k = 10 ; k < 14 ; k++)
		if (INT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from current position. */
	test_seek_or_die (file, 6, SEEK_CUR, 20, sfinfo.channels, __LINE__) ;

	test_read_int_or_die (file, 0, test + 20, 4, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (INT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : 0x%X => 0x%X).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from end of file. */
	test_seek_or_die (file, -(sfinfo.frames - 10), SEEK_END, 10, sfinfo.channels, __LINE__) ;

	test_read_int_or_die (file, 0, test + 10, 4, __LINE__) ;
	for (k = 10 ; k < 14 ; k++)
		if (INT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample D (#%d : 0x%X => 0x%X).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Check read past end of file followed by sf_seek (sndfile, 0, SEEK_CUR). */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	count = 0 ;
	while (count < sfinfo.frames)
		count += sf_read_int (file, test, 311) ;

	/* Check that no error has occurred. */
	if (sf_error (file))
	{	printf ("\n\nLine %d : Mono : error where there shouldn't have been one.\n", __LINE__) ;
		puts (sf_strerror (file)) ;
		exit (1) ;
		} ;

	/* Check that we haven't read beyond EOF. */
	if (count > sfinfo.frames)
	{	printf ("\n\nLines %d : read past end of file (%ld should be %ld)\n", __LINE__, (long) count, (long) sfinfo.frames) ;
		exit (1) ;
		} ;

	test_seek_or_die (file, 0, SEEK_CUR, sfinfo.frames, sfinfo.channels, __LINE__) ;

	sf_close (file) ;

	/*==================================================================================
	** Now test Mono RDWR.
	*/

	sfinfo.samplerate = SAMPLE_RATE ;
	sfinfo.frames     = DATA_LENGTH ;
	sfinfo.channels   = 1 ;
	sfinfo.format 	  = filetype ;

	if ((filetype & SF_FORMAT_TYPEMASK) == SF_FORMAT_RAW)
		unlink (filename) ;
	else
	{	/* Create a short file. */
		create_short_file (filename) ;

		/* Opening a already existing short file (ie invalid header) RDWR is disallowed.
		** If this returns a valif pointer sf_open() screwed up.
		*/
		if ((file = sf_open (filename, SFM_RDWR, &sfinfo)))
		{	printf ("\n\nLine %d: sf_open should (SFM_RDWR) have failed but didn't.\n", __LINE__) ;
			exit (1) ;
			} ;

		/* Truncate the file to zero bytes. */
		if (truncate (filename, 0) < 0)
		{	printf ("\n\nLine %d: truncate (%s) failed", __LINE__, filename) ;
			perror (NULL) ;
			exit (1) ;
			} ;
		} ;

	/* Opening a zero length file RDWR is allowed, but the SF_INFO struct must contain
	** all the usual data required when opening the file in WRITE mode.
	*/
	sfinfo.samplerate = SAMPLE_RATE ;
	sfinfo.frames     = DATA_LENGTH ;
	sfinfo.channels   = 1 ;
	sfinfo.format 	  = filetype ;

	file = test_open_file_or_die (filename, SFM_RDWR, &sfinfo, __LINE__) ;

	/* Do 3 writes followed by reads. After each, check the data and the current
	** read and write offsets.
	*/
	for (pass = 1 ; pass <= 3 ; pass ++)
	{	orig [20] = pass * 2 ;

		/* Write some data. */
		test_write_int_or_die (file, 0, orig, DATA_LENGTH, __LINE__) ;

		test_read_write_position_or_die (file, __LINE__, pass, (pass - 1) * DATA_LENGTH, pass * DATA_LENGTH) ;

		/* Read what we just wrote. */
		test_read_int_or_die (file, 0, test, DATA_LENGTH, __LINE__) ;

		/* Check the data. */
		for (k = 0 ; k < DATA_LENGTH ; k++)
			if (INT_ERROR (orig [k], test [k]))
			{	printf ("\n\nLine %d (pass %d): Error at sample %d (0x%X => 0x%X).\n", __LINE__, pass, k, orig [k], test [k]) ;
				oct_save_int (orig, test, DATA_LENGTH) ;
				exit (1) ;
				} ;

		test_read_write_position_or_die (file, __LINE__, pass, pass * DATA_LENGTH, pass * DATA_LENGTH) ;
		} ; /* for (pass ...) */

	sf_close (file) ;

	/* Open the file again to check the data. */
	file = test_open_file_or_die (filename, SFM_RDWR, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Returned format incorrect (0x%08X => 0x%08X).\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < 3 * DATA_LENGTH)
	{	printf ("\n\nLine %d : Not enough frames in file. (%ld < %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), 3 * DATA_LENGTH ) ;
		exit (1) ;
		}

	if (! long_file_ok && sfinfo.frames != 3 * DATA_LENGTH)
	{	printf ("\n\nLine %d : Incorrect number of frames in file. (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), 3 * DATA_LENGTH ) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	if (! long_file_ok)
		test_read_write_position_or_die (file, __LINE__, 0, 0, 3 * DATA_LENGTH) ;
	else
		test_seek_or_die (file, 3 * DATA_LENGTH, SFM_WRITE | SEEK_SET, 3 * DATA_LENGTH, sfinfo.channels, __LINE__) ;


	for (pass = 1 ; pass <= 3 ; pass ++)
	{	orig [20] = pass * 2 ;

		test_read_write_position_or_die (file, __LINE__, pass, (pass - 1) * DATA_LENGTH, 3 * DATA_LENGTH) ;

		/* Read what we just wrote. */
		test_read_int_or_die (file, pass, test, DATA_LENGTH, __LINE__) ;

		/* Check the data. */
		for (k = 0 ; k < DATA_LENGTH ; k++)
			if (INT_ERROR (orig [k], test [k]))
			{	printf ("\n\nLine %d (pass %d): Error at sample %d (0x%X => 0x%X).\n", __LINE__, pass, k, orig [k], test [k]) ;
				oct_save_int (orig, test, DATA_LENGTH) ;
				exit (1) ;
				} ;

		} ; /* for (pass ...) */

	sf_close (file) ;

	/*==================================================================================
	** Now test Stereo.
	*/

	sfinfo.samplerate  = 44100 ;
	sfinfo.frames     = SILLY_WRITE_COUNT ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels    = 2 ;
	sfinfo.format 	   = filetype ;

	if (! sf_format_check (&sfinfo))
	{	unlink (filename) ;
		printf ("ok, (no stereo)\n") ;
		return ;
		} ;

	gen_windowed_sine (orig_data, DATA_LENGTH, (1.0 * 0x7F000000)) ;

	orig = (int*) orig_data ;
	test = (int*) test_data ;

	/* Make this a macro so gdb steps over it in one go. */
	CONVERT_DATA (k, DATA_LENGTH, orig, orig_data) ;

	items = DATA_LENGTH ;
	frames = items / sfinfo.channels ;

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;

	sf_set_string (file, SF_STR_ARTIST, "Your name here") ;

	test_writef_int_or_die (file, 0, orig, frames, __LINE__) ;

	sf_set_string (file, SF_STR_COPYRIGHT, "Copyright (c) 2003") ;

	sf_close (file) ;

	memset (test, 0, items * sizeof (int)) ;

	if ((filetype & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
		memset (&sfinfo, 0, sizeof (sfinfo)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Stereo : Returned format incorrect (0x%08X => 0x%08X).\n",
				__LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < frames)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of frames in file (too short). (%ld should be %d)\n",
				__LINE__, SF_COUNT_TO_LONG (sfinfo.frames), frames) ;
		exit (1) ;
		} ;

	if (! long_file_ok && sfinfo.frames > frames)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of frames in file (too long). (%ld should be %d)\n",
				__LINE__, SF_COUNT_TO_LONG (sfinfo.frames), frames) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 2)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file) ;

	test_readf_int_or_die (file, 0, test, frames, __LINE__) ;
	for (k = 0 ; k < items ; k++)
		if (INT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to start of file. */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	test_readf_int_or_die (file, 0, test, 2, __LINE__) ;
	for (k = 0 ; k < 4 ; k++)
		if (INT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from start of file. */
	test_seek_or_die (file, 10, SEEK_SET, 10, sfinfo.channels, __LINE__) ;

	/* Check for errors here. */
	if (sf_error (file))
	{	printf ("Line %d: Should NOT return an error.\n", __LINE__) ;
		puts (sf_strerror (file)) ;
		exit (1) ;
		} ;

	if (sf_read_int (file, test, 1) > 0)
	{	printf ("Line %d: Should return 0.\n", __LINE__) ;
		exit (1) ;
		} ;

	if (! sf_error (file))
	{	printf ("Line %d: Should return an error.\n", __LINE__) ;
		exit (1) ;
		} ;
	/*-----------------------*/

	test_readf_int_or_die (file, 0, test + 10, 2, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (INT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from current position. */
	test_seek_or_die (file, 8, SEEK_CUR, 20, sfinfo.channels, __LINE__) ;

	test_readf_int_or_die (file, 0, test + 20, 2, __LINE__) ;
	for (k = 40 ; k < 44 ; k++)
		if (INT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from end of file. */
	test_seek_or_die (file, -(sfinfo.frames - 10), SEEK_END, 10, sfinfo.channels, __LINE__) ;

	test_readf_int_or_die (file, 0, test + 20, 2, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (INT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : 0x%X => 0x%X).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	sf_close (file) ;

	unlink (filename) ;
	printf ("ok\n") ;

	return ;
} /* pcm_test_int */


/*======================================================================================
*/

static void
pcm_test_float (char *filename, int filetype, int long_file_ok)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	float		*orig, *test ;
	sf_count_t	count ;
	int			k, items, frames, pass ;

	print_test_name ("pcm_test_float", filename) ;

	sfinfo.samplerate  = 44100 ;
	sfinfo.frames     = SILLY_WRITE_COUNT ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels    = 1 ;
	sfinfo.format 	   = filetype ;

	gen_windowed_sine (orig_data, DATA_LENGTH, 1.0) ;

	orig = (float*) orig_data ;
	test = (float*) test_data ;

	/* Make this a macro so gdb steps over it in one go. */
	CONVERT_DATA (k, DATA_LENGTH, orig, orig_data) ;

	items = DATA_LENGTH ;

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;

	sf_set_string (file, SF_STR_ARTIST, "Your name here") ;

	test_write_float_or_die (file, 0, orig, items, __LINE__) ;

	/* Add non-audio data after the audio. */
	sf_set_string (file, SF_STR_COPYRIGHT, "Copyright (c) 2003") ;

	sf_close (file) ;

	memset (test, 0, items * sizeof (float)) ;

	if ((filetype & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
		memset (&sfinfo, 0, sizeof (sfinfo)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Mono : Returned format incorrect (0x%08X => 0x%08X).\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < items)
	{	printf ("\n\nLine %d : Mono : Incorrect number of frames in file (too short). (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), items) ;
		exit (1) ;
		} ;

	if (! long_file_ok && sfinfo.frames > items)
	{	printf ("\n\nLine %d : Mono : Incorrect number of frames in file (too long). (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), items) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d : Mono : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file) ;

	test_read_float_or_die (file, 0, test, items, __LINE__) ;
	for (k = 0 ; k < items ; k++)
		if (FLOAT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d: Mono : Incorrect sample A (#%d : %g => %g).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to start of file. */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	test_read_float_or_die (file, 0, test, 4, __LINE__) ;
	for (k = 0 ; k < 4 ; k++)
		if (FLOAT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : %g => %g).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	if ((filetype & SF_FORMAT_SUBMASK) == SF_FORMAT_DWVW_16 ||
			(filetype & SF_FORMAT_SUBMASK) == SF_FORMAT_DWVW_24)
	{	unlink (filename) ;
		printf ("ok\n") ;
		return ;
		} ;

	/* Seek to offset from start of file. */
	test_seek_or_die (file, 10, SEEK_SET, 10, sfinfo.channels, __LINE__) ;

	test_read_float_or_die (file, 0, test + 10, 4, __LINE__) ;
	for (k = 10 ; k < 14 ; k++)
		if (FLOAT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : %g => %g).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from current position. */
	test_seek_or_die (file, 6, SEEK_CUR, 20, sfinfo.channels, __LINE__) ;

	test_read_float_or_die (file, 0, test + 20, 4, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (FLOAT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : %g => %g).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from end of file. */
	test_seek_or_die (file, -(sfinfo.frames - 10), SEEK_END, 10, sfinfo.channels, __LINE__) ;

	test_read_float_or_die (file, 0, test + 10, 4, __LINE__) ;
	for (k = 10 ; k < 14 ; k++)
		if (FLOAT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample D (#%d : %g => %g).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Check read past end of file followed by sf_seek (sndfile, 0, SEEK_CUR). */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	count = 0 ;
	while (count < sfinfo.frames)
		count += sf_read_float (file, test, 311) ;

	/* Check that no error has occurred. */
	if (sf_error (file))
	{	printf ("\n\nLine %d : Mono : error where there shouldn't have been one.\n", __LINE__) ;
		puts (sf_strerror (file)) ;
		exit (1) ;
		} ;

	/* Check that we haven't read beyond EOF. */
	if (count > sfinfo.frames)
	{	printf ("\n\nLines %d : read past end of file (%ld should be %ld)\n", __LINE__, (long) count, (long) sfinfo.frames) ;
		exit (1) ;
		} ;

	test_seek_or_die (file, 0, SEEK_CUR, sfinfo.frames, sfinfo.channels, __LINE__) ;

	sf_close (file) ;

	/*==================================================================================
	** Now test Mono RDWR.
	*/

	sfinfo.samplerate = SAMPLE_RATE ;
	sfinfo.frames     = DATA_LENGTH ;
	sfinfo.channels   = 1 ;
	sfinfo.format 	  = filetype ;

	if ((filetype & SF_FORMAT_TYPEMASK) == SF_FORMAT_RAW)
		unlink (filename) ;
	else
	{	/* Create a short file. */
		create_short_file (filename) ;

		/* Opening a already existing short file (ie invalid header) RDWR is disallowed.
		** If this returns a valif pointer sf_open() screwed up.
		*/
		if ((file = sf_open (filename, SFM_RDWR, &sfinfo)))
		{	printf ("\n\nLine %d: sf_open should (SFM_RDWR) have failed but didn't.\n", __LINE__) ;
			exit (1) ;
			} ;

		/* Truncate the file to zero bytes. */
		if (truncate (filename, 0) < 0)
		{	printf ("\n\nLine %d: truncate (%s) failed", __LINE__, filename) ;
			perror (NULL) ;
			exit (1) ;
			} ;
		} ;

	/* Opening a zero length file RDWR is allowed, but the SF_INFO struct must contain
	** all the usual data required when opening the file in WRITE mode.
	*/
	sfinfo.samplerate = SAMPLE_RATE ;
	sfinfo.frames     = DATA_LENGTH ;
	sfinfo.channels   = 1 ;
	sfinfo.format 	  = filetype ;

	file = test_open_file_or_die (filename, SFM_RDWR, &sfinfo, __LINE__) ;

	/* Do 3 writes followed by reads. After each, check the data and the current
	** read and write offsets.
	*/
	for (pass = 1 ; pass <= 3 ; pass ++)
	{	orig [20] = pass * 2 ;

		/* Write some data. */
		test_write_float_or_die (file, 0, orig, DATA_LENGTH, __LINE__) ;

		test_read_write_position_or_die (file, __LINE__, pass, (pass - 1) * DATA_LENGTH, pass * DATA_LENGTH) ;

		/* Read what we just wrote. */
		test_read_float_or_die (file, 0, test, DATA_LENGTH, __LINE__) ;

		/* Check the data. */
		for (k = 0 ; k < DATA_LENGTH ; k++)
			if (FLOAT_ERROR (orig [k], test [k]))
			{	printf ("\n\nLine %d (pass %d): Error at sample %d (%g => %g).\n", __LINE__, pass, k, orig [k], test [k]) ;
				oct_save_float (orig, test, DATA_LENGTH) ;
				exit (1) ;
				} ;

		test_read_write_position_or_die (file, __LINE__, pass, pass * DATA_LENGTH, pass * DATA_LENGTH) ;
		} ; /* for (pass ...) */

	sf_close (file) ;

	/* Open the file again to check the data. */
	file = test_open_file_or_die (filename, SFM_RDWR, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Returned format incorrect (0x%08X => 0x%08X).\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < 3 * DATA_LENGTH)
	{	printf ("\n\nLine %d : Not enough frames in file. (%ld < %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), 3 * DATA_LENGTH ) ;
		exit (1) ;
		}

	if (! long_file_ok && sfinfo.frames != 3 * DATA_LENGTH)
	{	printf ("\n\nLine %d : Incorrect number of frames in file. (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), 3 * DATA_LENGTH ) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	if (! long_file_ok)
		test_read_write_position_or_die (file, __LINE__, 0, 0, 3 * DATA_LENGTH) ;
	else
		test_seek_or_die (file, 3 * DATA_LENGTH, SFM_WRITE | SEEK_SET, 3 * DATA_LENGTH, sfinfo.channels, __LINE__) ;


	for (pass = 1 ; pass <= 3 ; pass ++)
	{	orig [20] = pass * 2 ;

		test_read_write_position_or_die (file, __LINE__, pass, (pass - 1) * DATA_LENGTH, 3 * DATA_LENGTH) ;

		/* Read what we just wrote. */
		test_read_float_or_die (file, pass, test, DATA_LENGTH, __LINE__) ;

		/* Check the data. */
		for (k = 0 ; k < DATA_LENGTH ; k++)
			if (FLOAT_ERROR (orig [k], test [k]))
			{	printf ("\n\nLine %d (pass %d): Error at sample %d (%g => %g).\n", __LINE__, pass, k, orig [k], test [k]) ;
				oct_save_float (orig, test, DATA_LENGTH) ;
				exit (1) ;
				} ;

		} ; /* for (pass ...) */

	sf_close (file) ;

	/*==================================================================================
	** Now test Stereo.
	*/

	sfinfo.samplerate  = 44100 ;
	sfinfo.frames     = SILLY_WRITE_COUNT ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels    = 2 ;
	sfinfo.format 	   = filetype ;

	if (! sf_format_check (&sfinfo))
	{	unlink (filename) ;
		printf ("ok, (no stereo)\n") ;
		return ;
		} ;

	gen_windowed_sine (orig_data, DATA_LENGTH, 1.0) ;

	orig = (float*) orig_data ;
	test = (float*) test_data ;

	/* Make this a macro so gdb steps over it in one go. */
	CONVERT_DATA (k, DATA_LENGTH, orig, orig_data) ;

	items = DATA_LENGTH ;
	frames = items / sfinfo.channels ;

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;

	sf_set_string (file, SF_STR_ARTIST, "Your name here") ;

	test_writef_float_or_die (file, 0, orig, frames, __LINE__) ;

	sf_set_string (file, SF_STR_COPYRIGHT, "Copyright (c) 2003") ;

	sf_close (file) ;

	memset (test, 0, items * sizeof (float)) ;

	if ((filetype & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
		memset (&sfinfo, 0, sizeof (sfinfo)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Stereo : Returned format incorrect (0x%08X => 0x%08X).\n",
				__LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < frames)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of frames in file (too short). (%ld should be %d)\n",
				__LINE__, SF_COUNT_TO_LONG (sfinfo.frames), frames) ;
		exit (1) ;
		} ;

	if (! long_file_ok && sfinfo.frames > frames)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of frames in file (too long). (%ld should be %d)\n",
				__LINE__, SF_COUNT_TO_LONG (sfinfo.frames), frames) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 2)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file) ;

	test_readf_float_or_die (file, 0, test, frames, __LINE__) ;
	for (k = 0 ; k < items ; k++)
		if (FLOAT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : %g => %g).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to start of file. */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	test_readf_float_or_die (file, 0, test, 2, __LINE__) ;
	for (k = 0 ; k < 4 ; k++)
		if (FLOAT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : %g => %g).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from start of file. */
	test_seek_or_die (file, 10, SEEK_SET, 10, sfinfo.channels, __LINE__) ;

	/* Check for errors here. */
	if (sf_error (file))
	{	printf ("Line %d: Should NOT return an error.\n", __LINE__) ;
		puts (sf_strerror (file)) ;
		exit (1) ;
		} ;

	if (sf_read_float (file, test, 1) > 0)
	{	printf ("Line %d: Should return 0.\n", __LINE__) ;
		exit (1) ;
		} ;

	if (! sf_error (file))
	{	printf ("Line %d: Should return an error.\n", __LINE__) ;
		exit (1) ;
		} ;
	/*-----------------------*/

	test_readf_float_or_die (file, 0, test + 10, 2, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (FLOAT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : %g => %g).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from current position. */
	test_seek_or_die (file, 8, SEEK_CUR, 20, sfinfo.channels, __LINE__) ;

	test_readf_float_or_die (file, 0, test + 20, 2, __LINE__) ;
	for (k = 40 ; k < 44 ; k++)
		if (FLOAT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : %g => %g).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from end of file. */
	test_seek_or_die (file, -(sfinfo.frames - 10), SEEK_END, 10, sfinfo.channels, __LINE__) ;

	test_readf_float_or_die (file, 0, test + 20, 2, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (FLOAT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : %g => %g).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	sf_close (file) ;

	unlink (filename) ;
	printf ("ok\n") ;

	return ;
} /* pcm_test_float */


/*======================================================================================
*/

static void
pcm_test_double (char *filename, int filetype, int long_file_ok)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	double		*orig, *test ;
	sf_count_t	count ;
	int			k, items, frames, pass ;

	print_test_name ("pcm_test_double", filename) ;

	sfinfo.samplerate  = 44100 ;
	sfinfo.frames     = SILLY_WRITE_COUNT ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels    = 1 ;
	sfinfo.format 	   = filetype ;

	gen_windowed_sine (orig_data, DATA_LENGTH, 1.0) ;

	orig = (double*) orig_data ;
	test = (double*) test_data ;

	/* Make this a macro so gdb steps over it in one go. */
	CONVERT_DATA (k, DATA_LENGTH, orig, orig_data) ;

	items = DATA_LENGTH ;

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;

	sf_set_string (file, SF_STR_ARTIST, "Your name here") ;

	test_write_double_or_die (file, 0, orig, items, __LINE__) ;

	/* Add non-audio data after the audio. */
	sf_set_string (file, SF_STR_COPYRIGHT, "Copyright (c) 2003") ;

	sf_close (file) ;

	memset (test, 0, items * sizeof (double)) ;

	if ((filetype & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
		memset (&sfinfo, 0, sizeof (sfinfo)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Mono : Returned format incorrect (0x%08X => 0x%08X).\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < items)
	{	printf ("\n\nLine %d : Mono : Incorrect number of frames in file (too short). (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), items) ;
		exit (1) ;
		} ;

	if (! long_file_ok && sfinfo.frames > items)
	{	printf ("\n\nLine %d : Mono : Incorrect number of frames in file (too long). (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), items) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d : Mono : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file) ;

	test_read_double_or_die (file, 0, test, items, __LINE__) ;
	for (k = 0 ; k < items ; k++)
		if (FLOAT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d: Mono : Incorrect sample A (#%d : %g => %g).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to start of file. */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	test_read_double_or_die (file, 0, test, 4, __LINE__) ;
	for (k = 0 ; k < 4 ; k++)
		if (FLOAT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : %g => %g).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	if ((filetype & SF_FORMAT_SUBMASK) == SF_FORMAT_DWVW_16 ||
			(filetype & SF_FORMAT_SUBMASK) == SF_FORMAT_DWVW_24)
	{	unlink (filename) ;
		printf ("ok\n") ;
		return ;
		} ;

	/* Seek to offset from start of file. */
	test_seek_or_die (file, 10, SEEK_SET, 10, sfinfo.channels, __LINE__) ;

	test_read_double_or_die (file, 0, test + 10, 4, __LINE__) ;
	for (k = 10 ; k < 14 ; k++)
		if (FLOAT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : %g => %g).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from current position. */
	test_seek_or_die (file, 6, SEEK_CUR, 20, sfinfo.channels, __LINE__) ;

	test_read_double_or_die (file, 0, test + 20, 4, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (FLOAT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample A (#%d : %g => %g).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from end of file. */
	test_seek_or_die (file, -(sfinfo.frames - 10), SEEK_END, 10, sfinfo.channels, __LINE__) ;

	test_read_double_or_die (file, 0, test + 10, 4, __LINE__) ;
	for (k = 10 ; k < 14 ; k++)
		if (FLOAT_ERROR (orig [k], test [k]))
		{	printf ("\n\nLine %d : Mono : Incorrect sample D (#%d : %g => %g).\n", __LINE__, k, test [k], orig [k]) ;
			exit (1) ;
			} ;

	/* Check read past end of file followed by sf_seek (sndfile, 0, SEEK_CUR). */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	count = 0 ;
	while (count < sfinfo.frames)
		count += sf_read_double (file, test, 311) ;

	/* Check that no error has occurred. */
	if (sf_error (file))
	{	printf ("\n\nLine %d : Mono : error where there shouldn't have been one.\n", __LINE__) ;
		puts (sf_strerror (file)) ;
		exit (1) ;
		} ;

	/* Check that we haven't read beyond EOF. */
	if (count > sfinfo.frames)
	{	printf ("\n\nLines %d : read past end of file (%ld should be %ld)\n", __LINE__, (long) count, (long) sfinfo.frames) ;
		exit (1) ;
		} ;

	test_seek_or_die (file, 0, SEEK_CUR, sfinfo.frames, sfinfo.channels, __LINE__) ;

	sf_close (file) ;

	/*==================================================================================
	** Now test Mono RDWR.
	*/

	sfinfo.samplerate = SAMPLE_RATE ;
	sfinfo.frames     = DATA_LENGTH ;
	sfinfo.channels   = 1 ;
	sfinfo.format 	  = filetype ;

	if ((filetype & SF_FORMAT_TYPEMASK) == SF_FORMAT_RAW)
		unlink (filename) ;
	else
	{	/* Create a short file. */
		create_short_file (filename) ;

		/* Opening a already existing short file (ie invalid header) RDWR is disallowed.
		** If this returns a valif pointer sf_open() screwed up.
		*/
		if ((file = sf_open (filename, SFM_RDWR, &sfinfo)))
		{	printf ("\n\nLine %d: sf_open should (SFM_RDWR) have failed but didn't.\n", __LINE__) ;
			exit (1) ;
			} ;

		/* Truncate the file to zero bytes. */
		if (truncate (filename, 0) < 0)
		{	printf ("\n\nLine %d: truncate (%s) failed", __LINE__, filename) ;
			perror (NULL) ;
			exit (1) ;
			} ;
		} ;

	/* Opening a zero length file RDWR is allowed, but the SF_INFO struct must contain
	** all the usual data required when opening the file in WRITE mode.
	*/
	sfinfo.samplerate = SAMPLE_RATE ;
	sfinfo.frames     = DATA_LENGTH ;
	sfinfo.channels   = 1 ;
	sfinfo.format 	  = filetype ;

	file = test_open_file_or_die (filename, SFM_RDWR, &sfinfo, __LINE__) ;

	/* Do 3 writes followed by reads. After each, check the data and the current
	** read and write offsets.
	*/
	for (pass = 1 ; pass <= 3 ; pass ++)
	{	orig [20] = pass * 2 ;

		/* Write some data. */
		test_write_double_or_die (file, 0, orig, DATA_LENGTH, __LINE__) ;

		test_read_write_position_or_die (file, __LINE__, pass, (pass - 1) * DATA_LENGTH, pass * DATA_LENGTH) ;

		/* Read what we just wrote. */
		test_read_double_or_die (file, 0, test, DATA_LENGTH, __LINE__) ;

		/* Check the data. */
		for (k = 0 ; k < DATA_LENGTH ; k++)
			if (FLOAT_ERROR (orig [k], test [k]))
			{	printf ("\n\nLine %d (pass %d): Error at sample %d (%g => %g).\n", __LINE__, pass, k, orig [k], test [k]) ;
				oct_save_double (orig, test, DATA_LENGTH) ;
				exit (1) ;
				} ;

		test_read_write_position_or_die (file, __LINE__, pass, pass * DATA_LENGTH, pass * DATA_LENGTH) ;
		} ; /* for (pass ...) */

	sf_close (file) ;

	/* Open the file again to check the data. */
	file = test_open_file_or_die (filename, SFM_RDWR, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Returned format incorrect (0x%08X => 0x%08X).\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < 3 * DATA_LENGTH)
	{	printf ("\n\nLine %d : Not enough frames in file. (%ld < %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), 3 * DATA_LENGTH ) ;
		exit (1) ;
		}

	if (! long_file_ok && sfinfo.frames != 3 * DATA_LENGTH)
	{	printf ("\n\nLine %d : Incorrect number of frames in file. (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), 3 * DATA_LENGTH ) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	if (! long_file_ok)
		test_read_write_position_or_die (file, __LINE__, 0, 0, 3 * DATA_LENGTH) ;
	else
		test_seek_or_die (file, 3 * DATA_LENGTH, SFM_WRITE | SEEK_SET, 3 * DATA_LENGTH, sfinfo.channels, __LINE__) ;


	for (pass = 1 ; pass <= 3 ; pass ++)
	{	orig [20] = pass * 2 ;

		test_read_write_position_or_die (file, __LINE__, pass, (pass - 1) * DATA_LENGTH, 3 * DATA_LENGTH) ;

		/* Read what we just wrote. */
		test_read_double_or_die (file, pass, test, DATA_LENGTH, __LINE__) ;

		/* Check the data. */
		for (k = 0 ; k < DATA_LENGTH ; k++)
			if (FLOAT_ERROR (orig [k], test [k]))
			{	printf ("\n\nLine %d (pass %d): Error at sample %d (%g => %g).\n", __LINE__, pass, k, orig [k], test [k]) ;
				oct_save_double (orig, test, DATA_LENGTH) ;
				exit (1) ;
				} ;

		} ; /* for (pass ...) */

	sf_close (file) ;

	/*==================================================================================
	** Now test Stereo.
	*/

	sfinfo.samplerate  = 44100 ;
	sfinfo.frames     = SILLY_WRITE_COUNT ; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels    = 2 ;
	sfinfo.format 	   = filetype ;

	if (! sf_format_check (&sfinfo))
	{	unlink (filename) ;
		printf ("ok, (no stereo)\n") ;
		return ;
		} ;

	gen_windowed_sine (orig_data, DATA_LENGTH, 1.0) ;

	orig = (double*) orig_data ;
	test = (double*) test_data ;

	/* Make this a macro so gdb steps over it in one go. */
	CONVERT_DATA (k, DATA_LENGTH, orig, orig_data) ;

	items = DATA_LENGTH ;
	frames = items / sfinfo.channels ;

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;

	sf_set_string (file, SF_STR_ARTIST, "Your name here") ;

	test_writef_double_or_die (file, 0, orig, frames, __LINE__) ;

	sf_set_string (file, SF_STR_COPYRIGHT, "Copyright (c) 2003") ;

	sf_close (file) ;

	memset (test, 0, items * sizeof (double)) ;

	if ((filetype & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)
		memset (&sfinfo, 0, sizeof (sfinfo)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;

	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d : Stereo : Returned format incorrect (0x%08X => 0x%08X).\n",
				__LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;

	if (sfinfo.frames < frames)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of frames in file (too short). (%ld should be %d)\n",
				__LINE__, SF_COUNT_TO_LONG (sfinfo.frames), frames) ;
		exit (1) ;
		} ;

	if (! long_file_ok && sfinfo.frames > frames)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of frames in file (too long). (%ld should be %d)\n",
				__LINE__, SF_COUNT_TO_LONG (sfinfo.frames), frames) ;
		exit (1) ;
		} ;

	if (sfinfo.channels != 2)
	{	printf ("\n\nLine %d : Stereo : Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file) ;

	test_readf_double_or_die (file, 0, test, frames, __LINE__) ;
	for (k = 0 ; k < items ; k++)
		if (FLOAT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : %g => %g).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to start of file. */
	test_seek_or_die (file, 0, SEEK_SET, 0, sfinfo.channels, __LINE__) ;

	test_readf_double_or_die (file, 0, test, 2, __LINE__) ;
	for (k = 0 ; k < 4 ; k++)
		if (FLOAT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : %g => %g).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from start of file. */
	test_seek_or_die (file, 10, SEEK_SET, 10, sfinfo.channels, __LINE__) ;

	/* Check for errors here. */
	if (sf_error (file))
	{	printf ("Line %d: Should NOT return an error.\n", __LINE__) ;
		puts (sf_strerror (file)) ;
		exit (1) ;
		} ;

	if (sf_read_double (file, test, 1) > 0)
	{	printf ("Line %d: Should return 0.\n", __LINE__) ;
		exit (1) ;
		} ;

	if (! sf_error (file))
	{	printf ("Line %d: Should return an error.\n", __LINE__) ;
		exit (1) ;
		} ;
	/*-----------------------*/

	test_readf_double_or_die (file, 0, test + 10, 2, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (FLOAT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : %g => %g).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from current position. */
	test_seek_or_die (file, 8, SEEK_CUR, 20, sfinfo.channels, __LINE__) ;

	test_readf_double_or_die (file, 0, test + 20, 2, __LINE__) ;
	for (k = 40 ; k < 44 ; k++)
		if (FLOAT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : %g => %g).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	/* Seek to offset from end of file. */
	test_seek_or_die (file, -(sfinfo.frames - 10), SEEK_END, 10, sfinfo.channels, __LINE__) ;

	test_readf_double_or_die (file, 0, test + 20, 2, __LINE__) ;
	for (k = 20 ; k < 24 ; k++)
		if (FLOAT_ERROR (test [k], orig [k]))
		{	printf ("\n\nLine %d : Stereo : Incorrect sample (#%d : %g => %g).\n", __LINE__, k, orig [k], test [k]) ;
			exit (1) ;
			} ;

	sf_close (file) ;

	unlink (filename) ;
	printf ("ok\n") ;

	return ;
} /* pcm_test_double */



/*----------------------------------------------------------------------------------------
*/

static void
create_short_file (char *filename)
{	FILE *file ;

	if (! (file = fopen (filename, "w")))
	{	printf ("create_short_file : fopen (%s, \"w\") failed.", filename) ;
		fflush (stdout) ;
		perror (NULL) ;
		exit (1) ;
		} ;

	fprintf (file, "This is the file data.\n") ;

	fclose (file) ;
} /* create_short_file */

#if (defined (WIN32) || defined (__WIN32))

/* Win32 does not have truncate (nor does it have the POSIX function ftruncate).
** Hack somethng up here to over come this. This function can only truncate to a
** length of zero.
*/

static int
truncate (const char *filename, int ignored)
{	int fd ;

	if ((fd = open (filename, O_RDWR | O_TRUNC | O_BINARY)) < 0)
		return 0 ;

	close (fd) ;

	return 0 ;
} /* truncate */

#endif
