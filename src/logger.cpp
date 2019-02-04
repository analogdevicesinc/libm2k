#include "logger.h"

using namespace libm2k;

Logger &Logger::getInstance()
{
    static Logger instance;

    return instance;
}

void Logger::warn(std::string message)
{
#ifdef LIBM2K_LOG4CPP
	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.addAppender(appender);

	root << log4cpp::Priority::WARN << message;
#else
	std::cout << message << std::endl;
#endif
}

Logger::Logger()
{
#ifdef LIBM2K_LOG4CPP
	appender = new log4cpp::OstreamAppender("console", &std::cerr);
	basicLayout = new log4cpp::BasicLayout();
	appender->setLayout(basicLayout);
#endif
}
