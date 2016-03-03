#include "Logging.h"

#include <boost/thread/thread.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/atomic.hpp>

#include <iostream>
#include <cstdlib>
#include <signal.h>


// Sources, URLs:
// - http://www.boost.org/doc/libs/1_54_0/doc/html/lockfree/examples.html#lockfree.examples.waitfree_single_producer_single_consumer_queue
// - http://www.gnu.org/software/libc/manual/html_node/Sigaction-Function-Example.html
// - http://www.boost.org/doc/libs/1_54_0/doc/html/boost/lockfree/spsc_queue.html


//typedef int Image;
struct Image {
    void set_id(const uint& _id) { m_id = _id; }
    const uint& id() const { return m_id; }

    uint m_id;
    //char* ptr_img;
};


const uint capacity_queue = 5;
boost::lockfree::spsc_queue<Image, boost::lockfree::capacity<capacity_queue> > spsc_queue;

// Scénario (normalement) représentant les temps d'acquisition/écriture dans le Kangoo
uint time_transfert_usb3 = 250;
uint time_transfert_disk = 100;
// Scénario avec des temps d'acquisition inversés pour provoquer des overflows
//const uint time_transfert_usb3 = 10;
//const uint time_transfert_disk = 250;

struct Capture
{
    // Capture une image LB
    void take()
    {
        // TEMPOrisaton pour le transfert USB3->RAM
        boost::this_thread::sleep(boost::posix_time::milliseconds(time_transfert_usb3));
        // on assigne un ID à l'image (pour la répérer dans les logs)
        img.set_id(std::rand());
    }

    const Image& get() const { return img; }

    Image img;
};

struct WrapperLadyBug
{
    WrapperLadyBug() {
        INFO << "Constructeur";
    }

    const Capture& read() { 
        cur_capture.take();
        return cur_capture;
    }

    Capture cur_capture;
};
typedef boost::shared_ptr<WrapperLadyBug> WrapperLadyBugPtr;

// PRODUCER
void runLadyBug(WrapperLadyBugPtr sptr_wLB)
{
    // Production infinie ...
    while(true) {
        WrapperLadyBug& ref_wLB = *sptr_wLB;
        const Capture& value = ref_wLB.read();
        const Image& img = value.get();
        while (!spsc_queue.push(img))
                ;
        INFO << "[PRODUCER] Transfert USB->RAM Image n° " << img.id();
    }
}

boost::atomic<bool> done (false);

// Function d'écriture de l'image
// RAM -> DISK
bool write_img_to_disk(const Image& _img) 
{
    // temps pour écrire
    boost::this_thread::sleep(boost::posix_time::milliseconds(time_transfert_disk));
    INFO << "[CONSUMER] Transfert RAM->DISK Image n° " << _img.id();
    return true;
}

// CONSUMER
void runwriteLadyBug (WrapperLadyBugPtr sptr_wLB) 
{
    while(true) {
        Image img;

        while (!done) {
            while (spsc_queue.pop(img))
                write_img_to_disk(img);
        }

        while (spsc_queue.pop(img))
            write_img_to_disk(img);
    }
}

void
termination_handler (int signum)
{
  exit(1);
}

void setup_interruption_handling()
{
    struct sigaction new_action, old_action;

    /* Set up the structure to specify the new action. */
    new_action.sa_handler = termination_handler;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction (SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
    sigaction (SIGINT, &new_action, NULL);
    sigaction (SIGHUP, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
    sigaction (SIGHUP, &new_action, NULL);
    sigaction (SIGTERM, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
    sigaction (SIGTERM, &new_action, NULL);
}

// INSTANCIATIONS
// Wrapper LadyBug
WrapperLadyBug *m_Wrapper = new WrapperLadyBug();


int main(int argc, char* argv[])
{
    using namespace std;

    srand(std::time(0));

    cout << "boost::lockfree::queue is ";
    if (!spsc_queue.is_lock_free())
        cout << "not ";
    cout << "lockfree" << endl;

    boost::thread recordThreadLadyBug(runLadyBug, WrapperLadyBugPtr(m_Wrapper));
    boost::thread writeThreadLadyBug(runwriteLadyBug, WrapperLadyBugPtr(m_Wrapper));

    recordThreadLadyBug.detach();
    done = true;
    writeThreadLadyBug.detach();

    setup_interruption_handling();    

    // boucle infinie d'exécution
    while(true) {
    }
}