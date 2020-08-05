/*
 *      TU Berlin --- Fachgebiet Regelungssystem
 *      TCP handle of the NetworkInterface Simulink block.
 *
 *      Copyright © 2020 Markus Valtin <github@markus.valtin.net>. All rights reserved.
 *
 *      File:           TCPHandle.cpp -> Source file for the C++ TCP class.
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

#include "TCPHandle.hpp"
//#define _TCP_HANDLE_DEBUG_ // uncomment for debug only

using boost::asio::ip::tcp;

void displayAndChangeTCP(boost::thread& daThread);



/***************************************************************************
 ***************************************************************************
 * Public Functions for TCPHandle
 ***************************************************************************
 **************************************************************************/

/**
 *  This class is used to create a socket for a given local port.
 *  A received package can be retrieved with a registered callback function.
 *  Data can be send with the sendData( uint8_t*, size_t) function.
 *
 *  @param local_port  Local port
 *  @param remote_port Remote port
 *  @param local_host  Local network interface
 *  @param remote_host Remote/Target interface
 */
TCPHandle::TCPHandle(int local_port, int remote_port, std::string local_host, std::string remote_host) {
	this->local_port = local_port;
	this->remote_port = remote_port;

	this->remote_host = remote_host;
	this->local_host  = local_host;

	this->thread = NULL;
	this->threadInitialized = false;

    this->io_service = NULL;
    this->tcp_acceptor = NULL;
    //this->tcp_connection = NULL;
    this->tcp_endpoint = NULL;
    this->tcp_endpoint_destination = NULL;
    this->isTCPConnected = false;
}

/**
 * This function must be called to set a callback function to deal with a lost TCP connection.
 */
void TCPHandle::setConnectionLostCallback(boost::function<void(void)> connectionLostCallback)
{
	this->connectionLostCallback = connectionLostCallback;
}

/**
 *  This function must be called to start the TCP server and set the package received callback function.
 *  Will also start an extra thread which will handle all received packages.
 */
void TCPHandle::run(boost::function<void(boost::array<uint8_t, 65500>, std::size_t)> receiveCallback)
{
	this->receiveCallback = receiveCallback;
	this->run();
}

/**
 *  This function must be called to start the TCP server. Will also start an extra thread
 *  which will handle all received packages.
 */
void TCPHandle::run() {
    #ifdef _TCP_HANDLE_DEBUG_
        printf("TCPHandle::run()\n");
    #endif

    if (!this->threadInitialized) {

        boost::unique_lock<boost::mutex> lock(this->mutex_starting_thread);

        this->thread = new boost::thread(boost::bind(&TCPHandle::openTCPServer, this));

        //TODO: Add root detection
        #if defined(BOOST_THREAD_PLATFORM_WIN32)
            // ... window version
        #elif defined(BOOST_THREAD_PLATFORM_PTHREAD)
            displayAndChangeTCP(*this->thread);
        #else
            #error "Boost threads unavailable on this platform"
        #endif

        if ( !this->threadInitialized ) {
            #ifdef _TCP_HANDLE_DEBUG_
                printf("Wait for thread to finish \n");
            #endif
            this->cond_started_thread.wait(lock);
        }
    }
}

/**
 * This function returns, if a TCP connection is currently active or not.
 */
bool TCPHandle::isConnected(void)
{
	return this->isTCPConnected;
}

/**
 * Public function called to write to the internal buffer.
 *
 * @param stream Data stream which should be sent.
 * @param msg_length Defines the amount of elements in the data stream
 */
void TCPHandle::sendData(const uint8_t* stream, std::size_t msg_length)
{
    if (this->threadInitialized) {
    	if (this->isTCPConnected){
    		this->tcp_connection->sendData(stream, msg_length);
    	}
    }
}

/**
 * This function must be called to stop the TCP server and join the started thread.
 */
void TCPHandle::stop()
{
    if (this->threadInitialized) {
        if (this->thread->joinable()) {
        	this->tcp_acceptor->close();
            this->io_service->stop();
            this->thread->join();
            this->threadInitialized = false;
        }
    }
}



/***************************************************************************
 ***************************************************************************
 * Private Functions for TCPHandle
 ***************************************************************************
 **************************************************************************/

/**
 *  This function opens a TCP server which is listening on the given port and interface.
 *  Should be opened in an extra thread.
 */
void TCPHandle::openTCPServer() {

#ifdef _TCP_HANDLE_DEBUG_
    printf("Local  address:[%s:%d] \n", this->local_host.c_str(), this->local_port);
    printf("Remote address:[%s:%d] \n", this->remote_host.c_str(), this->remote_port);
#endif

    this->io_service = new boost::asio::io_service();
    this->tcp_endpoint = new tcp::endpoint(tcp::v4(), this->local_port);

    this->tcp_endpoint_destination = new tcp::endpoint(
        boost::asio::ip::address::from_string(this->remote_host),
        this->remote_port);

    try {
        #ifdef _TCP_HANDLE_DEBUG_
            printf("%s\n", "Configure Thread");
        #endif

        this->tcp_acceptor = new tcp::acceptor(*this->io_service, *this->tcp_endpoint);

        this->startAcceptor();

        this->threadInitialized = true;

        #ifdef _TCP_HANDLE_DEBUG_
            printf("%s\n", "notify_all waiting threads");
        #endif

        this->cond_started_thread.notify_all();

        this->io_service->run();
    } catch (...) {
        printf("Address already in USE! [%s:%d] \n", this->local_host.c_str(), this->local_port);
        this->threadInitialized = false;
        this->isTCPConnected = false;
    }

    this->cond_started_thread.notify_all();
}

/**
 *  This function is called to trigger the current thread to wait
 *  for new incoming connection.
 */
void TCPHandle::startAcceptor()
{
	TCPConnection::pointer newConnection =
			TCPConnection::create(this->tcp_acceptor->get_io_service());

	this->tcp_acceptor->async_accept(newConnection->socket(),
			boost::bind(&TCPHandle::handleAccept, this, newConnection,
						boost::asio::placeholders::error));
}

/**
 *  Callback function which is triggered by an incoming connection.
 *  Will also call the this->otherHandleRecieve if defined.
 */
void TCPHandle::handleAccept(TCPConnection::pointer newConnection, const boost::system::error_code& error)
{
	if (!error)	{
		this->isTCPConnected = true;
		this->tcp_connection = newConnection;
		this->tcp_connection->connectionLostCallback = boost::bind(&TCPHandle::restartAcceptor, this);
		this->tcp_connection->receiveCallback = this->receiveCallback;
		this->tcp_connection->startReceive();
	} else {
        std::cout << std::endl <<
        		"NetworkInterface TCP accept ERROR: "<< error.message() << "!" << std::endl << std::endl;
	}
}

/**
 * This function is called, if the TCP connection is lost and restarts the acceptor.
 */
void TCPHandle::restartAcceptor()
{
	// reset the shared point -> deletes the TCPConnection object
	this->tcp_connection.reset();
	this->isTCPConnected = false;
	this->startAcceptor();

	if (this->connectionLostCallback){
		this->connectionLostCallback();
	}
}





/***************************************************************************
 ***************************************************************************
 * Public Functions for TCPConnection
 ***************************************************************************
 **************************************************************************/

/*
 * This function must be called to create a new object.
 */
TCPConnection::pointer TCPConnection::create(boost::asio::io_service& io_service)
{
	return pointer(new TCPConnection(io_service));
}

/*
 * This function must be called to retrieve the TCP socket.
 */
tcp::socket& TCPConnection::socket()
{
	return this->tcp_socket;
}

/**
 * This function must be called to start listening to packages to be received.
 */
void TCPConnection::startReceive()
{
	if (this->tcp_socket.is_open()){
		this->tcp_socket.async_receive(
				boost::asio::buffer(this->recv_buffer_),
                boost::bind(&TCPConnection::handleReceive, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    }
}

/**
 * This function must be called to write the data to be send to the internal buffer.
 *
 *  @param stream Data stream which should be sent.
 *  @param msg_length Defines the amount of elements in the data stream
 */
void TCPConnection::sendData(const uint8_t* stream, std::size_t msg_length)
{
	this->send_msg_length_ = msg_length;

	this->mutex_send_buffer_.lock();
	std::memcpy(this->send_buffer_.begin(), stream, sizeof(uint8_t)*msg_length);
	this->mutex_send_buffer_.unlock();
	this->sigSendData();
}



/***************************************************************************
 ***************************************************************************
 * Public Functions for TCPConnection
 ***************************************************************************
 **************************************************************************/

/*
 * Private class constructor.
 */
TCPConnection::TCPConnection(boost::asio::io_service& io_service)
: tcp_socket(io_service)
{
	this->send_msg_length_ = 0;

	this->sigSendData.connect(boost::bind(&TCPConnection::startSend, this));
	this->receiveCallback = NULL;
}

/**
 * Callback function which gets called if new data were written to the internal send_buffer_.
 */
void TCPConnection::startSend()
{
    if (this->tcp_socket.is_open()){

        this->mutex_send_buffer_.lock();
    	boost::asio::async_write(
    			this->tcp_socket, boost::asio::buffer(this->send_buffer_, sizeof(char)*this->send_msg_length_),
    			boost::bind(&TCPConnection::handleSend, shared_from_this(),
    					boost::asio::placeholders::error,
    					boost::asio::placeholders::bytes_transferred));

        this->mutex_send_buffer_.unlock();
    }
}

/**
 * Callback function which is triggered by a sent package.
 */
void TCPConnection::handleSend(const boost::system::error_code& error, std::size_t msg_length)
{
	if (!error){
		this->send_msg_length_ = 0;
	} else {
		if (error == boost::asio::error::eof ||
				boost::asio::error::operation_aborted) {
			this->connectionLost();
			return;
		}

		std::cout << std::endl <<
				"NetworkInterface TCP send ERROR: "<< error.message() << "!" << std::endl << std::endl;
	}
}

/**
 * Callback function which is triggered by an incoming connection.
 * Will also call the this->receiveCallback, if defined.
 */
void TCPConnection::handleReceive(const boost::system::error_code& error, std::size_t msg_length)
{
    if (!error)
    {
        if(this->receiveCallback) {
            this->receiveCallback(this->recv_buffer_, msg_length);
        }

        this->startReceive();
    } else {
        if (error == boost::asio::error::eof ||
        	boost::asio::error::operation_aborted) {
        	this->connectionLost();
        	return;
        }

        if (error == boost::asio::error::message_size) {
            printf("%s\n", "Message longer than buffer");
            std::cout << std::endl <<
            		"NetworkInterface TCP ERROR: The received message is longer than the buffer!" << std::endl << std::endl;
            this->startReceive();
            return;
        }

        std::cout << std::endl <<
        		"NetworkInterface TCP receive ERROR: "<< error.message() << "!" << std::endl << std::endl;
    }
}

/*
 * This function is called, if the TCP connection was lost or closed and resets the TCPHandle.
 */
void TCPConnection::connectionLost(void)
{
	this->tcp_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	this->tcp_socket.close();
	if (this->connectionLostCallback){
		this->connectionLostCallback();
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
void displayAndChangeTCP(boost::thread& daThread)
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

    #ifdef _TCP_HANDLE_DEBUG_
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

    #ifdef _TCP_HANDLE_DEBUG_
        std::cout << "  CHANGED: ";
        std::cout << "policy=" << ((policy == SCHED_FIFO)  ? "SCHED_FIFO" :
                                   (policy == SCHED_RR)    ? "SCHED_RR" :
                                   (policy == SCHED_OTHER) ? "SCHED_OTHER" :
                                                              "???")
                  << ", priority=" << param.sched_priority << std::endl;
    #endif
}
