#include <libLogger/include/Logging.h>


// STATICS
// Instantation des variables statiques de la classe Logger
// (=> allocations mémoires)
//
std::auto_ptr<Logger>Logger::_LoggerInstance =std::auto_ptr<Logger>(NULL);
// par défaut pas de préfixe (== ""), 
// on reste sur le format standard (par défaut) pour le nom de fichier
std::string Logger::_strPrefix = "";


Logger::Logger()
{

}

Logger::Logger(const Logger&)
{

}

Logger::~Logger()
{
;
}

Logger& Logger::Instance(std::string strPrefix)
{
    if(_LoggerInstance.get()==NULL)
    {
		setPrefix(strPrefix);
        setInstance(new Logger());
    }
    return * _LoggerInstance.get();
}

void Logger::setInstance(Logger* myLogger)
{
    std::cout << "Logger_MME::setInstance" << std::endl;
    _LoggerInstance.reset(myLogger);

    std::cout << "Logger_MME::setInstance init boost logger" << std::endl;
    boost::log::add_common_attributes();
    boost::log::register_simple_formatter_factory< boost::log::trivial::severity_level, char >("Severity");

    boost::log::add_file_log
        (
			// filename setting
            boost::log::keywords::file_name = 	(_strPrefix=="" ? "": _strPrefix + '_') +
												"MME_%Y-%m-%d_%H-%M-%S.%2N.log",
			//
            boost::log::keywords::rotation_size = 10 * 1024 * 1024,
            boost::log::keywords::format = "[%TimeStamp%][%Severity%]: %Message%" ,
            boost::log::keywords::auto_flush = true
        );
    boost::log::core::get()->set_filter
        (
            boost::log::trivial::severity >= boost::log::trivial::debug
        );
    std::cout << "Logger_MME::setInstance fin" << std::endl;
}

