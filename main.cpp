


#include "RtmpPusher.h"
#include <stdio.h>
#include <windows.h>  
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "Winmm.lib")
#include "LogModule.h"




int main()
{
	int ret = 0;

	RtmpPusher pusher;
	pusher.Initialize();
	
	// 打开录屏，即视频流
	ret = pusher.open_input_file(AVMEDIA_TYPE_VIDEO);
	MyLogger * myLoger = NULL;
	myLoger = MyLogger::getInstance();

	if (ret < 0)
	{
		MyLogger * myLoger = NULL;
		myLoger = MyLogger::getInstance();

		LOG4CPLUS_FATAL(myLoger->logger, "open video failed, ret = " << ret);
		return -1;
	}
	// 打开输出文件，推送到地址
	ret = pusher.open_output_file("rtmp://push.bcelive.com/live/7ni07uo6fathhtk8io");
	if (ret < 0)
	{
		MyLogger * myLoger = NULL;
		myLoger = MyLogger::getInstance();

		LOG4CPLUS_FATAL(myLoger->logger, "open output failed, ret = " << ret);
		//printf("open output failed\n");
		return -1;
	}
	//pusher.Transport();
	// 在此函数中创建线程，在线程中录屏推流
	pusher.StartPush();
	//Sleep(15000);
	//pusher.StopPush();
	char c;
	// 按Enter键终止StartPush()创建的线程
	while ((c = getchar()) != '\n')
	{
		Sleep(100);
	}
	pusher.StopPush();
	pusher.clean_up();
}