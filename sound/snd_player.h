#ifndef _SND_PLAYER_H
#define _SND_PLAYER_H

#ifdef _cplusplus
	extern "c" {
#endif

#include "snd_alsa.h"
#include "snd_codec.h"
#include "spectrum_analyser.h"

typedef enum {
	SND_OPEN,
	SND_PLAY,
	SND_PAUSE,
	SND_STOP,
	SND_CLOSE,
} snd_status_e;

typedef struct {
	snd_status_e status; 
	snd_alsa_t alsa;
	snd_codec_t codec;

	pthread_t aud_tid;

	spectrum_analyser_t sptaly;
} snd_player_t;


#ifdef _cplusplus
	}
#endif


#endif

