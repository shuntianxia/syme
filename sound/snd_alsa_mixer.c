#include <stdio.h>
#include <stdlib.h>

#include "snd_alsa_mixer.h"


int snd_amx_init(snd_alsa_mixer_t *amx, int range)
{
	snd_mixer_open(&amx->mixer, 0);
	snd_mixer_attach(amx->mixer, "default");
	snd_mixer_selem_register(amx->mixer, NULL, NULL);
	snd_mixer_load(amx->mixer);

	amx->master_element = snd_mixer_first_elem(amx->mixer);
	snd_mixer_selem_set_playback_volume_range(amx->master_element, 0, range);

	return 0;
}

int snd_amx_set_volume(snd_alsa_mixer_t *amx, int volume)
{
	int unmute;
	snd_mixer_selem_get_playback_switch(amx->master_element, 0, &unmute);
	if (unmute)
	   	fprintf(stderr,"Master is Unmute.\n");
	else
		fprintf(stderr,"Master is Mute.\n");
	snd_mixer_selem_set_playback_volume(amx->master_element, SND_MIXER_SCHN_FRONT_LEFT, volume);
	snd_mixer_selem_set_playback_volume(amx->master_element, SND_MIXER_SCHN_FRONT_RIGHT, volume);

	return 0;
}

int snd_amx_get_volume(snd_alsa_mixer_t *amx)
{
	int al, ar;
	int vol = 0;
	snd_mixer_selem_get_playback_volume(amx->master_element, SND_MIXER_SCHN_FRONT_LEFT, (long *)&al);
	snd_mixer_selem_get_playback_volume(amx->master_element, SND_MIXER_SCHN_FRONT_RIGHT, (long *)&ar);
	vol = (al + ar) >> 1;
	return vol;
}

int snd_amx_destory(snd_alsa_mixer_t *amx)
{
	return snd_mixer_close(amx->mixer);
}


