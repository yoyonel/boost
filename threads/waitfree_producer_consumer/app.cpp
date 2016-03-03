#include "Logging.h"

#include <boost/thread/thread.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/atomic.hpp>
#include <iostream>
#include <cstdlib>

typedef int Image;


// Sources, URLs:
// - http://www.boost.org/doc/libs/1_54_0/doc/html/lockfree/examples.html#lockfree.examples.waitfree_single_producer_single_consumer_queue

// Note: ce n'est pas ce que souhaite faire Julien !
// Dans ce pattern, le consommateur (ou l'action de consommer) bloque la production

uint time_transfert_usb3 = 250;
uint time_transfert_disk = 100;

struct Capture
{
    // Capture une image LB
    void take()
    {
        // 50 ms pour prendre une image
        boost::this_thread::sleep(boost::posix_time::milliseconds(76));
        // 
        img = std::rand();
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

boost::lockfree::spsc_queue<Image, boost::lockfree::capacity<1024> > spsc_queue;

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
        INFO << "[PRODUCER] Transfert USB->RAM Image n° " << img;
    }
}

boost::atomic<bool> done (false);

// CONSUMER
void runwriteLadyBug (WrapperLadyBugPtr sptr_wLB) 
{
    while(true) {
        Image value;

        /**
        while (!done) {
            while (spsc_queue.pop(value)) {
                // temps pour écrire
                boost::this_thread::sleep(boost::posix_time::milliseconds(75));
                //
                INFO << "[CONSUMER] Transfert RAM->DISK Image n° " << value;
            }
        }
        /**/
        while (spsc_queue.pop(value)) {
            // temps pour écrire
            boost::this_thread::sleep(boost::posix_time::milliseconds(75));
            INFO << "[CONSUMER] Transfert RAM->DISK Image n° " << value;
        }
    }
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

    while(true) {
    }
}