#include <iostream>

#include "libLogger/include/Logging.h"



///
///
///
int main(int argc, char *argv[])
{
	//Logger::Instance("TOTO");

    try
    {
        INFO << "[MMI::main] Info";       
        return 1;
    }

    catch (const std::exception & e )
    {
        FATAL <<  "[MMI::Main] Exception : "<< e.what();
    }
    catch ( ... ) // traite toutes les autres exceptions
    {
        FATAL << "[MMI::Main] Erreur inconnue.";
    }

    return 0;
}
