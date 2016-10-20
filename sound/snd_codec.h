#ifndef _SND_CODEC_H
#define _SND_CODEC_H

#ifdef _cplusplus
	extern "c" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>


typedef struct {
	 AVFormatContext *ctx;
	 uint8_t *data;
	 unsigned long frame_size;
	 AVFrame *frame;
	 int st_index[AVMEDIA_TYPE_NB];
	 
	int sample_rate;
	int nb_channels;
	int channel_layout;

	int eof;
} snd_codec_t;


#ifdef _cplusplus
	}
#endif

#endif

