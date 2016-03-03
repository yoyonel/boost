#ifndef _WRAPPERLADYBUG_H_
#define _WRAPPERLADYBUG_H_

#include "ladybug.h"
#include "ladybugstream.h"
#include "ladybuggeom.h"
#include "libXBase/XError.h"
#include <vector>

class WrapperLadyBug 
{
public:
	enum Error
	{
		OK,
		FILE_NOT_FOUND,
		FILE_ERROR,
		KEY_NOT_FOUND
	};

	WrapperLadyBug();
	WrapperLadyBug(XError* err);
	WrapperLadyBug(XError*, LadybugDataFormat);
	~WrapperLadyBug();

	void cleanUp();

	bool						initializeCamera();
	bool						startCamera();
	void						stopCamera();
	bool						setTriggerToCamera(bool triggerOnOff);

	bool						startAcquisition(const std::string &streamNameToOpen);
	void						stopAcquisition();
	//bool						recordingImage();

	unsigned char*				getCurrentImage(size_t &nCols, size_t &nLines);
    unsigned int                getUICols(){return _currentImage.uiFullCols;}
    unsigned int                getUIRows(){return _currentImage.uiFullRows;}

	unsigned char*				getNRedImage(const size_t &n, size_t &nColsEchOut, size_t &nLinesEchOut);

	void						recordImage(const std::string &filename, const int &sizePaquet, const int NbImgPerFile);

	bool						setShutter(float value);
	bool						setBayerPattern (int);
	bool						setFrameRate (float);
	bool						setGamma (float);
	bool						setGain (float);
	bool						setWhiteBalance (float);
	bool						setAutoShutter (int);
	bool						setDataFormat (int);
	bool						setImagesPerFile (unsigned int);

	// methode pour renvoyer les erreurs rencontrees
	bool						sendError(const char* origine);

	bool						getLbMessage(std::string &message);
	bool						getRecordingInProgress() {return _bRecordingInProgress;}
	long long					getCurrentTimeStamp();
	unsigned int				getImagesPerFile();
    size_t                      getBufferIndex() {return _bufferIndex;}
    LadybugImage                _raw_LB_data[3];
    void                        bufferIndexRotate();

	//methode pour convertir les erreurs ladybug en chaine de caracterer de type LADYBUG_typeErreur
	std::string					convertLadybugError(LadybugError const error);

protected:
	LadybugContext				_context;
	LadybugStreamContext		_streamContext;
    std::string					iniFiles;

	bool						_bRecordingInProgress;
	bool						_bCameraStarted;
	bool						_firstImage;

	LadybugAutoShutterRange		_shutterRange;
	LadybugImage				_currentImage;
	LadybugImage				_prevImage;
	LadybugDataFormat			_ladybugDataFormat;
	LadybugStippledFormat		_ladybugStippledFormat;

	double						_totalDiskSpace;
	unsigned long				_numFrames;
	int							_numBuffers;
	int							_iFrameCount;
	size_t						_totalImg;
    size_t                      _bufferIndex;

	float						_shutterValue;
	float						_framerate;
	float						_gamma;
	float						_gain;
	float						_whiteBalance;

	unsigned int				_imagesPerFile;

	XError*						_error;
	LadybugError				_lbError;


};



#endif
