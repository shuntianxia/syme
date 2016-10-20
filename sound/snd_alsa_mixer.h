#ifndef _SND_ALSA_MIXER_H
#define _SND_ALSA_MIXER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <alsa/asoundlib.h>


typedef struct {
	snd_mixer_t *mixer;
	snd_mixer_elem_t *master_element;
} snd_alsa_mixer_t;

int snd_amx_init(snd_alsa_mixer_t *amx, int range);
int snd_amx_set_volume(snd_alsa_mixer_t *amx, int volume);
int snd_amx_get_volume(snd_alsa_mixer_t *amx);
int snd_amx_destory(snd_alsa_mixer_t *amx);


#ifdef __cplusplus
}
#endif


#endif 

