/*
 *      TU Berlin --- Fachgebiet Regelungssystem
 *      NetworkInterface as a Simulink block.
 *
 *      Copyright © 2020 Markus Valtin <github@markus.valtin.net>. All rights reserved.
 *
 *      File:           NetworkInterface_01.cpp -> Source file for the C++ Simulink NetworkInterface class.
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

#include "NetworkInterface_01.hpp"
#include "UDPHandle.hpp"
#include "TCPHandle.hpp"

#include <limits>

#define START_MESSAGE_COUNTER 	1
//#define WITH_HW   				1 // for development only

using namespace std;


struct __attribute__((packed)) outputHeader_t {
    uint16_t sessionID;
    uint32_t msgCounter;
    double   time;
    uint16_t numberOfValues;
};

struct __attribute__((packed)) inputHeader_t {
    double sessionID;
    double msgCounter;
    double numberOfValues;
};


/***************************************************************************
 ***************************************************************************
 * Public Functions for NetworkInterface
 ***************************************************************************
 **************************************************************************/

/**
 *  This class encapsulates the NetworkInterface for the Simulink block.
 */
NetworkInterface::NetworkInterface(
    	uint32_t numberOfInputs, uint32_t numberOfOutputs, // Inputs / Outputs
		uint32_t numberOfIOOptions, uint32_t ioOptiones[],
		uint32_t numberOfInitValues, double initialOutputValues[],
		int useUDP, int startConnection, uint32_t localPort, signed char remoteIP[], uint32_t remotePort
    )
{
    #ifdef WITH_HW
        printf("NetworkInterface (Embedded Coder) set up:\n");
    #else
        printf("NetworkInterface (Simulation) set up:\n");
    #endif

    this->setupSuccessful = true;
    this->setupErrorMsgPrinted = false;

    // Input / Output variables
    this->sizeInputVector = numberOfInputs;
    this->sizeOutputVector = numberOfOutputs;

    // IO options
    this->isInputZero 		= (bool)ioOptiones[0];
    this->isOutputZero 		= (bool)ioOptiones[1];

    if (this->isInputZero){
    	this->sizeInputVector = 0;
    }
    if (this->isOutputZero){
    	this->sizeOutputVector = 0;
    }

    if (!this->isOutputZero && this->sizeOutputVector != numberOfInitValues){
    	this->setupSuccessful = false;
    } else {
    	for (uint32_t i=0; i <this->sizeOutputVector; i++) {
    		this->outputBufferInitialValue[i] = initialOutputValues[i];
    		this->outputBuffer[i] = this->outputBufferInitialValue[i];
    	}
    }

    //Store information about the data output stream
    this->inputSessionID  = 0;
    this->inputCounter = 0.0;
    this->outputSessionID = (uint16_t)this->sizeInputVector;
    this->outputCounter = START_MESSAGE_COUNTER;

    // Store network information
    this->useUDP = (bool)useUDP;
    this->startNetworkConnection = (bool)startConnection;
    this->networkServerStarted = false;

    this->remote_ip = (const char*)remoteIP;
    this->local_ip  = "0.0.0.0";
    this->local_port = localPort;
    this->remote_port= remotePort;

    this->udpHandle = NULL;
    this->tcpHandle = NULL;

    // Start the Network Server
    #ifdef WITH_HW
    this->createNetworkServer();
    if (startConnection) {
    	this->startNetworkServer();
    }
    #endif

    std::cout << "   Connect " << ((this->startNetworkConnection) ? "at Startup " : "during Run ") <<
    		"from " << this->local_ip << ":" << this->local_port << " to " << this->remote_ip << ":" << this->remote_port <<
			" using " << ((this->useUDP) ? "UDP" : "TCP") << std::endl << std::endl;
}

/*
 * Class Destructor
 */
NetworkInterface::~NetworkInterface() {
    #ifdef WITH_HW
   	NetworkInterface::stopNetworkServer();
    #endif
}

/*
 * This function can be used to connect or disconnect the network interface.
 */
void NetworkInterface::updateServerState(double controlInput)
{
    if (controlInput == 1.0) {
    	if (!this->networkServerStarted){
        #ifdef _PAPI_BLOCK_DEBUG_
            printf("NetworkInterface (Embedded Coder): Starting Server\n");
        #endif
            this->startNetworkServer();
    	}
    }
    if (controlInput == 2.0) {
    	if (this->networkServerStarted){
        #ifdef _PAPI_BLOCK_DEBUG_
            printf("NetworkInterface (Embedded Coder): Stopping Server\n");
        #endif
            this->stopNetworkServer();
    	}
    }
}

/*
 * This data sends the received input data via the network interface.
 */
void NetworkInterface::sendInput(const double u1[], double sim_time)
{
	if (!this->setupSuccessful){
		NetworkInterface::printSetupError();
		return;
	}

	if (this->isInputZero){
		return;
	}

	std::size_t msgLength = sizeof(outputHeader_t);
	outputHeader_t* msgHeader = (outputHeader_t*)&this->outputDataStream[0];
	msgHeader->sessionID = this->outputSessionID;
	msgHeader->msgCounter = this->outputCounter++;
	msgHeader->time = sim_time;
	msgHeader->numberOfValues = this->sizeInputVector;

	double* msgData = (double*)&this->outputDataStream[msgLength];
	for(uint16_t i=0; i<this->sizeInputVector; i++){
		msgData[i] = u1[i];
	}
	msgLength += sizeof(double) *this->sizeInputVector;

	uint16_t* msgFooter = (uint16_t*)&this->outputDataStream[msgLength];
	*msgFooter = this->outputSessionID;
	msgLength += sizeof(uint16_t);

	//send current input data
#ifdef WITH_HW
	if (this->useUDP){
		this->udpHandle->sendData(this->outputDataStream, msgLength);
	} else {
		this->tcpHandle->sendData(this->outputDataStream, msgLength);
	}
#endif
}

/*
 * This function writes the current values of the internal buffer to the output variables.
 */
void NetworkInterface::setOutput(double y1[], double* y2_OutputCounter)
{
	if (!this->setupSuccessful){
		NetworkInterface::printSetupError();
		return;
	}

    this->mutex_thread_data_update.lock();

    // set current output buffer as new output
    for(uint16_t i=0; i<this->sizeOutputVector; i++) {
        y1[i] = this->outputBuffer[i];
    }
    *y2_OutputCounter = this->inputCounter;

    this->mutex_thread_data_update.unlock();
}



/***************************************************************************
 ***************************************************************************
 * Private Functions for NetworkInterface
 ***************************************************************************
 **************************************************************************/

/*
 * This function creates and sets up the UDP/TCP connection.
 */
void NetworkInterface::createNetworkServer() {
    #ifdef WITH_HW
	if (this->useUDP){
        this->udpHandle = new UDPHandle(this->local_port, this->remote_port, this->local_ip, this->remote_ip);
        this->udpHandle->receiveCallback = boost::bind(&NetworkInterface::receiveCallback, this, _1, _2);
	} else {
	    this->tcpHandle = new TCPHandle(this->local_port, this->remote_port, this->local_ip, this->remote_ip);
	}
    #endif
}

/*
 * This function starts the connection.
 */
void NetworkInterface::startNetworkServer() {
    #ifdef WITH_HW
	if (this->useUDP){
        this->udpHandle->run();
	} else {
		this->tcpHandle->run(boost::bind(&NetworkInterface::receiveCallback, this, _1, _2));
	}
    #endif
}

/*
 * This function stops the connection.
 */
void NetworkInterface::stopNetworkServer() {
    #ifdef WITH_HW
	if (this->useUDP){
        this->udpHandle->stop();
	} else {
		this->tcpHandle->stop();
	}
    #endif
}

/*
 * This function is called, if a new package was received and updates the internal buffer.
 */
void NetworkInterface::receiveCallback(boost::array<uint8_t, 65500> buffer, std::size_t bufferSize)
{
	if (!this->setupSuccessful){
		return;
	}
	if (this->isOutputZero){
		return;
	}
	std::size_t msgLength = sizeof(inputHeader_t) + sizeof(double) *(this->sizeOutputVector +1);
	if (msgLength != bufferSize){
		std::cout << std::endl << "NetworkInterface ERROR: The received input is " << bufferSize <<
				" bytes long, but should be " << msgLength << " bytes long!" << std::endl <<
				"   -> " << (sizeof(inputHeader_t) + sizeof(double)) << " header bytes  +  " <<
				"8*DataLength data bytes (DataLength=" << this->sizeOutputVector << ")" << std::endl;
		return;
	}

	msgLength = sizeof(inputHeader_t);
    this->mutex_thread_data_update.lock();

    inputHeader_t* msgHeader = (inputHeader_t*)buffer.begin();
    if (this->inputSessionID == 0.0){
    	this->inputSessionID = msgHeader->sessionID;
    }
    if (msgHeader->sessionID != this->inputSessionID){
		std::cout << std::endl << "NetworkInterface ERROR: The received session ID is " << msgHeader->sessionID <<
				", but should be " << this->inputSessionID << "!" << std::endl << std::endl;
    	this->mutex_thread_data_update.unlock();
    	return;
    }

    if (msgHeader->numberOfValues != (double)this->sizeOutputVector){
    	std::cout << std::endl << "NetworkInterface ERROR: The received data contains " << (int)msgHeader->numberOfValues <<
    			" values, but should contain " << this->sizeOutputVector << " values!" << std::endl << std::endl;
    	this->mutex_thread_data_update.unlock();
    	return;
    }

	double* msgFooter = (double*)&buffer[msgLength +(sizeof(double) *this->sizeOutputVector)];
	if (*msgFooter != this->inputSessionID){
		std::cout << std::endl << "NetworkInterface ERROR: The received message footer is " << *msgFooter <<
				", but should be the session ID " << this->inputSessionID << "!" << std::endl << std::endl;
	    this->mutex_thread_data_update.unlock();
	    return;
	}

	// update the internal buffers
    this->inputCounter = msgHeader->msgCounter;

	double* msgData = (double*)&buffer[msgLength];
	for(uint16_t i=0; i<this->sizeOutputVector; i++){
		this->outputBuffer[i] = msgData[i];
	}

	this->mutex_thread_data_update.unlock();
}

/*
 * This function is used to print setup errors.
 */
void NetworkInterface::printSetupError()
{
	if (!this->setupErrorMsgPrinted){
		std::cout << std::endl << "NetworkInterface: Operation suspended because of setup ERROR!" << std::endl << std::endl;
	}
}


//*************************************************
//**** Wrappers for methods called in Simulink ****
//*************************************************

void lctNetworkInterface_Initialise (
		void **work1, //Working vector
		uint32_t numberOfInputs, uint32_t numberOfOutputs, // Inputs / Outputs
		uint32_t numberOfBlockOptions, uint32_t blockOptiones[],
		uint32_t numberOfInitValues, double initialOutputValues[],
		int useUDP, int startConnection, uint32_t localPort, signed char remoteIP[], uint32_t remotePort
)
{
    *work1 = (void *) new NetworkInterface(
   		numberOfInputs, numberOfOutputs, // Inputs / Outputs
		numberOfBlockOptions, blockOptiones,
		numberOfInitValues, initialOutputValues,
    	useUDP, startConnection, localPort, remoteIP, remotePort
    );
    //NetworkInterface *netInterface = (class NetworkInterface *) *work1;
}

void lctNetworkInterface_InputOutput(
	void **work1,
	double u1_Control, double u2_Input[], double u3_Time,
	double y1_Output[], double* y2_OutputCounter
)
{
	NetworkInterface *netInterface = (class NetworkInterface *) *work1;
	netInterface->updateServerState(u1_Control);
	netInterface->sendInput(u2_Input, u3_Time);
	netInterface->setOutput(y1_Output, y2_OutputCounter);
}

void lctNetworkInterface_Deinitialise(void **work1)
{
    NetworkInterface *netInterface = (class NetworkInterface *) *work1;
    delete netInterface;
}
