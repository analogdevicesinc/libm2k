#ifndef LOGGER_H
#define LOGGER_H

#ifdef LIBM2K_LOG4CPP

#include <log4cpp/Appender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/Category.hh>

#endif

#include <string>
#include <iostream>
#include <libm2k/m2kglobal.hpp>

#ifdef LIBM2K_ENABLE_LOG
	#define LOG(x) libm2k::Logger::getInstance().warn(x)
#else
	#define LOG(x)
#endif

namespace libm2k {
class LIBM2K_API Logger
{
public:
    static Logger & getInstance();
    void warn(std::string message);


private:
	Logger();

#ifdef LIBM2K_LOG4CPP

        log4cpp::Appender *appender;
        log4cpp::BasicLayout *basicLayout;

#endif
};
}

#endif // LOGGER_H
