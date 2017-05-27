/*
*标准使用，严格实现步骤1-6。
　*/
#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/layout.h"
#include "LogModule.h"
#include <windows.h>
#include <stdio.h>
#pragma comment(lib, "log4cplusSD.lib")

using namespace log4cplus;
using namespace log4cplus::helpers;

MyLogger *MyLogger::my_logger = NULL;
MyLogger::MyLogger()
{
	log4cplus::initialize();
	PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(MY_LOG_FILE_PATH));
	logger = Logger::getRoot();
}

MyLogger * MyLogger::getInstance()
{
	if (my_logger == NULL)
	{
		my_logger = new MyLogger();
	}
	return my_logger;
}

MyLogger::~MyLogger()
{
	if (my_logger)
	{
		delete my_logger;
	}
}



//int main()
//{
	/* step 1: Instantiate an appender object */
	//SharedObjectPtr<Appender> _append(new ConsoleAppender());
	//_append->setName(LOG4CPLUS_TEXT("append for test"));

	/* step 2: Instantiate a layout object */
	//std::auto_ptr<Layout> _layout(new PatternLayout(LOG4CPLUS_TEXT("%d{%m/%d/%y  %H:%M:%S}  - %m [%l]%n")));

	/* step 3: Attach the layout object to the appender */
	//_append->setLayout(_layout);

	/* step 4:  Instantiate a logger object */
	//Logger _logger = Logger::getInstance(LOG4CPLUS_TEXT("test"));

	/* step 5: Attach the appender object to the  logger  */
	//_logger.addAppender(_append);

	/* step 6: Set a priority for the logger  */
	//_logger.setLogLevel(ALL_LOG_LEVEL);

	/* log activity */
	//LOG4CPLUS_DEBUG(_logger, "This is the  FIRST log message...");
	//	Sleep(1);
	//LOG4CPLUS_WARN(_logger, "This is the  SECOND log message...");

	//_getch();
	//	return 0;


//}