/**alsa play test 
*ALSA�û��ռ���룬ALSA�������������û��ռ䣬����ֱ��ʹ�� 
*�ļ��ӿ��У���Ӧʹ��alsa-lib 
*��---->���ò���--->��д��Ƶ���� ALSAȫ��ʹ��alsa-lib�е�API 
*������� 
*export LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH 
*arm-linux-gcc -o alsa_play alsa_play_test.c -L. -lasound 
*��Ҫ���������libasound.so���֧�� 
* 
*/	
#include <stdio.h>	
#include <stdlib.h>  
#include "alsa/asoundlib.h"  
  
int main(int argc, char *argv[])  
{  
	int i;	
	int ret;  
	int buf[128];  
	unsigned int val;  
	int dir=0;	
	char *buffer;  
	int size;  
	snd_pcm_uframes_t frames;  
	snd_pcm_uframes_t periodsize;  
	snd_pcm_t *playback_handle;//PCM�豸���pcm.h  
	snd_pcm_hw_params_t *hw_params;//Ӳ����Ϣ��PCM������  
	if (argc != 2) {  
		printf("error: alsa_play_test [music name]\n");  
		exit(1);  
	}  
	printf("play song %s by wolf\n", argv[1]);	
	FILE *fp = fopen(argv[1], "rb");  
	if(fp == NULL)	
		return 0;  
	fseek(fp, 100, SEEK_SET);  
	
	//1. ��PCM�����һ������Ϊ0��ζ�ű�׼����  
	ret = snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);  
	if (ret < 0) {	
		perror("snd_pcm_open");  
		exit(1);  
	}  
	  
	//2. ����snd_pcm_hw_params_t�ṹ��	
	ret = snd_pcm_hw_params_malloc(&hw_params);  
	if (ret < 0) {	
		perror("snd_pcm_hw_params_malloc");  
		exit(1);  
	}  
	//3. ��ʼ��hw_params  
	ret = snd_pcm_hw_params_any(playback_handle, hw_params);  
	if (ret < 0) {	
		perror("snd_pcm_hw_params_any");  
		exit(1);  
	}  
	//4. ��ʼ������Ȩ��  
	ret = snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);	
	if (ret < 0) {	
		perror("snd_pcm_hw_params_set_access");  
		exit(1);  
	}  
	//5. ��ʼ��������ʽSND_PCM_FORMAT_U8,8λ  
	ret = snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S16_LE);	
	if (ret < 0) {	
		perror("snd_pcm_hw_params_set_format");  
		exit(1);  
	}  
	//6. ���ò����ʣ����Ӳ����֧���������õĲ����ʣ���ʹ����ӽ���  
	//val = 44100,��Щ¼������Ƶ�ʹ̶�Ϊ8KHz  
	  
  
	//val = 8000;  
	val = 44100;
	ret = snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, &val, &dir);	
	if (ret < 0) {	
		perror("snd_pcm_hw_params_set_rate_near");	
		exit(1);  
	}  
	//7. ����ͨ������  
	ret = snd_pcm_hw_params_set_channels(playback_handle, hw_params, 2);  
	if (ret < 0) {	
		perror("snd_pcm_hw_params_set_channels");  
		exit(1);  
	}  
	  
	/* Set period size to 32 frames. */  
	frames = 32;  
	periodsize = frames * 2;  
	ret = snd_pcm_hw_params_set_buffer_size_near(playback_handle, hw_params, &periodsize);	
	if (ret < 0)   
	{  
		 printf("Unable to set buffer size %li : %s\n", frames * 2, snd_strerror(ret));  
		   
	}  
	periodsize /= 2;	
  
	ret = snd_pcm_hw_params_set_period_size_near(playback_handle, hw_params, &periodsize, 0);  
	if (ret < 0)   
	{  
		printf("Unable to set period size %li : %s\n", periodsize,	snd_strerror(ret));  
	}  
									
	//8. ����hw_params	
	ret = snd_pcm_hw_params(playback_handle, hw_params);  
	if (ret < 0) {	
		perror("snd_pcm_hw_params");  
		exit(1);  
	}  
	  
	 /* Use a buffer large enough to hold one period */  
	snd_pcm_hw_params_get_period_size(hw_params, &frames, &dir);  
								  
	size = frames * 2 * 2; /* 2 bytes/sample, 2 channels */  
	buffer = (char *) malloc(size);  
	fprintf(stderr,  "size = %d\n",size);
	  
	while (1)	
	{  
		ret = fread(buffer, 1, size, fp);  
		if(ret == 0)   
		{  
			  fprintf(stderr, "end of file on input\n");  
			  break;  
		}	
		else if (ret != size)	
		{  
		}  
		//9. д��Ƶ���ݵ�PCM�豸  
		while(ret = snd_pcm_writei(playback_handle, buffer, frames)<0)	
		{  
			usleep(2000);  
			if (ret == -EPIPE)	
			{  
				  /* EPIPE means underrun */  
				  fprintf(stderr, "underrun occurred\n");  
				  //���Ӳ���������ã�ʹ�豸׼����	
				  snd_pcm_prepare(playback_handle);  
			}	
			else if (ret < 0)	
			{  
				  fprintf(stderr,   "error from writei: %s\n",  snd_strerror(ret));  
			}	 
		}  
		  
	}		  
	//10. �ر�PCM�豸���  
	snd_pcm_close(playback_handle);  
	  
	return 0;  
}  

