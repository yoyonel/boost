#ifndef _E_ARCHI_LADYBUG_SENSOR_H_
#define _E_ARCHI_LADYBUG_SENSOR_H_
/*!
 * \file EArchiLadyBugSensor.h
 * \brief Classe de gestion de la Ladybug
 */
#include "libXStereopolis/XArchiLadyBugSensor.h"

#include <boost/thread.hpp>
#include <boost/filesystem/operations.hpp>

#include "Ladybug/WrapperLadyBug/include/WrapperLadyBug.h"

	/**
	 * \struct EArchiLadyBugSensorImageSize_t
	 * \brief Structure permettant de stocker la taille d'une image d'une camera de la Ladybug
	 */
typedef struct
{
	unsigned int w;
	unsigned int h;
}EArchiLadyBugSensorImageSize_t;
/**
 *	\class EArchiLadyBugSensor
 *	\brief Classe permettant gere la Ladybug : initialisation, enregistrement des images, demarrage/arret de l'acquisition
 *	\brief herite de la classe XArchiLadyBugSensor
 */
class EArchiLadyBugSensor : public XArchiLadyBugSensor
{
protected:
	bool m_bConnected;/**< Information de connexion de la Ladybug */
	__int64 m_dLastTimeStamp;/**< datation du dernier declenchement */

	WrapperLadyBug *m_Wrapper;/**< Wrapper Ladybug */	
    boost::thread recordThreadLadyBug;/**< Thread de recuperation des images de la Ladybug por l'USB3 */
    boost::thread writeThreadLadyBug;/**< Thread d'ecriture des images de la Ladybug sur le disque */
	static std::string _config; /**< Configuration de la Ladybug pendant l'acquisition */

public:
	static size_t _nbDeclLadybug;/**< Nombre de declenchement dont les images ont ete enregistrees par la Ladybug */
	static bool _start;
    static bool _isWriting;
    static bool _hasSomethingtoWrite;
    static int _write_index;
    static int _buffer_index;
	/**
	 * \brief Constructeur de la classe EArchiLadyBugSensor
	 * \param PcUnit : les différents PC sur lesquels sont branchés les instruments (plusieurs pour Stereopolis, 1 pour kangoo)
	 * \param error : gestion des remontee d'erreur specifique XError
	 */
	static boost::recursive_mutex _mutexDeclLadybug;

	EArchiLadyBugSensor(XArchiPcUnit* PcUnit, XError* error);
	/**
	 * \brief Destructeur de la classe EArchiLadyBugSensor
	 */
	~EArchiLadyBugSensor();
	/**
	 * \brief Getter de l'information de connexion de la Ladybug
	 */
	bool Connected(){return m_bConnected;}
	/**
	 * \brief Methode d'initialisation de la Ladybug et initialiation du thread d'enregistrement des images
	 */
	bool Initialize();
	/**
	 * \brief Methode de chargement de la configuration par defaut de la Ladybug pour initialisation de la tete panoramique
	 */
	void LoadDefaultParametersConfiguration();
	/**
	 * \brief Getter de _config
	 */
	void GetLadybugConfig(std::string &message){message = _config;}
	/**
	 * \brief Methode de demarrage/arret de l'acquisition de la LadyBug
	 */
	void StartStop(bool state);

/**
	 * \brief Methode de gestion des erreurs
	 */	void ThrowErrorMessage(const std::string &origine,const std::string &error);

	static size_t nbDeclLadyBug;// a supprimer //MJ
    bool GetCurrentLbImages(std::vector<unsigned char*> &redImages, size_t &nColsOut, size_t &nLinesOut);
	bool GetLbMessage(std::string &messages);



	//Error
};


#endif

