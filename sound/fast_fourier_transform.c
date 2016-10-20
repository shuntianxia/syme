#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "fast_fourier_transform.h"


int bitrev(int j, int nu) {
	int j1 = j;
	int k = 0;
	int i = 0;
	for (i = 1; i <= nu; i++) {
		int j2 = j1 >> 1;
		k = ((k << 1) + j1) - (j2 << 1);
		j1 = j2;
	}

	return k;
}

void fft_prepare_FFT_tables(fft_t *fft)
{
	int n2 = fft->ss2;
	int nu1 = fft->nu - 1;

	//fft->fftSin = new float[fft->nu * n2];
	//fft->fftCos = new float[fft->nu * n2];

	fft->fftSin = (float *)malloc(sizeof(float) * fft->nu * n2);
	if (!fft->fftSin) {
		fprintf(stderr,"[%s:%d] no mem!\n",__FUNCTION__,__LINE__);
	}
	
	fft->fftCos = (float *)malloc(sizeof(float) * fft->nu * n2);
	if (!fft->fftCos) {
		fprintf(stderr,"[%s:%d] no mem!\n",__FUNCTION__,__LINE__);
	}

	int k = 0;
	int x = 0;
	int l = 0;
	for (l = 1; l <= fft->nu; l++) {
		while (k < fft->ss) {
			int i = 0;
			for (i = 1; i <= n2; i++) {
				float p = (float)bitrev(k >> nu1, fft->nu);
				float arg = (PI_2 * p) / (float) fft->ss;
				fft->fftSin[x] = (float) sin(arg);
				fft->fftCos[x] = (float) cos(arg);
				k++;
				x++;
			}

			k += n2;
		}

		k = 0;
		nu1--;
		n2 >>= 1;
	}

	//fft->fftBr = new int[fft->ss];
	fft->fftBr =(int *)malloc(sizeof(int) * fft->ss);
	if (!fft->fftBr) {
		fprintf(stderr,"[%s:%d] no mem!\n",__FUNCTION__,__LINE__);
	}
	for (k = 0; k < fft->ss; k++)
		fft->fftBr[k] = bitrev(k, fft->nu);
}

float* fft_calculate(fft_t *fft, float* sample, size_t sample_size) {
	int n2 = fft->ss2;
	int nu1 = fft->nu - 1;
	int wAps = sample_size / fft->ss;
	
	size_t a = 0;
	size_t b = 0;
	for (b = 0; a < sample_size && b<fft->ss; b++) {
		
		fft->xre[b] = sample[a];
		fft->xim[b] = 0.0F;
		a += wAps;
		if (b >= fft->ss) {
			fprintf(stderr,"----out of mem------------b---%d-- a----%d-----------\n",b,a);
		}
	}

	//fprintf(stderr,"----------------b---%d-----------------\n",b);

	int x = 0;
	int l = 0;
	for (l = 1; l <= fft->nu; l++) {
		int k = 0;
		for (k = 0; k < fft->ss; k += n2) {
			int i = 0;
			for (i = 1; i <= n2; i++) {
				float c = fft->fftCos[x];
				float s = fft->fftSin[x];
				int kn2 = k + n2;
				float tr = fft->xre[kn2] * c + fft->xim[kn2] * s;
				float ti = fft->xim[kn2] * c - fft->xre[kn2] * s;
				fft->xre[kn2] = fft->xre[k] - tr;
				fft->xim[kn2] = fft->xim[k] - ti;
				fft->xre[k] += tr;
				fft->xim[k] += ti;
				k++;
				x++;
			}
		}

		nu1--;
		n2 >>= 1;
	}
	
	int k = 0;
	for (k = 0; k < fft->ss; k++) {
		int r = fft->fftBr[k];
		if (r > k) {
			float tr = fft->xre[k];
			float ti = fft->xim[k];
			fft->xre[k] = fft->xre[r];
			fft->xim[k] = fft->xim[r];
			fft->xre[r] = tr;
			fft->xim[r] = ti;
		}
	}

	fft->mag[0] = (float) sqrt(fft->xre[0] * fft->xre[0] + fft->xim[0] * fft->xim[0]) / (float) fft->ss;
	int i = 0;
	for (i = 1; i < fft->ss2; i++)
		fft->mag[i] = (2.0F * (float) sqrt(fft->xre[i] * fft->xre[i] + fft->xim[i] * fft->xim[i])) / (float) fft->ss;

	return fft->mag;
}

int fft_init(fft_t *fft, size_t sample_size)
{
	fft->xre = NULL;
	fft->xim = NULL;
	fft->mag = NULL;
	fft->fftSin = NULL;
	fft->fftCos = NULL;
	fft->fftBr = NULL;

	fft->ss = sample_size;
	fft->ss2 = fft->ss >> 1;
	fft->nu = (int) (log((float)fft->ss) / log((float)2));
	fft->nu1 = fft->nu - 1;

	
	fft->xre = (float *)malloc(sizeof(float)*fft->ss); // real part
	if (!fft->xre) {
		fprintf(stderr,"[%s:%d] no mem!\n",__FUNCTION__,__LINE__);
	}
	fft->xim = (float *)malloc(sizeof(float)*fft->ss); // image part
	if (!fft->xim) {
		fprintf(stderr,"[%s:%d] no mem!\n",__FUNCTION__,__LINE__);
	}
	fft->mag = (float *)malloc(sizeof(float)*fft->ss2); 
	if (!fft->mag) {
		fprintf(stderr,"[%s:%d] no mem!\n",__FUNCTION__,__LINE__);
	}

	fft_prepare_FFT_tables(fft);
}

int fft_destory(fft_t *fft)
{
	if(fft->xre)
		free(fft->xre);

	if(fft->xim)
		free(fft->xim);

	if(fft->mag)
		free(fft->mag);

	if(fft->fftSin)
		free(fft->fftSin);

	if(fft->fftCos)
		free(fft->fftCos);

	if(fft->fftBr)
		free(fft->fftBr);

	fft->xre = NULL;
	fft->xim = NULL;
	fft->mag = NULL;
	fft->fftSin = NULL;
	fft->fftCos = NULL;
	fft->fftBr = NULL;

	return 0;
}



