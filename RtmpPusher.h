#ifndef RTMPPUSHER_H_
#define RTMPPUSHER_H_



extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/opt.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/audio_fifo.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/dict.h"
#include "libavutil/channel_layout.h"
#include "libavutil/samplefmt.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
}



typedef struct
{
	int	inVideoIndex;
	int	inAudioIndex;
	int	outVideoIndex;
	int	outAudioIndex;
	int	videoTranscode;
	int audioTranscode;
	AVFormatContext			*pAudioFmtCtx;
	AVFormatContext			*pVideoFmtCtx;
	AVFormatContext			*pOutFmtCtx;
	//AVCodecContext			*pAudioDecodecCtx;
	//AVCodecContext			*pAudioEncodecCtx;
}streamIndex;


class RtmpPusher
{
public:
	RtmpPusher();
	~RtmpPusher();

	void Initialize();
	int open_input_file(enum AVMediaType type);
	int open_output_file(const char *url);
	void clean_up();
	int Transport();
	//static int interrupt_cb(void *arg);
	static unsigned int __stdcall thread_entry(void  *xpvt_param);
	int StartPush();
	void StopPush();
	int AudioSupported(AVCodec *pCodec, AVCodecContext *pOutCodecCtx);
	int VideoSupported(AVCodec *pCodec, AVCodecContext *pOutCodecCtx);
	int bRunning;

private:
	
};

#endif
