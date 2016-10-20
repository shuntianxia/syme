#ifndef _FAST_FOURIER_TRANSFORM_H
#define _FAST_FOURIER_TRANSFORM_H

#ifdef _cplusplus
	extern "c" {
#endif

#include <stddef.h>


/************************************************************************/
/* FastFourierTransform                                               			 */
/************************************************************************/
#define PI_2 6.283185F
#define PI   3.1415925F
typedef struct 
{
	float* xre;
	float* xim;
	float* mag;
	float* fftSin;
	float* fftCos;
	int* fftBr;
	int ss;
	int ss2;
	int nu;
	int nu1;

	int (*bitrev)(int j, int nu);
} fft_t; /*fast fourier transform*/


int fft_init(fft_t *fft, size_t sample_size);
int fft_destory(fft_t *fft);
float* fft_calculate(fft_t *fft, float* sample, size_t sample_size);
void fft_prepare_FFT_tables(fft_t *fft);


#ifdef _cplusplus
	}
#endif

#endif

