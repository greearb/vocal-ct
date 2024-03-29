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
#include	<unistd.h>
#include	<math.h>

#include	<sndfile.h>

#include	"dft_cmp.h"
#include	"utils.h"

/* Configureation header from ../src or ../Win32. */
#include	<config.h>
#include	<float_cast.h>

#define	SAMPLE_RATE			11025

static void	float_scaled_test	(char *filename, int allow_exit, int replace_float, int filetype, double target_snr) ;
static void	double_scaled_test	(char *filename, int allow_exit, int replace_float, int filetype, double target_snr) ;

static	double	orig_data [DFT_DATA_LENGTH] ;
static	double	test_data [DFT_DATA_LENGTH] ;


int		
main (int argc, char *argv [])
{	int allow_exit = 1 ;

	if (argc == 2 && ! strstr (argv [1], "no-exit"))
		allow_exit = 0 ;

#if ((HAVE_LRINTF == 0) && (HAVE_LRINT_REPLACEMENT == 0))
	puts ("*** Cannot run this test on this platform because it lacks lrintf().") ;
	exit (0) ;
#endif

	/* Float tests. */
	float_scaled_test	("float.raw", allow_exit, SF_FALSE, SF_ENDIAN_LITTLE | SF_FORMAT_RAW  | SF_FORMAT_FLOAT, -163.0) ;

	/* Test both signed and unsigned 8 bit files. */
	float_scaled_test	("pcm_s8.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_PCM_S8, -39.0) ;
	float_scaled_test	("pcm_u8.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_PCM_U8, -39.0) ;

	float_scaled_test	("pcm_16.raw", allow_exit, SF_FALSE, SF_ENDIAN_BIG    | SF_FORMAT_RAW | SF_FORMAT_PCM_16, -87.0) ;
	float_scaled_test	("pcm_24.raw", allow_exit, SF_FALSE, SF_ENDIAN_LITTLE | SF_FORMAT_RAW | SF_FORMAT_PCM_24, -138.0) ;
	float_scaled_test	("pcm_32.raw", allow_exit, SF_FALSE, SF_ENDIAN_BIG    | SF_FORMAT_RAW | SF_FORMAT_PCM_32, -163.0) ;

	float_scaled_test	("ulaw.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_ULAW, -50.0) ;
	float_scaled_test	("alaw.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_ALAW, -49.0) ;

	float_scaled_test	("ima_adpcm.wav", allow_exit, SF_FALSE, SF_FORMAT_WAV | SF_FORMAT_IMA_ADPCM, -47.0) ;
	float_scaled_test	("ms_adpcm.wav" , allow_exit, SF_FALSE, SF_FORMAT_WAV | SF_FORMAT_MS_ADPCM, -40.0) ;
	float_scaled_test	("gsm610.raw"   , allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_GSM610, -33.0) ;

	float_scaled_test	("g721_32.au", allow_exit, SF_FALSE, SF_FORMAT_AU | SF_FORMAT_G721_32, -34.0) ;
	float_scaled_test	("g723_24.au", allow_exit, SF_FALSE, SF_FORMAT_AU | SF_FORMAT_G723_24, -34.0) ;
	float_scaled_test	("g723_40.au", allow_exit, SF_FALSE, SF_FORMAT_AU | SF_FORMAT_G723_40, -40.0) ;

	/*	PAF files do not use the same encoding method for 24 bit PCM data as other file
	**	formats so we need to explicitly test it here.
	*/
	float_scaled_test	("le_paf_24.paf", allow_exit, SF_FALSE, SF_ENDIAN_LITTLE | SF_FORMAT_PAF | SF_FORMAT_PCM_24, -149.0) ;
	float_scaled_test	("be_paf_24.paf", allow_exit, SF_FALSE, SF_ENDIAN_BIG    | SF_FORMAT_PAF | SF_FORMAT_PCM_24, -149.0) ;

	float_scaled_test	("dwvw_12.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_DWVW_12, -64.0) ;
	float_scaled_test	("dwvw_16.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_DWVW_16, -92.0) ;
	float_scaled_test	("dwvw_24.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_DWVW_24, -151.0) ;

	float_scaled_test	("adpcm.vox", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_VOX_ADPCM, -40.0) ;

	float_scaled_test	("dpcm_16.xi", allow_exit, SF_FALSE, SF_FORMAT_XI | SF_FORMAT_DPCM_16, -90.0) ;
	
	float_scaled_test	("dpcm_8.xi" , allow_exit, SF_FALSE, SF_FORMAT_XI | SF_FORMAT_DPCM_8 , -41.0) ;

	float_scaled_test	("replace_float.raw", allow_exit, SF_TRUE, SF_ENDIAN_LITTLE | SF_FORMAT_RAW  | SF_FORMAT_FLOAT, -163.0) ;

	/*==============================================================================
	** Double tests. 
	*/

	double_scaled_test	("double.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_DOUBLE, -300.0) ;

	/* Test both signed (AIFF) and unsigned (WAV) 8 bit files. */
	double_scaled_test	("pcm_s8.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_PCM_S8, -39.0) ;
	double_scaled_test	("pcm_u8.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_PCM_U8, -39.0) ;

	double_scaled_test	("pcm_16.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_PCM_16, -87.0) ;
	double_scaled_test	("pcm_24.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_PCM_24, -135.0) ;
	double_scaled_test	("pcm_32.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_PCM_32, -184.0) ;

	double_scaled_test	("ulaw.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_ULAW, -50.0) ;
	double_scaled_test	("alaw.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_ALAW, -49.0) ;

	double_scaled_test	("ima_adpcm.wav", allow_exit, SF_FALSE, SF_FORMAT_WAV | SF_FORMAT_IMA_ADPCM, -47.0) ;
	double_scaled_test	("ms_adpcm.wav" , allow_exit, SF_FALSE, SF_FORMAT_WAV | SF_FORMAT_MS_ADPCM, -40.0) ;
	double_scaled_test	("gsm610.raw"   , allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_GSM610, -33.0) ;

	double_scaled_test	("g721_32.au", allow_exit, SF_FALSE, SF_FORMAT_AU | SF_FORMAT_G721_32, -34.0) ;
	double_scaled_test	("g723_24.au", allow_exit, SF_FALSE, SF_FORMAT_AU | SF_FORMAT_G723_24, -34.0) ;
	double_scaled_test	("g723_40.au", allow_exit, SF_FALSE, SF_FORMAT_AU | SF_FORMAT_G723_40, -40.0) ;

	/*	24 bit PCM PAF files tested here. */
	double_scaled_test	("be_paf_24.paf", allow_exit, SF_FALSE, SF_ENDIAN_BIG    | SF_FORMAT_PAF  | SF_FORMAT_PCM_24, -151.0) ;
	double_scaled_test	("le_paf_24.paf", allow_exit, SF_FALSE, SF_ENDIAN_LITTLE | SF_FORMAT_PAF  | SF_FORMAT_PCM_24, -151.0) ;

	double_scaled_test	("dwvw_12.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_DWVW_12, -64.0) ;
	double_scaled_test	("dwvw_16.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_DWVW_16, -92.0) ;
	double_scaled_test	("dwvw_24.raw", allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_DWVW_24, -151.0) ;

	double_scaled_test	("adpcm.vox" , allow_exit, SF_FALSE, SF_FORMAT_RAW | SF_FORMAT_VOX_ADPCM, -40.0) ;

	double_scaled_test	("dpcm_16.xi", allow_exit, SF_FALSE, SF_FORMAT_XI | SF_FORMAT_DPCM_16, -90.0) ;
	double_scaled_test	("dpcm_8.xi" , allow_exit, SF_FALSE, SF_FORMAT_XI | SF_FORMAT_DPCM_8 , -42.0) ;

	double_scaled_test	("replace_double.raw", allow_exit, SF_TRUE, SF_FORMAT_RAW | SF_FORMAT_DOUBLE, -300.0) ;

	return 0;
} /* main */

/*============================================================================================
 *	Here are the test functions.
 */ 

static void	
float_scaled_test (char *filename, int allow_exit, int replace_float, int filetype, double target_snr)
{	static	float	float_orig [DFT_DATA_LENGTH] ;
	static	float	float_test [DFT_DATA_LENGTH] ;

	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	int			k ;
	double		snr ;
	
	print_test_name ("float_scaled_test", filename) ;
	
	gen_windowed_sine (orig_data, DFT_DATA_LENGTH, 1.0) ;
	
	for (k = 0 ; k < DFT_DATA_LENGTH ; k++)
		float_orig [k] = orig_data [k] ;
		
	sfinfo.samplerate = SAMPLE_RATE ;
	sfinfo.frames     = DFT_DATA_LENGTH ;
	sfinfo.channels   = 1 ;
	sfinfo.format     = filetype ;

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;
	sf_command (file, SFC_TEST_IEEE_FLOAT_REPLACE, NULL, replace_float) ;

	test_write_float_or_die (file, 0, float_orig, DFT_DATA_LENGTH, __LINE__) ;

	sf_close (file) ;

	memset (float_test, 0, sizeof (float_test)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;
	sf_command (file, SFC_TEST_IEEE_FLOAT_REPLACE, NULL, replace_float) ;
	
	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d: Returned format incorrect (0x%08X => 0x%08X).\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;
	
	if (sfinfo.frames < DFT_DATA_LENGTH)
	{	printf ("\n\nLine %d: Incorrect number of.frames in file (too short). (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), DFT_DATA_LENGTH) ;
		exit (1) ;
		} ;
	
	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d: Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file) ;
		
	test_read_float_or_die (file, 0, float_test, DFT_DATA_LENGTH, __LINE__) ;

	sf_close (file) ;

	for (k = 0 ; k < DFT_DATA_LENGTH ; k++)
		test_data [k] = float_test [k] ;

	snr = dft_cmp (__LINE__, orig_data, test_data, DFT_DATA_LENGTH, target_snr, allow_exit) ;

	if (snr < target_snr)
		printf ("% 6.1fdB SNR ... ok\n", snr) ;

	unlink (filename) ;

	return ;			
} /* float_scaled_test */

static void	
double_scaled_test (char *filename, int allow_exit, int replace_float, int filetype, double target_snr)
{	SNDFILE		*file ;
	SF_INFO		sfinfo ;
	double		snr ;

	print_test_name ("double_scaled_test", filename) ;
	
	gen_windowed_sine (orig_data, DFT_DATA_LENGTH, 0.95) ;
		
	sfinfo.samplerate = SAMPLE_RATE ;
	sfinfo.frames     = DFT_DATA_LENGTH ;
	sfinfo.channels   = 1 ;
	sfinfo.format     = filetype ;

	file = test_open_file_or_die (filename, SFM_WRITE, &sfinfo, __LINE__) ;
	sf_command (file, SFC_TEST_IEEE_FLOAT_REPLACE, NULL, replace_float) ;

	test_write_double_or_die (file, 0, orig_data, DFT_DATA_LENGTH, __LINE__) ;
		
	sf_close (file) ;
	
	memset (test_data, 0, sizeof (test_data)) ;

	file = test_open_file_or_die (filename, SFM_READ, &sfinfo, __LINE__) ;
	sf_command (file, SFC_TEST_IEEE_FLOAT_REPLACE, NULL, replace_float) ;
	
	if (sfinfo.format != filetype)
	{	printf ("\n\nLine %d: Returned format incorrect (0x%08X => 0x%08X).\n", __LINE__, filetype, sfinfo.format) ;
		exit (1) ;
		} ;
	
	if (sfinfo.frames < DFT_DATA_LENGTH)
	{	printf ("\n\nLine %d: Incorrect number of.frames in file (too short). (%ld should be %d)\n", __LINE__, SF_COUNT_TO_LONG (sfinfo.frames), DFT_DATA_LENGTH) ;
		exit (1) ;
		} ;
	
	if (sfinfo.channels != 1)
	{	printf ("\n\nLine %d: Incorrect number of channels in file.\n", __LINE__) ;
		exit (1) ;
		} ;

	check_log_buffer_or_die (file) ;
		
	test_read_double_or_die (file, 0, test_data, DFT_DATA_LENGTH, __LINE__) ;

	sf_close (file) ;

	snr = dft_cmp (__LINE__, orig_data, test_data, DFT_DATA_LENGTH, target_snr, allow_exit) ;

	if (snr < target_snr)
		printf ("% 6.1fdB SNR ... ok\n", snr) ;

	unlink (filename) ;

	return ;			
} /* double_scaled_test */
