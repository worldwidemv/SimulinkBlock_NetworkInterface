/*
 *      TU Berlin --- Fachgebiet Regelungssystem
 *      TCP handle of the NetworkInterface Simulink block.
 *
 *      Copyright © 2020 Markus Valtin <github@markus.valtin.net>. All rights reserved.
 *
 *      File:           TCPHandle.hpp -> Header file for the C++ TCP class.
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

#ifndef _TCP_HANDLE_
#define _TCP_HANDLE_

#include <cstdio>

//#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/signals2.hpp>



class TCPConnection
  : public boost::enable_shared_from_this<TCPConnection>
{
public:
  typedef boost::shared_ptr<TCPConnection> pointer;

  static pointer create(boost::asio::io_service& io_service);
  boost::asio::ip::tcp::socket& socket();

  void startReceive();
  void sendData(const uint8_t*, std::size_t);

  //Function callback's
  boost::function<void(boost::array<uint8_t, 65500>, std::size_t)> receiveCallback;
  boost::function<void(void)> connectionLostCallback;

private:
  TCPConnection(boost::asio::io_service& io_service);

  void startSend();
  void handleSend(const boost::system::error_code& error, std::size_t);
  void handleReceive(const boost::system::error_code& error, std::size_t msg_length);
  void connectionLost(void);

  //Buffer and buffer information
  std::size_t send_msg_length_;
  boost::array<uint8_t, 65500> recv_buffer_;
  boost::array<uint8_t, 65500> send_buffer_;
  boost::mutex mutex_send_buffer_;

  //Internal communication
  boost::signals2::signal<void ()> sigSendData;

  // TCP variables
  boost::asio::ip::tcp::socket tcp_socket;
};


class TCPHandle {
public:
    TCPHandle(int local_port, int remote_port, std::string local_host, std::string remote_host);

    void setConnectionLostCallback(boost::function<void(void)> connectionLostCallback);

    void run();
    void run(boost::function<void(boost::array<uint8_t, 65500>, std::size_t)> receiveCallback);

    bool isConnected(void);

    void sendData(const uint8_t*, std::size_t);

    void stop();

private:
    //Thread
    boost::thread* thread;
    bool threadInitialized;


    boost::mutex mutex_starting_thread;
    boost::condition_variable cond_started_thread;

    //Network stuff
    bool isTCPConnected;
    boost::asio::io_service* io_service;
    boost::asio::ip::tcp::acceptor* tcp_acceptor;
    TCPConnection::pointer tcp_connection;
    boost::asio::ip::tcp::endpoint* tcp_endpoint;
    boost::asio::ip::tcp::endpoint* tcp_endpoint_destination;
    int local_port;
    int remote_port;
    std::string remote_host;
    std::string local_host;

    //Function callback's
    boost::function<void(boost::array<uint8_t, 65500>, std::size_t)> receiveCallback;
    boost::function<void(void)> connectionLostCallback;

    void openTCPServer();

    void startAcceptor();
    void handleAccept(TCPConnection::pointer newConnection, const boost::system::error_code& error);
    void restartAcceptor();
};

#endif /* _TCP_HANDLE_ */
