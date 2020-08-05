/*
 *      TU Berlin --- Fachgebiet Regelungssystem
 *      NetworkInterface as a Simulink block.
 *
 *      Copyright © 2020 Markus Valtin <github@markus.valtin.net>. All rights reserved.
 *
 *      File:           NetworkInterface_01.hpp -> Header file for the C++ Simulink NetworkInterface class.
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

#ifndef _NETWORK_INTERFACE_01_
#define _NETWORK_INTERFACE_01_

#include <string>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <algorithm>
#include <signal.h>
#include <stdlib.h>
#include <boost/array.hpp>

#include "UDPHandle.hpp"
#include "TCPHandle.hpp"


#define NET_INTERFACE__MAX_NUMBER_OF_IOs		2000
#define NET_INTERFACE__MAX_OUTPUT_STREAM_SIZE	(NET_INTERFACE__MAX_NUMBER_OF_IOs *sizeof(double))


class NetworkInterface {
public:
    NetworkInterface(
    	uint32_t numberOfInputs, uint32_t numberOfOutputs, // Inputs / Outputs
		uint32_t numberOfBlockOptions, uint32_t blockOptiones[],
		uint32_t numberOfInitValues, double initialOutputValues[],
		int useUDP, int startConnection, uint32_t localPort, signed char remoteIP[], uint32_t remotePort
    );
    ~NetworkInterface();

    void updateServerState(double controlInput);
    void sendInput(const double u1[], double time);
    void setOutput(double y1[], double* y2_OutputCounter);

private:
    // Variables with information about the connection
    uint32_t sizeInputVector;
    uint32_t sizeOutputVector;
    bool isInputZero;
    bool isOutputZero;

    // SessionIDs for simple validity checks
    double   inputSessionID;
    uint16_t outputSessionID;
    double   inputCounter;
    uint32_t outputCounter;

    // Output vector buffer is used by the external thread to store the last received parameter changes
    double outputBuffer[NET_INTERFACE__MAX_NUMBER_OF_IOs];
    double outputBufferInitialValue[NET_INTERFACE__MAX_NUMBER_OF_IOs];

    bool setupSuccessful;
    bool setupErrorMsgPrinted;
    boost::mutex mutex_thread_data_update;

    // Network functions/variables
    bool useUDP;
    bool startNetworkConnection;
    bool networkServerStarted;

    uint32_t local_port;
    uint32_t remote_port;
    std::string local_ip;
    std::string remote_ip;

    uint8_t outputDataStream[NET_INTERFACE__MAX_OUTPUT_STREAM_SIZE];

    UDPHandle* udpHandle;
    TCPHandle* tcpHandle;

    void createNetworkServer();
    void startNetworkServer();
    void stopNetworkServer();
    void receiveCallback(boost::array<uint8_t, 65500> buffer, std::size_t bufferSizsee);

    void printSetupError();
};


// Method wrappers

// sfunction parameter
// numberOfInputs, numberOfOutputs, initialOutputValues, useUDP, startConnection, localPort, remoteIP, remotePort, sampleTime
extern void lctNetworkInterface_Initialise (
		void **work1, //Working vector
		uint32_t numberOfInputs, uint32_t numberOfOutputs, // Inputs / Outputs
		uint32_t numberOfBlockOptions, uint32_t blockOptiones[],
		uint32_t numberOfInitValues, double initialOutputValues[],
		int useUDP, int startConnection, uint32_t localPort, signed char remoteIP[], uint32_t remotePort
);

extern void lctNetworkInterface_InputOutput(
	void **work1, double u1_Control, double u2_Input[], double u3_Time, double y1_Output[], double* y2_OutputCounter
);

extern void lctNetworkInterface_Deinitialise(void **work1);

#endif /* _NETWORK_INTERFACE_ */
