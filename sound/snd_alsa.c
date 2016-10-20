#include <stdio.h>
#include <stdlib.h>

#include "snd_alsa.h"

int snd_alsa_open(snd_alsa_t *snd_alsa, char *name)
{
	int ret = 0;
	ret = snd_pcm_open(&snd_alsa->handle,name,SND_PCM_STREAM_PLAYBACK, 0);
	if (ret < 0) {
		fprintf (stderr, "cannot open audio device %s (%s)\n", name,snd_strerror (ret));
		goto lab_err;
	}

	ret = snd_pcm_hw_params_malloc (&snd_alsa->hw_params);
	if (ret < 0) {
		fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",snd_strerror (ret));
		goto lab_err;
	}
	
	ret = snd_pcm_hw_params_any (snd_alsa->handle,snd_alsa->hw_params);
	if (ret < 0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",snd_strerror (ret));
		goto lab_err;
	}

	//pthread_mutex_init(&snd_alsa->lock);
	return 0;
lab_err:
	return -1;
}

int snd_alsa_set(snd_alsa_t *snd_alsa, snd_pcm_format_t  pcm_fmt, unsigned int rate,unsigned int channel)
{
	int dir = 0;
	int ret = 0;
	
	if ((ret = snd_pcm_hw_params_set_access (snd_alsa->handle, snd_alsa->hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "cannot set access type (%s)\n",snd_strerror (ret));
		goto lab_err;
	}

	if ((ret = snd_pcm_hw_params_set_format (snd_alsa->handle, snd_alsa->hw_params, pcm_fmt)) < 0) {
		fprintf (stderr, "cannot set sample format (%s)\n",snd_strerror (ret));
		goto lab_err;
	}

	if ((ret = snd_pcm_hw_params_set_rate_near (snd_alsa->handle, snd_alsa->hw_params, &rate, &dir)) < 0) {
		fprintf (stderr, "cannot set sample rate (%s)\n",snd_strerror (ret));
		goto lab_err;
	}

	if ((ret = snd_pcm_hw_params_set_channels (snd_alsa->handle, snd_alsa->hw_params, channel)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n",snd_strerror (ret));
		goto lab_err;
	}

	if ((ret = snd_pcm_hw_params (snd_alsa->handle, snd_alsa->hw_params)) < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n",snd_strerror (ret));
		goto lab_err;
	}
	
	return 0;
lab_err:
	return -1;
}

int snd_alsa_writei(snd_alsa_t *snd_alsa, void *buf, snd_pcm_uframes_t size)
{
	int ret = 0;

	if (NULL == snd_alsa->handle) {
		return -1;
	}
	
	ret = snd_pcm_writei(snd_alsa->handle, buf, size);
	if (-EPIPE == ret) {
		snd_pcm_prepare(snd_alsa->handle);
	} else if (ret < 0) {
		ret = snd_pcm_recover(snd_alsa->handle, ret, 0);
	}
	
	return ret;
}

int snd_alsa_writen(snd_alsa_t *snd_alsa, void *buf, snd_pcm_uframes_t size)
{
	int ret = 0;
	ret = snd_pcm_writen(snd_alsa->handle, buf, size);
	if (-EPIPE == ret) {
		snd_pcm_prepare(snd_alsa->handle);
	} else if (ret < 0) {
		ret = snd_pcm_recover(snd_alsa->handle, ret, 0);
	}
	
	return ret;
}

int snd_alsa_pause(snd_alsa_t *snd_alsa)
{
	int ret = 0;
	//ret = snd_pcm_drop(snd_alsa->handle);
	ret = snd_pcm_pause(snd_alsa->handle,1); //hardware support
	if (ret < 0) {
		fprintf (stderr, "pause err (%s)\n",snd_strerror (ret));
	}

	return ret;
}

int snd_alsa_resume(snd_alsa_t *snd_alsa)
{
	int ret = 0;
	//ret = snd_pcm_prepare(snd_alsa->handle);
	ret = snd_pcm_pause(snd_alsa->handle,0);
	if (ret < 0) {
		fprintf (stderr, "resume err (%s)\n",snd_strerror (ret));
	}

	return ret;
}

int snd_alsa_close(snd_alsa_t *snd_alsa)
{
	snd_pcm_hw_params_free (snd_alsa->hw_params);
	snd_pcm_close(snd_alsa->handle);
	snd_alsa->handle = NULL;
	
	return 0;
}


