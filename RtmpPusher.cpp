#include "RtmpPusher.h"
#include "StringFunc.h"

#include <process.h>
#include "LogModule.h"



streamIndex stctx = { -1, -1, -1, -1, -1, -1, 0, 0, 0};

RtmpPusher::RtmpPusher()
	:bRunning(1)
{

}

RtmpPusher::~RtmpPusher()
{
}



int RtmpPusher::AudioSupported(AVCodec *pCodec, AVCodecContext *pOutCodecCtx)
{
	int i = 0;
	if (NULL != pCodec->channel_layouts)
	{
		uint64_t layout = av_get_default_channel_layout(pOutCodecCtx->channels);

		for (i = 0; pCodec->channel_layouts[i]; ++i)
		{
			if (layout == pCodec->channel_layouts[i])
				break;
		}

		if (0 == pCodec->channel_layouts[i])
		{
			pOutCodecCtx->channel_layout = pCodec->channel_layouts[i - 1];
			pOutCodecCtx->channels = av_get_channel_layout_nb_channels(pOutCodecCtx->channel_layout);
		}
	}

	//本编码解码器支持的采样率
	if (NULL != pCodec->supported_samplerates)
	{
		for (i = 0; pCodec->supported_samplerates[i]; ++i)
		{
			if (pOutCodecCtx->sample_rate == pCodec->supported_samplerates[i])
				break;
		}

		if (0 == pCodec->supported_samplerates[i])
		{
			pOutCodecCtx->sample_rate = pCodec->supported_samplerates[i - 1];
		}
	}

	//本编码解码器支持的样本
	if (NULL != pCodec->sample_fmts)
	{
		for (i = 0; pCodec->sample_fmts[i] != -1; ++i)
		{
			//printf("%d %d %d\n", i, pOutCodecCtx->sample_fmt, pCodec->sample_fmts[i]);
			if (pOutCodecCtx->sample_fmt == pCodec->sample_fmts[i])
				break;
		}

		if (-1 == pCodec->sample_fmts[i])
		{
			pOutCodecCtx->sample_fmt = pCodec->sample_fmts[i - 1];
		}
	}

	return 0;
}

//打开输入文件，type指定是音频或者是视频文件
int RtmpPusher::VideoSupported(AVCodec *pCodec, AVCodecContext *pOutCodecCtx)
{
	int i = 0;
	if (NULL != pCodec->pix_fmts)
	{
		for (i = 0; pCodec->pix_fmts[i] != -1; ++i)
		{
			if (pOutCodecCtx->pix_fmt == pCodec->pix_fmts[i])
				break;
		}

		if (-1 == pCodec->pix_fmts[i])
		{
			pOutCodecCtx->pix_fmt = pCodec->pix_fmts[i - 1];
		}
	}
	return 0;
}


int interrupt_cb(void *arg)
{
	RtmpPusher *pusher = (RtmpPusher*)arg;

	MyLogger * myLoger = NULL;
	myLoger = MyLogger::getInstance();
	LOG4CPLUS_INFO(myLoger->logger, "interrupt_cb, bRunning = " << pusher->bRunning);
	if (pusher->bRunning == 0)
		return 1;
	return 0;
}




void RtmpPusher::Initialize()
{
	av_register_all();
	avformat_network_init();
	avdevice_register_all();
	
}


int RtmpPusher::open_input_file(enum AVMediaType type)
{
	int					ret = 0;
	AVFormatContext		*pFmtCtx = NULL;
	AVCodecContext 		*pCodecCtx = NULL;

	//pFmtCtx = avformat_alloc_context();
	//pFmtCtx->interrupt_callback.callback = interrupt_cb;
	//pFmtCtx->interrupt_callback.opaque = this;
	
	do
	{
		// 暂不处理
		/*if (type == AVMEDIA_TYPE_AUDIO)
		{
			AVDictionary *pOptions = NULL;
			
			//av_dict_set(&pOptions, "in_channel_layout", "15", NULL);
			//av_dict_set(&pOptions, "in_sample_fmt", AV_SAMPLE_FMT_FLTP, NULL);
			av_dict_set(&pOptions, "in_sample_rate", "44100", NULL);
			av_dict_set(&pOptions, "channels", "2", 0);
			//av_dict_set(&pOptions, "sample_rate", "48000", 0);
	
			AVInputFormat *pInfmt = av_find_input_format("dshow");
			if(pInfmt == NULL)
			{
				printf("%s", "av_find_input_format \"dshow\" failed!");
				return -1;
			}
			char text[128];
			TextToUtf8((LPSTR)text, 128, L"audio=麦克风 (Realtek High Definition Au");
			if(avformat_open_input(&pFmtCtx, text, pInfmt, &pOptions) < 0)
			{
				printf("%s", "avformat_open_input \"麦克风 (Realtek High Definition Au\" failed!");
				return -1;
			}
			//pFmtCtx->interrupt_callback.callback = interrupt_cb;
			//pFmtCtx->interrupt_callback.opaque = this;
			ret = av_find_best_stream(pFmtCtx, type, -1, -1, NULL, 0);
			if (ret < 0)
				break;
			
			enum AVCodecID nid = pFmtCtx->streams[ret]->codec->codec_id;
			//if (nid != AV_CODEC_ID_MP3 && nid != AV_CODEC_ID_AAC && nid != AV_CODEC_ID_MP2 && nid != AV_CODEC_ID_VORBIS)
			{
				AVCodec *pCodec = avcodec_find_decoder(nid);
				if (!pCodec)
					break;
				ret = avcodec_open2(pFmtCtx->streams[ret]->codec, pCodec, NULL);
				if (ret < 0)
					break;
				stctx.audioTranscode = 1;
				stctx.inAudioIndex = ret;
				stctx.pAudioFmtCtx = pFmtCtx;
			}
			
		}*/
		if (type == AVMEDIA_TYPE_VIDEO)
		{
			AVDictionary *pOptions = NULL;
			av_dict_set(&pOptions, "framerate", "15", NULL);
			//av_dict_set(&options,"offset_x","20",0);
			//The distance from the top edge of the screen or desktop
			//av_dict_set(&options,"offset_y","40",0);
			//av_dict_set(&pOptions, "video_size", "1080x720", 0);
			
			AVInputFormat *pInfmt = av_find_input_format("gdigrab");
			if(pInfmt == NULL)
			{
				break;
			}
			if(avformat_open_input(&pFmtCtx, "desktop", pInfmt, &pOptions) != 0)
			{
				break;
			}
			ret = -1;
			//pFmtCtx->interrupt_callback.callback = interrupt_cb;
			//pFmtCtx->interrupt_callback.opaque = this;
			avformat_find_stream_info(pFmtCtx, NULL);
			
			for(int i = 0; i < pFmtCtx->nb_streams; i++)
			{
				if(pFmtCtx->streams[i]->codec->codec_type == type)
				{
					ret = i;
					break;
				}
			}
			//ret = av_find_best_stream(pFmtCtx, type, -1, -1, NULL, 0);
			if (ret < 0)
				break;
			//pFmtCtx->start_time = 0;
			
			pCodecCtx = pFmtCtx->streams[ret]->codec;
			//if (nid != AV_CODEC_ID_MP3 && nid != AV_CODEC_ID_AAC && nid != AV_CODEC_ID_MP2 && nid != AV_CODEC_ID_VORBIS)
			{
				AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
				if (!pCodec)
					break;
				ret = avcodec_open2(pCodecCtx, pCodec, NULL);
				if (ret < 0)
					break;
				stctx.videoTranscode = 1;
				stctx.inVideoIndex = ret;
				stctx.pVideoFmtCtx = pFmtCtx;
			}
		}
		return 0;
	} while (0);

	if (pFmtCtx)
		avformat_close_input(&pFmtCtx);

	return -1;
}

// 打开输出文件
int RtmpPusher::open_output_file(const char *url)
{
	int					ret = 0;
	AVFormatContext		*pFmtCtx = NULL;
	AVCodec				*pCodec = NULL;
	AVStream			*pStream1 = NULL, *pStream = NULL;
	AVStream			*pSrcSt = NULL;
	do
	{
		
		ret = avformat_alloc_output_context2(&pFmtCtx, NULL, "flv", url);
		if (ret < 0)
			break;


		
		// 在输出文件中创建音频流
		// 需要对音频进行转码，根据open_input_file:171行，对所有格式的输入音频进行转码
		if (stctx.audioTranscode == 1)
		{
			pSrcSt = stctx.pAudioFmtCtx->streams[stctx.inAudioIndex];
			pCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
			if (!pCodec)
				break;
			pStream1 = avformat_new_stream(pFmtCtx, pCodec);
			if (!pStream1)
				break;

			pStream1->id = pStream1->index;///*
			pStream1->codec->sample_rate = pSrcSt->codec->sample_rate;//44100;
			pStream1->codec->channels = pSrcSt->codec->channels;
			pStream1->codec->channel_layout = av_get_default_channel_layout(pSrcSt->codec->channels);
			pStream1->codec->sample_fmt = pSrcSt->codec->sample_fmt;
			pStream1->codec->frame_size = 1024;
			pStream1->codec->bit_rate = pSrcSt->codec->bit_rate;//*/
			pStream1->codec->time_base.num = 1;
			pStream1->codec->time_base.den = pSrcSt->codec->sample_rate;

			pStream1->codec->codec_tag = 0;
			if (pFmtCtx->oformat->flags & AVFMT_GLOBALHEADER)
				pStream1->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

			AudioSupported(pCodec, pStream1->codec);
			AVDictionary *param = 0;
			av_dict_set(&param, "strict", "experimental", 0);
			ret = avcodec_open2(pStream1->codec, pCodec, &param);
			if (ret < 0)
				break;
			stctx.outAudioIndex = pStream1->index;
		}
		else if (0)
		{
			pCodec = avcodec_find_encoder(pSrcSt->codec->codec_id);
			if (!pCodec)
				break;
			pStream1 = avformat_new_stream(pFmtCtx, pCodec);
			if (!pStream1)
				break;

			ret = avcodec_copy_context(pStream1->codec, pSrcSt->codec);
			if (ret < 0)
				break;
		}


		
		// 视频流也进行转码
		if (stctx.videoTranscode == 1)
		{
			pSrcSt = stctx.pVideoFmtCtx->streams[stctx.inVideoIndex];
			pCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
			if (!pCodec)
				break;
			pStream = avformat_new_stream(pFmtCtx, pCodec);
			if (!pStream)
				break;

			pStream->id = pStream->index;///*

			pStream->codec->height = pSrcSt->codec->height;
			pStream->codec->width = pSrcSt->codec->width;
			//pStream->codec->time_base = pSrcSt->time_base;
			pStream->codec->time_base.num = 1;
			pStream->codec->time_base.den = 15;
			pStream->codec->gop_size = 12;
			pStream->codec->bit_rate = pSrcSt->codec->bit_rate;//cfg->GetVbrate();
			pStream->codec->ticks_per_frame = 2;
			pStream->codec->max_b_frames = 2;
			pStream->codec->codec_tag = 0;
			pStream->codec->pix_fmt = AV_PIX_FMT_YUV420P;
			pStream->codec->qcompress = 0.6;
			pStream->codec->me_range = 16;			// 运动估算半径，最大为64，建议为16
			pStream->codec->max_qdiff = 4;
			pStream->codec->qmin = 20;				//调节清晰度和编码速度 //这个值调节编码后输出数据量越大输出数据量越小，越大编码速度越快，清晰度越差
			pStream->codec->qmax = 40;				//调节清晰度和编码速度

			pStream->codec->codec_tag = 0;
			if (pFmtCtx->oformat->flags & AVFMT_GLOBALHEADER)
				pStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

			VideoSupported(pCodec, pStream->codec);
			ret = avcodec_open2(pStream->codec, pCodec, NULL);
			if (ret < 0)
				break;
			stctx.outVideoIndex = pStream->index;
		}
		//pCodec = avcodec_find_encoder(pSrcSt->codec->codec_id);
		//pStream = avformat_new_stream(pFmtCtx, pCodec);
		//ret = avcodec_copy_context(pStream->codec, pSrcSt->codec);
		//ret = avcodec_parameters_copy(pStream->codecpar, pSrcSt->codecpar);


		
		av_dump_format(pFmtCtx, 0, url, 1);
		if (avio_open(&pFmtCtx->pb, url, AVIO_FLAG_WRITE) < 0)
			break;
		ret = avformat_write_header(pFmtCtx, NULL);
		if (ret < 0)
			break;
		stctx.pOutFmtCtx = pFmtCtx;
		return 0;
	} while (0);

	if (pFmtCtx)
		avformat_free_context(pFmtCtx);

	return -1;
}




void RtmpPusher::clean_up()
{
	if (stctx.inAudioIndex != -1)
		avcodec_close(stctx.pAudioFmtCtx->streams[stctx.inAudioIndex]->codec);
	if (stctx.inVideoIndex != -1)
		avcodec_close(stctx.pVideoFmtCtx->streams[stctx.inVideoIndex]->codec);
	if (stctx.outAudioIndex != -1)
		avcodec_close(stctx.pOutFmtCtx->streams[stctx.outAudioIndex]->codec);
	if (stctx.outVideoIndex != -1)
		avcodec_close(stctx.pOutFmtCtx->streams[stctx.outVideoIndex]->codec);

	if (stctx.pAudioFmtCtx)
		avformat_close_input(&stctx.pAudioFmtCtx);
	if (stctx.pVideoFmtCtx)
		avformat_close_input(&stctx.pVideoFmtCtx);
	if (stctx.pOutFmtCtx)
	{
		avio_closep(&stctx.pOutFmtCtx->pb);
		avformat_free_context(stctx.pOutFmtCtx);
		stctx.pOutFmtCtx = NULL;
	}
}



int RtmpPusher::Transport()
{
	int					ret;
	int64_t				video_tb = 0, audio_tb = 0, audioswrpts = 0;
	AVPacket			pkg, encpkt;
	AVStream			*vstream = stctx.pVideoFmtCtx->streams[stctx.inVideoIndex];
	AVStream			*astream;
	if (stctx.inAudioIndex != -1)
		astream = stctx.pAudioFmtCtx->streams[stctx.inAudioIndex];
	else
		astream = NULL;

	struct SwsContext*	pVideoSwsCtx = NULL;
	struct SwrContext*	pAudioSwrCtx = NULL;
	AVFrame*			pSwrFrame = NULL;
	AVFrame*			pAudioFrame = NULL;

	AVFrame*			pVideoSwsFrm = NULL;
	AVFrame*			pVideoFrame = NULL;

	AVAudioFifo* 		pSwrFifo = NULL;
	AVStream			*pStream;
	AVFormatContext		*pFmtCtx;
	int					stream_index, stream_index1, got_frame;

	uint8_t				**dst_data = NULL;
	int					dst_nb_samples, max_dst_nb_samples = 0, dst_linesize;
	int64_t				nIndex = 0, aIndex = 0;

	av_init_packet(&pkg);
	av_init_packet(&encpkt);
	pkg.data = NULL;
	pkg.size = 0;
	encpkt.data = NULL;
	encpkt.size = 0;

	
	AVCodecContext *pInCodecCtx;
	AVCodecContext *pOutCodecCtx;
	if (stctx.inAudioIndex != -1)
	 	pInCodecCtx = stctx.pAudioFmtCtx->streams[stctx.inAudioIndex]->codec;
	else
		pInCodecCtx = NULL;
	if (stctx.outAudioIndex != -1)
		pOutCodecCtx = stctx.pOutFmtCtx->streams[stctx.outAudioIndex]->codec;
	else
		pOutCodecCtx = NULL;

	AVCodecContext *pVInCodecCtx = stctx.pVideoFmtCtx->streams[stctx.inVideoIndex]->codec;
	AVCodecContext *pVOutCodecCtx = stctx.pOutFmtCtx->streams[stctx.outVideoIndex]->codec;


	if (stctx.audioTranscode == 1)
	{
		// 创建音频转码中使用的重采样参数
		pAudioSwrCtx = swr_alloc();
		av_opt_set_int(pAudioSwrCtx, "in_channel_layout", av_get_default_channel_layout(pInCodecCtx->channels), 0);
		av_opt_set_int(pAudioSwrCtx, "in_sample_rate", pInCodecCtx->sample_rate, 0);
		av_opt_set_sample_fmt(pAudioSwrCtx, "in_sample_fmt", pInCodecCtx->sample_fmt, 0);
		av_opt_set_int(pAudioSwrCtx, "out_channel_layout", pOutCodecCtx->channel_layout, 0);
		av_opt_set_int(pAudioSwrCtx, "out_sample_rate", pOutCodecCtx->sample_rate, 0);
		av_opt_set_sample_fmt(pAudioSwrCtx, "out_sample_fmt", pOutCodecCtx->sample_fmt, 0);
		swr_init(pAudioSwrCtx);
		pSwrFrame = av_frame_alloc();
		pAudioFrame = av_frame_alloc();
		pSwrFrame->format = pOutCodecCtx->sample_fmt;
		pSwrFrame->channel_layout = pOutCodecCtx->channel_layout;
		pSwrFrame->sample_rate = pOutCodecCtx->sample_rate;
		pSwrFrame->nb_samples = pOutCodecCtx->frame_size;
		av_frame_get_buffer(pSwrFrame, 0);

		pSwrFifo = av_audio_fifo_alloc(pOutCodecCtx->sample_fmt, pOutCodecCtx->channels, pOutCodecCtx->frame_size);
	}

	pVideoFrame = av_frame_alloc();
	if (pVInCodecCtx->height != pVOutCodecCtx->height || pVInCodecCtx->width != pVOutCodecCtx->width || pVInCodecCtx->pix_fmt != pVOutCodecCtx->pix_fmt)
	{
		pVideoSwsCtx = sws_getContext(pVInCodecCtx->width, pVInCodecCtx->height, pVInCodecCtx->pix_fmt, pVOutCodecCtx->width, pVOutCodecCtx->height, pVOutCodecCtx->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);

		pVideoSwsFrm = av_frame_alloc();
		pVideoSwsFrm->width = pVOutCodecCtx->width;
		pVideoSwsFrm->height = pVOutCodecCtx->height;
		pVideoSwsFrm->format = pVOutCodecCtx->pix_fmt;
		ret = av_frame_get_buffer(pVideoSwsFrm, 32);
		if (ret < 0)
			return -1;
	}

	while (bRunning)
	{
		if (pkg.data)
			av_free_packet(&pkg);
		// 根据当前的音频视频pts，决定下一个帧是从音频文件读还是从视频文件读
		//if (av_compare_ts(video_tb, stctx.pOutFmtCtx->streams[stctx.outVideoIndex]->time_base, audio_tb, stctx.pOutFmtCtx->streams[stctx.outAudioIndex]->time_base) <= 0)
		if (1)
		{
			stream_index = stctx.outVideoIndex;
			stream_index1 = stctx.inVideoIndex;
			pFmtCtx = stctx.pVideoFmtCtx;
			pStream = vstream;
		}
		else
		{
			stream_index = stctx.outAudioIndex;
			stream_index1 = stctx.inAudioIndex;
			pFmtCtx = stctx.pAudioFmtCtx;
			pStream = astream;
		}

		ret = av_read_frame(pFmtCtx, &pkg);
		if (ret < 0)
		{
			//printf("stop publish");
			break;
		}
		if (pkg.stream_index != stream_index1)
			continue;

		if (stream_index == stctx.outVideoIndex)
		{
			if (stctx.videoTranscode == 1)
			{
				ret = avcodec_decode_video2(pVInCodecCtx, pVideoFrame, &got_frame, &pkg);
				if (ret < 0)
					continue;
				if (!got_frame)
					continue;
				if (encpkt.data)
					av_free_packet(&encpkt);
				pVideoFrame->pts = av_frame_get_best_effort_timestamp(pVideoFrame);
				pVideoFrame->pts = nIndex++ * ((1000 / 1) / 15);
				
				if (pVideoSwsCtx)
				{
					sws_scale(pVideoSwsCtx, (const uint8_t * const *)pVideoFrame->data, pVideoFrame->linesize, 0, pVInCodecCtx->height, pVideoSwsFrm->data, pVideoSwsFrm->linesize);
					pVideoSwsFrm->pts = pVideoFrame->pts;
					pVideoSwsFrm->pkt_pts = pVideoFrame->pkt_pts;
					pVideoSwsFrm->pkt_dts = pVideoFrame->pkt_dts;
					avcodec_encode_video2(pVOutCodecCtx, &encpkt, pVideoSwsFrm, &got_frame);
				}
				else
				{
					avcodec_encode_video2(pVOutCodecCtx, &encpkt, pVideoFrame, &got_frame);
				}
				if (!got_frame)
					continue;
				//video_tb = AV_NOPTS_VALUE;
				
				//av_packet_rescale_ts(&pkg, pStream->time_base, stctx.pOutFmtCtx->streams[stream_index]->time_base);
				//encpkt.pts = av_rescale_q_rnd(encpkt.pts, pStream->time_base, stctx.pOutFmtCtx->streams[stream_index]->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
				//encpkt.dts = av_rescale_q_rnd(encpkt.dts, pStream->time_base, stctx.pOutFmtCtx->streams[stream_index]->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
				//encpkt.duration = av_rescale_q(encpkt.duration, pStream->time_base, stctx.pOutFmtCtx->streams[stream_index]->time_base);
				
				video_tb = encpkt.pts;
				encpkt.stream_index = stream_index;
				encpkt.pos = -1;

				ret = av_interleaved_write_frame(stctx.pOutFmtCtx, &encpkt);
				ret = 0;
			}
		}
		else
		{
			
			// 音频处理流程：解码->重采样->编码。重采样后的数据存入fifo，编码时从fifo读。swr_convert函数返回重采样得到的音频帧数量（长度根据不同的编码而不同），而avcodec_encode_audio2每次只接收1024个音频帧，因此需要先存入fifo再从fifo读
			if (stctx.audioTranscode == 1)
			{
				// 音频处理流程1：解码
				ret = avcodec_decode_audio4(pInCodecCtx, pAudioFrame, &got_frame, &pkg);
				if (ret < 0)
					continue;
				if (!got_frame)
					continue;
				pAudioFrame->pts = av_frame_get_best_effort_timestamp(pAudioFrame);

				if (dst_data == NULL)
				{
					int dst_nb_channels = av_get_channel_layout_nb_channels(pOutCodecCtx->channel_layout);
					dst_nb_samples = av_rescale_rnd(pAudioFrame->nb_samples, pOutCodecCtx->sample_rate, pInCodecCtx->sample_rate, AV_ROUND_UP);
					ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dst_nb_channels, dst_nb_samples, pOutCodecCtx->sample_fmt, 0);
					av_freep(&dst_data[0]);
					dst_nb_samples = av_rescale_rnd(swr_get_delay(pAudioSwrCtx, pInCodecCtx->sample_rate) + pAudioFrame->nb_samples, pOutCodecCtx->sample_rate, pInCodecCtx->sample_rate, AV_ROUND_UP);
					ret = av_samples_alloc(dst_data, &dst_linesize, pOutCodecCtx->channels, dst_nb_samples, pOutCodecCtx->sample_fmt, 1);
					max_dst_nb_samples = dst_nb_samples;
				}
				dst_nb_samples = av_rescale_rnd(swr_get_delay(pAudioSwrCtx, pInCodecCtx->sample_rate) + pAudioFrame->nb_samples, pOutCodecCtx->sample_rate, pInCodecCtx->sample_rate, AV_ROUND_UP);
				if (dst_nb_samples > max_dst_nb_samples)
				{
					av_freep(&dst_data[0]);
					ret = av_samples_alloc(dst_data, &dst_linesize, pOutCodecCtx->channels, dst_nb_samples, pOutCodecCtx->sample_fmt, 1);
					max_dst_nb_samples = dst_nb_samples;
				}


				// 音频处理流程2：重采样
				ret = swr_convert(pAudioSwrCtx, dst_data, dst_nb_samples, (const uint8_t**)pAudioFrame->data, pAudioFrame->nb_samples);
				if (ret < 0)
					continue;
				av_audio_fifo_realloc(pSwrFifo, av_audio_fifo_size(pSwrFifo) + ret);
				av_audio_fifo_write(pSwrFifo, (void **)dst_data, ret);
				//int len = av_audio_fifo_size(pSwrFifo);
				while (av_audio_fifo_size(pSwrFifo) >= pSwrFrame->nb_samples)
				{
					if (encpkt.data)
						av_free_packet(&encpkt);
					av_audio_fifo_read(pSwrFifo, (void **)pSwrFrame->data, pSwrFrame->nb_samples);
					audio_tb = audioswrpts;
					pSwrFrame->pts = audioswrpts;//pAudioFrame->pts;
					//pSwrFrame->pkt_dts = audioswrpts;//pAudioFrame->pkt_dts;
					//pSwrFrame->pkt_pts = audioswrpts;//pAudioFrame->pkt_pts;
					audioswrpts += aIndex++ * 1000*1024/22050;// pSwrFrame->nb_samples;// pSwrFrame->nb_samples;
					// 音频处理流程3：编码
					ret = avcodec_encode_audio2(pOutCodecCtx, &encpkt, pSwrFrame, &got_frame);
					//
					if (got_frame)
					{
						encpkt.stream_index = stream_index;
						//av_packet_rescale_ts(&encpkt, pStream->time_base, stctx.pOutFmtCtx->streams[stctx.outAudioIndex]->time_base);
						ret = av_interleaved_write_frame(stctx.pOutFmtCtx, &encpkt);
						ret = 0;
					}

				}

			}
		}
	}

	MyLogger * myLoger = NULL;
	myLoger = MyLogger::getInstance();
	// 对缓存在编码器中的音频帧flush写入文件
	do
	{
		if (!pOutCodecCtx)
			break;
		if (encpkt.buf)
			av_free_packet(&encpkt);
		ret = avcodec_encode_audio2(pOutCodecCtx, &encpkt, NULL, &got_frame);
		
		if (got_frame)
		{
			//printf("flush audio\n");
			encpkt.stream_index = stctx.outAudioIndex;
			av_interleaved_write_frame(stctx.pOutFmtCtx, &encpkt);
		}
	} while (got_frame);
	
	// 对缓存在编码器中的视频帧flush写入文件
	do
	{
		if (encpkt.buf)
			av_free_packet(&encpkt);
		ret = avcodec_encode_video2(pVOutCodecCtx, &encpkt, NULL, &got_frame);
		if (got_frame)
		{
			LOG4CPLUS_INFO(myLoger->logger, "flush video");
			//printf("		flush video\n");
			encpkt.stream_index = stctx.outVideoIndex;
			encpkt.pts = av_rescale_q_rnd(encpkt.pts, stctx.pVideoFmtCtx->streams[stctx.inVideoIndex]->time_base, stctx.pOutFmtCtx->streams[stctx.outVideoIndex]->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			encpkt.dts = av_rescale_q_rnd(encpkt.dts, stctx.pVideoFmtCtx->streams[stctx.inVideoIndex]->time_base, stctx.pOutFmtCtx->streams[stctx.outVideoIndex]->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			encpkt.duration = av_rescale_q(encpkt.duration, stctx.pVideoFmtCtx->streams[stctx.inVideoIndex]->time_base, stctx.pOutFmtCtx->streams[stctx.outVideoIndex]->time_base);

			av_interleaved_write_frame(stctx.pOutFmtCtx, &encpkt);
		}
	} while (got_frame);
	av_write_trailer(stctx.pOutFmtCtx);


	if (pSwrFrame)
		av_frame_free(&pSwrFrame);
	if (pAudioFrame)
		av_frame_free(&pAudioFrame);
	if (pVideoFrame)
		av_frame_free(&pVideoFrame);
	if (pVideoSwsFrm)
		av_frame_free(&pVideoSwsFrm);
	if (pAudioSwrCtx)
		swr_free(&pAudioSwrCtx);
	if (pVideoSwsCtx)
		sws_freeContext(pVideoSwsCtx);
	if (pSwrFifo)
		av_audio_fifo_free(pSwrFifo);
	if (dst_data)
	{
		av_freep(&dst_data[0]);
		av_freep(&dst_data);
	}

	return 0;
}


unsigned int __stdcall RtmpPusher::thread_entry(void  *xpvt_param)
{
	RtmpPusher *pusher = (RtmpPusher*)xpvt_param;
	pusher->Transport();
	return 0;
}

int RtmpPusher::StartPush()
{
	bRunning = 1;
	_beginthreadex(NULL, 0, &RtmpPusher::thread_entry, this, 0, NULL);
	return 0;
}

void RtmpPusher::StopPush()
{
	MyLogger * myLoger = NULL;
	myLoger = MyLogger::getInstance();
	LOG4CPLUS_INFO(myLoger->logger, "StopPush");
	bRunning = 0;
}


