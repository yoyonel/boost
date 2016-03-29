#include "Logging.h"

#include <boost/atomic.hpp>
<<<<<<< HEAD
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/thread/thread.hpp>
#include <cstdlib>
#include <iostream>

typedef int Image;

// Sources, URLs:
// -
// http://www.boost.org/doc/libs/1_54_0/doc/html/lockfree/examples.html#lockfree.examples.waitfree_single_producer_single_consumer_queue

// Note: ce n'est pas ce que souhaite faire Julien !
// Dans ce pattern, le consommateur (ou l'action de consommer) bloque la
// production

=======

#include <iostream>
#include <cstdlib>
#include <signal.h>

// Sources, URLs:
// http://www.boost.org/doc/libs/1_54_0/doc/html/lockfree/examples.html#lockfree.examples.waitfree_single_producer_single_consumer_queue
// http://www.gnu.org/software/libc/manual/html_node/Sigaction-Function-Example.html
// http://www.boost.org/doc/libs/1_54_0/doc/html/boost/lockfree/spsc_queue_captures.html
// https://msdn.microsoft.com/fr-fr/library/1z2f6c2k.aspx

// PARAMETRES pour le test
// Scénario (normalement) représentant les temps d'acquisition/écriture dans le
// Kangoo
>>>>>>> 019798e9ce24abd60defc42b8eb918325376dc2b
uint time_transfert_usb3 = 250;
uint time_transfert_disk = 100;
// Scénario avec des temps d'acquisition inversés pour provoquer des overflows
// const uint time_transfert_usb3 = 10;
// const uint time_transfert_disk = 250;

// Structure simple pour "stocker" une image
// C'est un prototype donc il n'y a pas de réservation mémoire
// Juste une gestion d'id pour chaque image
struct Image {
  // CONSTRUCTEUR
  Image() {}

  // SETTER
  void set_id(const uint &_id) { m_id = _id; }
  // GETTER
  const uint &id() const { return m_id; }

  friend std::ostream &operator<<(std::ostream &os, const Image &obj);

  // MEMBRES
  // id de l'image
  uint m_id;
  // char* ptr_img;
};

<<<<<<< HEAD
struct Capture {
  // Capture une image LB
  void take() {
    // 50 ms pour prendre une image
    boost::this_thread::sleep(boost::posix_time::milliseconds(76));
    //
    img = std::rand();
  }

  const Image& get() const { return img; }
=======
std::ostream &operator<<(std::ostream &os, const Image &obj) {
  // write obj to stream
  os << obj.m_id;
  return os;
}

struct Capture {
  // Capture une image LB
  // Non CONST, car on est censé stocker en RAM le transfert (from USB3)
  void take() {
    // TEMPOrisaton pour le transfert USB3->RAM
    boost::this_thread::sleep(
        boost::posix_time::milliseconds(time_transfert_usb3));
    // on assigne un ID à l'image (pour la répérer dans les logs)
    img.set_id(std::rand());
  }

  // GETTER
  const Image &get() const { return img; }

  friend std::ostream &operator<<(std::ostream &os, const Capture &obj);
>>>>>>> 019798e9ce24abd60defc42b8eb918325376dc2b

  Image img;
};

<<<<<<< HEAD
struct WrapperLadyBug {
  WrapperLadyBug() { INFO << "Constructeur"; }

  const Capture& read() {
    cur_capture.take();
    return cur_capture;
  }

  Capture cur_capture;
=======
std::ostream &operator<<(std::ostream &os, const Capture &obj) {
  // write obj to stream
  os << obj.img;
  return os;
}

struct WrapperLadyBug {
  // Constructeur
  // on peut initialiser l'instance de capture ici.
  WrapperLadyBug() { INFO << "Constructeur"; }

  // Interface du Wrapper pour récupérer une capture: USB3 -> RAM
  // On renvoie une ref_cst de capture
  const Capture &read(bool _debug = true) {
    // On lance le transfert USB3 -> RAM
    m_capture.take();
    if (_debug) {
      INFO << "->[PRODUCER] Transfert USB->RAM Image n° " << m_capture;
    }
    // On renvoie la capture
    return m_capture;
  }

  // Une instance de capture
  // car on ne capture qu'une capture à la fois
  // (=> pas de tampon mémoire coté LadyBug)
  Capture m_capture;
>>>>>>> 019798e9ce24abd60defc42b8eb918325376dc2b
};
// typedef pour simplifier l'écriture
typedef boost::shared_ptr<WrapperLadyBug> WrapperLadyBugPtr;

// QUEUE thread-safe pour le stockage des captures
const uint capacity_queue = 5;
boost::lockfree::spsc_queue<Capture, boost::lockfree::capacity<capacity_queue>>
    spsc_queue_captures;
    
// PRODUCER
void runLadyBug(WrapperLadyBugPtr sptr_wLB) {
  // Production infinie ...
  while (true) {
<<<<<<< HEAD
    WrapperLadyBug& ref_wLB = *sptr_wLB;
    const Capture& value = ref_wLB.read();
    const Image& img = value.get();
    while (!spsc_queue.push(img))
      ;
    INFO << "[PRODUCER] Transfert USB->RAM Image n° " << img;
=======
    WrapperLadyBug &ref_wLB = *sptr_wLB;
    const Capture &capture = ref_wLB.read();
    while (!spsc_queue_captures.push(capture))
      ;
    INFO << "\t->[PRODUCER] Transfert (USB->)RAM->RAM' Image n° " << capture;
>>>>>>> 019798e9ce24abd60defc42b8eb918325376dc2b
  }
}

boost::atomic<bool> done(false);
<<<<<<< HEAD

// CONSUMER
void runwriteLadyBug(WrapperLadyBugPtr sptr_wLB) {
  while (true) {
    Image value;

    // while (!done) {
    //     while (spsc_queue.pop(value)) {
    //         // temps pour écrire
    //         boost::this_thread::sleep(boost::posix_time::milliseconds(75));
    //         //
    //         INFO << "[CONSUMER] Transfert RAM->DISK Image n° " << value;
    //     }
    // }
    while (spsc_queue.pop(value)) {
      // temps pour écrire
      boost::this_thread::sleep(boost::posix_time::milliseconds(75));
      INFO << "[CONSUMER] Transfert RAM->DISK Image n° " << value;
    }
  }
=======

// Function d'écriture de l'image
// RAM -> DISK
// Ecriture de l'image
bool write_to_disk(const Image &_img) {
  // temps pour écrire
  boost::this_thread::sleep(
      boost::posix_time::milliseconds(time_transfert_disk));
  INFO << "<-[CONSUMER] Transfert RAM->DISK Image n° " << _img;
  return true;
}
// Ecrire de la capture
bool write_to_disk(const Capture &_capture) {
  return write_to_disk(_capture.get());
}

// CONSUMER
void runwriteLadyBug(WrapperLadyBugPtr sptr_wLB) {
  Capture capture;

  while (true) {
    while (!done) {
      while (spsc_queue_captures.pop(capture)) write_to_disk(capture);
    }

    while (spsc_queue_captures.pop(capture)) write_to_disk(capture);
  }
}

void termination_handler(int signum) { exit(1); }

void setup_interruption_handling() {
  struct sigaction new_action, old_action;

  /* Set up the structure to specify the new action. */
  new_action.sa_handler = termination_handler;
  sigemptyset(&new_action.sa_mask);
  new_action.sa_flags = 0;

  sigaction(SIGINT, NULL, &old_action);
  if (old_action.sa_handler != SIG_IGN) sigaction(SIGINT, &new_action, NULL);
  sigaction(SIGHUP, NULL, &old_action);
  if (old_action.sa_handler != SIG_IGN) sigaction(SIGHUP, &new_action, NULL);
  sigaction(SIGTERM, NULL, &old_action);
  if (old_action.sa_handler != SIG_IGN) sigaction(SIGTERM, &new_action, NULL);
>>>>>>> 019798e9ce24abd60defc42b8eb918325376dc2b
}

// INSTANCIATIONS
// Wrapper LadyBug
WrapperLadyBug* m_Wrapper = new WrapperLadyBug();

<<<<<<< HEAD
int main(int argc, char* argv[]) {
  using namespace std;

  srand(std::time(0));

  cout << "boost::lockfree::queue is ";
  if (!spsc_queue.is_lock_free()) cout << "not ";
  cout << "lockfree" << endl;

  boost::thread recordThreadLadyBug(runLadyBug, WrapperLadyBugPtr(m_Wrapper));
  boost::thread writeThreadLadyBug(runwriteLadyBug,
                                   WrapperLadyBugPtr(m_Wrapper));

  recordThreadLadyBug.detach();
  done = true;
  writeThreadLadyBug.detach();

  while (true) {
  }
}
=======
int main(int argc, char *argv[]) {
  using namespace std;

  srand(std::time(0));

  cout << "boost::lockfree::queue is ";
  if (!spsc_queue_captures.is_lock_free()) cout << "not ";
  cout << "lockfree" << endl;

  boost::thread recordThreadLadyBug(runLadyBug, WrapperLadyBugPtr(m_Wrapper));
  boost::thread writeThreadLadyBug(runwriteLadyBug,
                                   WrapperLadyBugPtr(m_Wrapper));

  recordThreadLadyBug.detach();
  done = true;
  writeThreadLadyBug.detach();

  setup_interruption_handling();

  // boucle infinie d'exécution
  while (true) {
    }
}   
>>>>>>> 019798e9ce24abd60defc42b8eb918325376dc2b
