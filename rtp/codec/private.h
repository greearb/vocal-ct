/**
 *
 * $Log: private.h,v $
 * Revision 1.1  2004/05/01 04:15:23  greear
 * Initial revision
 *
 * Revision 1.1  2004/01/23 08:04:37  greear
 * Add G726-16, 24, 32, and 40 codecs.  Only the 16 and 32 ones sound
 * right though...must have bit-packing errors in the other two yet.
 * --Ben
 *
 * Revision 1.1  2003/01/21 10:50:49  icahoon
 * Upgraded openh323/pwlib
 *
 * Revision 1.4  2002/11/20 04:29:13  robertj
 * Included optimisations for G.711 and G.726 codecs, thanks Ted Szoczei
 *
 * Revision 1.1  2002/02/11 23:24:23  robertj
 * Updated to openH323 v1.8.0
 *
 * Revision 1.2  2002/02/10 21:14:54  dereks
 * Add cvs log history to head of the file.
 * Ensure file is terminated by a newline.
 *
 *
 *
 */
#if !defined G726_PRIVATE
#define G726_PRIVATE

#include "g726.h"

#ifdef  __cplusplus
extern "C"
{
#endif


int g726_fmult(int an, int srn);
int g726_predictor_zero(	struct g726_state_s *state_ptr);
int g726_predictor_pole(	struct g726_state_s *state_ptr);
int g726_step_size(	struct g726_state_s *state_ptr);
int g726_quantize(	int		d,	/* Raw difference signal sample */
	int		y,	/* Step size multiplier */
	int *	table,	/* quantization table */
	int		size);	/* table size of short integers */
int g726_reconstruct(	int		sign,	/* 0 for non-negative value */
	int		dqln,	/* G.72x codeword */
	int		y);	/* Step size multiplier */
void g726_update(	int		code_size,	/* distinguish 723_40 with others */
	int		y,		/* quantizer step size */
	int		wi,		/* scale factor multiplier */
	int		fi,		/* for long/short term energies */
	int		dq,		/* quantized prediction difference */
	int		sr,		/* reconstructed signal */
	int		dqsez,		/* difference from 2-pole predictor */
	struct g726_state_s *state_ptr);	/* coder state pointer */
int g726_tandem_adjust_alaw(
	int		sr,	/* decoder output linear PCM sample */
	int		se,	/* predictor estimate sample */
	int		y,	/* quantizer step size */
	int		i,	/* decoder input code */
	int		sign,
	int *	qtab);
int g726_tandem_adjust_ulaw(
	int		sr,	/* decoder output linear PCM sample */
	int		se,	/* predictor estimate sample */
	int		y,	/* quantizer step size */
	int		i,	/* decoder input code */
	int		sign,
	int *	qtab);

#ifdef __cplusplus
};
#endif

#endif // G726_PRIVATE

