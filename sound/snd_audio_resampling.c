#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#define MAX_FRAME_SIZE (0x1200)

/*ape not work!!!*/
int audio_resampling(AVCodecContext * avctx, AVFrame * frame,
	int out_sample_fmt, int out_channels, int out_sample_rate, uint8_t **out_chunk)
{
	SwrContext * swr_ctx = NULL;
	int data_size = 0;
	int ret = 0;
	int64_t src_ch_layout = avctx->channel_layout;
	int64_t dst_ch_layout = AV_CH_LAYOUT_STEREO;
	int dst_nb_channels = 0;
	int dst_linesize = 0;
	int src_nb_samples = 0;
	int dst_nb_samples = 0;
	int max_dst_nb_samples = 0;
	uint8_t **dst_data = NULL;
	int resampled_data_size = 0;

	swr_ctx = swr_alloc();
	if (!swr_ctx) {
		printf("swr_alloc error \n");
		return -1;
	}

	src_ch_layout = (avctx->channels == av_get_channel_layout_nb_channels(avctx->channel_layout)) ?
		avctx->channel_layout :av_get_default_channel_layout(avctx->channels);

	if (out_channels == 1) {
		dst_ch_layout = AV_CH_LAYOUT_MONO;
		//printf("dst_ch_layout: AV_CH_LAYOUT_MONO\n");
	} else if (out_channels == 2) {
		dst_ch_layout = AV_CH_LAYOUT_STEREO;
		//printf("dst_ch_layout: AV_CH_LAYOUT_STEREO\n");
	} else {
		dst_ch_layout = AV_CH_LAYOUT_SURROUND;
		//printf("dst_ch_layout: AV_CH_LAYOUT_SURROUND\n");
	}

	if (src_ch_layout <= 0) {
		printf("src_ch_layout error \n");
		return -1;
	}

	src_nb_samples = frame->nb_samples;
	if (src_nb_samples <= 0) {
		printf("src_nb_samples error \n");
		return -1;
	}

	av_opt_set_int(swr_ctx, "in_channel_layout", src_ch_layout, 0);
	av_opt_set_int(swr_ctx, "in_sample_rate", avctx->sample_rate, 0);
	av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", avctx->sample_fmt, 0);
	
	av_opt_set_int(swr_ctx, "out_channel_layout", dst_ch_layout, 0);
	av_opt_set_int(swr_ctx, "out_sample_rate", out_sample_rate, 0);
	av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", (enum AVSampleFormat)out_sample_fmt, 0);
	if ((ret = swr_init(swr_ctx)) < 0) {
		printf("Failed to initialize the resampling context\n");
		return -1;
	}

	max_dst_nb_samples = dst_nb_samples = av_rescale_rnd(src_nb_samples, 
	out_sample_rate, avctx->sample_rate, AV_ROUND_UP);
	if (max_dst_nb_samples <= 0) {
		printf("av_rescale_rnd error \n");
		return -1;
	}

	dst_nb_channels = av_get_channel_layout_nb_channels(dst_ch_layout);
	ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dst_nb_channels,
		dst_nb_samples, (enum AVSampleFormat)out_sample_fmt, 0);
	if (ret < 0)
	{
		printf("av_samples_alloc_array_and_samples error \n");
		return -1;
	}
	
	dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, avctx->sample_rate) + src_nb_samples, 
		out_sample_rate, avctx->sample_rate, AV_ROUND_UP);
	if (dst_nb_samples <= 0) {
		printf("av_rescale_rnd error \n");
		return -1;
	}
	
	if (dst_nb_samples > max_dst_nb_samples) {
		av_free(dst_data[0]);
		ret = av_samples_alloc(dst_data, &dst_linesize, dst_nb_channels,
		dst_nb_samples, (enum AVSampleFormat)out_sample_fmt, 1);
		max_dst_nb_samples = dst_nb_samples;
	}

	if (swr_ctx) {
		//fprintf(stderr,"packet size:%d frame->nb_samples:%d\n",av_frame_get_pkt_size(frame),frame->nb_samples);
		
		ret = swr_convert(swr_ctx, dst_data, dst_nb_samples,(const uint8_t **)frame->data, frame->nb_samples);
		if (ret < 0) {
			printf("swr_convert error \n");
			return -1;
		}
		
		//fprintf(stderr,"swr_convert nb_samples:%d dst_nb_samples:%d\n",ret,dst_nb_samples);

		resampled_data_size = av_samples_get_buffer_size(&dst_linesize, dst_nb_channels,ret, (enum AVSampleFormat)out_sample_fmt, 1);
		if (resampled_data_size < 0) {
			printf("av_samples_get_buffer_size error \n");
			return -1;
		}
	} else {
		printf("swr_ctx null error \n");
		return -1;
	}

//	if (!*out_chunk) {
//		*out_chunk = (uint8_t *)av_malloc(MAX_FRAME_SIZE*sizeof(uint8_t)); /*MAX_FRAME_SIZE worth of vertebral*/
//	}
	
	if (*out_chunk) {
		av_free(*out_chunk);
	}
	*out_chunk = (uint8_t *)av_malloc(resampled_data_size*sizeof(uint8_t)); /*MAX_FRAME_SIZE worth of vertebral*/
	
	//fprintf(stderr,"resampled_data_size:0x%x\n",resampled_data_size);
	memcpy(*out_chunk, dst_data[0], resampled_data_size);

	//while ((ret = swr_convert(swr_ctx, dst_data, dst_nb_samples, NULL, 0)) > 0) {
	//	fprintf(stderr,"-------------------while -swr_convert nb_samples:%d dst_nb_samples:%d\n",ret,dst_nb_samples);
	//}

	if (dst_data) {
		av_freep(&dst_data[0]);
	}
	av_freep(&dst_data);
	dst_data = NULL;
	
	if (swr_ctx) {
		swr_free(&swr_ctx);
	}
	
	return resampled_data_size;
}


