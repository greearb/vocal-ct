/*
 * g711.h
 *
 * Vovida file 
 *
 */

#ifndef _G711_H
#define _G711_H

#ifdef  __cplusplus
extern "C"
{
#endif

    unsigned char linear2ulaw( int pcm_val );
    int ulaw2linear( unsigned char u_val );
    unsigned char linear2alaw( int pcm_val );
    int alaw2linear( unsigned char a_val );

    int predictor_zero( struct g72x_state *state_ptr );
    int predictor_pole( struct g72x_state *state_ptr );
    int step_size( struct g72x_state *state_ptr );
    int quantize( int d, int y, short *table, int size );
    int reconstruct( int sign, int dqln, int y );
    void update( int code_size, int y, int wi, int fi,
                 int dq, int sr, int dqsez, struct g72x_state *state_ptr );
    int tandem_adjust_alaw( int sr, int se, int y, int i, int sign, short *qtab );
    int tandem_adjust_ulaw( int sr, int se, int y, int i, int sign, short *qtab );

    void g72x_init_state( struct g72x_state *state_ptr);

#ifdef __cplusplus
};
#endif

#endif /* !_G711_H */

