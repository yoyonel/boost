#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <windows.h>
#include <algorithm>

#include "../include/WrapperLadyBug.h"

#include "libLogger\include\Logging.h"

#undef min

///
///
/// Constructeur
WrapperLadyBug::WrapperLadyBug(XError* err)
{
	_iFrameCount = 1;
	_context = NULL;
	_streamContext = NULL;
	_bRecordingInProgress = false;
	_bCameraStarted = false;
	_totalDiskSpace = 0.0;
	_numFrames = 0;
	_numBuffers = NULL;
	_ladybugDataFormat = LADYBUG_DATAFORMAT_RAW12;
	_totalImg = 0;
    _bufferIndex=0;
}

WrapperLadyBug::WrapperLadyBug(XError* err, LadybugDataFormat eLadybugDataFormat)
{
	_iFrameCount = 1;
	_context = NULL;
	_streamContext = NULL;
	_bRecordingInProgress = false;
	_bCameraStarted = false;
	_totalDiskSpace = 0.0;
	_numFrames = 0;
	_numBuffers = NULL;
	_ladybugDataFormat = eLadybugDataFormat;
	_totalImg = 0;
    _bufferIndex=0;
}
///
///
///
WrapperLadyBug::~WrapperLadyBug()
{

}

///
///
///
bool WrapperLadyBug::sendError(const char* origine)
{
	if( _lbError != LADYBUG_OK )
	{
		ERR<<"[WrapperLadyBug::sendError] Erreur Ladybug : "<< ladybugErrorToString(_lbError);
		return XErrorError(_error, origine, "Erreur ladybug", ladybugErrorToString(_lbError));
	}
	else //modif MJ
	{
		DEBUG<<"[WrapperLadyBug::sendError] OK Ladybug : "<< ladybugErrorToString(_lbError);
		DEBUG<<"[WrapperLadybug::sendError] OK Ladybug2 : " << _lbError;
		return true;
	}
}

///
///
///
void WrapperLadyBug::cleanUp()
{
	// remise a zero des variables
	_bRecordingInProgress = false;
	_bCameraStarted = false;
	DEBUG<<"[WrapperLadyBug::sendError] Destroying context";
	// mode du trigger a default
	LadybugTriggerMode triggerMode;
	_lbError = ladybugGetTriggerMode(_context, &triggerMode);
	triggerMode.bOnOff = false;
	_lbError  = ladybugSetTriggerMode(_context, &triggerMode);
	if (!sendError(__FUNCTION__)) return;
	// destruction du contexte de la LB
	if(_context != NULL)
	{
		_lbError = ladybugDestroyContext(&_context);
		if(!sendError(__FUNCTION__)) return;
		_context = NULL;
	}
	// destruction du contexte de stream de la LB
	if(_streamContext != NULL)
	{
		_lbError = ladybugDestroyStreamContext(&_streamContext);
		if(!sendError(__FUNCTION__)) return;
		_streamContext = NULL;
	}	
}


///
///
///
bool WrapperLadyBug::initializeCamera()
{
	DEBUG<<"[WrapperLadyBug::initializeCamera]";
	// Initialize context
	_lbError = ladybugCreateContext(&_context);
	if (!sendError(__FUNCTION__)) return false;
	if(_context == NULL)
        DEBUG<<"[WrapperLadyBug::initializeCamera] context null";
	else
        DEBUG<<"[WrapperLadyBug::initializeCamera] Context : "<<_context;
	printf("LadyBug initialisation\n");
	//XErrorCommentaire(_error, __FUNCTION__, "Ladybug initialisation");
	TRACE<<"[WrapperLadyBug::initializeCamera] num buffer : "<<_numBuffers;
	_lbError = ladybugInitializePlus(_context, 0, _numBuffers, NULL, 0);
	if (!sendError(__FUNCTION__)) return false;
	if(_context == NULL)
		DEBUG<<"[WrapperLadyBug::initializeCamera] context null";

	// Set the timeout value for the grab function to zero
	// Do not wait if there is no image waiting
	ladybugSetGrabTimeout(_context, 1000); // ms
	// Start Ladybug with the specified data format
	DEBUG<<"[WrapperLadyBug::initializeCamera] Ladybug setgrabTimeout";
	_lbError = ladybugStart(_context, _ladybugDataFormat);
	INFO<<"[WrapperLadyBug::initializeCamera] Info ladybug : "<< ladybugErrorToString(_lbError);
	if (!sendError(__FUNCTION__)) return false;

	int maxTries = 0;
	do
	{
		ladybugUnlockAll(_context);
		Sleep(100);
		if(ladybugLockNext(_context, &_prevImage) == LADYBUG_OK)
		{
			DEBUG<<"[WrapperLadyBug::initializeCamera] WL context ok";
			return true;
		}
	}
	while(maxTries++ < 10);
	return false;
}

///
///
///
bool WrapperLadyBug::startCamera()
{
	DEBUG<<"[WrapperLadyBug::startCamera]Debut init camera";
	if(!initializeCamera())
	{
		cleanUp();
		DEBUG<<"[WrapperLadyBug::startCamera]WL nettoyage necessaire";
		if(!initializeCamera())
			if(!sendError(__FUNCTION__)) return false;
	}
	else
		DEBUG<<"[WrapperLadyBug::startCamera]WL LadyBug l'initialisation s'est bien passee";
	// load config file from the head
	_lbError = ladybugLoadConfig(_context, NULL);
	if(!sendError(__FUNCTION__)) return false;


	LadybugTriggerModeInfo triggerModeInfo;
	_lbError = ladybugGetTriggerModeInfo(_context, &triggerModeInfo);
	DEBUG << "[WrapperLadyBUg::startCamera] lbError after setTriggerMode =" << ladybugErrorToString(_lbError);
	if(!sendError(__FUNCTION__)) return false;
	
	_bCameraStarted = true;
	DEBUG<<"[WrapperLadyBug::startCamera] Fin initialisation camera";
	return true;
}

///
///
///
void WrapperLadyBug::stopCamera()
{
	_lbError = ladybugDestroyContext(&_context);
	INFO<<"[WrapperLadyBug::stopCamera] ladyBug off ? "<< ladybugErrorToString(_lbError);
	_bCameraStarted = false;
}

///
///
///
bool WrapperLadyBug::startAcquisition(const std::string &streamNameToOpen)
{
	if(!_bCameraStarted)
	{
		ERR<<"[WrapperLadyBug::startAcquisition] La camera n'est pas allumee !";
		return XErrorError(_error, __FUNCTION__, "La camera n'est pas allumee !");
	}
	char streamNameOpened [ _MAX_PATH ];

	if(_context == NULL)
	{
		ERR<<"[WrapperLadyBug::startAcquisition] Le contexte camera n'a pas ete initialise !";
		printf("Le contexte camera n'a pas ete initialise\n");
	}
	if(_streamContext == NULL)
	{
		_lbError = ladybugCreateStreamContext(&_streamContext);
		if(_lbError != LADYBUG_OK)
		{
			ERR<<"[WrapperLadyBug::startAcquisition] Erreur ladyBug : "<<ladybugErrorToString(_lbError);
			printf("Erreur ladyBug : %s\n", ladybugErrorToString(_lbError));
		}
	}

	_firstImage = true;

	_lbError = ladybugInitializeStreamForWriting(_streamContext, streamNameToOpen.c_str(), _context, streamNameOpened, true);
	_bRecordingInProgress = ( _lbError == LADYBUG_OK );
	return true;
}

///
///
///
unsigned char* WrapperLadyBug::getCurrentImage(size_t &nCols, size_t &nLines)
{

    DEBUG << "[WrapperLadyBug::getCurrentImage] ERREUR : si on passe ici le code n'est pas campatible triple buffering";
	nCols = _currentImage.uiFullCols;
	nLines = _currentImage.uiFullRows;
	return _currentImage.pData;
}

///
///
///
unsigned char* WrapperLadyBug::getNRedImage(const size_t &n, size_t &nColsEchOut, size_t &nLinesEchOut)
{
    DEBUG << "[WrapperLadyBug::getNRedImage] ERREUR : si on passe ici le code n'est pas campatible triple buffering";
	/// dimensions de l'image totale (6 tetes)
	size_t nCols, nLines;
	nCols = _currentImage.uiFullCols;
	nLines = _currentImage.uiFullRows;

	/// ptr a utiliser pour le sous ech
	unsigned char *img1 = _currentImage.pData;
	unsigned char *img2 = _currentImage.pData;
	unsigned char *img11 = _currentImage.pData;
	unsigned char *img21 = _currentImage.pData;

	/// securite si pas d'image
	if(nCols == 0 || nLines == 0)
	{
		DEBUG<<"[WrapperLadyBug::getNRedImage]";
		return 0;
	}
		
	/// on redresse les images : inversion lignes et colonnes + sous ech pour affichage
	DEBUG<<"[WrapperLadyBug::getNRedImage]On redresse les images + sous ech";
	size_t facEch = 8;
	size_t nColsEch = nLines / facEch;
	size_t nLinesEch = nCols / facEch;

	nColsEchOut = nColsEch;
	nLinesEchOut = nLinesEch;
	
	
	/// ptr a utiliser pour l'image renvoyee
	unsigned char *outImg = new unsigned char[nColsEchOut*nLinesEchOut*3]; // * 3 = RGB

	// facteur 1.5 : images en 12 bits et bayer (nCols et nLines doivent etre pairs)
	size_t offSetImg = nCols * nLines * 3 / 2;
	size_t offSetLine = nCols * 3 / 2;
	size_t reste = nCols - nLinesEch * facEch; // on a fait une rotation
	size_t resteImg = nLines - nColsEch * facEch;

	img1 += n*offSetImg;
	img11 += n*offSetImg;
	img2 += n*offSetImg;
	img21 += n*offSetImg;

	// le bleu est decale
	img2 += offSetLine;
	img11 += offSetLine * facEch / 2;
	img21 += offSetLine * (1 + facEch / 2);

	TRACE<<"[WrapperLadyBug::getNRedImage]Reste = "<<reste<<" Resteimg = "<<resteImg;

	DEBUG<<"[WrapperLadyBug::getNRedImage] Debut de l'image n "<<n;
	for(size_t l = 0; l < nColsEch; ++l)
	{
		DEBUG<<"[WrapperLadyBug::getNRedImage] Ligne "<< l;
		for(size_t c = 0; c < nLinesEch; ++c)
		{
			DEBUG<<"[WrapperLadyBug::getNRedImage] a_r_g_"<<c;

			int a1 = *img1;
			int a2 = *(++img1);
			int a3 = *(++img1);
			int a3_r = (a1*16 + a2%16);
					
			int a3_g = (a3*16 + a2/16);

			img1 += (size_t) (facEch*1.5 / 2 -2); // *2 car 16 bits, /2 car on se deplace de la moitie seulement
					
			DEBUG<<"[WrapperLadyBug::getNRedImage] b_r_g_"<<c;

			int b1 = *img1;
			int b2 = *(++img1);
			int b3 = *(++img1);

			int b3_r = (b1*16 + b2%16);
					
			int b3_g = (b3*16 + b2/16);

					
			DEBUG<<"[WrapperLadyBug::getNRedImage] a_b_"<<c;
			///
			int a1_b = *(img2);
			int a2_b = *(++img2);

			int a3_b = (a2_b%16 + a1_b*16);
			
			img2 += (size_t) (facEch*1.5 / 2 -1);
			
			DEBUG<<"[WrapperLadyBug::getNRedImage] b_b_"<<c;

			int b1_b = *(img2);
			int b2_b = *(++img2);
			int b3_b = (b2_b%16 + b1_b*16);

			////***********************************************************************
					
			DEBUG<<"[WrapperLadyBug::getNRedImage] c_r_g_"<<c;
			int c1 = *img11;
			int c2 = *(++img11);
			int c3 = *(++img11);
			
			int c3_r = (c2%16 + c1*16);
					
			int c3_g = (c3*16 + c2/16);

			img11 += (size_t) (facEch*1.5 / 2 -2); // *2 car 16 bits, /2 car on se deplace de la moitie seulement, *1.5 car 12 bits

					
			DEBUG<<"[WrapperLadyBug::getNRedImage] d_r_g_"<<c;
			int d1 = *img11;
			int d2 = *(++img11);
			int d3 = *(++img11);
			
			int d3_r = (d2%16 + d1*16);
					
			int d3_g = (d3*16 + d2/16);


					
			DEBUG<<"[WrapperLadyBug::getNRedImage] c_b_"<<c;
			///
			int c1_b = *(img21);
			int c2_b = *(++img21);

			int c3_b = (c2_b%16 + c1_b*16);

			img21 += (size_t) (facEch*1.5 / 2 -1);

					
			DEBUG<<"[WrapperLadyBug::getNRedImage] d_b_"<<c;
			int d1_b = *(img21);
			int d2_b = *(++img21);
			int d3_b = (d2_b%16 + d1_b*16);

			/// on fait les moyennes (sous ech moyenne)
			DEBUG<<"[WrapperLadyBug::getNRedImage] On fait les moyennes";
			a3_r = (a3_r + b3_r + c3_r + d3_r) / 4;
			a3_g = (a3_g + b3_g + c3_g + d3_g) / 4;
			a3_b = (a3_b + b3_b + c3_b + d3_b) / 4;
			
			DEBUG<<"[WrapperLadyBug::getNRedImage] Recuperation des valeurs des pixels en sortie";
			size_t col2, l2;
			col2 = nColsEch - l - 1;
			l2 = c;
			size_t pix = l2*nColsEch + col2;
			outImg[3*pix] =  std::min(255,a3_r/16);
			outImg[3*pix+1] =  std::min(255,a3_g/16);
			outImg[3*pix+2] =  std::min(255,a3_b/16);

			DEBUG<<"[WrapperLadyBug::getNRedImage] Decalages pour le pixel suivant";
			/// on fait les decalages pour le pixel suivant
			img1 += (size_t) (facEch * 1.5 / 2 - 2); // *1.5 car 12 bits, / 2 car deja deplace pour le sous ech
			img2 += (size_t) (facEch * 1.5 / 2 - 1);
			img11 += (size_t) (facEch * 1.5 / 2 - 2);
			img21 += (size_t) (facEch * 1.5 / 2 - 1);
		} // colonnes

		DEBUG<<"[WrapperLadyBug::getNRedImage] Verif si fin d'image";
		if (l == (nColsEch-1)) 
		{
			DEBUG<<"[WrapperLadyBug::getNRedImage] Deja en fin d'image";
		}
		else
		{
			DEBUG<<"[WrapperLadyBug::getNRedImage] On se decale car pas en fin d'image";
			img1 += reste;
			img2 += reste;
			img11 += reste;
			img21 += reste;

			DEBUG<<"[WrapperLadyBug::getNRedImage] offsetLine : "<<offSetLine;

			img1 += offSetLine * (facEch-1);
			img2 += offSetLine * (facEch-1);
			img11 += offSetLine * (facEch-1);
			img21 += offSetLine * (facEch-1);

			DEBUG<<"[WrapperLadyBug::getNRedImage] Fin des decalages";

		}

	} //lignes

	DEBUG<<"[WrapperLadyBug::getNRedImage] END";

	return outImg;
}

///
///
///
void WrapperLadyBug::recordImage(const std::string &filename, const int &sizePaquet, const int nbImgPerFile)
{

    DEBUG << "[WrapperLadyBug::recordImage] ERREUR : si on passe ici le code n'est pas campatible triple buffering";
	bool verbose = 0;
	std::ostringstream oss;
	oss << filename << "_" << ++_totalImg; 

	if (nbImgPerFile <= 0)
	{
		FATAL << "[WrapperLadyBug::recordImage] ERREUR : il faut au minimum une image par fichier";
		return;
	}

	
	DEBUG<<"[WrapperLadyBug::recordImage] ouverture du fichier / nom = "<< oss.str();

	FILE * fichOut;
	//fopen_s(&fichOut, oss.str().c_str(), "wb+");
	fopen_s(&fichOut, filename.c_str(), "ab+");
	if(fichOut==NULL)
	{
		ERR<<"[WrapperLadyBug::recordImage] exception fichier de sortie null";
		return;
	}

	unsigned char* img1 = _currentImage.pData;
	size_t nCols = _currentImage.uiFullCols;
	size_t nLines = _currentImage.uiFullRows;
	size_t size = 0;
	size_t nbPaquet = 0;
	TRACE<<"[WrapperLadyBug::recordImage] debut d'ecriture du fichier / size paquet = "<<sizePaquet;
	while(size < nCols*nLines*3/2*6)
	{
		size_t trueSize = std::min((size_t) sizePaquet, (nCols*nLines*3/2*6 - size));
		try
		{
            fwrite(img1, trueSize, 1, fichOut);
		}
		catch(std::exception &e)
		{
			ERR<<"[WrapperLadyBug::recordImage] STL exception " <<  e.what();
		}
		catch(...)
		{
			ERR<<"[WrapperLadyBug::recordImage] unknown exception";
		}
		size += trueSize;
		if(trueSize == sizePaquet) 
		{
			img1 += trueSize;
			nbPaquet++;
		}
	}
	TRACE<<"[WrapperLadyBug::recordImage] fin d'ecriture du fichier ";
	fclose(fichOut);
	DEBUG<<"[WrapperLadyBug::recordImage] fermeture du fichier";
}


///
///
///
long long	WrapperLadyBug::getCurrentTimeStamp() 
{
    TRACE << "[WrapperLadyBug::getCurrentTimeStamp] Avant ";
    LadybugError error = ladybugGrabImage(_context, &_raw_LB_data[_bufferIndex]);
    //_bufferIndex++;
    DEBUG << "[WrapperLadyBug::getCurrentTimeStamp] Après grab image. Buffer Index : "<< _bufferIndex;
    //if (_bufferIndex == 3) _bufferIndex = 0;
	if(error == LADYBUG_OK) 
        return _raw_LB_data[_bufferIndex].timeStamp.ulSeconds*1000000+_raw_LB_data[_bufferIndex].timeStamp.ulMicroSeconds;
	else return -1;
}

///
///
///
unsigned int WrapperLadyBug::getImagesPerFile()
{
    return _imagesPerFile;
}

void WrapperLadyBug::bufferIndexRotate()
{
    _bufferIndex ++;
    if (_bufferIndex == 3) _bufferIndex = 0;

}

///
///
///
bool WrapperLadyBug::setShutter(float value)
{
	if( (value < 0.02) || (value > 200) || (_context == NULL) || !_bCameraStarted)
		return false;

	_lbError = ladybugSetAbsProperty( _context, LADYBUG_SHUTTER, value);
	float effectiveShutterValue = 0;
	if(!sendError(__FUNCTION__)) return false;
	_shutterValue = value;
	return true;
}


///
///
///
void WrapperLadyBug::stopAcquisition()
{
	_lbError = ladybugStopStream(_streamContext);
	_bRecordingInProgress = false;
	INFO<<"[WrapperLadyBug::stopAcquisition] Fin de l'enregistrement : "<<ladybugErrorToString(_lbError);
	XErrorCommentaire( _error, __FUNCTION__, "Fin de l'enregistrement");
}

///
///
///
bool WrapperLadyBug::setBayerPattern (int nBayerPattern)
{
	_lbError = ladybugSetColorTileFormat( _context, (LadybugStippledFormat)nBayerPattern);
	if(!sendError(__FUNCTION__)) return false;
	_ladybugStippledFormat = (LadybugStippledFormat)nBayerPattern;
	return true;
}

///
///
///
bool WrapperLadyBug::setFrameRate (float fFrameRate)
{
	_lbError = ladybugGetAbsProperty( _context, LADYBUG_FRAME_RATE, &fFrameRate);
	if(!sendError(__FUNCTION__)) return false;
	_framerate = fFrameRate;
	return true;
}

///
///
///
bool WrapperLadyBug::setGamma (float fGamma)
{
	_lbError = ladybugGetAbsProperty( _context, LADYBUG_GAMMA, &fGamma);
	if(!sendError(__FUNCTION__)) return false;
	_gamma = fGamma;
	return true;
}

///
///
///
bool WrapperLadyBug::setGain (float fGain)
{
	_lbError = ladybugGetAbsProperty( _context, LADYBUG_GAIN, &fGain);
	if(!sendError(__FUNCTION__)) return false;
	_gain = fGain;
	return true;
}

///
///
///
bool WrapperLadyBug::setWhiteBalance (float fWhiteBalance)
{
	_lbError = ladybugGetAbsProperty( _context, LADYBUG_WHITE_BALANCE, &fWhiteBalance);
	if(!sendError(__FUNCTION__)) return false;
	_whiteBalance = fWhiteBalance;
	return true;
}

///
///
///
bool WrapperLadyBug::setAutoShutter (int nShutterRange)
{
	_lbError = ladybugSetAutoShutterRange( _context, (LadybugAutoShutterRange)nShutterRange);
	if(!sendError(__FUNCTION__)) return false;
	_shutterRange = (LadybugAutoShutterRange)nShutterRange;
	return true;
}

///
///
///
bool WrapperLadyBug::setDataFormat (int nDataFormat)
{
	_ladybugDataFormat = (LadybugDataFormat)nDataFormat;
	return true;
}

///
///
///
bool WrapperLadyBug::setTriggerToCamera (bool triggerOnOff)
{
	LadybugTriggerMode triggerMode;
	_lbError = ladybugGetTriggerMode(_context, &triggerMode);
	if(!sendError(__FUNCTION__)) return false;
	triggerMode.bOnOff = triggerOnOff;
	triggerMode.uiMode = 0; /// pour test !!! (0)
	triggerMode.uiParameter = 0;
	triggerMode.uiPolarity = 0;
	triggerMode.uiSource = 0;
	_lbError = ladybugSetTriggerMode(_context, &triggerMode);

	if(!sendError(__FUNCTION__)) return false;
	return true;
}

///
///
///
bool WrapperLadyBug::setImagesPerFile (unsigned int imagesPerFile)
{
	_imagesPerFile = imagesPerFile;
	return true;
}

std::string WrapperLadyBug::convertLadybugError(LadybugError const error)
{
	std::string strError;
	switch(error)
	{
	case 0:
		strError = "LADYBUG_OK";
		break;
	case 1:
		strError = "LADYBUG_FAILED";
		break;
	case 2:
		strError = "LADYBUG_INVALID_ARGUMENT";
		break;
	case 3:
		strError = "LADYBUG_INVALID_CONTEXT";
		break;
	case 4:
		strError = "LADYBUG_NOT_IMPLEMENTED";
		break;
	case 5:
		strError = "LADYBUG_NOT_SUPPORTED";
		break;
	case 6:
		strError = "LADYBUG_ALREADY_INITIALIZED";
		break;
	case 7:
		strError = "LADYBUG_ALREADY_STARTED";
		break;
	case 8:
		strError = "LADYBUG_COULD_NOT_OPEN_FILE";
		break;
	case 9:
		strError = "LADYBUG_MEMORY_ALLOC_ERROR";
		break;
	case 10:
		strError = "LADYBUG_ERROR_DISK_NOT_ENOUGH_SPACE";
		break;
	case 11:
		strError = "LADYBUG_STREAM_FILE_NOT_OPENED";
		break;
	case 12:
		strError = "LADYBUG_INVALID_STREAM_FILE_NAME";
		break;
	case 13:
		strError = "LADYBUG_NOT_INITIALIZED";
		break;
	case 14:
		strError = "LADYBUG_NOT_STARTED";
		break;
	case 15:
		strError = "LADYBUG_MAX_BANDWIDTH_EXCEEDED";
		break;
	case 16:
		strError = "LADYBUG_INVALID_VIDEO_SETTING";
		break;
	case 17:
		strError = "LADYBUG_NEED_RECTIFY_RESOLUTION";
		break;
	case 18:
		strError = "LADYBUG_DEPRECATED";
		break;
	case 19:
		strError = "LADYBUG_IMAGE_TOO_SMALL";
		break;
	case 20:
		strError = "LADYBUG_TIMEOUT";
		break;
	case 21:
		strError = "LADYBUG_TOO_MANY_LOCKED_BUFFERS";
		break;
	case 22:
		strError = "LADYBUG_CALIBRATION_FILE_NOT_FOUND";
		break;
	case 23:
		strError = "LADYBUG_JPEG_ERROR";
		break;
	case 24:
		strError = "LADYBUG_JPEG_HEADER_ERROR";
		break;
	case 25:
		strError = "LADYBUG_JPEG_INCOMPLETE_COMPRESSION";
		break;
	case 26:
		strError = "LADYBUG_JPEG_NO_IMAGE";
		break;
	case 27:
		strError = "LADYBUG_CORRUPTED_IMAGE_DATA";
		break;
	case 28:
		strError = "LADYBUG_OFFSCREEN_BUFFER_INIT_ERROR";
		break;
	case 29:
		strError = "LADYBUG_FRAMEBUFFER_UNSUPPORTED_FORMAT";
		break;
	case 30:
		strError = "LADYBUG_FRAMEBUFFER_INCOMPLETE";
		break;
	case 31:
		strError = "LADYBUG_GPS_COULD_NOT_BE_STARTED";
		break;
	case 32:
		strError = "LADYBUG_GPS_NOT_STARTED";
		break;
	case 33:
		strError = "LADYBUG_GPS_NO_DATA";
		break;
	case 34:
		strError = "LADYBUG_GPS_NO_DATA_FOR_THIS_SENTENCE";
		break;
	case 35:
		strError = "LADYBUG_GPS_COMM_PORT_IN_USE";
		break;
	case 36:
		strError = "LADYBUG_GPS_COMM_PORT_DOES_NOT_EXIST";
		break;
	case 37:
		strError = "LADYBUG_OPENGL_DISPLAYLIST_NOT_INITIALIZED";
		break;
	case 38:
		strError = "LADYBUG_OPENGL_TEXTUREIMAGE_NOT_UPDATED";
		break;
	case 39:
		strError = "LADYBUG_INVALID_OPENGL_DEVICE_CONTEXT";
		break;
	case 40:
		strError = "LADYBUG_INVALID_OPENGL_RENDERING_CONTEXT";
		break;
	case 41:
		strError = "LADYBUG_INVALID_OPENGL_TEXTURE";
		break;
	case 42:
		strError = "LADYBUG_INVALID_OPENGL_OPERATION";
		break;
	case 43:
		strError = "LADYBUG_NOT_ENOUGH_RESOURCE_FOR_OPENGL_TEXTURE";
		break;
	case 44:
		strError = "LADYBUG_SHARING_DISPLAYLIST_FAILED";
		break;
	case 45:
		strError = "LADYBUG_INVALID_OFFSCREEN_BUFFER_SIZE";
		break;
	case 46:
		strError = "LADYBUG_STILL_WORKING";
		break;
	case 47:
		strError = "LADYBUG_CORRUPTED_PGR_STREAM";
		break;
	case 48:
		strError = "LADYBUG_GPU_CUDA_DRIVER_ERROR";
		break;
	case 49:
		strError = "LADYBUG_GPU_ERROR";
		break;
	case 50:
		strError = "LADYBUG_LOW_LEVEL_FAILURE";
		break;
	case 51:
		strError = "LADYBUG_REGISTER_FAILED";
		break;
	case 52:
		strError = "LADYBUG_ISOCH_FAILED";
		break;
	case 53:
		strError = "LADYBUG_RETRIEVE_BUFFER_FAILED";
		break;
	case 54:
		strError = "LADYBUG_IMAGE_LIBRARY_FAILED";
		break;
	case 55:
		strError = "LADYBUG_BUS_MASTER_FAILED";
		break;
	case 56:
		strError = "LADYBUG_ERROR_UNKNOWN";
		break;
	case 57:
		strError = "LADYBUG_BAD_VOLTAGE";
		break;
	case 58:
		strError = "LADYBUG_BAD_INTERFACE";
		break;
	case 59:
		strError = "LADYBUG_OVEREXPOSED";
		break;
	case 60:
		strError = "LADYBUG_INTERNAL_ERROR";
		break;
	case 61:
		strError = "LADYBUG_NUM_LADYBUG_ERRORS";
		break;
	case 62:
		strError = "LADYBUG_ERROR_FORCE_QUADLET";
		break;
	}

	return strError;
}

///
///
///
bool WrapperLadyBug::getLbMessage(std::string &message)
{
	DEBUG << "[WrapperLadybug::getLbMessage] lbError = " << ladybugErrorToString(_lbError);
	std::string lbError = ladybugErrorToString(_lbError);

	if(lbError == "")
	{
		DEBUG << "[WrapperLadyBug::getLbMessage] lbError == NULL";
		return false;
	}
	else
	{
		message = convertLadybugError(_lbError);
		DEBUG << "[WrapperLadyBug::getLbMessage] statut = " << message;
		return true;
	}
}
