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


#include	<unistd.h>

#include	"sndfile.h"
#include	"config.h"
#include	"sfendian.h"
#include	"float_cast.h"
#include	"common.h"

/* Need to be able to handle 3 byte (24 bit) integers. So defined a
** type and use SIZEOF_TRIBYTE instead of (tribyte).
*/

typedef	void  tribyte ;

#define	SIZEOF_TRIBYTE	3

static sf_count_t	pcm_read_sc2s  (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_uc2s  (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bes2s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_les2s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bet2s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_let2s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bei2s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_lei2s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;

static sf_count_t	pcm_read_sc2i  (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_uc2i  (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bes2i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_les2i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bet2i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_let2i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bei2i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_lei2i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;

static sf_count_t	pcm_read_sc2f  (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_uc2f  (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bes2f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_les2f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bet2f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_let2f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bei2f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_lei2f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;

static sf_count_t	pcm_read_sc2d  (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_uc2d  (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bes2d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_les2d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bet2d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_let2d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_bei2d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_read_lei2d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;


static sf_count_t	pcm_write_s2sc  (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_s2uc  (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_s2bes (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_s2les (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_s2bet (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_s2let (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_s2bei (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_s2lei (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;

static sf_count_t	pcm_write_i2sc  (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_i2uc  (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_i2bes (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_i2les (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_i2bet (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_i2let (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_i2bei (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_i2lei (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;

static sf_count_t	pcm_write_f2sc  (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_f2uc  (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_f2bes (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_f2les (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_f2bet (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_f2let (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_f2bei (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_f2lei (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;

static sf_count_t	pcm_write_d2sc  (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_d2uc  (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_d2bes (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_d2les (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_d2bet (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_d2let (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_d2bei (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;
static sf_count_t	pcm_write_d2lei (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static void sc2s_array	(signed char *src, int count, short *dest) ;
static void uc2s_array	(unsigned char *src, int count, short *dest) ;

static void bet2s_array (tribyte *src, int count, short *dest) ;
static void let2s_array (tribyte *src, int count, short *dest) ;
static void bei2s_array (int *src, int count, short *dest) ;
static void lei2s_array (int *src, int count, short *dest) ;

static void sc2i_array	(signed char *src, int count, int *dest) ;
static void uc2i_array	(unsigned char *src, int count, int *dest) ;
static void bes2i_array (short *src, int count, int *dest) ;
static void les2i_array (short *src, int count, int *dest) ;
static void bet2i_array (tribyte *src, int count, int *dest) ;
static void let2i_array (tribyte *src, int count, int *dest) ;

static void sc2f_array	(signed char *src, int count, float *dest, float normfact) ;
static void uc2f_array	(unsigned char *src, int count, float *dest, float normfact) ;
static void bes2f_array (short *src, int count, float *dest, float normfact) ;
static void les2f_array (short *src, int count, float *dest, float normfact) ;
static void bet2f_array (tribyte *src, int count, float *dest, float normfact) ;
static void let2f_array (tribyte *src, int count, float *dest, float normfact) ;
static void bei2f_array (int *src, int count, float *dest, float normfact) ;
static void lei2f_array (int *src, int count, float *dest, float normfact) ;

static void sc2d_array	(signed char *src, int count, double *dest, double normfact) ;
static void uc2d_array	(unsigned char *src, int count, double *dest, double normfact) ;
static void bes2d_array (short *src, int count, double *dest, double normfact) ;
static void les2d_array (short *src, int count, double *dest, double normfact) ;
static void bet2d_array (tribyte *src, int count, double *dest, double normfact) ;
static void let2d_array (tribyte *src, int count, double *dest, double normfact) ;
static void bei2d_array (int *src, int count, double *dest, double normfact) ;
static void lei2d_array (int *src, int count, double *dest, double normfact) ;

static void s2sc_array	(short *src, signed char *dest, int count) ;
static void s2uc_array	(short *src, unsigned char *dest, int count) ;
static void s2bet_array (short *src, tribyte *dest, int count) ;
static void s2let_array (short *src, tribyte *dest, int count) ;
static void s2bei_array (short *src, int *dest, int count) ;
static void s2lei_array (short *src, int *dest, int count) ;

static void i2sc_array	(int *src, signed char *dest, int count) ;
static void i2uc_array	(int *src, unsigned char *dest, int count) ;
static void i2bes_array (int *src, short *dest, int count) ;
static void i2les_array (int *src, short *dest, int count) ;
static void i2bet_array (int *src, tribyte *dest, int count) ;
static void i2let_array (int *src, tribyte *dest, int count) ;

static void f2sc_array	(float *src, signed char *dest, int count, float normfact) ;
static void f2uc_array	(float *src, unsigned char *dest, int count, float normfact) ;
static void f2bes_array (float *src, short *dest, int count, float normfact) ;
static void f2les_array (float *src, short *dest, int count, float normfact) ;
static void f2bet_array (float *src, tribyte *dest, int count, float normfact) ;
static void f2let_array (float *src, tribyte *dest, int count, float normfact) ;
static void f2bei_array (float *src, int *dest, int count, float normfact) ;
static void f2lei_array (float *src, int *dest, int count, float normfact) ;

static void d2sc_array	(double *src, signed char *dest, int count, double normfact) ;
static void d2uc_array	(double *src, unsigned char *dest, int count, double normfact) ;
static void d2bes_array (double *src, short *dest, int count, double normfact) ;
static void d2les_array (double *src, short *dest, int count, double normfact) ;
static void d2bet_array (double *src, tribyte *dest, int count, double normfact) ;
static void d2let_array (double *src, tribyte *dest, int count, double normfact) ;
static void d2bei_array (double *src, int *dest, int count, double normfact) ;
static void d2lei_array (double *src, int *dest, int count, double normfact) ;

/*-----------------------------------------------------------------------------------------------
*/

enum
{	/* Char type for 8 bit files. */
	SF_CHARS_SIGNED		= 200,
	SF_CHARS_UNSIGNED	= 201
} ;

/*-----------------------------------------------------------------------------------------------
*/

int
pcm_init (SF_PRIVATE *psf)
{	int chars = 0 ;

	if (psf->bytewidth == 0 || psf->sf.channels == 0)
		return SFE_INTERNAL ;


	psf->blockwidth = psf->bytewidth * psf->sf.channels ;

	if ((psf->sf.format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_S8)
		chars = SF_CHARS_SIGNED ;
	else if ((psf->sf.format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_U8)
		chars = SF_CHARS_UNSIGNED ;

	if (psf->mode == SFM_READ || psf->mode == SFM_RDWR)
	{	switch (psf->bytewidth * 0x10000 + psf->endian + chars)
		{	case (0x10000 + SF_ENDIAN_BIG + SF_CHARS_SIGNED) :
			case (0x10000 + SF_ENDIAN_LITTLE + SF_CHARS_SIGNED) :
					psf->read_short  = pcm_read_sc2s ;
					psf->read_int    = pcm_read_sc2i ;
					psf->read_float  = pcm_read_sc2f ;
					psf->read_double = pcm_read_sc2d ;
					break ;
			case (0x10000 + SF_ENDIAN_BIG + SF_CHARS_UNSIGNED) :
			case (0x10000 + SF_ENDIAN_LITTLE + SF_CHARS_UNSIGNED) :
					psf->read_short  = pcm_read_uc2s ;
					psf->read_int    = pcm_read_uc2i ;
					psf->read_float  = pcm_read_uc2f ;
					psf->read_double = pcm_read_uc2d ;
					break ;

			case  (2 * 0x10000 + SF_ENDIAN_BIG) :
					psf->read_short  = pcm_read_bes2s ;
					psf->read_int    = pcm_read_bes2i ;
					psf->read_float  = pcm_read_bes2f ;
					psf->read_double = pcm_read_bes2d ;
					break ;
			case  (3 * 0x10000 + SF_ENDIAN_BIG) :
					psf->read_short  = pcm_read_bet2s ;
					psf->read_int    = pcm_read_bet2i ;
					psf->read_float  = pcm_read_bet2f ;
					psf->read_double = pcm_read_bet2d ;
					break ;
			case  (4 * 0x10000 + SF_ENDIAN_BIG) :
					psf->read_short  = pcm_read_bei2s ;
					psf->read_int    = pcm_read_bei2i ;
					psf->read_float  = pcm_read_bei2f ;
					psf->read_double = pcm_read_bei2d ;
					break ;

			case  (2 * 0x10000 + SF_ENDIAN_LITTLE) :
					psf->read_short  = pcm_read_les2s ;
					psf->read_int    = pcm_read_les2i ;
					psf->read_float  = pcm_read_les2f ;
					psf->read_double = pcm_read_les2d ;
					break ;
			case  (3 * 0x10000 + SF_ENDIAN_LITTLE) :
					psf->read_short  = pcm_read_let2s ;
					psf->read_int    = pcm_read_let2i ;
					psf->read_float  = pcm_read_let2f ;
					psf->read_double = pcm_read_let2d ;
					break ;
			case  (4 * 0x10000 + SF_ENDIAN_LITTLE) :
					psf->read_short  = pcm_read_lei2s ;
					psf->read_int    = pcm_read_lei2i ;
					psf->read_float  = pcm_read_lei2f ;
					psf->read_double = pcm_read_lei2d ;
					break ;
			default :
				psf_log_printf (psf, "pcm.c returning SFE_UNIMPLEMENTED\n") ;
				return SFE_UNIMPLEMENTED ;
			} ;
		} ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
	{	switch (psf->bytewidth * 0x10000 + psf->endian + chars)
		{	case (0x10000 + SF_ENDIAN_BIG + SF_CHARS_SIGNED) :
			case (0x10000 + SF_ENDIAN_LITTLE + SF_CHARS_SIGNED) :
					psf->write_short  = pcm_write_s2sc ;
					psf->write_int    = pcm_write_i2sc ;
					psf->write_float  = pcm_write_f2sc ;
					psf->write_double = pcm_write_d2sc ;
					break ;
			case (0x10000 + SF_ENDIAN_BIG + SF_CHARS_UNSIGNED) :
			case (0x10000 + SF_ENDIAN_LITTLE + SF_CHARS_UNSIGNED) :
					psf->write_short  = pcm_write_s2uc ;
					psf->write_int    = pcm_write_i2uc ;
					psf->write_float  = pcm_write_f2uc ;
					psf->write_double = pcm_write_d2uc ;
					break ;

			case  (2 * 0x10000 + SF_ENDIAN_BIG) :
					psf->write_short  = pcm_write_s2bes ;
					psf->write_int    = pcm_write_i2bes ;
					psf->write_float  = pcm_write_f2bes ;
					psf->write_double = pcm_write_d2bes ;
					break ;

			case  (3 * 0x10000 + SF_ENDIAN_BIG) :
					psf->write_short  = pcm_write_s2bet ;
					psf->write_int    = pcm_write_i2bet ;
					psf->write_float  = pcm_write_f2bet ;
					psf->write_double = pcm_write_d2bet ;
					break ;

			case  (4 * 0x10000 + SF_ENDIAN_BIG) :
					psf->write_short  = pcm_write_s2bei ;
					psf->write_int    = pcm_write_i2bei ;
					psf->write_float  = pcm_write_f2bei ;
					psf->write_double = pcm_write_d2bei ;
					break ;

			case  (2 * 0x10000 + SF_ENDIAN_LITTLE) :
					psf->write_short  = pcm_write_s2les ;
					psf->write_int    = pcm_write_i2les ;
					psf->write_float  = pcm_write_f2les ;
					psf->write_double = pcm_write_d2les ;
					break ;

			case  (3 * 0x10000 + SF_ENDIAN_LITTLE) :
					psf->write_short  = pcm_write_s2let ;
					psf->write_int    = pcm_write_i2let ;
					psf->write_float  = pcm_write_f2let ;
					psf->write_double = pcm_write_d2let ;
					break ;

			case  (4 * 0x10000 + SF_ENDIAN_LITTLE) :
					psf->write_short  = pcm_write_s2lei ;
					psf->write_int    = pcm_write_i2lei ;
					psf->write_float  = pcm_write_f2lei ;
					psf->write_double = pcm_write_d2lei ;
					break ;

			default :
				psf_log_printf (psf, "pcm.c returning SFE_UNIMPLEMENTED\n") ;
				return SFE_UNIMPLEMENTED ;
			} ;

		} ;

	if (psf->filelength > psf->dataoffset)
	{	psf->datalength = (psf->dataend > 0) ? psf->dataend - psf->dataoffset :
							psf->filelength - psf->dataoffset ;
		}
	else
		psf->datalength = 0 ;

	psf->sf.frames  = psf->datalength / psf->blockwidth ;

	return 0 ;
} /* pcm_init */

/*-----------------------------------------------------------------------------------------------
*/

static sf_count_t
pcm_read_sc2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (signed char), readcount, psf) ;
		sc2s_array ((signed char*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_sc2s */

static sf_count_t
pcm_read_uc2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (unsigned char), readcount, psf) ;
		uc2s_array ((unsigned char*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_uc2s */

static sf_count_t
pcm_read_bes2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int		total ;

	total = psf_fread (ptr, sizeof (short), len, psf) ;
	if (CPU_IS_LITTLE_ENDIAN)
		endswap_short_array (ptr, len) ;

	return total ;
} /* pcm_read_bes2s */

static sf_count_t
pcm_read_les2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int		total ;

	total = psf_fread (ptr, sizeof (short), len, psf) ;
	if (CPU_IS_BIG_ENDIAN)
		endswap_short_array (ptr, len) ;

	return total ;
} /* pcm_read_les2s */

static sf_count_t
pcm_read_bet2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		bet2s_array ((tribyte*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bet2s */

static sf_count_t
pcm_read_let2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		let2s_array ((tribyte*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_let2s */

static sf_count_t
pcm_read_bei2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (int), readcount, psf) ;
		bei2s_array ((int*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bei2s */

static sf_count_t
pcm_read_lei2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (int), readcount, psf) ;
		lei2s_array ((int*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_lei2s */

/*-----------------------------------------------------------------------------------------------
*/

static sf_count_t
pcm_read_sc2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (signed char), readcount, psf) ;
		sc2i_array ((signed char*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_sc2i */

static sf_count_t
pcm_read_uc2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (unsigned char), readcount, psf) ;
		uc2i_array ((unsigned char*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_uc2i */

static sf_count_t
pcm_read_bes2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		bes2i_array ((short*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bes2i */

static sf_count_t
pcm_read_les2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		les2i_array ((short*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_les2i */

static sf_count_t
pcm_read_bet2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		bet2i_array ((tribyte*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bet2i */

static sf_count_t
pcm_read_let2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		let2i_array ((tribyte*) (psf->buffer), thisread, ptr + total) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_let2i */

static sf_count_t
pcm_read_bei2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int		total ;

	total = psf_fread (ptr, sizeof (int), len, psf) ;
	if (CPU_IS_LITTLE_ENDIAN)
		endswap_int_array	(ptr, len) ;

	return total ;
} /* pcm_read_bei2i */

static sf_count_t
pcm_read_lei2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int		total ;

	total = psf_fread (ptr, sizeof (int), len, psf) ;
	if (CPU_IS_BIG_ENDIAN)
		endswap_int_array	(ptr, len) ;

	return total ;
} /* pcm_read_lei2i */

/*-----------------------------------------------------------------------------------------------
*/

static sf_count_t
pcm_read_sc2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x80) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (signed char), readcount, psf) ;
		sc2f_array ((signed char*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_sc2f */

static sf_count_t
pcm_read_uc2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x80) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (unsigned char), readcount, psf) ;
		uc2f_array ((unsigned char*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_uc2f */

static sf_count_t
pcm_read_bes2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x8000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		bes2f_array ((short*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bes2f */

static sf_count_t
pcm_read_les2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x8000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		les2f_array ((short*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_les2f */

static sf_count_t
pcm_read_bet2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	/* Special normfactor because tribyte value is read into an int. */
	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x80000000) : 1.0 / 256.0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		bet2f_array ((tribyte*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bet2f */

static sf_count_t
pcm_read_let2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	/* Special normfactor because tribyte value is read into an int. */
	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x80000000) : 1.0 / 256.0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		let2f_array ((tribyte*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_let2f */

static sf_count_t
pcm_read_bei2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x80000000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (int), readcount, psf) ;
		bei2f_array ((int*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bei2f */

static sf_count_t
pcm_read_lei2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x80000000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (int), readcount, psf) ;
		lei2f_array ((int*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_lei2f */

/*-----------------------------------------------------------------------------------------------
*/

static sf_count_t
pcm_read_sc2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x80) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (signed char), readcount, psf) ;
		sc2d_array ((signed char*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_sc2d */

static sf_count_t
pcm_read_uc2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x80) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (unsigned char), readcount, psf) ;
		uc2d_array ((unsigned char*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_uc2d */

static sf_count_t
pcm_read_bes2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x8000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		bes2d_array ((short*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bes2d */

static sf_count_t
pcm_read_les2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x8000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (short), readcount, psf) ;
		les2d_array ((short*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_les2d */

static sf_count_t
pcm_read_bet2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x80000000) : 1.0 / 256.0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		bet2d_array ((tribyte*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bet2d */

static sf_count_t
pcm_read_let2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	/* Special normfactor because tribyte value is read into an int. */
	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x80000000) : 1.0 / 256.0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, SIZEOF_TRIBYTE, readcount, psf) ;
		let2d_array ((tribyte*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_let2d */

static sf_count_t
pcm_read_bei2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x80000000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (int), readcount, psf) ;
		bei2d_array ((int*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_bei2d */

static sf_count_t
pcm_read_lei2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, readcount, thisread ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? 1.0 / ((double) 0x80000000) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	readcount = (len >= bufferlen) ? bufferlen : (int) len ;
		thisread = psf_fread (psf->buffer, sizeof (int), readcount, psf) ;
		lei2d_array ((int*) (psf->buffer), thisread, ptr + total, normfact) ;
		total += thisread ;
		len -= thisread ;
		if (thisread < readcount)
			break ;
		} ;

	return total ;
} /* pcm_read_lei2d */

/*===============================================================================================
**-----------------------------------------------------------------------------------------------
**===============================================================================================
*/

static sf_count_t
pcm_write_s2sc	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2sc_array (ptr + total, (signed char*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (signed char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_s2sc */

static sf_count_t
pcm_write_s2uc	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2uc_array (ptr + total, (unsigned char*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (unsigned char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_s2uc */

static sf_count_t
pcm_write_s2bes	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{
	if (CPU_IS_BIG_ENDIAN)
		return psf_fwrite (ptr, sizeof (short), len, psf) ;
	else
	{	int			bufferlen, writecount, thiswrite ;
		sf_count_t	total = 0 ;

		bufferlen = sizeof (psf->buffer) / sizeof (short) ;

		while (len > 0)
		{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
			endswap_short_copy ((short*) (psf->buffer), ptr + total, writecount) ;
			thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
			total += thiswrite ;
			len -= thiswrite ;
			if (thiswrite < writecount)
				break ;
			} ;

		return total ;
		} ;
} /* pcm_write_s2bes */

static sf_count_t
pcm_write_s2les	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{
	if (CPU_IS_LITTLE_ENDIAN)
		return psf_fwrite (ptr, sizeof (short), len, psf) ;
	else
	{	int			bufferlen, writecount, thiswrite ;
		sf_count_t	total = 0 ;

		bufferlen = sizeof (psf->buffer) / sizeof (short) ;

		while (len > 0)
		{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
			endswap_short_copy ((short*) (psf->buffer), ptr + total, writecount) ;
			thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
			total += thiswrite ;
			len -= thiswrite ;
			if (thiswrite < writecount)
				break ;
			} ;

		return total ;
		} ;
} /* pcm_write_s2les */

static sf_count_t
pcm_write_s2bet	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2bet_array (ptr + total, (tribyte*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_s2bet */

static sf_count_t
pcm_write_s2let	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2let_array (ptr + total, (tribyte*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_s2let */

static sf_count_t
pcm_write_s2bei	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2bei_array (ptr + total, (int*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_s2bei */

static sf_count_t
pcm_write_s2lei	(SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		s2lei_array (ptr + total, (int*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_s2lei */

/*-----------------------------------------------------------------------------------------------
*/

static sf_count_t
pcm_write_i2sc	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2sc_array (ptr + total, (signed char*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (signed char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_i2sc */

static sf_count_t
pcm_write_i2uc	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2uc_array (ptr + total, (unsigned char*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (signed char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_i2uc */

static sf_count_t
pcm_write_i2bes	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2bes_array (ptr + total, (short*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_i2bes */

static sf_count_t
pcm_write_i2les	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2les_array (ptr + total, (short*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_i2les */

static sf_count_t
pcm_write_i2bet	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2bet_array (ptr + total, (tribyte*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_i2bet */

static sf_count_t
pcm_write_i2let	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		i2let_array (ptr + total, (tribyte*) (psf->buffer), writecount) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_i2les */

static sf_count_t
pcm_write_i2bei	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{
	if (CPU_IS_BIG_ENDIAN)
		return psf_fwrite (ptr, sizeof (int), len, psf) ;
	else
	{	int			bufferlen, writecount, thiswrite ;
		sf_count_t	total = 0 ;

		bufferlen = sizeof (psf->buffer) / sizeof (int) ;

		while (len > 0)
		{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
			endswap_int_copy ((int*) (psf->buffer), ptr + total, writecount) ;
			thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
			total += thiswrite ;
			len -= thiswrite ;
			if (thiswrite < writecount)
				break ;
			} ;

		return total ;
		} ;
} /* pcm_write_i2bei */

static sf_count_t
pcm_write_i2lei	(SF_PRIVATE *psf, int *ptr, sf_count_t len)
{
	if (CPU_IS_LITTLE_ENDIAN)
		return psf_fwrite (ptr, sizeof (int), len, psf) ;
	else
	{	int			bufferlen, writecount, thiswrite ;
		sf_count_t	total = 0 ;

		bufferlen = sizeof (psf->buffer) / sizeof (int) ;

		while (len > 0)
		{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
			endswap_int_copy ((int*) (psf->buffer), ptr + total, writecount) ;
			thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
			total += thiswrite ;
			len -= thiswrite ;
			if (thiswrite < writecount)
				break ;
			} ;

		return total ;
		} ;
} /* pcm_write_i2lei */

/*-----------------------------------------------------------------------------------------------
*/

static sf_count_t
pcm_write_f2sc	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	float		normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7F) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		f2sc_array (ptr + total, (signed char*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (signed char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2sc */

static sf_count_t
pcm_write_f2uc	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	float		normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7F) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		f2uc_array (ptr + total, (unsigned char*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (unsigned char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2uc */

static sf_count_t
pcm_write_f2bes	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	float		normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		f2bes_array (ptr + total, (short*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2bes */

static sf_count_t
pcm_write_f2les	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	float		normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		f2les_array (ptr + total, (short*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2les */

static sf_count_t
pcm_write_f2let	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	float		normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFFFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		f2let_array (ptr + total, (tribyte*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2let */

static sf_count_t
pcm_write_f2bet	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	float		normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFFFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		f2bet_array (ptr + total, (tribyte*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2bes */

static sf_count_t
pcm_write_f2bei	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	float		normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFFFFFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		f2bei_array (ptr + total, (int*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2bei */

static sf_count_t
pcm_write_f2lei	(SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	float		normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFFFFFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		f2lei_array (ptr + total, (int*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_f2lei */

/*-----------------------------------------------------------------------------------------------
*/

static sf_count_t
pcm_write_d2sc	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7F) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (signed char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		d2sc_array (ptr + total, (signed char*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (signed char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2sc */

static sf_count_t
pcm_write_d2uc	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7F) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (unsigned char) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		d2uc_array (ptr + total, (unsigned char*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (unsigned char), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2uc */

static sf_count_t
pcm_write_d2bes	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		d2bes_array (ptr + total, (short*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2bes */

static sf_count_t
pcm_write_d2les	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7FFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (short) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		d2les_array (ptr + total, (short*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (short), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2les */

static sf_count_t
pcm_write_d2let	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7FFFFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		d2let_array (ptr + total, (tribyte*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2les */

static sf_count_t
pcm_write_d2bet	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7FFFFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / SIZEOF_TRIBYTE ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		d2bet_array (ptr + total, (tribyte*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, SIZEOF_TRIBYTE, writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2bes */

static sf_count_t
pcm_write_d2bei	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7FFFFFFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		d2bei_array (ptr + total, (int*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2bei */

static sf_count_t
pcm_write_d2lei	(SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	int			bufferlen, writecount, thiswrite ;
	sf_count_t	total = 0 ;
	double		normfact ;

	normfact = (psf->norm_double == SF_TRUE) ? (1.0 * 0x7FFFFFFF) : 1.0 ;

	bufferlen = sizeof (psf->buffer) / sizeof (int) ;

	while (len > 0)
	{	writecount = (len >= bufferlen) ? bufferlen : (int) len ;
		d2lei_array (ptr + total, (int*) (psf->buffer), writecount, normfact) ;
		thiswrite = psf_fwrite (psf->buffer, sizeof (int), writecount, psf) ;
		total += thiswrite ;
		len -= thiswrite ;
		if (thiswrite < writecount)
			break ;
		} ;

	return total ;
} /* pcm_write_d2lei */

/*-----------------------------------------------------------------------------------------------
*/

static	void
sc2s_array	(signed char *src, int count, short *dest)
{	while (count)
	{	count -- ;
		dest [count] = src [count] << 8 ;
		} ;
} /* sc2s_array */

static	void
uc2s_array	(unsigned char *src, int count, short *dest)
{	while (count)
	{	count -- ;
		dest [count] = (((short) src [count]) - 0x80) << 8 ;
		} ;
} /* uc2s_array */

static void
let2s_array (tribyte *src, int count, short *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		dest [count] = LET2H_SHORT_PTR (ucptr) ;
		} ;
} /* let2s_array */

static void
bet2s_array (tribyte *src, int count, short *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		dest [count] = BET2H_SHORT_PTR (ucptr) ;
		} ;
} /* bet2s_array */

static void
lei2s_array (int *src, int count, short *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 4 * count ;
	while (count)
	{	count -- ;
		ucptr -= 4 ;
		dest [count] = LEI2H_SHORT_PTR (ucptr) ;
		} ;
} /* lei2s_array */

static void
bei2s_array (int *src, int count, short *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 4 * count ;
	while (count)
	{	count -- ;
		ucptr -= 4 ;
		dest [count] = BEI2H_SHORT_PTR (ucptr) ;
		} ;
} /* bei2s_array */

/*-----------------------------------------------------------------------------------------------
*/

static	void
sc2i_array	(signed char *src, int count, int *dest)
{	while (count)
	{	count -- ;
		dest [count] = ((int) src [count]) << 24 ;
		} ;
} /* sc2i_array */

static	void
uc2i_array	(unsigned char *src, int count, int *dest)
{	while (count)
	{	count -- ;
		dest [count] = (((int) src [count]) - 128) << 24 ;
		} ;
} /* uc2i_array */

static void
bes2i_array (short *src, int count, int *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 2 * count ;
	while (count)
	{	count -- ;
		ucptr -= 2 ;
		dest [count] = BES2H_INT_PTR (ucptr) ;
		} ;
} /* bes2i_array */

static void
les2i_array (short *src, int count, int *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 2 * count ;
	while (count)
	{	count -- ;
		ucptr -= 2 ;
		dest [count] = LES2H_INT_PTR (ucptr) ;
		} ;
} /* les2i_array */

static void
bet2i_array (tribyte *src, int count, int *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		dest [count] = BET2H_INT_PTR (ucptr) ;
		} ;
} /* bet2i_array */

static void
let2i_array (tribyte *src, int count, int *dest)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		dest [count] = LET2H_INT_PTR (ucptr) ;
		} ;
} /* let2i_array */

/*-----------------------------------------------------------------------------------------------
*/


static	void
sc2f_array	(signed char *src, int count, float *dest, float normfact)
{	while (count)
	{	count -- ;
		dest [count] = ((float) src [count]) * normfact ;
		} ;
} /* sc2f_array */

static	void
uc2f_array	(unsigned char *src, int count, float *dest, float normfact)
{	while (count)
	{	count -- ;
		dest [count] = (((float) src [count]) - 128.0) * normfact ;
		} ;
} /* uc2f_array */

static void
les2f_array (short *src, int count, float *dest, float normfact)
{	short	value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = LES2H_SHORT (value) ;
		dest [count] = ((float) value) * normfact ;
		} ;
} /* les2f_array */

static void
bes2f_array (short *src, int count, float *dest, float normfact)
{	short			value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = BES2H_SHORT (value) ;
		dest [count] = ((float) value) * normfact ;
		} ;
} /* bes2f_array */

static void
let2f_array (tribyte *src, int count, float *dest, float normfact)
{	unsigned char	*ucptr ;
	int 			value ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = LET2H_INT_PTR (ucptr) ;
		dest [count] = ((float) value) * normfact ;
		} ;
} /* let2f_array */

static void
bet2f_array (tribyte *src, int count, float *dest, float normfact)
{	unsigned char	*ucptr ;
	int 	value ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = BET2H_INT_PTR (ucptr) ;
		dest [count] = ((float) value) * normfact ;
		} ;
} /* bet2f_array */

static void
lei2f_array (int *src, int count, float *dest, float normfact)
{	int 			value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = LEI2H_INT (value) ;
		dest [count] = ((float) value) * normfact ;
		} ;
} /* lei2f_array */

static void
bei2f_array (int *src, int count, float *dest, float normfact)
{	int 			value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = BEI2H_INT (value) ;
		dest [count] = ((float) value) * normfact ;
		} ;
} /* bei2f_array */

/*-----------------------------------------------------------------------------------------------
*/

static	void
sc2d_array	(signed char *src, int count, double *dest, double normfact)
{	while (count)
	{	count -- ;
		dest [count] = ((double) src [count]) * normfact ;
		} ;
} /* sc2d_array */

static	void
uc2d_array	(unsigned char *src, int count, double *dest, double normfact)
{	while (count)
	{	count -- ;
		dest [count] = (((double) src [count]) - 128.0) * normfact ;
		} ;
} /* uc2d_array */

static void
les2d_array (short *src, int count, double *dest, double normfact)
{	short	value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = LES2H_SHORT (value) ;
		dest [count] = ((double) value) * normfact ;
		} ;
} /* les2d_array */

static void
bes2d_array (short *src, int count, double *dest, double normfact)
{	short	value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = BES2H_SHORT (value) ;
		dest [count] = ((double) value) * normfact ;
		} ;
} /* bes2d_array */

static void
let2d_array (tribyte *src, int count, double *dest, double normfact)
{	unsigned char	*ucptr ;
	int 	value ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = LET2H_INT_PTR (ucptr) ;
		dest [count] = ((double) value) * normfact ;
		} ;
} /* let2d_array */

static void
bet2d_array (tribyte *src, int count, double *dest, double normfact)
{	unsigned char	*ucptr ;
	int 	value ;

	ucptr = ((unsigned char*) src) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = (ucptr [0] << 24) | (ucptr [1] << 16) | (ucptr [2] << 8) ;
		dest [count] = ((double) value) * normfact ;
		} ;
} /* bet2d_array */

static void
lei2d_array (int *src, int count, double *dest, double normfact)
{	int 	value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = LEI2H_INT (value) ;
		dest [count] = ((double) value) * normfact ;
		} ;
} /* lei2d_array */

static void
bei2d_array (int *src, int count, double *dest, double normfact)
{	int 	value ;

	while (count)
	{	count -- ;
		value = src [count] ;
		value = BEI2H_INT (value) ;
		dest [count] = ((double) value) * normfact ;
		} ;
} /* bei2d_array */

/*-----------------------------------------------------------------------------------------------
*/

static	void
s2sc_array	(short *src, signed char *dest, int count)
{	while (count)
	{	count -- ;
		dest [count] = src [count] >> 8 ;
		} ;
} /* s2sc_array */

static	void
s2uc_array	(short *src, unsigned char *dest, int count)
{	while (count)
	{	count -- ;
		dest [count] = (src [count] >> 8) + 0x80 ;
		} ;
} /* s2uc_array */

static void
s2let_array (short *src, tribyte *dest, int count)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) dest) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		ucptr [0] = 0 ;
		ucptr [1] = src [count] ;
		ucptr [2] = src [count] >> 8 ;
		} ;
} /* s2let_array */

static void
s2bet_array (short *src, tribyte *dest, int count)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) dest) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		ucptr [2] = 0 ;
		ucptr [1] = src [count] ;
		ucptr [0] = src [count] >> 8 ;
		} ;
} /* s2bet_array */

static void
s2lei_array (short *src, int *dest, int count)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) dest) + 4 * count ;
	while (count)
	{	count -- ;
		ucptr -= 4 ;
		ucptr [0] = 0 ;
		ucptr [1] = 0 ;
		ucptr [2] = src [count] ;
		ucptr [3] = src [count] >> 8 ;
		} ;
} /* s2lei_array */

static void
s2bei_array (short *src, int *dest, int count)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) dest) + 4 * count ;
	while (count)
	{	count -- ;
		ucptr -= 4 ;
		ucptr [0] = src [count] >> 8 ;
		ucptr [1] = src [count] ;
		ucptr [2] = 0 ;
		ucptr [3] = 0 ;
		} ;
} /* s2bei_array */

/*-----------------------------------------------------------------------------------------------
*/

static	void
i2sc_array	(int *src, signed char *dest, int count)
{	while (count)
	{	count -- ;
		dest [count] = (src [count] >> 24) ;
		} ;
} /* i2sc_array */

static	void
i2uc_array	(int *src, unsigned char *dest, int count)
{	while (count)
	{	count -- ;
		dest [count] = ((src [count] >> 24) + 128) ;
		} ;
} /* i2uc_array */

static void
i2bes_array (int *src, short *dest, int count)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) dest) + 2 * count ;
	while (count)
	{	count -- ;
		ucptr -= 2 ;
		ucptr [0] = src [count] >> 24 ;
		ucptr [1] = src [count] >> 16 ;
		} ;
} /* i2bes_array */

static void
i2les_array (int *src, short *dest, int count)
{	unsigned char	*ucptr ;

	ucptr = ((unsigned char*) dest) + 2 * count ;
	while (count)
	{	count -- ;
		ucptr -= 2 ;
		ucptr [0] = src [count] >> 16 ;
		ucptr [1] = src [count] >> 24 ;
		} ;
} /* i2les_array */

static void
i2let_array (int *src, tribyte *dest, int count)
{	unsigned char	*ucptr ;
	int				value ;

	ucptr = ((unsigned char*) dest) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = src [count] >> 8 ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [2] = value >> 16 ;
		} ;
} /* i2let_array */

static void
i2bet_array (int *src, tribyte *dest, int count)
{	unsigned char	*ucptr ;
	int				value ;

	ucptr = ((unsigned char*) dest) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = src [count] >> 8 ;
		ucptr [2] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [0] = value >> 16 ;
		} ;
} /* i2bet_array */

/*-----------------------------------------------------------------------------------------------
*/

static	void
f2sc_array	(float *src, signed char *dest, int count, float normfact)
{	while (count)
	{	count -- ;
		dest [count] = lrintf (src [count] * normfact) ;
		} ;
} /* f2sc_array */

static	void
f2uc_array	(float *src, unsigned char *dest, int count, float normfact)
{	while (count)
	{	count -- ;
		dest [count] = lrintf (src [count] * normfact) + 128 ;
		} ;
} /* f2uc_array */

static void
f2bes_array (float *src, short *dest, int count, float normfact)
{	unsigned char	*ucptr ;
	short			value ;
	ucptr = ((unsigned char*) dest) + 2 * count ;
	while (count)
	{	count -- ;
		ucptr -= 2 ;
		value = lrintf (src [count] * normfact) ;
		ucptr [1] = value ;
		ucptr [0] = value >> 8 ;
		} ;
} /* f2bes_array */

static void
f2les_array (float *src, short *dest, int count, float normfact)
{	unsigned char	*ucptr ;
	short			value ;
	ucptr = ((unsigned char*) dest) + 2 * count ;
	while (count)
	{	count -- ;
		ucptr -= 2 ;
		value = lrintf (src [count] * normfact) ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		} ;
} /* f2les_array */

static void
f2bet_array (float *src, tribyte *dest, int count, float normfact)
{	unsigned char	*ucptr ;
	int		value ;

	ucptr = ((unsigned char*) dest) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = lrintf (src [count] * normfact) ;
		ucptr [0] = value >> 16 ;
		ucptr [1] = value >> 8 ;
		ucptr [2] = value ;
		} ;
} /* f2bet_array */

static void
f2let_array (float *src, tribyte *dest, int count, float normfact)
{	unsigned char	*ucptr ;
	int		value ;

	ucptr = ((unsigned char*) dest) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = lrintf (src [count] * normfact) ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [2] = value >> 16 ;
		} ;
} /* f2let_array */

static void
f2bei_array (float *src, int *dest, int count, float normfact)
{	unsigned char	*ucptr ;
	int				value ;

	ucptr = ((unsigned char*) dest) + 4 * count ;
	while (count)
	{	count -- ;
		ucptr -= 4 ;
		value = lrintf (src [count] * normfact) ;
		ucptr [0] = value >> 24 ;
		ucptr [1] = value >> 16 ;
		ucptr [2] = value >> 8 ;
		ucptr [3] = value ;
		} ;
} /* f2bei_array */

static void
f2lei_array (float *src, int *dest, int count, float normfact)
{	unsigned char	*ucptr ;
	int				value ;

	ucptr = ((unsigned char*) dest) + 4 * count ;
	while (count)
	{	count -- ;
		ucptr -= 4 ;
		value = lrintf (src [count] * normfact) ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [2] = value >> 16 ;
		ucptr [3] = value >> 24 ;
		} ;
} /* f2lei_array */

/*-----------------------------------------------------------------------------------------------
*/

static	void
d2sc_array	(double *src, signed char *dest, int count, double normfact)
{	while (count)
	{	count -- ;
		dest [count] = lrint (src [count] * normfact) ;
		} ;
} /* d2sc_array */

static	void
d2uc_array	(double *src, unsigned char *dest, int count, double normfact)
{	while (count)
	{	count -- ;
		dest [count] = lrint (src [count] * normfact) + 128 ;
		} ;
} /* d2uc_array */

static void
d2bes_array (double *src, short *dest, int count, double normfact)
{	unsigned char	*ucptr ;
	short			value ;
	ucptr = ((unsigned char*) dest) + 2 * count ;
	while (count)
	{	count -- ;
		ucptr -= 2 ;
		value = lrint (src [count] * normfact) ;
		ucptr [1] = value ;
		ucptr [0] = value >> 8 ;
		} ;
} /* d2bes_array */

static void
d2les_array (double *src, short *dest, int count, double normfact)
{	unsigned char	*ucptr ;
	short			value ;
	ucptr = ((unsigned char*) dest) + 2 * count ;
	while (count)
	{	count -- ;
		ucptr -= 2 ;
		value = lrint (src [count] * normfact) ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		} ;
} /* d2les_array */

static void
d2bet_array (double *src, tribyte *dest, int count, double normfact)
{	unsigned char	*ucptr ;
	int		value ;

	ucptr = ((unsigned char*) dest) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = lrint (src [count] * normfact) ;
		ucptr [2] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [0] = value >> 16 ;
		} ;
} /* d2bet_array */

static void
d2let_array (double *src, tribyte *dest, int count, double normfact)
{	unsigned char	*ucptr ;
	int		value ;

	ucptr = ((unsigned char*) dest) + 3 * count ;
	while (count)
	{	count -- ;
		ucptr -= 3 ;
		value = lrint (src [count] * normfact) ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [2] = value >> 16 ;
		} ;
} /* d2let_array */

static void
d2bei_array (double *src, int *dest, int count, double normfact)
{	unsigned char	*ucptr ;
	int				value ;

	ucptr = ((unsigned char*) dest) + 4 * count ;
	while (count)
	{	count -- ;
		ucptr -= 4 ;
		value = lrint (src [count] * normfact) ;
		ucptr [0] = value >> 24 ;
		ucptr [1] = value >> 16 ;
		ucptr [2] = value >> 8 ;
		ucptr [3] = value ;
		} ;
} /* d2bei_array */

static void
d2lei_array (double *src, int *dest, int count, double normfact)
{	unsigned char	*ucptr ;
	int				value ;

	ucptr = ((unsigned char*) dest) + 4 * count ;
	while (count)
	{	count -- ;
		ucptr -= 4 ;
		value = lrint (src [count] * normfact) ;
		ucptr [0] = value ;
		ucptr [1] = value >> 8 ;
		ucptr [2] = value >> 16;
		ucptr [3] = value >> 24 ;
		} ;
} /* d2lei_array */

/*==============================================================================
*/
