/*
 *      TU Berlin --- Fachgebiet Regelungssystem
 *      UDP handle of the NetworkInterface Simulink block.
 *
 *      Copyright © 2020 Markus Valtin <github@markus.valtin.net>. All rights reserved.
 *
 *      File:           UDPHandle.cpp -> Source file for the C++ UDP class.
 *      Version:        01 (2020)
 *      Changelog:
 *      	- 05.2020: initial release
 *
 *
 *      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 *      NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *      IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *      WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *      SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * 		You should have received a copy of the GNU General Public License along with this project.
 * 		If not, see <http://www.gnu.org/licenses/>.
 *
 * 		SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "UDPHandle.hpp"
//#define _UDP_HANDLE_DEBUG_ // un-comment for debug only

using boost::asio::ip::udp;

void displayAndChangeUDP(boost::thread& daThread);

/**
 * *  This class is used to create a socket for a given local port.
 *  A received package can be retrieved with a registered callback function.
 *  Data can be send with the sendData( uint8_t*, size_t) function.
 *
 *  @param local_port  Local port
 *  @param remote_port Remote port
 *  @param local_host  Local network interface
 *  @param remote_host Remote/Target interface
*/
UDPHandle::UDPHandle(int local_port, int remote_port, std::string local_host, std::string remote_host)
{
    this->local_port = local_port;
    this->remote_port = remote_port;

    this->remote_host = remote_host;
    this->local_host  = local_host;

    this->thread = NULL;
    this->threadInitialized = false;

    this->send_msg_length_ = 0;

    this->io_service = NULL;
    this->udp_socket = NULL;
    this->udp_endpoint = NULL;
    this->udp_endpoint_destination = NULL;
}

/**
 * This function opens a UDP server which is listening on the given port and interface.
 * Should be opened in an extra thread.
 */
void UDPHandle::openUDPServer()
{
    this->io_service = new boost::asio::io_service();
    this->udp_endpoint = new udp::endpoint(udp::v4(), this->local_port);

    this->udp_endpoint_destination = new udp::endpoint(
        boost::asio::ip::address::from_string(this->remote_host),
        this->remote_port);

    #ifdef _UDP_HANDLE_DEBUG_
        printf("Local  address:[%s:%d] \n", this->local_host.c_str(), this->local_port);
        printf("Remote address:[%s:%d] \n", this->remote_host.c_str(), this->remote_port);
    #endif

    try {
        #ifdef _UDP_HANDLE_DEBUG_
            printf("%s\n", "Configure Thread");
        #endif

        this->udp_socket = new udp::socket(*this->io_service, *this->udp_endpoint);

        this->sigSendData.connect(boost::bind(&UDPHandle::startSend, this));
        this->startReceive();

        this->threadInitialized = true;

        #ifdef _UDP_HANDLE_DEBUG_
            printf("%s\n", "notify_all waiting threads");
        #endif

        this->cond_started_thread.notify_all();

        this->io_service->run();
    } catch (...) {
        printf("Address already in USE! [%s:%d] \n", this->local_host.c_str(), this->local_port);
        this->threadInitialized = false;
    }

    this->cond_started_thread.notify_all();
}

/**
 * This function is called to trigger the current thread to wait
 * for new incoming packages.
 */
void UDPHandle::startReceive()
{
    if (this->udp_socket->is_open()){
        this->udp_socket->async_receive_from(
                boost::asio::buffer(this->recv_buffer_), *this->udp_endpoint,
                boost::bind(&UDPHandle::handleReceive, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    }
}

/**
 * Callback function which gets called if new data were written to the internal send_buffer_.
 */
void UDPHandle::startSend()
{
    if (!this->threadInitialized) {
        return;
    }

    if (this->udp_socket->is_open()){

        boost::system::error_code ignored_error;
        this->mutex_send_buffer_.lock();
        this->udp_socket->send_to(
            boost::asio::buffer(this->send_buffer_, sizeof(char)*this->send_msg_length_) ,
            *this->udp_endpoint_destination, 0, ignored_error
        );
        this->mutex_send_buffer_.unlock();

        if (ignored_error) {
            printf("StartSend:: Error occurred: %s \n", ignored_error.message().c_str());
        }
    }
}

/**
 * Public function called to write to the internal buffer.
 *
 * @param stream Data stream which should be sent.
 * @param msg_length Defines the amount of elements in the data stream
 */
void UDPHandle::sendData(const uint8_t* stream, std::size_t msg_length) {
    if (this->threadInitialized) {
        this->send_msg_length_ = msg_length;

        this->mutex_send_buffer_.lock();
        std::memcpy(this->send_buffer_.begin(), stream, sizeof(uint8_t)*msg_length);
        this->mutex_send_buffer_.unlock();
        this->sigSendData();
    }
}

/**
 * Callback function which is triggered by an incoming package.
 * Will also call the this->otherHandleRecieve if defined.
 */
void UDPHandle::handleReceive(const boost::system::error_code& error, std::size_t msg_length)
{
    if (!error){
        if(this->receiveCallback) {
            this->receiveCallback(this->recv_buffer_, msg_length);
        }

        this->startReceive();
    } else {
        printf("Error occurred: %s \n", error.message().c_str());
    }

    if (error == boost::asio::error::message_size) {
        printf("%s\n", "Message longer than buffer");
        this->startReceive();
    }
}

/**
 * Callback function which is triggered by a sent package.
 */
void UDPHandle::handleSend(const boost::system::error_code& error, std::size_t msg_length)
{
    if(error) {
        printf("HandleSend:: Error occurred: %s \n", error.message().c_str());
    }
}

/**
 * This function must be called to start the UDP server. Will also start an extra thread
 * which will handle all received packages.
*/
void UDPHandle::run() {
    #ifdef _UDP_HANDLE_DEBUG_
        printf("UDPHandle::run()\n");
    #endif

    if (!this->threadInitialized) {

        boost::unique_lock<boost::mutex> lock(this->mutex_starting_thread);

        this->thread = new boost::thread(boost::bind(&UDPHandle::openUDPServer, this));

        //TODO: Add root detection
        #if defined(BOOST_THREAD_PLATFORM_WIN32)
            // ... window version
        #elif defined(BOOST_THREAD_PLATFORM_PTHREAD)
            displayAndChangeUDP(*this->thread);
        #else
            #error "Boost threads unavailable on this platform"
        #endif

        if ( !this->threadInitialized ) {
            #ifdef _UDP_HANDLE_DEBUG_
                printf("Wait for thread to finish \n");
            #endif
            this->cond_started_thread.wait(lock);
        }
    }
}

/**
 * This function must be called to stop the UDP server and join the started thread.
 */
void UDPHandle::stop() {
    if (this->threadInitialized) {
        if (this->thread->joinable()) {
            this->io_service->stop();
            this->thread->join();
            this->udp_socket->close();
            this->threadInitialized = false;
        }
    }
}

//*************************************************
//**** Helper functions
//*************************************************

/*
    Was taken from here: http://stackoverflow.com/questions/1479945/setting-thread-priority-in-linux-with-boost
    Minor modification: Set FIFO priority to 'sched_get_priority_max(SCHED_FIFO)'
    Date: 18. Juli 2015

    @param daThread boost::thread which should be manipulated
    @param display Flag which can be set to 'ne 0' to display some thread information
*/
void displayAndChangeUDP(boost::thread& daThread)
{

    int retcode;
    int policy;

    pthread_t threadID = (pthread_t) daThread.native_handle();

    struct sched_param param;

    if ((retcode = pthread_getschedparam(threadID, &policy, &param)) != 0)
    {
        errno = retcode;
        perror("pthread_getschedparam");
        exit(EXIT_FAILURE);
    }

    #ifdef _UDP_HANDLE_DEBUG_
            std::cout << "INHERITED: ";
            std::cout << "policy=" << ((policy == SCHED_FIFO)  ? "SCHED_FIFO" :
                                       (policy == SCHED_RR)    ? "SCHED_RR" :
                                       (policy == SCHED_OTHER) ? "SCHED_OTHER" :
                                                                 "???")
                      << ", priority=" << param.sched_priority << std::endl;
    #endif

    policy = SCHED_FIFO;
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);

    if ((retcode = pthread_setschedparam(threadID, policy, &param)) != 0)
    {
        errno = retcode;
        perror("pthread_setschedparam");
        exit(EXIT_FAILURE);
    }

    #ifdef _UDP_HANDLE_DEBUG_
        std::cout << "  CHANGED: ";
        std::cout << "policy=" << ((policy == SCHED_FIFO)  ? "SCHED_FIFO" :
                                   (policy == SCHED_RR)    ? "SCHED_RR" :
                                   (policy == SCHED_OTHER) ? "SCHED_OTHER" :
                                                              "???")
                  << ", priority=" << param.sched_priority << std::endl;
    #endif
}
