#include "Logging.h"

#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// Sources URL
// - http://ashishgrover.com/boost-multi-threadingfor-c/
// - http://www.boost.org/doc/libs/1_46_0/doc/html/thread/thread_management.html
// - http://www.boost.org/doc/libs/1_54_0/doc/html/thread/thread_management.html
// - http://www.boost.org/doc/libs/1_60_0/libs/smart_ptr/shared_ptr.htm
// - http://www.boost.org/doc/libs/1_60_0/libs/smart_ptr/example/shared_ptr_example.cpp
// - http://boost.2283326.n4.nabble.com/how-to-sleep-a-thread-for-specified-milliseconds-and-or-nanoseconds-td2576885.html

struct WrapperLadyBug
{
	WrapperLadyBug() : _compteur(0) {
		INFO << "Constructeur";
	}

	void read() { inc_compteur(); }

	void inc_compteur() { 
		// MUTEX
		// scoped_lock permet de locker le mutex dans le scope courant
		// cad celui de `inc_compteur`.
		// A la sortie de cette méthode, le mutex est unlock (car on sort du scope)
		boost::mutex::scoped_lock lock(_writerMutex);
		// MAJ du compteur
		_compteur = (_compteur + 1) % 3; 
	}

	const uint & compteur() const { return _compteur; }

	uint _compteur;

	// utilisation d'un mutex pour protéger l'accès au compteur
	// pendant l'écriture (inc_compteur)
	static boost::mutex _writerMutex;
};
typedef boost::shared_ptr<WrapperLadyBug> WrapperLadyBugPtr;

// THREADS
boost::thread recordThreadLadyBug;
boost::thread writeThreadLadyBug;

// METHODS utilisées par les THREADS
void runLadyBug (WrapperLadyBugPtr	sptr_wLB) 
les{
	// Récupération d'une référence de l'objet contenu par le shared_ptr
	WrapperLadyBug& ref_wLB = *sptr_wLB;

	// Boucle d'exécution
	while(true) 
	{
		// lit et incrémente le compteur
		ref_wLB.read();	
		// DEBUG INFO
		INFO << "[runLadyBug] Valeur du compteur: " << ref_wLB.compteur();
		// tempo: 0.123s
		boost::this_thread::sleep(boost::posix_time::milliseconds(123));
	};
}

void runwriteLadyBug (WrapperLadyBugPtr	sptr_wLB) 
{
	// Récupération d'une référence de l'objet contenu par le shared_ptr
	WrapperLadyBug& ref_wLB = *sptr_wLB;

	// Boucle d'exécution
	while(true)
	{
		// récupération du compteur
		const uint compteur = ref_wLB.compteur();
		// DEBUG INFO
		INFO << "[runwriteLadyBug] Valeur du compteur: " << compteur;
		// tempo: 0.166s
		boost::this_thread::sleep(boost::posix_time::milliseconds(166));
	}
}

// INSTANCIATIONS
// Wrapper LadyBug
WrapperLadyBug *m_Wrapper = new WrapperLadyBug();
// Mutex lié au Wrapper
boost::mutex WrapperLadyBug::_writerMutex;


// MAIN
int main(int argc, char **argv)
{
	// LANCEMENT des THREADS
	// avec transmission d'un shareptr sur le WRAPPER LadyBug
	//
	// THREAD de mise à jour du compteur (aka transfert USB->RAM LB)
	recordThreadLadyBug = boost::thread(
		runLadyBug,
		//boost::shared_ptr<WrapperLadyBug>(m_Wrapper)
		WrapperLadyBugPtr(m_Wrapper)
		);
    recordThreadLadyBug.detach();
    //
    // THREAD de transfert de l'image sur disque (aka transfert RAM->DISK)
    writeThreadLadyBug = boost::thread(
		runwriteLadyBug,
		//boost::shared_ptr<WrapperLadyBug>(m_Wrapper)
		WrapperLadyBugPtr(m_Wrapper)
		);
    writeThreadLadyBug.detach();
	
    // Boucle d'exécution
	while(true)
	{
		// INFO DEBUG sur la valeur du compteur (global)
		INFO << "[main] Valeur du compteur: " << m_Wrapper->compteur();
		// tempo d'1s.
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	};

    return 0;
}
