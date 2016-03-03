#ifndef _LOGGER_H_
#define _LOGGER_H_

#pragma once

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/locale/date_time_facet.hpp>

#include <boost/system/config.hpp>



#define DEBUG BOOST_LOG_SEV(Logger::Instance().logger(), boost::log::trivial::debug)
#define TRACE BOOST_LOG_SEV(Logger::Instance().logger(), boost::log::trivial::trace)
#define INFO BOOST_LOG_SEV(Logger::Instance().logger(), boost::log::trivial::info)
#define WARN BOOST_LOG_SEV(Logger::Instance().logger(), boost::log::trivial::warning)
#define ERR BOOST_LOG_SEV(Logger::Instance().logger(), boost::log::trivial::error)
#define FATAL BOOST_LOG_SEV(Logger::Instance().logger(), boost::log::trivial::fatal)



class Logger
{
    Logger();
    Logger(const Logger&);

public:
    typedef boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level > logger_type;
    static Logger& Instance(std::string strPrefix="");
    static void setInstance(Logger* mylogger);

	inline static void setPrefix(const std::string& strPrefix) { _strPrefix = strPrefix; }

    logger_type& logger(){return boostlogger;}

    virtual ~Logger();

protected:
    static std::auto_ptr<Logger>    _LoggerInstance;
	//
	//static std::string _strPrefix;

private:
    logger_type boostlogger;

	static std::string _strPrefix;
};

#endif
