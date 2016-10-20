#ifndef _SPECTRUM_ANALYSER_H
#define _SPECTRUM_ANALYSER_H

#ifdef _cplusplus
	extern "c" {
#endif

#include <linux/types.h>

#include "fast_fourier_transform.h"
#include "cdlb_alk_fifo.h"

# if __WORDSIZE == 64
	typedef long int              int64_t;
	typedef unsigned long int uint64_t;
# else
	__extension__
	typedef long long int      int64_t;
	typedef unsigned long long uint64_t;
#endif

typedef int64_t			jlong;
typedef unsigned int		juint;
typedef  uint64_t			julong;
typedef long				jint;
typedef signed char			jbyte;

typedef void (*cb_func_t)(float *feq_domain, int num, void *cb_data);

typedef struct {
	fft_t m_FFT;

	/* digital signal process */
	unsigned long m_AudioDataBufferLength;
	jbyte* m_AudioDataBuffer;
	int m_SampleSize;
	long m_FpsAsNS;
	long m_DesiredFpsAsNS;
	float* m_Left;
	float* m_Right;
	int m_position;
	int m_offset;
	int m_sampleType;
	int m_channelMode;

	int* m_peaks;
	int* m_peaksDelay;
	float* m_oldFFT;
	int m_saFFTSampleSize;
	int m_saBands;
	float m_saMultiplier;
	float m_saDecay;
	int m_barOffset;
	int m_peakDelay;

	float *m_freqDomain;

	alk_fifo_t *alk_fifo;

	void *cb_data;
	cb_func_t cb_func;
} spectrum_analyser_t;


int sptaly_init(spectrum_analyser_t *sptaly);
int sptaly_destory(spectrum_analyser_t *sptaly);
void sptaly_prepare(spectrum_analyser_t *sptaly);
void sptaly_process(spectrum_analyser_t *sptaly, float pFrameRateRatioHint);
int sptaly_copy_sample(spectrum_analyser_t *sptaly, char *data, int len);
int sptaly_get_freq_domain(spectrum_analyser_t *sptaly, float *freq, int num);
int register_cb_func(spectrum_analyser_t *sptaly, cb_func_t func, void *cb_data);









#ifdef _cplusplus
	}
#endif

#endif




