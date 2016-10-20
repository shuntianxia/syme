/**
** it's not good!!!
**
**/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#include "spectrum_analyser.h"

/* constants used in direct sound */
#define DEFAULT_BUFFER_SIZE 88200
#define DEFAULT_SAMPLE_RATE 44100.0F
#define DEFAULT_FRAME_SIZE	4
#define DEFAULT_BITS_PER_SAMPLE	16
#define DEFAULT_CHANNELS	2
#define DEFAULT_SECONDS		2
#define DEFAULT_DS_BUFFER_SIZE	DEFAULT_SECONDS*DEFAULT_SAMPLE_RATE*(DEFAULT_BITS_PER_SAMPLE>>3)*DEFAULT_CHANNELS

/* constants used in digital signal process */
#define DEFAULT_SAMPLE_SIZE 2048
#define DEFAULT_FPS 30
#define DEFAULT_SPECTRUM_ANALYSER_FFT_SAMPLE_SIZE 512
#define DEFAULT_SPECTRUM_ANALYSER_BAND_COUNT 30
#define DEFAULT_SPECTRUM_ANALYSER_DECAY 0.05F
#define DEFAULT_SPECTRUM_ANALYSER_PEAK_DELAY 5 /* the value is more lower, fall faster */
#define DEFAULT_SPECTRUM_ANALYSER_PEAK_DELAY_FPS_RATIO 0.4F
#define DEFAULT_SPECTRUM_ANALYSER_PEAK_DELAY_FPS_RATIO_RANGE 0.1F
#define MIN_SPECTRUM_ANALYSER_DECAY	0.02F
#define MAX_SPECTRUM_ANALYSER_DECAY 0.08F
#define SAMPLE_TYPE_EIGHT_BIT 1
#define SAMPLE_TYPE_SIXTEEN_BIT 2
#define CHANNEL_MODE_MONO 1
#define CHANNEL_MODE_STEREO 2


int sptaly_init(spectrum_analyser_t *sptaly)
{
	
	/* digital signal process */
	sptaly->m_AudioDataBufferLength = DEFAULT_BUFFER_SIZE << 1;
	//sptaly->m_AudioDataBuffer = new jbyte[sptaly->m_AudioDataBufferLength];
	sptaly->m_AudioDataBuffer = (jbyte *)malloc(sizeof(jbyte)*sptaly->m_AudioDataBufferLength);
	sptaly->m_SampleSize = DEFAULT_SAMPLE_SIZE;
	sptaly->m_DesiredFpsAsNS = 0x3B9ACA00L / DEFAULT_FPS;
	sptaly->m_FpsAsNS = sptaly->m_DesiredFpsAsNS;
	//sptaly->m_Left = new float[DEFAULT_SAMPLE_SIZE];
	//sptaly->m_Right = new float[DEFAULT_SAMPLE_SIZE];
	sptaly->m_Left = (float *)malloc(sizeof(float)*DEFAULT_SAMPLE_SIZE);
	sptaly->m_Right = (float *)malloc(sizeof(float)*DEFAULT_SAMPLE_SIZE);
	sptaly->m_position = 0;
	
	sptaly->m_sampleType = SAMPLE_TYPE_SIXTEEN_BIT;
	sptaly->m_channelMode = CHANNEL_MODE_STEREO;
	
	memset(sptaly->m_AudioDataBuffer, 0, sptaly->m_AudioDataBufferLength);
	memset(sptaly->m_Left, 0, DEFAULT_SAMPLE_SIZE);
	memset(sptaly->m_Right, 0, DEFAULT_SAMPLE_SIZE);
	
	sptaly->m_saFFTSampleSize = DEFAULT_SPECTRUM_ANALYSER_FFT_SAMPLE_SIZE;
	sptaly->m_saBands = DEFAULT_SPECTRUM_ANALYSER_BAND_COUNT;
	sptaly->m_saDecay = DEFAULT_SPECTRUM_ANALYSER_DECAY;
	//sptaly->m_FFT = new CFastFourierTransform(sptaly->m_saFFTSampleSize);
	fft_init(&sptaly->m_FFT,sptaly->m_saFFTSampleSize);
	////sptaly->m_peaks = new INT[sptaly->m_saBands];
	////sptaly->m_peaksDelay = new INT[sptaly->m_saBands];
	//sptaly->m_oldFFT = new float[sptaly->m_saFFTSampleSize];
	sptaly->m_oldFFT = (float *)malloc(sizeof(float)*sptaly->m_saFFTSampleSize);
	sptaly->m_saMultiplier = (float)((sptaly->m_saFFTSampleSize / 2) / sptaly->m_saBands);

	sptaly->m_freqDomain =  (float *)malloc(sizeof(float) * sptaly->m_saBands);
	sptaly->alk_fifo = alk_fifo_init((unsigned char * )sptaly->m_AudioDataBuffer,sptaly->m_AudioDataBufferLength);
	sptaly->cb_func = NULL;
	sptaly->cb_data = NULL;
	return 0;
}

int sptaly_destory(spectrum_analyser_t *sptaly)
{
	if(sptaly->m_AudioDataBuffer)
		free(sptaly->m_AudioDataBuffer);

	if(sptaly->m_Left)
		free(sptaly->m_Left);

	if(sptaly->m_Right)
		free(sptaly->m_Right);

	//if(sptaly->m_peaks)
	//	free(sptaly->m_peaks);

	//if(sptaly->m_peaksDelay)
	//	free(sptaly->m_peaksDelay);

	if(sptaly->m_oldFFT)
		free(sptaly->m_oldFFT);

	if(sptaly->m_freqDomain)
		free(sptaly->m_freqDomain);
	
	fft_destory(&sptaly->m_FFT);
	alk_fifo_destory(sptaly->alk_fifo);
	
	sptaly->m_AudioDataBuffer = NULL;
	sptaly->m_Left = NULL;
	sptaly->m_Right = NULL;
	//sptaly->m_peaks = NULL;
	//sptaly->m_peaksDelay = NULL;
	sptaly->m_oldFFT = NULL;
	//sptaly->m_FFT = NULL;

	return 0;
}

void sptaly_prepare(spectrum_analyser_t *sptaly)
{
	int len = 0;
	int a = 0;
	int c = 0; //start pos
	int frame_size = 0;
	char buf[8192] = {0};
	
	frame_size = sptaly->m_channelMode * sptaly->m_sampleType;
	len = alk_fifo_get(sptaly->alk_fifo,(unsigned char *)buf,sptaly->m_SampleSize * frame_size);
	if (sptaly->m_channelMode == 1 && sptaly->m_sampleType == 1) {
		for (a = 0; a < sptaly->m_SampleSize && c < len;) {
			sptaly->m_Left[a] = (float) buf[c] / 128.0F;
			sptaly->m_Right[a] = sptaly->m_Left[a];
			a++;
			c++;
		}
	} else if (sptaly->m_channelMode == 2 && sptaly->m_sampleType == 1) {
		for (a = 0; a < sptaly->m_SampleSize && c<len;) {
			sptaly->m_Left[a] = (float) buf[c] / 128.0F;
			sptaly->m_Right[a] = (float) buf[c + 1] / 128.0F;
			a++;
			c += 2;
		}
	} else if (sptaly->m_channelMode == 1 && sptaly->m_sampleType == 2) {
		for (a = 0; a < sptaly->m_SampleSize && c<len;) {
			sptaly->m_Left[a] = (float) ((buf[c + 1] << 8) + buf[c]) / 32767.0F;
			sptaly->m_Right[a] = sptaly->m_Left[a];
			a++;
			c += 2;
		}
	} else if (sptaly->m_channelMode == 2 && sptaly->m_sampleType == 2) {
		for (a = 0; a < sptaly->m_SampleSize && c < len;) {
			sptaly->m_Left[a] = (float) ((buf[c + 1] << 8) + buf[c]) / 32767.0F;
			sptaly->m_Right[a] = (float) ((buf[c + 3] << 8) + buf[c + 2]) / 32767.0F;
			a++;
			c += 4;
		}
	}

	//fprintf(stderr,"a:%d c:%d\n",a,c);
}

void sptaly_process(spectrum_analyser_t *sptaly, float pFrameRateRatioHint)
{
	int a = 0;
	int bd = 0;
	
	for (a = 0; a < sptaly->m_SampleSize; a++) {
		sptaly->m_Left[a] = (sptaly->m_Left[a] + sptaly->m_Right[a]) / 2.0f;
	}

	//fprintf(stderr,"pFrameRateRatioHint:%f\n",pFrameRateRatioHint);
	
	float pFrrh = pFrameRateRatioHint;
	//float* wFFT = sptaly->m_FFT->Calculate(sptaly->m_Left, sptaly->m_SampleSize);
	float* wFFT = fft_calculate(&sptaly->m_FFT,sptaly->m_Left, sptaly->m_SampleSize);
	float wSadfrr = sptaly->m_saDecay * pFrrh;

	
	float wFs = 0;
	for (a = 0,bd = 0; bd < sptaly->m_saBands; a += (int)sptaly->m_saMultiplier, bd++) {
		//float wFs = 0;
		wFs = 0;
		int b = 0;
		for (b = 0; b < (int)sptaly->m_saMultiplier; b++) {
			wFs += wFFT[a + b];
		}
		
		wFs = (wFs * (float) log(bd + 2.0F));

		if(wFs > 0.005F && wFs < 0.009F)
			wFs *= 50.0F;
		else if(wFs > 0.01F && wFs < 0.1F)
			wFs *= 10.0F;
		else if(wFs > 0.1F && wFs < 0.5F)
			wFs *= PI; //enlarge PI times, if do not, the bar display abnormally, why??

		if (wFs > 1.0F) {
			wFs = 1.0F;
		}
		
		if (wFs >= (sptaly->m_oldFFT[a] - wSadfrr)) {
			sptaly->m_oldFFT[a] = wFs;
		} else {
			sptaly->m_oldFFT[a] -= wSadfrr;
			if (sptaly->m_oldFFT[a] < 0) {
				sptaly->m_oldFFT[a] = 0;
			}
			wFs = sptaly->m_oldFFT[a];
		}
		
		sptaly->m_freqDomain[bd] = wFs;
	}

	if (sptaly->cb_func) {
		(*sptaly->cb_func)(sptaly->m_freqDomain,sptaly->m_saBands,sptaly->cb_data);
	}

#if 0
	int k = 0;
	for (k=0; k<sptaly->m_saBands; k++) {
		fprintf(stderr," %f ",sptaly->m_freqDomain[k] );
	}
	fprintf(stderr,"\n");
#endif
}

int sptaly_copy_sample(spectrum_analyser_t *sptaly, char *data, int len)
{
	return alk_fifo_put(sptaly->alk_fifo,(unsigned char *)data,len);
}

int sptaly_get_freq_domain(spectrum_analyser_t *sptaly, float *freq, int num)
{
	if (sptaly->m_freqDomain) {
		memcpy(freq,sptaly->m_freqDomain,num*sizeof(float));
	} else {
		memset(freq,0,num*sizeof(float));
	}
	
	return 0;
}

int register_cb_func(spectrum_analyser_t *sptaly, cb_func_t func, void *cb_data)
{
	sptaly->cb_func = func;
	sptaly->cb_data = cb_data;
	
	return 0;
}

#if 0
int main(int argc, char *argv[])
{
	int rd_size = 0;
	char *aud_file_path = NULL;
	
	spectrum_analyser_t sptaly;
	sptaly_init(&sptaly);

	aud_file_path = argv[1];
	
	char buf[DEFAULT_SAMPLE_SIZE] = {0};
	int fd = open(aud_file_path,O_RDONLY,0);
	if (fd < 0) {
		fprintf(stderr,"open failed!\n");
		return -1;
	}
	
	float wFrr = (float) sptaly.m_FpsAsNS / (float) sptaly.m_DesiredFpsAsNS;
	//while (rd_size = read(fd,buf,sizeof(buf))) {
	//	memcpy(sptaly.m_AudioDataBuffer,buf,rd_size);
	//	sptaly.m_SampleSize = rd_size;
	//	sptaly_prepare(&sptaly);
	//	sptaly_process(&sptaly,wFrr);
	//}
	sptaly_test_float(&sptaly,1.2,1.6,1.8);
	sptaly_test_float(&sptaly,3.42,3.54,1.69);
	sptaly_test_float(&sptaly,1.84,1.48,1.7812);
	
	close(fd);
	sptaly_destory(&sptaly);

	return 0;
}
#endif

