#ifndef _SND_ALSA_H
#define _SND_ALSA_H

#ifdef _cplusplus
	extern "c" {
#endif

#include <alsa/asoundlib.h>


typedef struct {
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *hw_params;

	int sample_rate;
	int nb_channel;
	snd_pcm_format_t fmt;

	//pthread_mutex_t lock;
} snd_alsa_t;


#ifdef _cplusplus
	}
#endif

#endif

