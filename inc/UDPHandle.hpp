/*
 *      TU Berlin --- Fachgebiet Regelungssystem
 *      UDP handle of the NetworkInterface Simulink block.
 *
 *      Copyright © 2020 Markus Valtin <github@markus.valtin.net>. All rights reserved.
 *
 *      File:           UDPHandle.hpp -> Header file for the C++ UDP class.
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

#ifndef _UDP_HANDLE_
#define _UDP_HANDLE_

#include <cstdio>

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/signals2.hpp>


class UDPHandle {
public:
    UDPHandle(int local_port, int remote_port, std::string local_host, std::string remote_host);

    void run();

    void sendData(const uint8_t*, std::size_t);

    void stop();

    //other handle called when defined
    boost::function<void(boost::array<uint8_t, 65500>, std::size_t)> receiveCallback;

private:
    //Thread
    boost::thread* thread;
    bool threadInitialized;

    //Buffer and buffer information
    std::size_t send_msg_length_;
    boost::array<uint8_t, 65500> recv_buffer_;
    boost::array<uint8_t, 65500> send_buffer_;
    boost::mutex mutex_send_buffer_;

    boost::mutex mutex_starting_thread;
    boost::condition_variable cond_started_thread;

    //Internal communication
    boost::signals2::signal<void ()> sigSendData;

    //Network stuff
    boost::asio::ip::udp::socket* udp_socket;
    boost::asio::ip::udp::endpoint* udp_endpoint;
    boost::asio::ip::udp::endpoint* udp_endpoint_destination;
    boost::asio::io_service* io_service;
    int local_port;
    int remote_port;
    std::string remote_host;
    std::string local_host;

    void openUDPServer();

    void startReceive();
    void handleReceive(const boost::system::error_code& error, std::size_t);

    void startSend();
    void handleSend(const boost::system::error_code& error, std::size_t);
};

#endif /* _UDP_HANDLE_ */
