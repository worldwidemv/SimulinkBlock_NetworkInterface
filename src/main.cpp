/*
 * main.cpp
 *
 *  Created on: 04.05.2020
 *      Author: valtin
 */


#include "TCPHandle.hpp"

void disconnected(void)
{
	printf("MAIN: Disconnected ....\n");
}

void receiveData(boost::array<uint8_t, 65500> buffer, std::size_t bufferSize)
{
	printf("MAIN: Data: ");
	for (uint16_t i=0; i<bufferSize; i++){
		printf("%d,", buffer[i]);
	}
	printf("\n");
}

int main()
{
	uint32_t local_port  = 20000;
	uint32_t remote_port = 20001;
	std::string local_ip  = "127.0.0.1";
	std::string remote_ip = "127.0.0.1";

TCPHandle* con = new TCPHandle(local_port, remote_port, local_ip, remote_ip);
//con->run();
con->run(boost::bind(&receiveData, _1, _2));
con->setConnectionLostCallback(boost::bind(&disconnected));



usleep(10000);
uint8_t temp[10] = {0,1,2,3,4,5,6,7,8,9};

uint32_t i = 0;
while(i < 100){

	i++;
	usleep(3000000);
	con->sendData(temp, 5);
}


  return 0;
}
