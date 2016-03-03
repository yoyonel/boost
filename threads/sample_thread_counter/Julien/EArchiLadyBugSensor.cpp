#include "EArchiLadyBugSensor.h"
#include "EArchiMobile.h"

#include "libXBase/XError.h"
#include "libXFileSystem/XSystemInfo.h"
#include "libXStereopolis/XArchiPcUnit.h"
#include "libXStereopolis/XArchiChantier.h"
#include "libXStereopolis/XArchiMetaData.h"
#include "libXStereopolis/XArchiPanoramique.h"

#include "libLogger\include\Logging.h"

size_t EArchiLadyBugSensor::_nbDeclLadybug =0;
boost::recursive_mutex EArchiLadyBugSensor::_mutexDeclLadybug;
bool EArchiLadyBugSensor::_start = true;
bool EArchiLadyBugSensor::_isWriting = false;
bool EArchiLadyBugSensor::_hasSomethingtoWrite = false;
int EArchiLadyBugSensor::_write_index = -1;
int EArchiLadyBugSensor::_buffer_index = 0;
std::string EArchiLadyBugSensor::_config = "";

//-----------------------------------------------------------------------------
EArchiLadyBugSensor::EArchiLadyBugSensor(XArchiPcUnit* PcUnit, XError* error): XArchiLadyBugSensor(PcUnit,error) 
{
	m_bConnected = false;
}
//-----------------------------------------------------------------------------
EArchiLadyBugSensor::~EArchiLadyBugSensor()
{
}

//-----------------------------------------------------------------------------
void EArchiLadyBugSensor::ThrowErrorMessage(const std::string &origine,const std::string &error)
{
	XErrorError(m_Error,(Origine()+" "+ origine).c_str(),error.c_str());
}
//-----------------------------------------------------------------------------
bool EArchiLadyBugSensor::Initialize()
{
	m_Wrapper = new WrapperLadyBug (NULL);
	LoadDefaultParametersConfiguration();
	DEBUG<<"[EArchiLadyBugSensor::Initialize] initialisation config defaut LB ok";
	m_bConnected = true;

	return true;
}

//---------------------------------------------------------------
void runwriteLadyBug (WrapperLadyBug &wLB, std::string & filename,size_t imagesPerFile,size_t sizePaquet)
//void runwriteLadyBug (WrapperLadyBug *ptr_wLB, std::string & filename,size_t imagesPerFile,size_t sizePaquet)
{
    //Hack pour forcer le passage de la référence et éviter que boost::thread fasse une copie de l'objet
    //WrapperLadyBug &wLB = *ptr_wLB;

    TRACE << "[EArchiLadyBugSensor::runwriteLadyBug] LadyBug nom de fichier " << filename;
    int compteur = 0;
    int i = 0; // index du fichier qui va contenir les images



    while(true)
    {
        DEBUG << "[EArchiLadyBugSensor::runwriteLadyBug] début boucle while : _write index : " << EArchiLadyBugSensor::_write_index << " et wLB bufferindex : " << EArchiLadyBugSensor::_buffer_index;
        if (EArchiLadyBugSensor::_write_index > -1 && EArchiLadyBugSensor::_write_index != EArchiLadyBugSensor::_buffer_index)
        {

            TRACE<<"[EArchiLadyBugSensor::runwriteLadyBug] Il y a une image a écrire";
            //do some writing

            std::stringstream ssFileName;
            ssFileName << filename << std::setw(3) << std::setfill ('0') << i << ".lb";


            if (imagesPerFile <= 0)
            {
                FATAL << "[EArchiLadyBugSensor::runwriteLadyBug] ERREUR : il faut au minimum une image par fichier";
                break;
            }
            if(wLB._raw_LB_data[EArchiLadyBugSensor::_write_index].pData == NULL)
            {
                ERR << "[EArchiLadyBugSensor::runwriteLadyBug] On a une image a ecrire mais elle est nulle (objet mal partagé?)";
                continue;
            }

            FILE * fichOut;
            fopen_s(&fichOut, ssFileName.str().c_str(), "ab+");
            if(fichOut==NULL)
            {
                ERR<<"[EArchiLadyBugSensor::runwriteLadyBug] exception fichier de sortie null";
                break;
            }
            size_t nCols = wLB.getUICols();
            size_t nLines = wLB.getUIRows();
            size_t size = 0;
            size_t nbPaquet = 0;
            TRACE<<"[EArchiLadyBugSensor::runwriteLadyBug] debut d'ecriture du fichier / size paquet = "<<sizePaquet;
            while(size < nCols*nLines*3/2*6)
            {
                size_t trueSize = std::min((size_t) sizePaquet, (nCols*nLines*3/2*6 - size));
                try
                {
                    fwrite(wLB._raw_LB_data[EArchiLadyBugSensor::_write_index].pData, trueSize, 1, fichOut);
                }
                catch(std::exception &e)
                {
                    ERR<<"[EArchiLadyBugSensor::runwriteLadyBug] STL exception " <<  e.what();
                }
                catch(...)
                {
                    ERR<<"[EArchiLadyBugSensor::runwriteLadyBug] unknown exception";
                }
                size += trueSize;
                if(trueSize == sizePaquet)
                {
                    wLB._raw_LB_data[EArchiLadyBugSensor::_write_index].pData += trueSize;
                    nbPaquet++;
                }
            }
            TRACE<<"[EArchiLadyBugSensor::runwriteLadyBug] fin d'ecriture du fichier ";
            fclose(fichOut);
            DEBUG<<"[WrapperLadyBug::recordImage] fermeture du fichier";


            DEBUG << "[EArchiLadyBugSensor::runwriteLadyBug] fin  fonction ecriture image";
            //On incremente le compteur d'images dans l'archive
            compteur++;
            //Si on depasse le nb d'image dans l'archive, on change d'archive
            if (compteur >= (int)imagesPerFile)
            {
                DEBUG << "[EArchiLadyBugSensor::runwriteLadyBug] changement d'archive";
                compteur = 0;
                i++;
            }

            //On incremente l'index d'ecriture
            EArchiLadyBugSensor::_write_index ++;
            //si l'index est a 3, on le repasse a 0
            if (EArchiLadyBugSensor::_write_index == 3) EArchiLadyBugSensor::_write_index =0;

        }
        else
        {
            TRACE<<"[EArchiLadyBugSensor::runwriteLadyBug] Pas d'image a écrire ...  wait for 50ms";
        }
        //On peut se permettre de commencer l'écriture avec 50ms de retard (lecture des donnée LB env. 250ms, écriture : 100ms sur SSD
        boost::this_thread::sleep(boost::posix_time::milliseconds(50));
    }
}

//---------------------------------------------------------------
void runLadyBug (WrapperLadyBug &wLB)
//void runLadyBug (WrapperLadyBug *ptr_wLB)
{
    //Hack pour forcer le passage de la référence et éviter que boost::thread fasse une copie de l'objet
    //WrapperLadyBug &wLB = *ptr_wLB;

	size_t nbTest = 0;
	size_t attenteMax = 20*10;
	size_t compteurBoucle = 0;
	int	m_dCurrentTimeStamp=-1;


    //initialisation du compteur LB a la valeur du compteur général : jlh
    DEBUG << "[EArchiLadyBugSensor::runLadyBug] Recalage des tops. Tops LB:" << EArchiLadyBugSensor::_nbDeclLadybug <<" - Tops Systeme :" << EArchiMobile::_nbCompteur;

    EArchiLadyBugSensor::_nbDeclLadybug = EArchiMobile::_nbCompteur + 1;

	EArchiMobile::_nbReady++;

	DEBUG << "[EArchiLadyBugSensor::runLadyBug] instrument LadyBug pret "<<EArchiMobile::_nbReady<<" "<<EArchiMobile::_nbInit;
	while(EArchiMobile::_nbReady!=EArchiMobile::_nbInit)
	{
		if (compteurBoucle >= EArchiMobile::_nTimeout)
		{
			DEBUG << "[EArchiLadyBugSensor::runLadyBug] attente max atteinte on sort";
			compteurBoucle = 0;
			return;
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		compteurBoucle += 10;
	}

	while(EArchiLadyBugSensor::_start)
	{
		//size_t sizePaquet = 5120; //taille des paquets (en Ko) par defaut : éventuellement à adapter en fontions des tests
		//de performance effectués avec LadybugDemo
		EArchiLadyBugSensor::_mutexDeclLadybug.lock();
		int ts = wLB.getCurrentTimeStamp();
        DEBUG << "[EArchiLadyBugSensor::runLadyBug] timestamp : " << ts;
		
		if (ts == -1 || ts == m_dCurrentTimeStamp)
		{
			if(nbTest >attenteMax)
			{
				DEBUG << "[EArchiLadyBugSensor::runLadyBug] attente max atteinte on sort";
				return;
			}
			else
			{
				nbTest++;
                TRACE << "[EArchiLadyBugSensor::runLadyBug] wait for 10ms till next LB image is ready to transfert...";
				boost::this_thread::sleep(boost::posix_time::milliseconds(10));
				continue;
			}
		}
		nbTest=0;
		m_dCurrentTimeStamp = ts;

        TRACE << "[EArchiLadyBugSensor::runLadyBug] debut recuperation image rotation de l'index d'écriture";

        wLB.bufferIndexRotate();
        EArchiLadyBugSensor::_buffer_index = wLB.getBufferIndex();

        //initialisation : si on a une image et que l'écriture n'est pas initialisée (ie a -1) : on l'initialise
        if (EArchiLadyBugSensor::_write_index == -1) EArchiLadyBugSensor::_write_index = wLB.getBufferIndex()-1  ;
        DEBUG << "[EArchiLadyBugSensor::runLadyBug] Après lecture et rotation, Buffer index = " << wLB.getBufferIndex() << " et _Write_index = " << EArchiLadyBugSensor::_write_index;
        //Gestion du cas de la collision (peu probable ...)
        if (wLB.getBufferIndex() == EArchiLadyBugSensor::_write_index )
        {
            //Collision lecture / Ecriture // a gerer plus tard
            ERR << "[EArchiLadyBugSensor::runLadyBug] Collision : la lecture des images LB a rattrappé l'écriture !!!";
            continue;

        }
        TRACE << "[EArchiLadyBugSensor::runLadyBug] fin recuperation image";
        EArchiLadyBugSensor::_nbDeclLadybug++; //l'image est là :on incremente pour ne pas locker l'ecriture du XML si tous les autre scapteurs sont prets
        TRACE << "[EArchiLadyBugSensor::runLadyBug] incrementation de EArchiLadyBugSensor::_nbDeclLadybug";

        EArchiLadyBugSensor::_mutexDeclLadybug.unlock();

        //todo jlh passage a 20ms pour relire l'image suivante plus vite
        boost::this_thread::sleep(boost::posix_time::milliseconds(50));
	}
}
//-----------------------------------------------------------------------------
void EArchiLadyBugSensor::StartStop(bool state)
{
	DEBUG << "[EArchiLadyBugSensor::runLadyBug] StartStop LB Sensor";
	XArchiMetaData MetaDataTemp;
	MetaDataTemp.Chantier(Mobile()->Chantier()->Specif()->Name());
	MetaDataTemp.Position(m_id);
	MetaDataTemp.Section(Mobile()->SectionEnCours());
	MetaDataTemp.Date(Mobile()->DateCourante());
	MetaDataTemp.Heure(Mobile()->SessionCourante());

	//Send the trigger state
	m_Wrapper->setTriggerToCamera(state);

	if(state) //start
	{
		TRACE << "[EArchiLadyBugSensor::runLadyBug] start LB Sensor : "<<Mobile()->LadyBugSensor()->StockData();
		std::stringstream stSession;
		stSession<<Mobile()->SessionCourante();
		std::stringstream stSection;
		stSection<<Mobile()->SectionEnCours();
		boost::filesystem::path pDataLadyBug(Mobile()->LadyBugSensor()->StockData()+"Session_"+stSession.str());
		pDataLadyBug /= "Section_"+stSection.str();
		if(!boost::filesystem::exists(pDataLadyBug))
		{
			boost::filesystem::create_directories(pDataLadyBug);
		}
		pDataLadyBug.make_preferred();
		pDataLadyBug /= MetaDataTemp.CompleteMissionName();
		TRACE<<"[EArchiLadyBugSensor::StartStop] outputDir : "<<pDataLadyBug.string();

        recordThreadLadyBug = boost::thread(runLadyBug,*m_Wrapper);
        //recordThreadLadyBug = boost::thread(runLadyBug,m_Wrapper);
        recordThreadLadyBug.detach();
        writeThreadLadyBug = boost::thread(runwriteLadyBug,*m_Wrapper,pDataLadyBug.string(),
        //writeThreadLadyBug = boost::thread(runwriteLadyBug,m_Wrapper,pDataLadyBug.string(),
                                           Mobile()->LadyBugSensor()->SensorConfig()->ImagesPerFile(),
                                           Mobile()->LadyBugSensor()->SensorConfig()->SizePaquet());
        writeThreadLadyBug.detach();
	}
	else //state = false = stop
    {
		DEBUG<<"[EArchiLadyBugSensor::StartStop] stop";
		if (m_Wrapper->getRecordingInProgress())
		{
			EArchiLadyBugSensor::_start = false;
			DEBUG<<"[EArchiLadyBugSensor::StartStop] getRecordingInProgress : stop";
			m_Wrapper->stopAcquisition();
		}
		m_Wrapper->stopCamera();
		DEBUG<<"[EArchiLadyBugSensor::StartStop] ladybug stand by";
	}
}
//-----------------------------------------------------------------------------
bool EArchiLadyBugSensor::GetCurrentLbImages(std::vector<unsigned char*> &redImages, size_t &nColsOut, size_t &nLinesOut)
{
	DEBUG << "[EArchiLadyBugSensor::GetCurrentLbImages]";
	if(m_Wrapper == NULL)
	{
		ERR<<"[EArchiLadyBugSensor::StartStop] Wrapper Ladybug NULL !";
		return XErrorError(m_Error, __FUNCTION__, "WrapperLadyBug NULL !");
	}

	m_Wrapper->getCurrentImage(nColsOut, nLinesOut);

	for(size_t k = 0; k<1; ++k) //1 seule image pour le moment
	{ 
		TRACE << "[EArchiLadyBugSensor::StartStop] on recupere l'image n" << k ;
		redImages.push_back(m_Wrapper->getNRedImage(k, nColsOut, nLinesOut));
	}

	return true;
}
//-----------------------------------------------------------------------------
bool EArchiLadyBugSensor::GetLbMessage(std::string &message)
{
	if(m_Wrapper == NULL)
		return XErrorError(m_Error, __FUNCTION__, "WrapperLadyBug NULL !");


	m_Wrapper->getLbMessage(message);
	DEBUG << "[EArchiLadyBugSensor::GetLbMessage] statut = " << message;
	return true;
}

//-----------------------------------------------------------------------------
void EArchiLadyBugSensor::LoadDefaultParametersConfiguration()
{
	DEBUG<<"[EArchiLadyBugSensor::LoadDefaultParametersConfiguration] Init";
	m_Wrapper->setDataFormat ((int)SensorConfig()->ColorMode());
	DEBUG<<"[EArchiLadyBugSensor::LoadDefaultParametersConfiguration] debut start";
	bool okStartCamera = m_Wrapper->startCamera();
	DEBUG<<"[EArchiLadyBugSensor::LoadDefaultParametersConfiguration] LadyBug start camera ? "<<okStartCamera;
	
	m_Wrapper->setAutoShutter ((int)SensorConfig()->ShutterAuto());
	m_Wrapper->setFrameRate (SensorConfig()->FrameRate());
	m_Wrapper->setGamma (SensorConfig()->Gamma());
	m_Wrapper->setGain (SensorConfig()->Gain());
	m_Wrapper->setWhiteBalance (SensorConfig()->WhiteBalance());
	m_Wrapper->setBayerPattern ((int)SensorConfig()->bayerPattern());
	m_Wrapper->setImagesPerFile(SensorConfig()->ImagesPerFile());

	//Send the trigger state
	m_Wrapper->setTriggerToCamera(false);
}
