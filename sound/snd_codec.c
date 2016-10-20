#include "snd_codec.h"

static int audio_thread(void *arg)
{
	AVFrame *frame = av_frame_alloc();
	
}


int snd_codec_ffmpeg_prepare(snd_codec_t *snd_codec)
{
	int i = 0;
	av_register_all();
	for (i=0; i<AVMEDIA_TYPE_NB; i++) {
		snd_codec->st_index[i] = -1;
	}

	snd_codec->ctx = NULL;
	snd_codec->frame = av_frame_alloc();
	snd_codec->eof = 0;
	snd_codec->data = NULL;
	return 0;
}


int snd_codec_ffmpeg_open(snd_codec_t *snd_codec, char *path)
{
	int i = 0;
	int ret = 0;
	 AVCodec *codec = NULL;
	 AVCodecContext *avctx = NULL;
	 int stream_index = 0;
	
	ret = avformat_open_input(&snd_codec->ctx, path, NULL, NULL);
	if (ret < 0) {
		fprintf(stderr,"open err:%s ret:0x%x\n",path,ret);
		return -1;
	}

	avformat_find_stream_info(snd_codec->ctx, NULL);
	for (i = 0; i < snd_codec->ctx->nb_streams; i++) {
		AVStream *st = snd_codec->ctx->streams[i];
		enum AVMediaType type = st->codec->codec_type;
		//st->discard = AVDISCARD_ALL;
		if (-1 == snd_codec->st_index[type] && st->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
				snd_codec->st_index[type] = i;
				break;
		}
	}

	stream_index = snd_codec->st_index[AVMEDIA_TYPE_AUDIO];
	avctx = snd_codec->ctx->streams[stream_index]->codec;
	codec = avcodec_find_decoder(avctx->codec_id);
	ret = avcodec_open2(avctx, codec, NULL);
	if (0 == ret) {
		snd_codec->sample_rate    = avctx->sample_rate;
		snd_codec->nb_channels    = avctx->channels;
		snd_codec->channel_layout = avctx->channel_layout;
	}

	fprintf(stderr,"avctx->sample_fmt:%d\n",avctx->sample_fmt);
	
	return ret;
}

int snd_codec_ffmpeg_decode(snd_codec_t *snd_codec)
{
	int ret = 0;
	AVPacket packet;
	int got_frame = 0;
	int stream_index = 0;
	AVCodecContext *avctx = NULL;
	int sample_size = 0;
	int resampe_size = 0;

	stream_index = snd_codec->st_index[AVMEDIA_TYPE_AUDIO];
	avctx = snd_codec->ctx->streams[stream_index]->codec;
	ret = av_read_frame(snd_codec->ctx, &packet);
	if (ret < 0) {
		fprintf(stderr,"av read frame err:%s\n",av_err2str(ret));
		if (ret == AVERROR_EOF) {
			snd_codec->eof = 1;
		}
		
		return -1;
	}

	//static int total_len = 0;
	//total_len += packet.size;
	//fprintf(stderr,"has read size:0x%x\n",total_len);

	if (packet.stream_index != snd_codec->st_index[AVMEDIA_TYPE_AUDIO]) {
		return 1; //skip
	}
	
	ret = avcodec_decode_audio4(avctx, snd_codec->frame, &got_frame, &packet);
	//fprintf(stderr,"frame format:%d\n",snd_codec->frame->format);
	if (AV_SAMPLE_FMT_S16P == avctx->sample_fmt) {
		//AudioResampling(pCodecCtx, pFrame, AV_SAMPLE_FMT_S16, 2, 44100);
		resampe_size = audio_resampling(avctx,snd_codec->frame,AV_SAMPLE_FMT_S16,avctx->channels,avctx->sample_rate,&snd_codec->data);
		sample_size = av_get_bytes_per_sample(avctx->sample_fmt);
		snd_codec->frame_size = resampe_size /avctx->channels / sample_size;
	} else if (AV_SAMPLE_FMT_FLTP == avctx->sample_fmt) {
		resampe_size = audio_resampling(avctx,snd_codec->frame,AV_SAMPLE_FMT_FLT,avctx->channels,avctx->sample_rate,&snd_codec->data);
		sample_size = av_get_bytes_per_sample(avctx->sample_fmt);
		snd_codec->frame_size = resampe_size /avctx->channels / sample_size;
	} else {
		snd_codec->data = snd_codec->frame->data[0];
		if (avctx->frame_size <= 0) {
			sample_size = av_get_bytes_per_sample(avctx->sample_fmt);
			snd_codec->frame_size =  av_samples_get_buffer_size(NULL, avctx->channels, snd_codec->frame->nb_samples, avctx->sample_fmt, 1) /avctx->channels / sample_size ;
			//fprintf(stderr,"frame_size:%d\n",snd_codec->frame_size);
		} else {
			snd_codec->frame_size = avctx->frame_size;
		}
	}
		
		//fprintf(stderr,"frame_size:%d\n",snd_codec->frame_size);
	
	return 0;
}

int snd_codec_ffmpeg_close(snd_codec_t *snd_codec)
{
	av_free(snd_codec->data);
	av_free(snd_codec->frame);
	avformat_close_input(&snd_codec->ctx);
	return 0;
}

int snd_codec_ffmpeg_get_duration(snd_codec_t *snd_codec)
{
	return (snd_codec->ctx->duration / 1000000LL);
}
