/**
***
***bad code!!!!!
***
**/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "snd_player.h"


#define MIN(x, y)           (((x) < (y))?(x):(y))

static long get_time_gap(struct timespec st, struct timespec end)
{
	return (long)(end.tv_nsec-st.tv_nsec + (end.tv_sec-st.tv_sec)*1000000000);
}

static int is_analysis_spectrum(spectrum_analyser_t *sptaly, int frame_size)
{
	if (alk_fifo_len(sptaly->alk_fifo) >=  sptaly->m_SampleSize * frame_size && alk_fifo_len(sptaly->alk_fifo) >= sptaly->alk_fifo->size *2/3) {
		return 1;
	}

	return 0;
}

static void *audio_thread(void *param)
{
	int ret = 0;
	int copy_len = 0;
	int remain_len = 0;
	int frame_size = 0;
	uint8_t *buf;
	float wFrr = 0.0;
	snd_player_t *player = (snd_player_t *)param;

	//uint8_t *rd_buf = (uint8_t *)calloc(1,8192*sizeof(uint8_t));
	//FILE *pf = fopen("./sample.data","r");
	//if (!pf) {
	//	fprintf(stderr,"open err!\n");
	//}
	
	do {
		if (SND_PLAY == player->status) {
			ret = snd_codec_ffmpeg_decode(&player->codec);
			if (0 == ret) {
				snd_alsa_writei(&player->alsa,player->codec.data,player->codec.frame_size);

				//fprintf(stderr,"------player->codec.frame_size:%d\n",player->codec.frame_size);
				buf = player->codec.data;
				remain_len = player->codec.frame_size;
				frame_size = player->sptaly.m_channelMode * player->sptaly.m_sampleType;
				
				//fread(rd_buf,8192,1,pf);
				//buf = rd_buf;
				//remain_len = 8192;
				//frame_size = 4;
				//player->codec.data = rd_buf + remain_len;
				
				/*
				need to adjust the synchronization with the music!!!
				*/
				do {
					copy_len = MIN(remain_len, player->sptaly.m_SampleSize*frame_size);
					sptaly_copy_sample(&player->sptaly,buf,copy_len);
					
					if (is_analysis_spectrum(&player->sptaly,frame_size) || player->codec.eof) {
						wFrr = (float) player->sptaly.m_FpsAsNS / (float) player->sptaly.m_DesiredFpsAsNS;
						sptaly_prepare(&player->sptaly);
						
						struct timespec time_start={0, 0},time_end={0, 0};
						clock_gettime(CLOCK_REALTIME, &time_start);
						sptaly_process(&player->sptaly,wFrr);
						clock_gettime(CLOCK_REALTIME, &time_end);
					#if 1
						long delay = player->sptaly.m_FpsAsNS - get_time_gap(time_start,time_end);
						
						if (delay > 0L)
						{
							int64_t ms = (int64_t)delay / 0xf4240L;
							int64_t ns = (int64_t)delay % 0xf4240L;
							if(ns >= 500000) ms++;			
								usleep(ms*100);

							if (player->sptaly.m_FpsAsNS > player->sptaly.m_DesiredFpsAsNS)
								player->sptaly.m_FpsAsNS -= delay;
							else
								player->sptaly.m_FpsAsNS = player->sptaly.m_DesiredFpsAsNS;
						}
						else
						{
							player->sptaly.m_FpsAsNS += -delay;
							usleep(100);
						}
					#endif
						
						//usleep(500000);
					}
					
					buf += copy_len;
					remain_len -= copy_len;
				} while (buf < player->codec.data);
			}
		} else {
			//lock it
			fprintf(stderr,"sleep a little!\n");
			usleep(1000);
		}
	} while (SND_CLOSE != player->status && 0 == player->codec.eof);
}

static int codec_fmt_to_alsa(snd_codec_t *codec, snd_alsa_t *alsa)
{
	AVCodecContext *avctx = NULL;
	int stream_index = 0;

	stream_index = codec->st_index[AVMEDIA_TYPE_AUDIO];
	avctx = codec->ctx->streams[stream_index]->codec;

	switch(avctx->sample_fmt) {
		case AV_SAMPLE_FMT_U8:
			alsa->fmt = SND_PCM_FORMAT_S8;
			//snd_alsa_set(player->alsa,SND_PCM_FORMAT_S8,player->codec.sample_rate,player->codec.nb_channels);
			break;
		case AV_SAMPLE_FMT_S16:
		case AV_SAMPLE_FMT_S16P:
			alsa->fmt = SND_PCM_FORMAT_S16;
			//snd_alsa_set(player->alsa,SND_PCM_FORMAT_S16,player->codec.sample_rate,player->codec.nb_channels);
			break;
		case AV_SAMPLE_FMT_FLTP:
			alsa->fmt = SND_PCM_FORMAT_FLOAT;
			break;
		default:
			break;
	}

	alsa->sample_rate = codec->sample_rate;
	alsa->nb_channel = codec->nb_channels;
	return 0;
}

int snd_player_open(snd_player_t *player, char *file_path)
{
	
	
	snd_alsa_open(&player->alsa,"default");
	snd_codec_ffmpeg_prepare(&player->codec);
	snd_codec_ffmpeg_open(&player->codec,file_path);
	
	codec_fmt_to_alsa(&player->codec,&player->alsa);
	snd_alsa_set(&player->alsa,player->alsa.fmt,player->codec.sample_rate,player->codec.nb_channels);

	fprintf(stderr,"fmt:%d sample rate:%d nb_channel:%d\n",player->alsa.fmt,player->alsa.sample_rate,player->alsa.nb_channel);

	sptaly_init(&player->sptaly);
	
	player->status = SND_OPEN;
	
	return 0;
}

int snd_player_play(snd_player_t *player)
{
	int ret = 0;
	if (player->status != SND_PAUSE) {
		ret = pthread_create(&player->aud_tid,NULL,(void  *) audio_thread,(void *)player);
		if (ret < 0) {
			fprintf(stderr,"create thread err!\n");
			return -1;
		}
	} else {
		snd_alsa_resume(&player->alsa);
	}
	player->status = SND_PLAY;
	
	return 0;
}

int snd_player_pause(snd_player_t *player)
{
	player->status = SND_PAUSE;
	snd_alsa_pause(&player->alsa);
	return 0;
}

int snd_player_stop(snd_player_t *player)
{
	player->status = SND_STOP;
	return 0;
}

int snd_player_close(snd_player_t *player)
{
	player->status = SND_CLOSE;
	usleep(300000);
	snd_alsa_close(&player->alsa);
	snd_codec_ffmpeg_close(&player->codec);

	sptaly_destory(&player->sptaly);
	
	return 0;
}

int snd_player_get_duration(snd_player_t *player)
{
	return snd_codec_ffmpeg_get_duration(&player->codec);
}

//#define SND_ALSA_TEST

#if 0
int main(int argc, char *argv[])
{
	snd_player_t player;
	char *aud_file_path = NULL;
	//char *aud_file_path = "/home/work/mg-story_machine/G.memories.wav";
	//char *aud_file_path = "/home/work/mg-story_machine/congcongnanian.mp3";

	aud_file_path = argv[1];

#ifdef SND_ALSA_TEST
	snd_alsa_t alsa;
	snd_alsa_open(&alsa,"default");
	snd_alsa_set(&alsa,SND_PCM_FORMAT_S16_LE,44100,2);
	
	char buf[4*32] = {0};
	int fd = open(aud_file_path,O_RDONLY,0);
	if (fd < 0) {
		fprintf(stderr,"open failed!\n");
		return -1;
	}

	while (read(fd,buf,sizeof(buf))) {
		snd_alsa_writei(&alsa,buf,32);
	}

	close(fd);
#else
	snd_player_open(&player,aud_file_path);
	snd_player_play(&player);

	while(1) {
		//usleep(3000000);
		//snd_player_pause(&player);
		//usleep(2000000);
		//snd_player_play(&player);

		usleep(2000000);
		snd_player_close(&player);

		usleep(10000000);
		snd_player_open(&player,aud_file_path);
		snd_player_play(&player);
	}
#endif
	
	return 0;
}

#endif

